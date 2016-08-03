#include <SDL2/SDL.h>

#include <GL/glew.h>

#include <papki/FSFile.hpp>

#include <morda/Morda.hpp>
#include <morda/widgets/label/TextLabel.hpp>
#include <morda/widgets/button/Button.hpp>


const int width_d = 640;
const int height_d = 480;



morda::Key_e sdlScancodeToMordaKey(SDL_Scancode sc){
	switch(sc){
		//TODO: add all key mappings
		case SDL_SCANCODE_LEFT:
			return morda::Key_e::LEFT;
		case SDL_SCANCODE_RIGHT:
			return morda::Key_e::RIGHT;
		case SDL_SCANCODE_LCTRL:
			return morda::Key_e::LEFT_CONTROL;
		case SDL_SCANCODE_RCTRL:
			return morda::Key_e::RIGHT_CONTROL;
		case SDL_SCANCODE_END:
			return morda::Key_e::END;
		case SDL_SCANCODE_HOME:
			return morda::Key_e::HOME;
		case SDL_SCANCODE_BACKSPACE:
			return morda::Key_e::BACKSPACE;
		case SDL_SCANCODE_DELETE:
			return morda::Key_e::DELETE;
		case SDL_SCANCODE_LSHIFT:
			return morda::Key_e::LEFT_SHIFT;
		case SDL_SCANCODE_RSHIFT:
			return morda::Key_e::RIGHT_SHIFT;
		default:
			return morda::Key_e::UNKNOWN;
	}
}


int main( int argc, char* args[] ) { 
	//Initialize SDL 
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) { 
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() ); 
		return 1;
	}
	
	//Use OpenGL 2.0
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 ); 
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
	
	//Create window 
	SDL_Window* window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width_d, height_d, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); 
	if( window == NULL ) { 
		printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() ); 
		SDL_Quit();
		return 1;
	}
	
	//Create OpenGL context 
	SDL_GLContext gContext = SDL_GL_CreateContext( window );
	if( gContext == NULL ) {
		printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
		SDL_DestroyWindow( window ); 
		SDL_Quit();
	}
	
	//Use Vsync
//	if( SDL_GL_SetSwapInterval( 1 ) < 0 ) {
//		printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
//	}
	
	if(glewInit() != GLEW_OK){
		SDL_DestroyWindow( window ); 
		SDL_Quit();
		return 1;
	}
	
	Uint32 userEventType = SDL_RegisterEvents(1);
	if(userEventType == ((Uint32)-1)){
		SDL_DestroyWindow( window ); 
		SDL_Quit();
		return 1;
	}
	
	//create morda singleton
	class SDLMorda : public morda::Morda{
	public:
		const Uint32 userEventType;
		
		SDLMorda(Uint32 userEventType) :
				morda::Morda(96, 1),
				userEventType(userEventType)
		{}
				
		void postToUiThread_ts(std::function<void()>&& f)override{
			SDL_Event e;
			SDL_memset(&e, 0, sizeof(e));
			e.type = this->userEventType;
			e.user.code = 0;
			e.user.data1 = new std::function<void()>(std::move(f));
			e.user.data2 = 0;
			SDL_PushEvent(&e);
		}
	} sdlMorda(userEventType);
	
	morda::Morda::inst().setViewportSize(morda::Vec2r(width_d, height_d));
	
	papki::FSFile fi;

	morda::Morda::inst().initStandardWidgets(fi);
	
	
	//Inflate widgets hierarchy from GUI description script and set it up
	{
		fi.setPath("res/main.gui.stob");
		auto c = morda::Morda::inst().inflater.inflate(fi);

		//set the widgets hierarchy to the application
		morda::Morda::inst().setRootWidget(c);

		auto textLabel = c->findChildByNameAs<morda::TextLabel>("info_text");
		ASSERT(textLabel)

		auto button = c->findChildByNameAs<morda::PushButton>("hw_button");

		auto textLabelWeak = utki::makeWeak(textLabel);//make a weak pointer to the TextLabel widget.

		bool even = true;

		//connect some action on button click
		button->clicked = [textLabelWeak, even](morda::PushButton&) mutable {
			if(auto tl = textLabelWeak.lock()){
				even = !even;
				if(even){
					tl->setText("even");
				}else{
					tl->setText("odd");
				}
			}
			morda::Morda::inst().postToUiThread_ts([](){
				std::cout << "Hello from UI thread!" << std::endl;
			});
		};
	}
	
	//Enable text input 
	SDL_StartTextInput(); 
	//While application is running 
	for(bool quit = false; !quit;) { 
		morda::Morda::inst().update();
		
		SDL_Event e;
		//Handle events on queue 
		while( SDL_PollEvent( &e ) != 0 ) { 
			//User requests quit 
			if( e.type == SDL_QUIT ) { 
				quit = true; 
			}else if(e.type == SDL_MOUSEMOTION){
				int x = 0, y = 0;
				SDL_GetMouseState(&x, &y);
				
				morda::Morda::inst().onMouseMove(morda::Vec2r(x, height_d - y), 0);
			}else if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP){
				int x = 0, y = 0;
				SDL_GetMouseState(&x, &y);
				
				morda::Morda::inst().onMouseButton(
						e.button.type == SDL_MOUSEBUTTONDOWN,
						morda::Vec2r(x, height_d - y),
						e.button.button == 1 ? morda::Widget::MouseButton_e::LEFT : morda::Widget::MouseButton_e::RIGHT,
						0
					);
			}else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){
				std::cout << "SDL scancode= " << unsigned(e.key.keysym.scancode) << std::endl;
				if(e.key.repeat == 0){
					morda::Morda::inst().onKeyEvent(e.key.type == SDL_KEYDOWN, sdlScancodeToMordaKey(e.key.keysym.scancode));
				}
				if(e.type == SDL_KEYDOWN){
					struct SDLUnicodeDummyProvider : public morda::Morda::UnicodeProvider{
						std::u32string get()const override{
							return std::u32string();
						}
					};
					morda::Morda::inst().onCharacterInput(SDLUnicodeDummyProvider(), sdlScancodeToMordaKey(e.key.keysym.scancode));
				}
			}else if( e.type == SDL_TEXTINPUT ) {
				struct SDLUnicodeProvider : public morda::Morda::UnicodeProvider{
					char32_t c;
					SDLUnicodeProvider(char inputChar) :
							c(inputChar)
					{}
					std::u32string get()const override{
						return std::u32string(&this->c, 1);
					}
				} sdlUnicodeProvider(e.text.text[0]);
				morda::Morda::inst().onCharacterInput(sdlUnicodeProvider, morda::Key_e::UNKNOWN);
			}else if(e.type == sdlMorda.userEventType){
				std::unique_ptr<std::function<void()>> f(reinterpret_cast<std::function<void()>*>(e.user.data1));
				f->operator ()();
			}
		}
		
		glClearColor( 0.5f, 0.5f, 0.5f, 1.f );
		glClear( GL_COLOR_BUFFER_BIT );
		
		//Render quad 
		morda::Morda::inst().render();
		
		//Update screen 
		SDL_GL_SwapWindow( window ); 
	} 
	
	//Disable text input 
	SDL_StopTextInput();

	SDL_GL_DeleteContext(gContext);
	
	//Destroy window 
	SDL_DestroyWindow( window ); 
	
	//Quit SDL subsystems 
	SDL_Quit();
	return 0;
}

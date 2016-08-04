#include <SDL2/SDL.h>

#include <GL/glew.h>

#include <papki/FSFile.hpp>

#include <morda/Morda.hpp>
#include <morda/widgets/label/TextLabel.hpp>
#include <morda/widgets/button/Button.hpp>


int width = 640;
int height = 480;

std::array<morda::Key_e, 0x100> keyMap = {{
	morda::Key_e::UNKNOWN, // 0
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 10
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 20
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 30
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 40
	morda::Key_e::UNKNOWN,
	morda::Key_e::BACKSPACE,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 50
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 60
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 70
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::HOME,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::DELETE,
	morda::Key_e::END,
	morda::Key_e::UNKNOWN,
	morda::Key_e::RIGHT,
	morda::Key_e::LEFT,    // 80
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 90
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 100
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 110
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 120
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 130
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 140
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 150
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 160
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 170
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 180
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 190
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 200
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 210
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 220
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::LEFT_CONTROL,
	morda::Key_e::LEFT_SHIFT, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::RIGHT_CONTROL,
	morda::Key_e::RIGHT_SHIFT,
	morda::Key_e::UNKNOWN, // 230
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 240
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // x5
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, // 250
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN  // 255
}};

morda::Key_e sdlScancodeToMordaKey(SDL_Scancode sc){
	if(sc >= keyMap.size()){
		return morda::Key_e::UNKNOWN;
	}
	
	return keyMap[sc];
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
	SDL_Window* window = SDL_CreateWindow(
			"SDL Tutorial",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width,
			height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
		); 
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
	
	morda::Morda::inst().setViewportSize(morda::Vec2r(width, height));
	
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
		if(SDL_WaitEventTimeout(nullptr, morda::Morda::inst().update()) == 1){
			SDL_Event e;
			//Handle events on queue 
			while( SDL_PollEvent( &e ) != 0 ) { 
				//User requests quit 
				if( e.type == SDL_QUIT ) { 
					quit = true; 
				}else if(e.type == SDL_WINDOWEVENT){
					switch(e.window.event) {
						default:
							break;
						case SDL_WINDOWEVENT_RESIZED:
						case SDL_WINDOWEVENT_SIZE_CHANGED:
							width = e.window.data1;
							height = e.window.data2;
//							std::cout << "w = " << e.window.data1 << " h = " << e.window.data2 << std::endl;
							morda::Morda::inst().setViewportSize(morda::Vec2r(width, height));
							glViewport(0, 0, width, height);
							break;
						case SDL_WINDOWEVENT_ENTER:
							morda::Morda::inst().onMouseHover(true, 0);
							break;
						case SDL_WINDOWEVENT_LEAVE:
							morda::Morda::inst().onMouseHover(false, 0);
							break;
					}
				}else if(e.type == SDL_MOUSEMOTION){
					int x = 0, y = 0;
					SDL_GetMouseState(&x, &y);

					morda::Morda::inst().onMouseMove(morda::Vec2r(x, height - y), 0);
				}else if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP){
					int x = 0, y = 0;
					SDL_GetMouseState(&x, &y);

					morda::Morda::inst().onMouseButton(
							e.button.type == SDL_MOUSEBUTTONDOWN,
							morda::Vec2r(x, height - y),
							e.button.button == 1 ? morda::MouseButton_e::LEFT : morda::MouseButton_e::RIGHT,
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

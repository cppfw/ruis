#include <utki/config.hpp>

#if M_COMPILER == M_COMPILER_MSVC
#	include <SDL.h>
#else
#	include <SDL2/SDL.h>
#endif

#include <GL/glew.h>

#include <papki/FSFile.hpp>

#include <morda/Morda.hpp>
#include <mordaren/OpenGL2Renderer.hpp>
#include <morda/widgets/label/Text.hpp>
#include <morda/widgets/button/Button.hpp>
#include <morda/widgets/button/PushButton.hpp>


int width = 640;
int height = 480;

std::array<morda::Key_e, 0x100> keyMap = {{
	morda::Key_e::UNKNOWN, // 0
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::A,
	morda::Key_e::B, // x5
	morda::Key_e::C,
	morda::Key_e::D,
	morda::Key_e::E,
	morda::Key_e::F,
	morda::Key_e::G, // 10
	morda::Key_e::H,
	morda::Key_e::I,
	morda::Key_e::J,
	morda::Key_e::K,
	morda::Key_e::L, // x5
	morda::Key_e::M,
	morda::Key_e::N,
	morda::Key_e::O,
	morda::Key_e::P,
	morda::Key_e::Q, // 20
	morda::Key_e::R,
	morda::Key_e::S,
	morda::Key_e::T,
	morda::Key_e::U,
	morda::Key_e::V, // x5
	morda::Key_e::W,
	morda::Key_e::X,
	morda::Key_e::Y,
	morda::Key_e::Z,
	morda::Key_e::ONE, // 30
	morda::Key_e::TWO,
	morda::Key_e::THREE,
	morda::Key_e::FOUR,
	morda::Key_e::FIVE,
	morda::Key_e::SIX, // x5
	morda::Key_e::SEVEN,
	morda::Key_e::EIGHT,
	morda::Key_e::NINE,
	morda::Key_e::ZERO,
	morda::Key_e::ENTER, // 40
	morda::Key_e::ESCAPE,
	morda::Key_e::BACKSPACE,
	morda::Key_e::TAB,
	morda::Key_e::SPACE,
	morda::Key_e::MINUS, // x5
	morda::Key_e::EQUALS,
	morda::Key_e::LEFT_SQUARE_BRACKET,
	morda::Key_e::RIGHT_SQUARE_BRACKET,
	morda::Key_e::BACKSLASH,
	morda::Key_e::BACKSLASH, // 50
	morda::Key_e::SEMICOLON,
	morda::Key_e::APOSTROPHE,
	morda::Key_e::GRAVE,
	morda::Key_e::COMMA,
	morda::Key_e::PERIOD, // x5
	morda::Key_e::SLASH,
	morda::Key_e::CAPSLOCK,
	morda::Key_e::F1,
	morda::Key_e::F2,
	morda::Key_e::F3, // 60
	morda::Key_e::F4,
	morda::Key_e::F5,
	morda::Key_e::F6,
	morda::Key_e::F7,
	morda::Key_e::F8, // x5
	morda::Key_e::F9,
	morda::Key_e::F10,
	morda::Key_e::F11,
	morda::Key_e::F12,
	morda::Key_e::PRINT_SCREEN, // 70
	morda::Key_e::UNKNOWN,
	morda::Key_e::PAUSE,
	morda::Key_e::INSERT,
	morda::Key_e::HOME,
	morda::Key_e::PAGE_UP, // x5
	morda::Key_e::DELETE,
	morda::Key_e::END,
	morda::Key_e::PAGE_DOWN,
	morda::Key_e::RIGHT,
	morda::Key_e::LEFT,    // 80
	morda::Key_e::DOWN,
	morda::Key_e::UP,
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
	morda::Key_e::F13,
	morda::Key_e::F14, // x5
	morda::Key_e::F15,
	morda::Key_e::F16,
	morda::Key_e::F17,
	morda::Key_e::F18,
	morda::Key_e::F19, // 110
	morda::Key_e::F20,
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
	morda::Key_e::UNKNOWN, // x5
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
	morda::Key_e::LEFT_ALT,
	morda::Key_e::UNKNOWN,
	morda::Key_e::RIGHT_CONTROL,
	morda::Key_e::RIGHT_SHIFT,
	morda::Key_e::RIGHT_ALT, // 230
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
	if(size_t(sc) >= keyMap.size()){
		return morda::Key_e::UNKNOWN;
	}
	
	return keyMap[sc];
}


int main( int argc, char* args[] ) {
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) { 
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() ); 
		return 1;
	}
	
	//Use OpenGL 2.0
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 ); 
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
	
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
				morda::Morda(std::make_shared<mordaren::OpenGL2Renderer>(), 96, 1),
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
	
	morda::Morda::inst().setViewportSize(morda::Vec2r(morda::real(width), morda::real(height)));
	
	papki::FSFile fi;

	morda::Morda::inst().initStandardWidgets(fi);
	
	
	//Inflate widgets hierarchy from GUI description script and set it up
	{
		fi.setPath("res/main.gui");
		auto c = morda::Morda::inst().inflater.inflate(fi);

		//set the widgets hierarchy to the application
		morda::Morda::inst().setRootWidget(c);

		auto textLabel = c->findChildByNameAs<morda::Text>("info_text");
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
	
	SDL_StartTextInput(); 

	for(bool quit = false; !quit;) { 
		if(SDL_WaitEventTimeout(nullptr, morda::Morda::inst().update()) == 1){
			SDL_Event e;
			while( SDL_PollEvent( &e ) != 0 ) { 
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
							morda::Morda::inst().setViewportSize(morda::Vec2r(morda::real(width), morda::real(height)));
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

					morda::Morda::inst().onMouseMove(morda::Vec2r(morda::real(x), morda::real(y)), 0);
				}else if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP){
					int x = 0, y = 0;
					SDL_GetMouseState(&x, &y);

					morda::Morda::inst().onMouseButton(
							e.button.type == SDL_MOUSEBUTTONDOWN,
							morda::Vec2r(morda::real(x), morda::real(y)),
							e.button.button == 1 ? morda::MouseButton_e::LEFT : morda::MouseButton_e::RIGHT,
							0
						);
				}else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){
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
						char* text;
						SDLUnicodeProvider(char* text) :
								text(text)
						{}
						std::u32string get()const override{
							return unikod::toUtf32(this->text);
						}
					} sdlUnicodeProvider(e.text.text); //save pointer to text, the ownership of text buffer is not taken!!!
					morda::Morda::inst().onCharacterInput(sdlUnicodeProvider, morda::Key_e::UNKNOWN);
				}else if(e.type == sdlMorda.userEventType){
					std::unique_ptr<std::function<void()>> f(reinterpret_cast<std::function<void()>*>(e.user.data1));
					f->operator ()();
				}
			}
		}
		
		glClearColor( 0.5f, 0.5f, 0.5f, 1.f );
		glClear( GL_COLOR_BUFFER_BIT );
		
		morda::Morda::inst().render();
		
		SDL_GL_SwapWindow( window ); 
	} 
	
	SDL_StopTextInput();

	SDL_GL_DeleteContext(gContext);
	
	SDL_DestroyWindow( window ); 
	
	SDL_Quit();
	
	return 0;
}

//This file contains implementations of platform dependent methods from App class.


#include "../App.hpp"

#include <cerrno>
#include <ctime>
#include <csignal>

#include <android/native_activity.h>
#include <android/configuration.h>

#include <ting/Array.hpp>
#include <ting/types.hpp>
#include <ting/utf8.hpp>

#include <time.h>
#include <signal.h>

#include "AssetFile.hpp"
#include "morda/App.hpp"
#include "morda/KeyListener.hpp"



using namespace morda;



namespace{

ANativeActivity* nativeActivity = 0;

ANativeWindow* androidWindow = 0;

morda::Vec2f curWinDim(0, 0);

AInputQueue* curInputQueue = 0;

struct AppInfo{
	//Path to this application's internal data directory.
	const char* internalDataPath;

	//Path to this application's external (removable/mountable) data directory.
	const char* externalDataPath;

	//Pointer to the Asset Manager instance for the application. The application
	//uses this to access binary assets bundled inside its own .apk file.
	AAssetManager* assetManager;

	//Holds info about saved state if restoring from previously saved state.
	ting::Array<ting::u8> savedState;
} appInfo;



//array of current pointer positions, needed to detect which pointers have actually moved.
ting::StaticBuffer<morda::Vec2f, 10> pointers;



inline morda::Vec2f AndroidWinCoordsToMordaWinRectCoords(const morda::Rect2f& winRect, const morda::Vec2f& p){
	morda::Vec2f ret(
			p.x,
			curWinDim.y - p.y - winRect.p.y - 1.0f
		);
//	TRACE(<< "AndroidWinCoordsToMordaWinRectCoords(): ret = " << ret << std::endl)
	return ret;
}



struct AndroidConfiguration{
	AConfiguration* ac;

	AndroidConfiguration(){
		this->ac = AConfiguration_new();
	}

	~AndroidConfiguration()throw(){
		AConfiguration_delete(this->ac);
	}

	static inline ting::Ptr<AndroidConfiguration> New(){
		return ting::Ptr<AndroidConfiguration>(new AndroidConfiguration());
	}
};

ting::Ptr<AndroidConfiguration> curConfig;



class JavaFunctionsWrapper{
	JNIEnv *env;
	jclass clazz;
	jobject obj;
	
	jmethodID resolveKeycodeUnicodeMeth;
	
	jmethodID showVirtualKeyboardMeth;
	jmethodID hideVirtualKeyboardMeth;
protected:
	JavaFunctionsWrapper(ANativeActivity* a){
		this->env = a->env;
		this->obj = a->clazz;
		this->clazz = this->env->GetObjectClass(this->obj);
		ASSERT(this->clazz)
		
		
		this->resolveKeycodeUnicodeMeth = this->env->GetMethodID(this->clazz, "resolveKeyUnicode", "(III)I");
		ASSERT(this->resolveKeycodeUnicodeMeth)
		
		this->showVirtualKeyboardMeth = this->env->GetMethodID(this->clazz, "showVirtualKeyboard", "()V");
		ASSERT(this->showVirtualKeyboardMeth)
		
		this->hideVirtualKeyboardMeth = this->env->GetMethodID(this->clazz, "hideVirtualKeyboard", "()V");
		ASSERT(this->hideVirtualKeyboardMeth)
	}
	
public:
	
	static ting::Ptr<JavaFunctionsWrapper> New(ANativeActivity* a){
		return ting::Ptr<JavaFunctionsWrapper>(
				new JavaFunctionsWrapper(a)
			);
	}
	
	~JavaFunctionsWrapper()throw(){
	}
	
	jint ResolveKeyUnicode(int32_t devId, int32_t metaState, int32_t keyCode){
		return this->env->CallIntMethod(this->obj, this->resolveKeycodeUnicodeMeth, jint(devId), jint(metaState), jint(keyCode));
	}
	
	void HideVirtualKeyboard(){
		this->env->CallVoidMethod(this->obj, this->hideVirtualKeyboardMeth);
	}
	
	void ShowVirtualKeyboard(){
		this->env->CallVoidMethod(this->obj, this->showVirtualKeyboardMeth);
	}
};



ting::Ptr<JavaFunctionsWrapper> javaFunctionsWrapper;



class KeyEventToUnicodeResolver{
public:	
	int32_t kc;//key code
	int32_t ms;//meta state
	int32_t di;//device id
	
	ting::Array<ting::u32> Resolve(){
		ASSERT(javaFunctionsWrapper)
//		TRACE(<< "KeyEventToUnicodeResolver::Resolve(): this->kc = " << this->kc << std::endl)
		jint res = javaFunctionsWrapper->ResolveKeyUnicode(this->di, this->ms, this->kc);

		//0 means that key did not produce any unicode character
		if(res == 0){
			return ting::Array<ting::u32>();
		}
		
		ting::Array<ting::u32> ret(1);
		ret[0] = ting::u32(res);
		
		return ret;
	}
	

} keyUnicodeResolver;



//================
// for Updateable
//================

class FDFlag{
	//use pipe to implement file descriptor flag which will be risen upon timer expiration
	//TODO: use eventfd when it is available in Android NDK
	int pipeEnds[2];
public:
	FDFlag(){
		if(::pipe(&this->pipeEnds[0]) < 0){
			std::stringstream ss;
			ss << "FDFlag::FDFlag(): could not create pipe (*nix) for implementing Waitable,"
					<< " error code = " << errno << ": " << strerror(errno);
			throw ting::Exc(ss.str().c_str());
		}
	}

	~FDFlag()throw(){
		close(this->pipeEnds[0]);
		close(this->pipeEnds[1]);
	}

	inline int GetFD()throw(){
		return this->pipeEnds[0];
	}

	inline void Set(){
		ting::u8 oneByteBuf[1];
		if(write(this->pipeEnds[1], oneByteBuf, 1) != 1){
			ASSERT(false)
		}
	}

	inline void Clear(){
		ting::u8 oneByteBuf[1];
		if(read(this->pipeEnds[0], oneByteBuf, 1) != 1){
			throw ting::Exc("Queue::Wait(): read() failed");
		}
	}
} fdFlag;



class LinuxTimer{
	timer_t timer;

	//Handler for SIGALRM signal
	static void OnSIGALRM(int){
		fdFlag.Set();
	}

public:	
	LinuxTimer(){
		int res = timer_create(
				CLOCK_MONOTONIC,
				0,//means SIGALRM signal is emitted when timer expires
				&this->timer
			);
		if(res != 0){
			throw morda::Exc("timer_create() failed");
		}

		struct sigaction sa;
		sa._u._sa_handler = &LinuxTimer::OnSIGALRM;
		sa.sa_flags = SA_NODEFER;
		sa.sa_mask = 0;

		res = sigaction(SIGALRM, &sa, 0);
		ASSERT(res == 0)
	}

	~LinuxTimer()throw(){
		//set default handler for SIGALRM
		struct sigaction sa;
		sa._u._sa_handler = SIG_DFL;
		sa.sa_flags = 0;
		sa.sa_mask = 0;

#ifdef DEBUG
		int res =
#endif
		sigaction(SIGALRM, &sa, 0);
		ASSERT(res == 0)

		//delete timer
#ifdef DEBUG
		res =
#endif
		timer_delete(this->timer);
		ASSERT(res == 0)
	}

	//if timer is already armed, it will re-set the expiration time
	void Arm(ting::u32 dt){
		itimerspec ts;
		ts.it_value.tv_sec = dt / 1000;
		ts.it_value.tv_nsec = (dt % 1000) * 1000000;
		ts.it_interval.tv_nsec = 0;//one shot timer
		ts.it_interval.tv_sec = 0;//one shot timer

#ifdef DEBUG
		int res =
#endif
		timer_settime(this->timer, 0, &ts, 0);
		ASSERT(res == 0)
	}

	//returns true if timer was disarmed
	//returns false if timer has fired before it was disarmed.
	bool Disarm(){
		itimerspec oldts;
		itimerspec newts;
		newts.it_value.tv_nsec = 0;
		newts.it_value.tv_sec = 0;

#ifdef DEBUG
		int res =
#endif
		timer_settime(this->timer, 0, &newts, &oldts);
		ASSERT(res == 0)

		if(oldts.it_value.tv_nsec != 0 || oldts.it_value.tv_sec != 0){
			return true;
		}
		return false;
	}
} timer;



//TODO: this mapping is not final
const key::Key keyCodeMap[ting::u8(-1) + 1] = {
	key::UNKNOWN, //AKEYCODE_UNKNOWN
	key::LEFT, //AKEYCODE_SOFT_LEFT
	key::RIGHT, //AKEYCODE_SOFT_RIGHT
	key::HOME, //AKEYCODE_HOME
	key::ESCAPE, //AKEYCODE_BACK
	key::F11, //AKEYCODE_CALL
	key::F12, //AKEYCODE_ENDCALL
	key::ZERO, //AKEYCODE_0
	key::ONE, //AKEYCODE_1
	key::TWO, //AKEYCODE_2
	key::THREE, //AKEYCODE_3
	key::FOUR, //AKEYCODE_4
	key::FIVE, //AKEYCODE_5
	key::SIX, //AKEYCODE_6
	key::SEVEN, //AKEYCODE_7
	key::EIGHT, //AKEYCODE_8
	key::NINE, //AKEYCODE_9
	key::UNKNOWN, //AKEYCODE_STAR
	key::UNKNOWN, //AKEYCODE_POUND
	key::UP, //AKEYCODE_DPAD_UP
	key::DOWN, //AKEYCODE_DPAD_DOWN
	key::LEFT, //AKEYCODE_DPAD_LEFT
	key::RIGHT, //AKEYCODE_DPAD_RIGHT
	key::ENTER, //AKEYCODE_DPAD_CENTER
	key::PAGE_UP, //AKEYCODE_VOLUME_UP
	key::PAGE_DOWN, //AKEYCODE_VOLUME_DOWN
	key::F10, //AKEYCODE_POWER
	key::F9, //AKEYCODE_CAMERA
	key::BACKSPACE, //AKEYCODE_CLEAR
	key::A, //AKEYCODE_A
	key::B, //AKEYCODE_B
	key::C, //AKEYCODE_C
	key::D, //AKEYCODE_D
	key::E, //AKEYCODE_E
	key::F, //AKEYCODE_F
	key::G, //AKEYCODE_G
	key::H, //AKEYCODE_H
	key::I, //AKEYCODE_I
	key::G, //AKEYCODE_J
	key::K, //AKEYCODE_K
	key::L, //AKEYCODE_L
	key::M, //AKEYCODE_M
	key::N, //AKEYCODE_N
	key::O, //AKEYCODE_O
	key::P, //AKEYCODE_P
	key::Q, //AKEYCODE_Q
	key::R, //AKEYCODE_R
	key::S, //AKEYCODE_S
	key::T, //AKEYCODE_T
	key::U, //AKEYCODE_U
	key::V, //AKEYCODE_V
	key::W, //AKEYCODE_W
	key::X, //AKEYCODE_X
	key::Y, //AKEYCODE_Y
	key::Z, //AKEYCODE_Z
	key::V, //AKEYCODE_COMMA
	key::B, //AKEYCODE_PERIOD
	key::N, //AKEYCODE_ALT_LEFT
	key::M, //AKEYCODE_ALT_RIGHT
	key::LEFT_SHIFT, //AKEYCODE_SHIFT_LEFT
	key::RIGHT_SHIFT, //AKEYCODE_SHIFT_RIGHT
	key::TAB, //AKEYCODE_TAB
	key::SPACE, //AKEYCODE_SPACE
	key::LEFT_CONTROL, //AKEYCODE_SYM
	key::F8, //AKEYCODE_EXPLORER
	key::F7, //AKEYCODE_ENVELOPE
	key::ENTER, //AKEYCODE_ENTER
	key::DELETE, //AKEYCODE_DEL
	key::F6, //AKEYCODE_GRAVE
	key::MINUS, //AKEYCODE_MINUS
	key::EQUALS, //AKEYCODE_EQUALS
	key::LEFT_SQUARE_BRACKET, //AKEYCODE_LEFT_BRACKET
	key::RIGHT_SQUARE_BRACKET, //AKEYCODE_RIGHT_BRACKET
	key::BACKSLASH, //AKEYCODE_BACKSLASH
	key::SEMICOLON, //AKEYCODE_SEMICOLON
	key::APOSTROPHE, //AKEYCODE_APOSTROPHE
	key::SLASH, //AKEYCODE_SLASH
	key::TICK, //AKEYCODE_AT
	key::F5, //AKEYCODE_NUM
	key::F4, //AKEYCODE_HEADSETHOOK
	key::F3, //AKEYCODE_FOCUS (camera focus)
	key::F2, //AKEYCODE_PLUS
	key::F1, //AKEYCODE_MENU
	key::END, //AKEYCODE_NOTIFICATION
	key::RIGHT_CONTROL, //AKEYCODE_SEARCH
	key::UNKNOWN, //AKEYCODE_MEDIA_PLAY_PAUSE
	key::UNKNOWN, //AKEYCODE_MEDIA_STOP
	key::UNKNOWN, //AKEYCODE_MEDIA_NEXT
	key::UNKNOWN, //AKEYCODE_MEDIA_PREVIOUS
	key::UNKNOWN, //AKEYCODE_MEDIA_REWIND
	key::UNKNOWN, //AKEYCODE_MEDIA_FAST_FORWARD
	key::UNKNOWN, //AKEYCODE_MUTE
	key::PAGE_UP, //AKEYCODE_PAGE_UP
	key::PAGE_DOWN, //AKEYCODE_PAGE_DOWN
	key::UNKNOWN, //AKEYCODE_PICTSYMBOLS
	key::CAPSLOCK, //AKEYCODE_SWITCH_CHARSET
	key::UNKNOWN, //AKEYCODE_BUTTON_A
	key::UNKNOWN, //AKEYCODE_BUTTON_B
	key::UNKNOWN, //AKEYCODE_BUTTON_C
	key::UNKNOWN, //AKEYCODE_BUTTON_X
	key::UNKNOWN, //AKEYCODE_BUTTON_Y
	key::UNKNOWN, //AKEYCODE_BUTTON_Z
	key::UNKNOWN, //AKEYCODE_BUTTON_L1
	key::UNKNOWN, //AKEYCODE_BUTTON_R1
	key::UNKNOWN, //AKEYCODE_BUTTON_L2
	key::UNKNOWN, //AKEYCODE_BUTTON_R2
	key::UNKNOWN, //AKEYCODE_BUTTON_THUMBL
	key::UNKNOWN, //AKEYCODE_BUTTON_THUMBR
	key::UNKNOWN, //AKEYCODE_BUTTON_START
	key::UNKNOWN, //AKEYCODE_BUTTON_SELECT
	key::UNKNOWN, //AKEYCODE_BUTTON_MODE
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN, //
	key::UNKNOWN  //
};



key::Key GetKeyFromKeyEvent(AInputEvent& event)throw(){
	int32_t kc = AKeyEvent_getKeyCode(&event);
	ASSERT(0 <= kc && kc <= ting::u8(-1))
	return keyCodeMap[ting::u8(kc)];
}



struct UnicodeResolver{
	ting::Array<ting::u32> chars;

	ting::Array<ting::u32> Resolve(){
		return this->chars;
	}
};



}//~namespace



namespace morda{

void HandleCharacterInputEvent(ting::Array<ting::u32> chars){
	UnicodeResolver resolver;
	resolver.chars = chars;
	
	//notify about input event
	morda::App::Inst().HandleKeyEvent<true, true, UnicodeResolver>(
			key::SPACE,//will be ignored
			resolver
		);
}

}//~namespace



namespace{



JNIEXPORT void JNICALL Java_com_googlecode_morda_tests_SharedLibLoaderNativeActivity_handleCharacterStringInput(
		JNIEnv *env,
		jclass clazz,
		jstring chars
	)
{
	TRACE(<< "handleCharacterStringInput(): invoked" << std::endl)
	
	const char *utf8Chars = env->GetStringUTFChars(chars, 0);

	if(utf8Chars == 0 || *utf8Chars == 0){
		TRACE(<< "handleCharacterStringInput(): empty string passed in" << std::endl)
		return;
	}
	
	typedef std::vector<ting::u32> T_Vector;
	T_Vector utf32;

	for(ting::utf8::Iterator i(utf8Chars); i.IsNotEnd(); ++i){
		utf32.push_back(i.Char());
	}
	
	ting::Array<ting::u32> utf32Chars(utf32.size());

	ting::u32* dst = utf32Chars.Begin();
	for(T_Vector::iterator src = utf32.begin(); src != utf32.end(); ++src, ++dst){
		*dst = *src;
	}

	HandleCharacterInputEvent(utf32Chars);

	env->ReleaseStringUTFChars(chars, utf8Chars);
}



}//~namespace



jint JNI_OnLoad(JavaVM* vm, void* reserved){
	TRACE(<< "JNI_OnLoad(): invoked" << std::endl)
	
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

	static JNINativeMethod methods[] = {
		{"handleCharacterStringInput", "(Ljava/lang/String;)V", (void*)&Java_com_googlecode_morda_tests_SharedLibLoaderNativeActivity_handleCharacterStringInput},
	};
	jclass clazz = env->FindClass("com/googlecode/morda/MordaActivity");
	ASSERT(clazz)
	if(env->RegisterNatives(clazz, methods, 1) < 0){
		ASSERT(false)
	}

    return JNI_VERSION_1_6;
}



namespace morda{



App::EGLDisplayWrapper::EGLDisplayWrapper(){
	this->d = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(this->d == EGL_NO_DISPLAY){
		throw morda::Exc("eglGetDisplay(): failed, no matching display connection found");
	}

	if(eglInitialize(this->d, 0, 0) == EGL_FALSE){
		eglTerminate(this->d);
		throw morda::Exc("eglInitialize() failed");
	}
}



App::EGLDisplayWrapper::~EGLDisplayWrapper()throw(){
	eglTerminate(this->d);
}



App::EGLConfigWrapper::EGLConfigWrapper(const WindowParams& wp, EGLDisplayWrapper& d){
	//TODO: allow stencil configuration etc. via WindowParams
	//Here specify the attributes of the desired configuration.
	//Below, we select an EGLConfig with at least 8 bits per color
	//component compatible with on-screen windows
	const EGLint attribs[] = {
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, //we want OpenGL ES 2.0
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_NONE
	};

	//Here, the application chooses the configuration it desires. In this
	//sample, we have a very simplified selection process, where we pick
	//the first EGLConfig that matches our criteria
	EGLint numConfigs;
	eglChooseConfig(d.d, attribs, &this->c, 1, &numConfigs);
	if(numConfigs <= 0){
		throw morda::Exc("eglChooseConfig() failed, no matching config found");
	}
}



App::EGLSurfaceWrapper::EGLSurfaceWrapper(EGLDisplayWrapper& d, EGLConfigWrapper& c) :
		d(d)
{
	//EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	//guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	//As soon as we picked a EGLConfig, we can safely reconfigure the
	//ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
	EGLint format;
	if(eglGetConfigAttrib(d.d, c.c, EGL_NATIVE_VISUAL_ID, &format) == EGL_FALSE){
		throw morda::Exc("eglGetConfigAttrib() failed");
	}

	ASSERT(androidWindow)
	ANativeWindow_setBuffersGeometry(androidWindow, 0, 0, format);

	this->s = eglCreateWindowSurface(d.d, c.c, androidWindow, NULL);
	if(this->s == EGL_NO_SURFACE){
		throw morda::Exc("eglCreateWindowSurface() failed");
	}
}



App::EGLSurfaceWrapper::~EGLSurfaceWrapper()throw(){
	eglDestroySurface(this->d.d, this->s);
}



App::EGLContextWrapper::EGLContextWrapper(EGLDisplayWrapper& d, EGLConfigWrapper& config, EGLSurfaceWrapper& s) :
		d(d)
{
	EGLint contextAttrs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2, //This is needed on Android, otherwise eglCreateContext() thinks that we want OpenGL ES 1.1, but we want 2.0
		EGL_NONE
	};

	this->c = eglCreateContext(d.d, config.c, NULL, contextAttrs);
	if(this->c == EGL_NO_CONTEXT){
		throw morda::Exc("eglCreateContext() failed");
	}

	if(eglMakeCurrent(d.d, s.s, s.s, this->c) == EGL_FALSE){
		eglDestroyContext(d.d, this->c);
		throw morda::Exc("eglMakeCurrent() failed");
	}
}



App::EGLContextWrapper::~EGLContextWrapper()throw(){
	eglMakeCurrent(this->d.d, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(this->d.d, this->c);
}



App::App(const WindowParams& requestedWindowParams) :
		eglConfig(requestedWindowParams, eglDisplay),
		eglSurface(eglDisplay, eglConfig),
		eglContext(eglDisplay, eglConfig, eglSurface)
{
	EGLint width, height;
	eglQuerySurface(eglDisplay.d, eglSurface.s, EGL_WIDTH, &width);
	eglQuerySurface(eglDisplay.d, eglSurface.s, EGL_HEIGHT, &height);

	this->UpdateWindowRect(morda::Rect2f(0, 0, float(width), float(height)));
}



ting::Ptr<ting::fs::File> App::CreateResourceFileInterface(const std::string& path)const{
	return AssetFile::New(appInfo.assetManager, path);
}



void App::ShowVirtualKeyboard()throw(){
	//ANativeActivity_showSoftInput(nativeActivity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED);
	
	ASSERT(javaFunctionsWrapper)
	javaFunctionsWrapper->ShowVirtualKeyboard();
}



void App::HideVirtualKeyboard()throw(){
	//ANativeActivity_hideSoftInput(nativeActivity, ANATIVEACTIVITY_HIDE_SOFT_INPUT_NOT_ALWAYS);
	
	ASSERT(javaFunctionsWrapper)
	javaFunctionsWrapper->HideVirtualKeyboard();
}



inline void UpdateWindowRect(App& app, const morda::Rect2f& rect){
//	TRACE(<< "UpdateWindowRect(): rect = " << rect << std::endl)
	app.UpdateWindowRect(rect);
}



inline void Render(App& app){
	app.Render();
}



inline ting::u32 Update(App& app){
	return app.updater.Update();
}



void HandleInputEvents(){
	morda::App& app = morda::App::Inst();

	//Read and handle input events
	AInputEvent* event;
	while(AInputQueue_getEvent(curInputQueue, &event) >= 0){
		ASSERT(event)
		
//		TRACE(<< "New input event: type = " << AInputEvent_getType(event) << std::endl)
		if(AInputQueue_preDispatchEvent(curInputQueue, event)){
			continue;
		}

		int32_t eventType = AInputEvent_getType(event);
		int32_t eventAction = AMotionEvent_getAction(event);

		bool consume = false;

		if(app.rootContainer.IsValid()){
			switch(eventType){
				case AINPUT_EVENT_TYPE_MOTION:
					switch(eventAction & AMOTION_EVENT_ACTION_MASK){
						case AMOTION_EVENT_ACTION_POINTER_DOWN:
	//						TRACE(<< "Pointer down" << std::endl)
						case AMOTION_EVENT_ACTION_DOWN:
							{
								unsigned pointerIndex = ((eventAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
								unsigned pointerId = unsigned(AMotionEvent_getPointerId(event, pointerIndex));

								if(pointerId >= pointers.Size()){
									TRACE(<< "Pointer ID is too big, only " << pointers.Size() << " pointers supported at maximum")
									continue;
								}

//								TRACE(<< "Action down, ptr id = " << pointerId << std::endl)

								morda::Vec2f p(AMotionEvent_getX(event, pointerIndex), AMotionEvent_getY(event, pointerIndex));
								pointers[pointerId] = p;

								ASSERT(app.rootContainer.IsValid())
								app.rootContainer->OnMouseButtonDown(
										AndroidWinCoordsToMordaWinRectCoords(app.curWinRect, p),
										morda::Widget::LEFT,
										pointerId
									);
							}
							break;
						case AMOTION_EVENT_ACTION_POINTER_UP:
	//						TRACE(<< "Pointer up" << std::endl)
						case AMOTION_EVENT_ACTION_UP:
							{
								unsigned pointerIndex = ((eventAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
								unsigned pointerId = unsigned(AMotionEvent_getPointerId(event, pointerIndex));

								if(pointerId >= pointers.Size()){
									TRACE(<< "Pointer ID is too big, only " << pointers.Size() << " pointers supported at maximum")
									continue;
								}

//								TRACE(<< "Action up, ptr id = " << pointerId << std::endl)

								morda::Vec2f p(AMotionEvent_getX(event, pointerIndex), AMotionEvent_getY(event, pointerIndex));
								pointers[pointerId] = p;

								ASSERT(app.rootContainer.IsValid())
								app.rootContainer->OnMouseButtonUp(
										AndroidWinCoordsToMordaWinRectCoords(app.curWinRect, p),
										morda::Widget::LEFT,
										pointerId
									);
							}
							break;
						case AMOTION_EVENT_ACTION_MOVE:
							{							
								size_t numPointers = AMotionEvent_getPointerCount(event);
								ASSERT(numPointers >= 1)
								for(size_t pointerNum = 0; pointerNum < numPointers; ++pointerNum){
									unsigned pointerId = unsigned(AMotionEvent_getPointerId(event, pointerNum));
									if(pointerId >= pointers.Size()){
										TRACE(<< "Pointer ID is too big, only " << pointers.Size() << " pointers supported at maximum")
										continue;
									}

									//notify root Container only if there was actual movement
									morda::Vec2f p(AMotionEvent_getX(event, pointerNum), AMotionEvent_getY(event, pointerNum));
									if(pointers[pointerId] == p){
										//pointer position did not change
										continue;
									}

	//								TRACE(<< "Action move, ptr id = " << pointerId << std::endl)

									pointers[pointerId] = p;

									ASSERT(app.rootContainer.IsValid())
									app.rootContainer->OnMouseMove(
											AndroidWinCoordsToMordaWinRectCoords(app.curWinRect, p),
											pointerId
										);
								}//~for(every pointer)
							}
							break;
						default:
							TRACE(<< "unknown eventAction = " << eventAction << std::endl)
							break;
					}//~switch(event action)
					consume = true;
					break;
				case AINPUT_EVENT_TYPE_KEY:
					{
//						TRACE(<< "AINPUT_EVENT_TYPE_KEY" << std::endl)
						
						keyUnicodeResolver.kc = AKeyEvent_getKeyCode(event);
						keyUnicodeResolver.ms = AKeyEvent_getMetaState(event);
						keyUnicodeResolver.di = AInputEvent_getDeviceId(event);
						
//						TRACE(<< "AINPUT_EVENT_TYPE_KEY: keyUnicodeResolver.kc = " << keyUnicodeResolver.kc << std::endl)
						
						//detect auto-repeated key events
						if(AKeyEvent_getRepeatCount(event) != 0){
							if(eventAction == AKEY_EVENT_ACTION_DOWN){
								app.HandleKeyEvent<true, true, KeyEventToUnicodeResolver>(
										GetKeyFromKeyEvent(*ASS(event)),
										keyUnicodeResolver
									);
							}
							break;
						}
						
						switch(eventAction){
							case AKEY_EVENT_ACTION_DOWN:
//								TRACE(<< "AKEY_EVENT_ACTION_DOWN" << std::endl)
								app.HandleKeyEvent<true, false, KeyEventToUnicodeResolver>(
										GetKeyFromKeyEvent(*ASS(event)),
										keyUnicodeResolver
									);
								break;
							case AKEY_EVENT_ACTION_UP:
//								TRACE(<< "AKEY_EVENT_ACTION_UP" << std::endl)
								app.HandleKeyEvent<false, false, KeyEventToUnicodeResolver>(
										GetKeyFromKeyEvent(*ASS(event)),
										keyUnicodeResolver
									);
								break;
							default:
								TRACE(<< "unknown AINPUT_EVENT_TYPE_KEY eventAction" << std::endl)
								break;
						}
					}
					break;
				default:
					break;
			}//~switch(event type)
		}//~if(app.rootContainer.IsValid())

		AInputQueue_finishEvent(
				curInputQueue,
				event,
				consume
			);
	}//~while(there are events in input queue)

	//TODO: render only if needed
	app.Render();

	ting::u32 dt = app.updater.Update();
	timer.Arm(dt == 0 ? 1 : dt);	
}



}//~namespace



namespace{



void OnDestroy(ANativeActivity* activity){
	TRACE(<< "OnDestroy(): invoked" << std::endl)
	
	javaFunctionsWrapper.Reset();
}



void OnStart(ANativeActivity* activity){
	TRACE(<< "OnStart(): invoked" << std::endl)
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnStart();
}



void OnResume(ANativeActivity* activity){
	TRACE(<< "OnResume(): invoked" << std::endl)
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnResume();
}



void* OnSaveInstanceState(ANativeActivity* activity, size_t* outSize){
	TRACE(<< "OnSaveInstanceState(): invoked" << std::endl)
	//TODO:
//    return static_cast<morda::App*>(activity->instance)->OnSaveInstanceState(outSize);
}



void OnPause(ANativeActivity* activity){
	TRACE(<< "OnPause(): invoked" << std::endl)
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnPause();
}



void OnStop(ANativeActivity* activity){
	TRACE(<< "OnStop(): invoked" << std::endl)
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnStop();
}



void OnConfigurationChanged(ANativeActivity* activity){
	TRACE(<< "OnConfigurationChanged(): invoked" << std::endl)

	int32_t diff;
	{
		ting::Ptr<AndroidConfiguration> config = AndroidConfiguration::New();
		AConfiguration_fromAssetManager(config->ac, appInfo.assetManager);

		diff = AConfiguration_diff(curConfig->ac, config->ac);

		curConfig = config;
	}

	//if orientation has changed
	if(diff & ACONFIGURATION_ORIENTATION){
		int32_t orientation = AConfiguration_getOrientation(curConfig->ac);
		switch(orientation){
			case ACONFIGURATION_ORIENTATION_LAND:
			case ACONFIGURATION_ORIENTATION_PORT:
				std::swap(curWinDim.x, curWinDim.y);
				break;
			case ACONFIGURATION_ORIENTATION_SQUARE:
				//do nothing
				break;
			case ACONFIGURATION_ORIENTATION_ANY:
				ASSERT(false)
			default:
				ASSERT(false)
				break;
		}
	}
}



void OnLowMemory(ANativeActivity* activity){
	TRACE(<< "OnLowMemory(): invoked" << std::endl)
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnLowMemory();
}



void OnWindowFocusChanged(ANativeActivity* activity, int hasFocus){
	TRACE(<< "OnWindowFocusChanged(): invoked" << std::endl)
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnWindowFocusChanged(bool(hasFocus));
}



int OnUpdateTimerExpired(int fd, int events, void* data){
//	TRACE(<< "OnUpdateTimerExpired(): invoked" << std::endl)

	ting::u32 dt = Update(App::Inst());
	if(dt == 0){
		//do not arm the timer and do not clear the flag
	}else{
		fdFlag.Clear();
		timer.Arm(dt);
	}

//	TRACE(<< "OnUpdateTimerExpired(): armed timer for " << dt << std::endl)

	return 1; //1 means do not remove descriptor from looper
}



void OnNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window){
	TRACE(<< "OnNativeWindowCreated(): invoked" << std::endl)

	//save window in a static var, so it is accessible for OGL initializers from morda::App class
	androidWindow = window;

	curWinDim.x = float(ANativeWindow_getWidth(window));
	curWinDim.y = float(ANativeWindow_getHeight(window));

	ASSERT(!activity->instance)
	try{
		//use local auto-pointer for now because an exception can be thrown and need to delete object then.
		ting::Ptr<AndroidConfiguration> cfg = AndroidConfiguration::New();
		//retrieve current configuration
		AConfiguration_fromAssetManager(cfg->ac, appInfo.assetManager);

		activity->instance = morda::CreateApp(0, 0, appInfo.savedState).Extract();

		//save current configuration in global variable
		curConfig = cfg;

		ALooper_addFd(
				ALooper_prepare(0),
				fdFlag.GetFD(),
				ALOOPER_POLL_CALLBACK,
				ALOOPER_EVENT_INPUT,
				&OnUpdateTimerExpired,
				0
			);
	}catch(std::exception& e){
		TRACE(<< "std::exception uncaught while creating App instance: " << e.what() << std::endl)
		throw;
	}catch(...){
		TRACE(<< "unknown exception uncaught while creating App instance!" << std::endl)
		throw;
	}
}



void OnNativeWindowResized(ANativeActivity* activity, ANativeWindow* window){
	TRACE(<< "OnNativeWindowResized(): invoked" << std::endl)

	//save window dimensions
	curWinDim.x = float(ANativeWindow_getWidth(window));
	curWinDim.y = float(ANativeWindow_getHeight(window));
}



void OnNativeWindowRedrawNeeded(ANativeActivity* activity, ANativeWindow* window){
	TRACE(<< "OnNativeWindowRedrawNeeded(): invoked" << std::endl)

	morda::Render(*static_cast<morda::App*>(activity->instance));
}



void OnNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window){
	TRACE(<< "OnNativeWindowDestroyed(): invoked" << std::endl)

	//remove fdFlag from looper
	ALooper_removeFd(ALooper_prepare(0), fdFlag.GetFD());

	//TODO: need to destroy app right before window is destroyed, i.e. OGL de-initialized
	//destroy app object
	delete static_cast<morda::App*>(activity->instance);
	activity->instance = 0;

	//delete configuration object
	curConfig.Reset();
}



int OnInputEventsReadyForReadingFromQueue(int fd, int events, void* data){
//	TRACE(<< "OnInputEventsReadyForReadingFromQueue(): invoked" << std::endl)

	ASSERT(curInputQueue) //if we get events we should have input queue

	//If window is not created yet, ignore events.
	if(!morda::App::IsCreated()){
		ASSERT(false)
		AInputEvent* event;
		while(AInputQueue_getEvent(curInputQueue, &event) >= 0){
			if(AInputQueue_preDispatchEvent(curInputQueue, event)){
				continue;
			}

			AInputQueue_finishEvent(curInputQueue, event, false);
		}
		return 1;
	}

	ASSERT(morda::App::IsCreated())

	morda::HandleInputEvents();

	return 1; //we don't want to remove input queue descriptor from looper
}



void OnInputQueueCreated(ANativeActivity* activity, AInputQueue* queue){
	TRACE(<< "OnInputQueueCreated(): invoked" << std::endl)
	ASSERT(queue);
	ASSERT(!curInputQueue)
	curInputQueue = queue;

	//attach queue to looper
	AInputQueue_attachLooper(
			curInputQueue,
			ALooper_prepare(0), //get looper for current thread (main thread)
			0, //'ident' is ignored since we are using callback
			&OnInputEventsReadyForReadingFromQueue,
			activity->instance
		);
}



void OnInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue){
	TRACE(<< "OnInputQueueDestroyed(): invoked" << std::endl)
	ASSERT(queue)
	ASSERT(curInputQueue == queue)

	//detach queue from looper
	AInputQueue_detachLooper(queue);

	curInputQueue = 0;
}



//called when, for example, on-screen keyboard has been shown
void OnContentRectChanged(ANativeActivity* activity, const ARect* rect){
	TRACE(<< "OnContentRectChanged(): invoked, left = " << rect->left << " right = " << rect->right << " top = " << rect->top << " bottom = " << rect->bottom << std::endl)

//	TRACE(<< "OnContentRectChanged(): winDim = " << winDim << std::endl)

	morda::App& app = *static_cast<morda::App*>(activity->instance);

	UpdateWindowRect(
			app,
			morda::Rect2f(
					float(rect->left),
					curWinDim.y - float(rect->bottom),
					float(rect->right - rect->left),
					float(rect->bottom - rect->top)
				)
		);

	//redraw, since WindowRedrawNeeded not always comes
	Render(app);
}



}//~namespace



void ANativeActivity_onCreate(
		ANativeActivity* activity,
		void* savedState,
		size_t savedStateSize
	)
{
	TRACE(<< "ANativeActivity_onCreate(): invoked" << std::endl)
	activity->callbacks->onDestroy = &OnDestroy;
	activity->callbacks->onStart = &OnStart;
	activity->callbacks->onResume = &OnResume;
	activity->callbacks->onSaveInstanceState = &OnSaveInstanceState;
	activity->callbacks->onPause = &OnPause;
	activity->callbacks->onStop = &OnStop;
	activity->callbacks->onConfigurationChanged = &OnConfigurationChanged;
	activity->callbacks->onLowMemory = &OnLowMemory;
	activity->callbacks->onWindowFocusChanged = &OnWindowFocusChanged;
	activity->callbacks->onNativeWindowCreated = &OnNativeWindowCreated;
	activity->callbacks->onNativeWindowResized = &OnNativeWindowResized;
	activity->callbacks->onNativeWindowRedrawNeeded = &OnNativeWindowRedrawNeeded;
	activity->callbacks->onNativeWindowDestroyed = &OnNativeWindowDestroyed;
	activity->callbacks->onInputQueueCreated = &OnInputQueueCreated;
	activity->callbacks->onInputQueueDestroyed = &OnInputQueueDestroyed;
	activity->callbacks->onContentRectChanged = &OnContentRectChanged;

	activity->instance = 0;
	
	nativeActivity = activity;	

	appInfo.internalDataPath = activity->internalDataPath;
	appInfo.externalDataPath = activity->externalDataPath;
	appInfo.assetManager = activity->assetManager;
	if(savedState){
		//copy saved state data because no guarantee that the data will be kept alive after returning from this function
		appInfo.savedState.Init(savedStateSize);
		memcpy(appInfo.savedState.Begin(), savedState, savedStateSize);
	}

//	ANativeActivity_setWindowFlags(activity, 1024, 1024); //set fullscreen flag
	
	javaFunctionsWrapper = JavaFunctionsWrapper::New(activity);
}

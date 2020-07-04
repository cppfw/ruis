#include <cerrno>
#include <ctime>
#include <csignal>

#include <android/native_activity.h>
#include <android/configuration.h>
#include <android/asset_manager.h>
#include <android/window.h>

#include <utki/unicode.hpp>
#include <nitki/queue.hpp>
#include <utki/destructable.hpp>

#include <sys/eventfd.h>

#include "../../application.hpp"

#include <morda/render/opengles2/renderer.hpp>

#include <EGL/egl.h>

#include "../friendAccessors.cxx"

using namespace mordavokne;

namespace{
ANativeActivity* nativeActivity = 0;

mordavokne::application& getApp(ANativeActivity* activity){
	ASSERT(activity)
	ASSERT(activity->instance)
	return *static_cast<mordavokne::application*>(activity->instance);
}

ANativeWindow* androidWindow = 0;

class JavaFunctionsWrapper : public utki::destructable{
	JNIEnv *env;
	jclass clazz;
	jobject obj;

	jmethodID resolveKeycodeUnicodeMeth;

	jmethodID getDotsPerInchMeth;

	jmethodID showVirtualKeyboardMeth;
	jmethodID hideVirtualKeyboardMeth;

	jmethodID listDirContentsMeth;

	jmethodID getStorageDirMeth;
public:
	JavaFunctionsWrapper(ANativeActivity* a){
		this->env = a->env;
		this->obj = a->clazz;
		this->clazz = this->env->GetObjectClass(this->obj);
		ASSERT(this->clazz)

		this->resolveKeycodeUnicodeMeth = this->env->GetMethodID(this->clazz, "resolveKeyUnicode", "(III)I");
		ASSERT(this->resolveKeycodeUnicodeMeth)

		this->getDotsPerInchMeth = this->env->GetMethodID(this->clazz, "getDotsPerInch", "()F");

		this->listDirContentsMeth = this->env->GetMethodID(this->clazz, "listDirContents", "(Ljava/lang/String;)[Ljava/lang/String;");
		ASSERT(this->listDirContentsMeth)

		this->showVirtualKeyboardMeth = this->env->GetMethodID(this->clazz, "showVirtualKeyboard", "()V");
		ASSERT(this->showVirtualKeyboardMeth)

		this->hideVirtualKeyboardMeth = this->env->GetMethodID(this->clazz, "hideVirtualKeyboard", "()V");
		ASSERT(this->hideVirtualKeyboardMeth)

		this->getStorageDirMeth = this->env->GetMethodID(this->clazz, "getStorageDir", "()Ljava/lang/String;");
		ASSERT(this->getStorageDirMeth)
	}

	~JavaFunctionsWrapper()noexcept{
	}

	char32_t resolveKeyUnicode(int32_t devId, int32_t metaState, int32_t keyCode){
		return char32_t(this->env->CallIntMethod(
				this->obj,
				this->resolveKeycodeUnicodeMeth,
				jint(devId),
				jint(metaState),
				jint(keyCode)
			));
	}

	float getDotsPerInch(){
		return float(this->env->CallFloatMethod(this->obj, this->getDotsPerInchMeth));
	}

	void hide_virtual_keyboard(){
		this->env->CallVoidMethod(this->obj, this->hideVirtualKeyboardMeth);
	}

	void show_virtual_keyboard(){
		this->env->CallVoidMethod(this->obj, this->showVirtualKeyboardMeth);
	}

	std::vector<std::string> listDirContents(const std::string& path){
		jstring p = this->env->NewStringUTF(path.c_str());
		jobject res = this->env->CallObjectMethod(this->obj, this->listDirContentsMeth, p);
		this->env->DeleteLocalRef(p);

		utki::scope_exit scopeExit([this, res](){
			this->env->DeleteLocalRef(res);
		});

		std::vector<std::string> ret;

		if(res == nullptr){
			return ret;
		}

		jobjectArray arr = static_cast<jobjectArray>(res);

		int count = env->GetArrayLength(arr);

		for (int i = 0; i < count; ++i) {
			jstring str = static_cast<jstring>(env->GetObjectArrayElement(arr, i));
			auto chars = env->GetStringUTFChars(str, nullptr);
			ret.push_back(std::string(chars));
			this->env->ReleaseStringUTFChars(str, chars);
			this->env->DeleteLocalRef(str);
		}

		return ret;
	}

	std::string getStorageDir(){
		jobject res = this->env->CallObjectMethod(this->obj, this->getStorageDirMeth);
		utki::scope_exit resScopeExit([this, &res](){
			this->env->DeleteLocalRef(res);
		});

		jstring str = static_cast<jstring>(res);

		auto chars = env->GetStringUTFChars(str, nullptr);
		utki::scope_exit charsScopeExit([this, &chars, &str](){
			this->env->ReleaseStringUTFChars(str, chars);
		});

		return std::string(chars);
	}
};

std::unique_ptr<JavaFunctionsWrapper> javaFunctionsWrapper;

struct WindowWrapper : public utki::destructable{
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;

	nitki::queue uiQueue;

	WindowWrapper(const window_params& wp){
		this->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		if(this->display == EGL_NO_DISPLAY){
			throw std::runtime_error("eglGetDisplay(): failed, no matching display connection found");
		}

		utki::scope_exit eglDisplayScopeExit([this](){
			eglTerminate(this->display);
		});

		if(eglInitialize(this->display, 0, 0) == EGL_FALSE){
			throw std::runtime_error("eglInitialize() failed");
		}

		// TODO: allow stencil configuration etc. via window_params
		// Here specify the attributes of the desired configuration.
		// Below, we select an EGLConfig with at least 8 bits per color
		// component compatible with on-screen windows
		const EGLint attribs[] = {
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
				EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, //we want OpenGL ES 2.0
				EGL_BLUE_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_RED_SIZE, 8,
				EGL_NONE
		};

		EGLConfig config;

		// Here, the application chooses the configuration it desires. In this
		// sample, we have a very simplified selection process, where we pick
		// the first EGLConfig that matches our criteria
		EGLint numConfigs;
		eglChooseConfig(this->display, attribs, &config, 1, &numConfigs);
		if(numConfigs <= 0){
			throw std::runtime_error("eglChooseConfig() failed, no matching config found");
		}

		// EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
		// guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
		// As soon as we picked a EGLConfig, we can safely reconfigure the
		// ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
		EGLint format;
		if(eglGetConfigAttrib(this->display, config, EGL_NATIVE_VISUAL_ID, &format) == EGL_FALSE){
			throw std::runtime_error("eglGetConfigAttrib() failed");
		}

		ASSERT(androidWindow)
		ANativeWindow_setBuffersGeometry(androidWindow, 0, 0, format);

		this->surface = eglCreateWindowSurface(this->display, config, androidWindow, NULL);
		if(this->surface == EGL_NO_SURFACE){
			throw std::runtime_error("eglCreateWindowSurface() failed");
		}

		utki::scope_exit eglSurfaceScopeExit([this](){
			eglDestroySurface(this->display, this->surface);
		});

		EGLint contextAttrs[] = {
				EGL_CONTEXT_CLIENT_VERSION, 2, //This is needed on Android, otherwise eglCreateContext() thinks that we want OpenGL ES 1.1, but we want 2.0
				EGL_NONE
		};

		this->context = eglCreateContext(this->display, config, NULL, contextAttrs);
		if(this->context == EGL_NO_CONTEXT){
			throw std::runtime_error("eglCreateContext() failed");
		}

		utki::scope_exit eglContextScopeExit([this](){
			eglDestroyContext(this->display, this->context);
		});

		if(eglMakeCurrent(this->display, this->surface, this->surface, this->context) == EGL_FALSE){
			throw std::runtime_error("eglMakeCurrent() failed");
		}

		eglContextScopeExit.reset();
		eglSurfaceScopeExit.reset();
		eglDisplayScopeExit.reset();
	}

	r4::vec2ui getWindowSize(){
		EGLint width, height;
		eglQuerySurface(this->display, this->surface, EGL_WIDTH, &width);
		eglQuerySurface(this->display, this->surface, EGL_HEIGHT, &height);
		return r4::vec2ui(width, height);
	}

	void swapBuffers(){
		eglSwapBuffers(this->display, this->surface);
	}

	~WindowWrapper()noexcept{
		eglMakeCurrent(this->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroyContext(this->display, this->context);
		eglDestroySurface(this->display, this->surface);
		eglTerminate(this->display);
	}
};

WindowWrapper& getImpl(const std::unique_ptr<utki::destructable>& pimpl){
	ASSERT(dynamic_cast<WindowWrapper*>(pimpl.get()))
	return static_cast<WindowWrapper&>(*pimpl);
}

class asset_file : public papki::file{
	AAssetManager* manager;

	mutable AAsset* handle = nullptr;

public:

	asset_file(AAssetManager* manager, const std::string& pathName = std::string()) :
			manager(manager),
			papki::file(pathName)
	{
		ASSERT(this->manager)
	}

	virtual void open_internal(mode mode)override{
		switch(mode){
			case papki::file::mode::WRITE:
			case papki::file::mode::CREATE:
				throw std::invalid_argument("WRITE and CREATE open modes are not supported by Android assets");
			case papki::file::mode::READ:
				break;
			default:
				throw std::invalid_argument("unknown mode");
		}
		this->handle = AAssetManager_open(this->manager, this->path().c_str(), AASSET_MODE_UNKNOWN); //don't know what this MODE means at all
		if(!this->handle){
			std::stringstream ss;
			ss << "AAssetManager_open(" << this->path() << ") failed";
			throw std::runtime_error(ss.str());
		}
	}

	virtual void close_internal()const noexcept override{
		ASSERT(this->handle)
		AAsset_close(this->handle);
		this->handle = 0;
	}

	virtual size_t read_internal(utki::span<std::uint8_t> buf)const override{
		ASSERT(this->handle)
		int numBytesRead = AAsset_read(this->handle, &*buf.begin(), buf.size());
		if(numBytesRead < 0){ // something happened
			throw std::runtime_error("AAsset_read() error");
		}
		ASSERT(numBytesRead >= 0)
		return size_t(numBytesRead);
	}

	virtual size_t write_internal(utki::span<const std::uint8_t> buf)override{
		ASSERT(this->handle)
		throw std::runtime_error("write() is not supported by Android assets");
	}

	virtual size_t seek_forward_internal(size_t numBytesToSeek)const override{
		return this->seek(numBytesToSeek, true);
	}

	virtual size_t seek_backward_internal(size_t numBytesToSeek)const override{
		return this->seek(numBytesToSeek, false);
	}

	virtual void rewind_internal()const override{
		if(!this->is_open()){
			throw std::logic_error("file is not opened, cannot rewind");
		}

		ASSERT(this->handle)
		if(AAsset_seek(this->handle, 0, SEEK_SET) < 0){
			throw std::runtime_error("AAsset_seek() failed");
		}
	}

	virtual bool exists()const override{
		if(this->is_open()){ // file is opened => it exists
			return true;
		}

		if(this->path().size() == 0){
			return false;
		}

		if(this->is_dir()){
			// try opening the directory to check its existence
			AAssetDir* pdir = AAssetManager_openDir(this->manager, this->path().c_str());

			if(!pdir){
				return false;
			}else{
				AAssetDir_close(pdir);
				return true;
			}
		}else{
			return this->file::exists();
		}
	}

	virtual std::vector<std::string> list_dir(size_t maxEntries = 0)const override{
		if(!this->isDir()){
			throw std::logic_error("asset_file::list_dir(): this is not a directory");
		}

		// Trim away trailing '/', as Android does not work with it.
		auto p = this->path().substr(0, this->path().size() - 1);

		ASSERT(javaFunctionsWrapper)
		return javaFunctionsWrapper->listDirContents(p);
	}

	std::unique_ptr<papki::file> spawn()override{
		return utki::make_unique<asset_file>(this->manager);
	}

	~asset_file()noexcept{
	}

	size_t seek(size_t numBytesToSeek, bool seekForward)const{
		if(!this->is_open()){
			throw std::logic_error("file is not opened, cannot seek");
		}

		ASSERT(this->handle)

		// NOTE: AAsset_seek() accepts 'off_t' as offset argument which is signed and can be
		//       less than size_t value passed as argument to this function.
		//       Therefore, do several seek operations with smaller offset if necessary.

		off_t assetSize = AAsset_getLength(this->handle);
		ASSERT(assetSize >= 0)

		if(seekForward){
			ASSERT(size_t(assetSize) >= this->curPos())
			utki::clampTop(numBytesToSeek, size_t(assetSize) - this->curPos());
		}else{
			utki::clampTop(numBytesToSeek, this->curPos());
		}

		typedef off_t T_FSeekOffset;
		const size_t DMax = ((size_t(T_FSeekOffset(-1))) >> 1);
		ASSERT((size_t(1) << ((sizeof(T_FSeekOffset) * 8) - 1)) - 1 == DMax)
		static_assert(size_t(-(-T_FSeekOffset(DMax))) == DMax, "size mismatch");

		for(size_t numBytesLeft = numBytesToSeek; numBytesLeft != 0;){
			ASSERT(numBytesLeft <= numBytesToSeek)

			T_FSeekOffset offset;
			if(numBytesLeft > DMax){
				offset = T_FSeekOffset(DMax);
			}else{
				offset = T_FSeekOffset(numBytesLeft);
			}

			ASSERT(offset > 0)

			if(AAsset_seek(this->handle, seekForward ? offset : (-offset), SEEK_CUR) < 0){
				throw std::runtime_error("AAsset_seek() failed");
			}

			ASSERT(size_t(offset) < size_t(-1))
			ASSERT(numBytesLeft >= size_t(offset))

			numBytesLeft -= size_t(offset);
		}
		return numBytesToSeek;
	}
};

morda::vector2 curWinDim(0, 0);

AInputQueue* curInputQueue = 0;

struct AppInfo{
	// Path to this application's internal data directory.
	const char* internalDataPath;

	// Path to this application's external (removable/mountable) data directory.
	const char* externalDataPath;

	// Pointer to the Asset Manager instance for the application. The application
	// uses this to access binary assets bundled inside its own .apk file.
	AAssetManager* assetManager;
} appInfo;

// array of current pointer positions, needed to detect which pointers have actually moved.
std::array<morda::vector2, 10> pointers;

inline morda::vector2 AndroidWinCoordsToMordaWinRectCoords(const morda::vector2& winDim, const morda::vector2& p){
	morda::vector2 ret(
			p.x,
			p.y - (curWinDim.y - winDim.y)
		);
//	TRACE(<< "AndroidWinCoordsToMordaWinRectCoords(): ret = " << ret << std::endl)
	return ret.rounded();
}

struct AndroidConfiguration{
	AConfiguration* ac;

	AndroidConfiguration(){
		this->ac = AConfiguration_new();
	}

	~AndroidConfiguration()noexcept{
		AConfiguration_delete(this->ac);
	}

	static inline std::unique_ptr<AndroidConfiguration> New(){
		return std::unique_ptr<AndroidConfiguration>(new AndroidConfiguration());
	}
};

std::unique_ptr<AndroidConfiguration> curConfig;

class KeyEventToUnicodeResolver : public morda::gui::unicode_provider{
public:
	int32_t kc;//key code
	int32_t ms;//meta state
	int32_t di;//device id

	std::u32string get()const{
		ASSERT(javaFunctionsWrapper)
//		TRACE(<< "KeyEventToUnicodeResolver::Resolve(): this->kc = " << this->kc << std::endl)
		char32_t res = javaFunctionsWrapper->resolveKeyUnicode(this->di, this->ms, this->kc);

		//0 means that key did not produce any unicode character
		if(res == 0){
			TRACE(<< "key did not produce any unicode character, returning empty string" << std::endl)
			return std::u32string();
		}

		return std::u32string(&res, 1);
	}


} keyUnicodeResolver;



//================
// for Updatable
//================

class FDFlag{
	int eventFD;
public:
	FDFlag(){
		this->eventFD = eventfd(0, EFD_NONBLOCK);
		if(this->eventFD < 0){
			throw std::system_error(errno, std::generic_category(), "could not create eventFD (*nix) for implementing Waitable");
		}
	}

	~FDFlag()noexcept{
		close(this->eventFD);
	}

	int GetFD()noexcept{
		return this->eventFD;
	}

	void Set(){
		if(eventfd_write(this->eventFD, 1) < 0){
			ASSERT(false)
		}
	}

	void Clear(){
		eventfd_t value;
		if(eventfd_read(this->eventFD, &value) < 0){
			if(errno == EAGAIN){
				return;
			}
			ASSERT(false)
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
			throw std::runtime_error("timer_create() failed");
		}

		struct sigaction sa;
		sa.sa_handler = &LinuxTimer::OnSIGALRM;
		sa.sa_flags = SA_NODEFER;
		memset(&sa.sa_mask, 0, sizeof(sa.sa_mask));

		res = sigaction(SIGALRM, &sa, 0);
		ASSERT(res == 0)
	}

	~LinuxTimer()noexcept{
		//set default handler for SIGALRM
		struct sigaction sa;
		sa.sa_handler = SIG_DFL;
		sa.sa_flags = 0;
		memset(&sa.sa_mask, 0, sizeof(sa.sa_mask));

#ifdef DEBUG
		int res =
#endif
		sigaction(SIGALRM, &sa, 0);
		ASSERT_INFO(res == 0, " res = " << res << " errno = " << errno)

		//delete timer
#ifdef DEBUG
		res =
#endif
		timer_delete(this->timer);
		ASSERT_INFO(res == 0, " res = " << res << " errno = " << errno)
	}

	//if timer is already armed, it will re-set the expiration time
	void Arm(std::uint32_t dt){
		itimerspec ts;
		ts.it_value.tv_sec = dt / 1000;
		ts.it_value.tv_nsec = (dt % 1000) * 1000000;
		ts.it_interval.tv_nsec = 0;//one shot timer
		ts.it_interval.tv_sec = 0;//one shot timer

#ifdef DEBUG
		int res =
#endif
		timer_settime(this->timer, 0, &ts, 0);
		ASSERT_INFO(res == 0, " res = " << res << " errno = " << errno)
	}

	//returns true if timer was disarmed
	//returns false if timer has fired before it was disarmed.
	bool Disarm(){
		itimerspec oldts;
		itimerspec newts;
		newts.it_value.tv_nsec = 0;
		newts.it_value.tv_sec = 0;

		int res = timer_settime(this->timer, 0, &newts, &oldts);
		if(res != 0){
			ASSERT_INFO(false, "errno = " << errno << " res = " << res)
		}

		if(oldts.it_value.tv_nsec != 0 || oldts.it_value.tv_sec != 0){
			return true;
		}
		return false;
	}
} timer;



//TODO: this mapping is not final
const std::array<morda::key, std::uint8_t(-1) + 1> keyCodeMap = {
	morda::key::unknown, // AKEYCODE_UNKNOWN
	morda::key::left, // AKEYCODE_SOFT_LEFT
	morda::key::right, // AKEYCODE_SOFT_RIGHT
	morda::key::home, // AKEYCODE_HOME
	morda::key::escape, // AKEYCODE_BACK
	morda::key::f11, // AKEYCODE_CALL
	morda::key::f12, // AKEYCODE_ENDCALL
	morda::key::zero, // AKEYCODE_0
	morda::key::one, // AKEYCODE_1
	morda::key::two, // AKEYCODE_2
	morda::key::three, // AKEYCODE_3
	morda::key::four, // AKEYCODE_4
	morda::key::five, // AKEYCODE_5
	morda::key::six, // AKEYCODE_6
	morda::key::seven, // AKEYCODE_7
	morda::key::eight, // AKEYCODE_8
	morda::key::nine, // AKEYCODE_9
	morda::key::unknown, // AKEYCODE_STAR
	morda::key::unknown, // AKEYCODE_POUND
	morda::key::up, // AKEYCODE_DPAD_UP
	morda::key::down, // AKEYCODE_DPAD_DOWN
	morda::key::left, // AKEYCODE_DPAD_LEFT
	morda::key::right, // AKEYCODE_DPAD_RIGHT
	morda::key::enter, // AKEYCODE_DPAD_CENTER
	morda::key::page_up, // AKEYCODE_VOLUME_UP
	morda::key::page_down, // AKEYCODE_VOLUME_DOWN
	morda::key::f10, // AKEYCODE_POWER
	morda::key::f9, // AKEYCODE_CAMERA
	morda::key::backspace, // AKEYCODE_CLEAR
	morda::key::a, // AKEYCODE_A
	morda::key::b, // AKEYCODE_B
	morda::key::c, // AKEYCODE_C
	morda::key::d, // AKEYCODE_D
	morda::key::e, // AKEYCODE_E
	morda::key::f, // AKEYCODE_F
	morda::key::g, // AKEYCODE_G
	morda::key::h, // AKEYCODE_H
	morda::key::i, // AKEYCODE_I
	morda::key::g, // AKEYCODE_J
	morda::key::k, // AKEYCODE_K
	morda::key::l, // AKEYCODE_L
	morda::key::m, // AKEYCODE_M
	morda::key::n, // AKEYCODE_N
	morda::key::o, // AKEYCODE_O
	morda::key::p, // AKEYCODE_P
	morda::key::q, // AKEYCODE_Q
	morda::key::r, // AKEYCODE_R
	morda::key::s, // AKEYCODE_S
	morda::key::t, // AKEYCODE_T
	morda::key::u, // AKEYCODE_U
	morda::key::v, // AKEYCODE_V
	morda::key::w, // AKEYCODE_W
	morda::key::x, // AKEYCODE_X
	morda::key::y, // AKEYCODE_Y
	morda::key::z, // AKEYCODE_Z
	morda::key::v, // AKEYCODE_COMMA
	morda::key::b, // AKEYCODE_PERIOD
	morda::key::n, // AKEYCODE_ALT_LEFT
	morda::key::m, // AKEYCODE_ALT_RIGHT
	morda::key::left_shift, // AKEYCODE_SHIFT_LEFT
	morda::key::right_shift, // AKEYCODE_SHIFT_RIGHT
	morda::key::tabulator, // AKEYCODE_TAB
	morda::key::space, // AKEYCODE_SPACE
	morda::key::left_control, // AKEYCODE_SYM
	morda::key::f8, // AKEYCODE_EXPLORER
	morda::key::f7, // AKEYCODE_ENVELOPE
	morda::key::enter, // AKEYCODE_ENTER
	morda::key::deletion, // AKEYCODE_DEL
	morda::key::f6, // AKEYCODE_GRAVE
	morda::key::minus, // AKEYCODE_MINUS
	morda::key::equals, // AKEYCODE_EQUALS
	morda::key::left_square_bracket, // AKEYCODE_LEFT_BRACKET
	morda::key::right_square_bracket, // AKEYCODE_RIGHT_BRACKET
	morda::key::backslash, // AKEYCODE_BACKSLASH
	morda::key::semicolon, // AKEYCODE_SEMICOLON
	morda::key::apostrophe, // AKEYCODE_APOSTROPHE
	morda::key::slash, // AKEYCODE_SLASH
	morda::key::grave, // AKEYCODE_AT
	morda::key::f5, // AKEYCODE_NUM
	morda::key::f4, // AKEYCODE_HEADSETHOOK
	morda::key::f3, // AKEYCODE_FOCUS (camera focus)
	morda::key::f2, // AKEYCODE_PLUS
	morda::key::f1, // AKEYCODE_MENU
	morda::key::end, // AKEYCODE_NOTIFICATION
	morda::key::right_control, // AKEYCODE_SEARCH
	morda::key::unknown, // AKEYCODE_MEDIA_PLAY_PAUSE
	morda::key::unknown, // AKEYCODE_MEDIA_STOP
	morda::key::unknown, // AKEYCODE_MEDIA_NEXT
	morda::key::unknown, // AKEYCODE_MEDIA_PREVIOUS
	morda::key::unknown, // AKEYCODE_MEDIA_REWIND
	morda::key::unknown, // AKEYCODE_MEDIA_FAST_FORWARD
	morda::key::unknown, // AKEYCODE_MUTE
	morda::key::page_up, // AKEYCODE_PAGE_UP
	morda::key::page_down, // AKEYCODE_PAGE_DOWN
	morda::key::unknown, // AKEYCODE_PICTSYMBOLS
	morda::key::capslock, // AKEYCODE_SWITCH_CHARSET
	morda::key::unknown, // AKEYCODE_BUTTON_A
	morda::key::unknown, // AKEYCODE_BUTTON_B
	morda::key::unknown, // AKEYCODE_BUTTON_C
	morda::key::unknown, // AKEYCODE_BUTTON_X
	morda::key::unknown, // AKEYCODE_BUTTON_Y
	morda::key::unknown, // AKEYCODE_BUTTON_Z
	morda::key::unknown, // AKEYCODE_BUTTON_L1
	morda::key::unknown, // AKEYCODE_BUTTON_R1
	morda::key::unknown, // AKEYCODE_BUTTON_L2
	morda::key::unknown, // AKEYCODE_BUTTON_R2
	morda::key::unknown, // AKEYCODE_BUTTON_THUMBL
	morda::key::unknown, // AKEYCODE_BUTTON_THUMBR
	morda::key::unknown, // AKEYCODE_BUTTON_START
	morda::key::unknown, // AKEYCODE_BUTTON_SELECT
	morda::key::unknown, // AKEYCODE_BUTTON_MODE
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown, //
	morda::key::unknown  //
};

morda::key getKeyFromKeyEvent(AInputEvent& event)noexcept{
	size_t kc = size_t(AKeyEvent_getKeyCode(&event));
	ASSERT(kc < keyCodeMap.size())
	return keyCodeMap[kc];
}

struct UnicodeProvider : public morda::gui::unicode_provider{
	std::u32string chars;

	std::u32string get()const override{
		return std::move(this->chars);
	}
};

}

namespace{

JNIEXPORT void JNICALL Java_io_github_igagis_mordavokne_MordaVOkneActivity_handleCharacterStringInput(
		JNIEnv *env,
		jclass clazz,
		jstring chars
	)
{
	TRACE(<< "handleCharacterStringInput(): invoked" << std::endl)

	const char *utf8Chars = env->GetStringUTFChars(chars, 0);

	utki::scope_exit scopeExit([env, &chars, utf8Chars](){
		env->ReleaseStringUTFChars(chars, utf8Chars);
	});

	if(utf8Chars == nullptr || *utf8Chars == 0){
		TRACE(<< "handleCharacterStringInput(): empty string passed in" << std::endl)
		return;
	}

	TRACE(<< "handleCharacterStringInput(): utf8Chars = " << utf8Chars << std::endl)

	std::vector<char32_t> utf32;

	for(utki::utf8_iterator i(utf8Chars); !i.is_end(); ++i){
		utf32.push_back(i.character());
	}

	UnicodeProvider resolver;
	resolver.chars = std::u32string(&*utf32.begin(), utf32.size());

//    TRACE(<< "handleCharacterStringInput(): resolver.chars = " << resolver.chars << std::endl)

	mordavokne::handleCharacterInput(mordavokne::inst(), resolver, morda::key::unknown);
}

}

jint JNI_OnLoad(JavaVM* vm, void* reserved){
	TRACE(<< "JNI_OnLoad(): invoked" << std::endl)

	JNIEnv* env;
	if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
		return -1;
	}

	static JNINativeMethod methods[] = {
		{"handleCharacterStringInput", "(Ljava/lang/String;)V", (void*)&Java_io_github_igagis_mordavokne_MordaVOkneActivity_handleCharacterStringInput},
	};
	jclass clazz = env->FindClass("io/github/igagis/mordavokne/MordaVOkneActivity");
	ASSERT(clazz)
	if(env->RegisterNatives(clazz, methods, 1) < 0){
		ASSERT(false)
	}

	return JNI_VERSION_1_6;
}

namespace{
std::string initializeStorageDir(const std::string& appName){
	ASSERT(javaFunctionsWrapper)

	auto dir = javaFunctionsWrapper->getStorageDir();

	if(*dir.rend() != '/'){
		dir.append(1, '/');
	}
	return dir;
}
}

mordavokne::application::application(std::string&& name, const window_params& requestedWindowParams) :
		name(name),
		windowPimpl(std::make_unique<WindowWrapper>(requestedWindowParams)),
		gui(
				std::make_shared<morda::render_opengles2::renderer>(),
				std::make_shared<morda::updater>(),
				[this](std::function<void()>&& a){
					getImpl(getWindowPimpl(*this)).uiQueue.push_back(std::move(a));
				},
				[]() -> float{
					ASSERT(javaFunctionsWrapper)

					return javaFunctionsWrapper->getDotsPerInch();
				}(),
				[this]() -> float{
					auto res = getImpl(this->windowPimpl).getWindowSize();
					auto dim = (res.to<float>() / javaFunctionsWrapper->getDotsPerInch()) * 25.4f;
					return application::get_pixels_per_dp(res, dim.to<unsigned>());
				}()
			),
		storage_dir(initializeStorageDir(this->name))
{
	auto winSize = getImpl(this->windowPimpl).getWindowSize();
	this->updateWindowRect(morda::rectangle(morda::vector2(0), winSize.to<morda::real>()));
}

std::unique_ptr<papki::file> mordavokne::application::get_res_file(const std::string& path)const{
	return utki::make_unique<asset_file>(appInfo.assetManager, path);
}

void mordavokne::application::swapFrameBuffers(){
	auto& ww = getImpl(this->windowPimpl);
	ww.swapBuffers();
}

void mordavokne::application::set_mouse_cursor_visible(bool visible){
	// do nothing
}

void mordavokne::application::set_fullscreen(bool enable) {
	ASSERT(nativeActivity)
	if(enable) {
		ANativeActivity_setWindowFlags(nativeActivity, AWINDOW_FLAG_FULLSCREEN, 0);
	}else{
		ANativeActivity_setWindowFlags(nativeActivity, 0, AWINDOW_FLAG_FULLSCREEN);
	}
}

void mordavokne::application::quit()noexcept{
	ASSERT(nativeActivity)
	ANativeActivity_finish(nativeActivity);
}

void mordavokne::application::show_virtual_keyboard()noexcept{
	// NOTE:
	// ANativeActivity_showSoftInput(nativeActivity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED);
	// did not work for some reason.

	ASSERT(javaFunctionsWrapper)
	javaFunctionsWrapper->show_virtual_keyboard();
}

void mordavokne::application::hide_virtual_keyboard()noexcept{
	// NOTE:
	// ANativeActivity_hideSoftInput(nativeActivity, ANATIVEACTIVITY_HIDE_SOFT_INPUT_NOT_ALWAYS);
	// did not work for some reason

	ASSERT(javaFunctionsWrapper)
	javaFunctionsWrapper->hide_virtual_keyboard();
}

namespace{
void handleInputEvents(){
	auto& app = mordavokne::inst();

	// read and handle input events
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

		switch(eventType){
			case AINPUT_EVENT_TYPE_MOTION:
				switch(eventAction & AMOTION_EVENT_ACTION_MASK){
					case AMOTION_EVENT_ACTION_POINTER_DOWN:
//						TRACE(<< "Pointer down" << std::endl)
					case AMOTION_EVENT_ACTION_DOWN:
					{
						unsigned pointerIndex = ((eventAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
						unsigned pointerId = unsigned(AMotionEvent_getPointerId(event, pointerIndex));

						if(pointerId >= pointers.size()){
							TRACE(<< "Pointer ID is too big, only " << pointers.size() << " pointers supported at maximum")
							continue;
						}

//								TRACE(<< "Action down, ptr id = " << pointerId << std::endl)

						morda::vector2 p(AMotionEvent_getX(event, pointerIndex), AMotionEvent_getY(event, pointerIndex));
						pointers[pointerId] = p;

						handleMouseButton(
								app,
								true,
								AndroidWinCoordsToMordaWinRectCoords(app.window_dims(), p),
								morda::mouse_button::left,
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

						if(pointerId >= pointers.size()){
							TRACE(<< "Pointer ID is too big, only " << pointers.size() << " pointers supported at maximum")
							continue;
						}

//								TRACE(<< "Action up, ptr id = " << pointerId << std::endl)

						morda::vector2 p(AMotionEvent_getX(event, pointerIndex), AMotionEvent_getY(event, pointerIndex));
						pointers[pointerId] = p;

						handleMouseButton(
								app,
								false,
								AndroidWinCoordsToMordaWinRectCoords(app.window_dims(), p),
								morda::mouse_button::left,
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
							if(pointerId >= pointers.size()){
								TRACE(<< "Pointer ID is too big, only " << pointers.size() << " pointers supported at maximum")
								continue;
							}

							// notify root container only if there was actual movement
							morda::vector2 p(AMotionEvent_getX(event, pointerNum), AMotionEvent_getY(event, pointerNum));
							if(pointers[pointerId] == p){
								// pointer position did not change
								continue;
							}

//								TRACE(<< "Action move, ptr id = " << pointerId << std::endl)

							pointers[pointerId] = p;

							handleMouseMove(
									app,
									AndroidWinCoordsToMordaWinRectCoords(app.window_dims(), p),
									pointerId
							);
						}
					}
						break;
					default:
						TRACE(<< "unknown eventAction = " << eventAction << std::endl)
						break;
				}
				consume = true;
				break;
			case AINPUT_EVENT_TYPE_KEY:
				{
//					TRACE(<< "AINPUT_EVENT_TYPE_KEY" << std::endl)

					ASSERT(event)
					morda::key key = getKeyFromKeyEvent(*event);

					keyUnicodeResolver.kc = AKeyEvent_getKeyCode(event);
					keyUnicodeResolver.ms = AKeyEvent_getMetaState(event);
					keyUnicodeResolver.di = AInputEvent_getDeviceId(event);

//    				TRACE(<< "AINPUT_EVENT_TYPE_KEY: keyUnicodeResolver.kc = " << keyUnicodeResolver.kc << std::endl)

					switch(eventAction){
						case AKEY_EVENT_ACTION_DOWN:
//    						TRACE(<< "AKEY_EVENT_ACTION_DOWN, count = " << AKeyEvent_getRepeatCount(event) << std::endl)
							// detect auto-repeated key events
							if(AKeyEvent_getRepeatCount(event) == 0){
								handleKeyEvent(app, true, key);
							}
							handleCharacterInput(app, keyUnicodeResolver, key);
							break;
						case AKEY_EVENT_ACTION_UP:
//    						TRACE(<< "AKEY_EVENT_ACTION_UP" << std::endl)
							handleKeyEvent(app, false, key);
							break;
						case AKEY_EVENT_ACTION_MULTIPLE:
//                          TRACE(<< "AKEY_EVENT_ACTION_MULTIPLE"
//                                  << " count = " << AKeyEvent_getRepeatCount(event)
//                                  << " keyCode = " << AKeyEvent_getKeyCode(event)
//                                  << std::endl)

							// Ignore, it is handled on Java side.

							break;
						default:
							TRACE(<< "unknown AINPUT_EVENT_TYPE_KEY eventAction: " << eventAction << std::endl)
							break;
					}
				}
				break;
			default:
				break;
		}

		AInputQueue_finishEvent(
				curInputQueue,
				event,
				consume
		);
	}

	render(app);

	fdFlag.Set();
}
}

namespace{
void OnDestroy(ANativeActivity* activity){
	TRACE(<< "OnDestroy(): invoked" << std::endl)

	javaFunctionsWrapper.reset();
}

void OnStart(ANativeActivity* activity){
	TRACE(<< "OnStart(): invoked" << std::endl)
}

void OnResume(ANativeActivity* activity){
	TRACE(<< "OnResume(): invoked" << std::endl)
}

void* OnSaveInstanceState(ANativeActivity* activity, size_t* outSize){
	TRACE(<< "OnSaveInstanceState(): invoked" << std::endl)

	// Do nothing, we don't use this mechanism of saving state.

	return nullptr;
}

void OnPause(ANativeActivity* activity){
	TRACE(<< "OnPause(): invoked" << std::endl)
}

void OnStop(ANativeActivity* activity){
	TRACE(<< "OnStop(): invoked" << std::endl)
}

void OnConfigurationChanged(ANativeActivity* activity){
	TRACE(<< "OnConfigurationChanged(): invoked" << std::endl)

	int32_t diff;
	{
		std::unique_ptr<AndroidConfiguration> config = AndroidConfiguration::New();
		AConfiguration_fromAssetManager(config->ac, appInfo.assetManager);

		diff = AConfiguration_diff(curConfig->ac, config->ac);

		curConfig = std::move(config);
	}

	// if orientation has changed
	if(diff & ACONFIGURATION_ORIENTATION){
		int32_t orientation = AConfiguration_getOrientation(curConfig->ac);
		switch(orientation){
			case ACONFIGURATION_ORIENTATION_LAND:
			case ACONFIGURATION_ORIENTATION_PORT:
				std::swap(curWinDim.x, curWinDim.y);
				break;
			case ACONFIGURATION_ORIENTATION_SQUARE:
				// do nothing
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
//    static_cast<morda::application*>(activity->instance)->OnLowMemory();
}

void OnWindowFocusChanged(ANativeActivity* activity, int hasFocus){
	TRACE(<< "OnWindowFocusChanged(): invoked" << std::endl)
}

int OnUpdateTimerExpired(int fd, int events, void* data){
//	TRACE(<< "OnUpdateTimerExpired(): invoked" << std::endl)

	auto& app = application::inst();

	std::uint32_t dt = app.gui.update();
	if(dt == 0){
		// do not arm the timer and do not clear the flag
	}else{
		fdFlag.Clear();
		timer.Arm(dt);
	}

	// after updating need to re-render everything
	render(app);

//	TRACE(<< "OnUpdateTimerExpired(): armed timer for " << dt << std::endl)

	return 1; // 1 means do not remove descriptor from looper
}

int OnQueueHasMessages(int fd, int events, void* data){
	auto& ww = getImpl(getWindowPimpl(application::inst()));

	while(auto m = ww.uiQueue.pop_front()){
		m();
	}

	return 1; // 1 means do not remove descriptor from looper
}

void OnNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window){
	TRACE(<< "OnNativeWindowCreated(): invoked" << std::endl)

	// save window in a static var, so it is accessible for OpenGL initializers from morda::application class
	androidWindow = window;

	curWinDim.x = float(ANativeWindow_getWidth(window));
	curWinDim.y = float(ANativeWindow_getHeight(window));

	ASSERT(!activity->instance)
	try{
		// use local auto-pointer for now because an exception can be thrown and need to delete object then.
		std::unique_ptr<AndroidConfiguration> cfg = AndroidConfiguration::New();
		// retrieve current configuration
		AConfiguration_fromAssetManager(cfg->ac, appInfo.assetManager);

		application* app = mordavokne::create_application(0, nullptr).release();

		activity->instance = app;

		// save current configuration in global variable
		curConfig = std::move(cfg);

		ALooper* looper = ALooper_prepare(0);
		ASSERT(looper)

		// add timer descriptor to looper, this is needed for updatable to work
		if(ALooper_addFd(
				looper,
				fdFlag.GetFD(),
				ALOOPER_POLL_CALLBACK,
				ALOOPER_EVENT_INPUT,
				&OnUpdateTimerExpired,
				0
			) == -1)
		{
			throw std::runtime_error("failed to add timer descriptor to looper");
		}

		// add UI message queue descriptor to looper
		if(ALooper_addFd(
				looper,
				getImpl(getWindowPimpl(*app)).uiQueue.get_handle(),
				ALOOPER_POLL_CALLBACK,
				ALOOPER_EVENT_INPUT,
				&OnQueueHasMessages,
				0
			) == -1)
		{
			throw std::runtime_error("failed to add UI message queue descriptor to looper");
		}

		fdFlag.Set(); // this is to call the Update() for the first time if there were any updateables started during creating application object
	}catch(std::exception& e){
		TRACE(<< "std::exception uncaught while creating application instance: " << e.what() << std::endl)
		throw;
	}catch(...){
		TRACE(<< "unknown exception uncaught while creating application instance!" << std::endl)
		throw;
	}
}

void OnNativeWindowResized(ANativeActivity* activity, ANativeWindow* window){
	TRACE(<< "OnNativeWindowResized(): invoked" << std::endl)

	// save window dimensions
	curWinDim.x = float(ANativeWindow_getWidth(window));
	curWinDim.y = float(ANativeWindow_getHeight(window));

	TRACE(<< "OnNativeWindowResized(): curWinDim = " << curWinDim << std::endl)
}

void OnNativeWindowRedrawNeeded(ANativeActivity* activity, ANativeWindow* window){
	TRACE(<< "OnNativeWindowRedrawNeeded(): invoked" << std::endl)

	render(getApp(activity));
}

// This function is called right before destroying Window object, according to documentation:
// https://developer.android.com/ndk/reference/struct/a-native-activity-callbacks#onnativewindowdestroyed
void OnNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window){
	TRACE(<< "OnNativeWindowDestroyed(): invoked" << std::endl)

	ALooper* looper = ALooper_prepare(0);
	ASSERT(looper)

	// remove UI message queue descriptor from looper
	ALooper_removeFd(
			looper,
			getImpl(getWindowPimpl(application::inst())).uiQueue.get_handle()
		);

	// remove fdFlag from looper
	ALooper_removeFd(looper, fdFlag.GetFD());

	// Need to destroy app right before window is destroyed, i.e. before OGL is de-initialized
	delete static_cast<mordavokne::application*>(activity->instance);
	activity->instance = nullptr;

	// delete configuration object
	curConfig.reset();
}

int OnInputEventsReadyForReadingFromQueue(int fd, int events, void* data){
//	TRACE(<< "OnInputEventsReadyForReadingFromQueue(): invoked" << std::endl)

	ASSERT(curInputQueue) // if we get events we should have input queue

	// if window is not created yet, ignore events
	if(!mordavokne::application::isCreated()){
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

	ASSERT(mordavokne::application::isCreated())

	handleInputEvents();

	return 1; // we don't want to remove input queue descriptor from looper
}

void OnInputQueueCreated(ANativeActivity* activity, AInputQueue* queue){
	TRACE(<< "OnInputQueueCreated(): invoked" << std::endl)
	ASSERT(queue);
	ASSERT(!curInputQueue)
	curInputQueue = queue;

	// attach queue to looper
	AInputQueue_attachLooper(
			curInputQueue,
			ALooper_prepare(0), // get looper for current thread (main thread)
			0, // 'ident' is ignored since we are using callback
			&OnInputEventsReadyForReadingFromQueue,
			activity->instance
		);
}

void OnInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue){
	TRACE(<< "OnInputQueueDestroyed(): invoked" << std::endl)
	ASSERT(queue)
	ASSERT(curInputQueue == queue)

	// detach queue from looper
	AInputQueue_detachLooper(queue);

	curInputQueue = 0;
}

// called when, for example, on-screen keyboard has been shown
void OnContentRectChanged(ANativeActivity* activity, const ARect* rect){
	TRACE(<< "OnContentRectChanged(): invoked, left = " << rect->left << " right = " << rect->right << " top = " << rect->top << " bottom = " << rect->bottom << std::endl)
	TRACE(<< "OnContentRectChanged(): curWinDim = " << curWinDim << std::endl)

	// Sometimes Android calls OnContentRectChanged() even after native window was destroyed,
	// i.e. OnNativeWindowDestroyed() was called and, thus, application object was destroyed.
	// So need to check if our application is still alive.
	if(!activity->instance){
		TRACE(<< "OnContentRectChanged(): application is not alive, ignoring content rect change." << std::endl)
		return;
	}

	auto& app = getApp(activity);

	updateWindowRect(
			app,
			morda::rectangle(
					float(rect->left),
					curWinDim.y - float(rect->bottom),
					float(rect->right - rect->left),
					float(rect->bottom - rect->top)
				)
		);

	// redraw, since WindowRedrawNeeded not always comes
	render(app);
}
}

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

	javaFunctionsWrapper = std::make_unique<JavaFunctionsWrapper>(activity);
}

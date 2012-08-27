//This file contains implementations of platform dependent methods from App class.


#include "../App.hpp"


#include <android/native_activity.h>
#include <ting/Array.hpp>


using namespace morda;



namespace{

ANativeWindow* androidWindow = 0;

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

}//~namespace



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



App::EGLConfigWrapper::EGLConfigWrapper(EGLDisplayWrapper& d){
	//TODO: allow stencil configuration etc.
	//Here specify the attributes of the desired configuration.
	//Below, we select an EGLConfig with at least 8 bits per color
	//component compatible with on-screen windows
	const EGLint attribs[] = {
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
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
	this->c = eglCreateContext(d.d, config.c, NULL, NULL);
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



App::App(unsigned w, unsigned h) :
		eglConfig(eglDisplay),
		eglSurface(eglDisplay, eglConfig),
		eglContext(eglDisplay, eglConfig, eglSurface)
{
	EGLint width, height;
	eglQuerySurface(eglDisplay.d, eglSurface.s, EGL_WIDTH, &width);
	eglQuerySurface(eglDisplay.d, eglSurface.s, EGL_HEIGHT, &height);
	
	this->UpdateWindowDimensions(tride::Vec2f(float(width), float(height)));
}



inline void UpdateWindowDimensions(App* app, const tride::Vec2f& newWinDim){
	app->UpdateWindowDimensions(newWinDim);
}



inline void Render(App* app){
	app->Render();
}



}//~namespace



namespace{



void OnDestroy(ANativeActivity* activity){
}



void OnStart(ANativeActivity* activity){
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnStart();
}



void OnResume(ANativeActivity* activity){
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnResume();
}



void* OnSaveInstanceState(ANativeActivity* activity, size_t* outSize){
	//TODO:
//    return static_cast<morda::App*>(activity->instance)->OnSaveInstanceState(outSize);
}



void OnPause(ANativeActivity* activity){
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnPause();
}



void OnStop(ANativeActivity* activity){
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnStop();
}



void OnConfigurationChanged(ANativeActivity* activity){
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnConfigurationChanged();
}



void OnLowMemory(ANativeActivity* activity){
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnLowMemory();
}



void OnWindowFocusChanged(ANativeActivity* activity, int hasFocus){
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnWindowFocusChanged(bool(hasFocus));
}



void OnNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window){
    TRACE(<< "OnNativeWindowCreated(): invoked" << std::endl)
	
	//save window in a static var, so it is accessible for OGL initializers from morda::App class
	androidWindow = window;
	
	ASSERT(!activity->instance)
	activity->instance = morda::CreateApp(0, 0, appInfo.savedState).Extract();;
}



void OnNativeWindowResized(ANativeActivity* activity, ANativeWindow* window){
//    TRACE(<< "OnNativeWindowResized(): invoked" << std::endl)
	
	morda::UpdateWindowDimensions(
			static_cast<morda::App*>(activity->instance),
			tride::Vec2f(
					float(ANativeWindow_getWidth(window)),
					float(ANativeWindow_getHeight(window))
				)
		);
}



void OnNativeWindowRedrawNeeded(ANativeActivity* activity, ANativeWindow* window){
//    TRACE(<< "OnNativeWindowRedrawNeeded(): invoked" << std::endl)
	
	morda::Render(static_cast<morda::App*>(activity->instance));
}



void OnNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window){
//    TRACE(<< "OnNativeWindowDestroyed(): invoked" << std::endl)
	
	//TODO: need to destroy app right before window is destroyed, i.e. OGL de-initialized
	//destroy app object
	delete static_cast<morda::App*>(activity->instance);
	activity->instance = 0;
}



AInputQueue* curInputQueue = 0;



int OnInputEventsReadyForReadingFromQueue(int fd, int events, void* data){
	//TODO:
//    Activity& a = *static_cast<Activity*>(data);
//
////    TRACE(<< "OnInputEventsReadyForReadingFromQueue(): invoked" << std::endl)
//
//    ASSERT(curInputQueue) //if we get events we should have input queue
//
//    //Read and handle input events
//    AInputEvent* event;
//    while(AInputQueue_getEvent(curInputQueue, &event) >= 0){
////        TRACE(<< "New input event: type = " << AInputEvent_getType(event) << std::endl);
//        if(AInputQueue_preDispatchEvent(curInputQueue, event)){
//            continue;
//        }
//
//        AInputQueue_finishEvent(
//                curInputQueue,
//                event,
//                a.OnInput(event)
//            );
//    }

	return 1; //we don't want to remove input queue descriptor from looper
}



void OnInputQueueCreated(ANativeActivity* activity, AInputQueue* queue){
//	TRACE(<< "OnInputQueueCreated(): invoked" << std::endl)
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
//	TRACE(<< "OnInputQueueDestroyed(): invoked" << std::endl)
	ASSERT(queue)
	ASSERT(curInputQueue == queue)

	//detach queue from looper
	AInputQueue_detachLooper(queue);

	curInputQueue = 0;
}



void OnContentRectChanged(ANativeActivity* activity, const ARect* rect){
//	TRACE(<< "OnContentRectChanged(): invoked" << std::endl)
	//TODO:
}



}//~namespace



void ANativeActivity_onCreate(
		ANativeActivity* activity,
		void* savedState,
		size_t savedStateSize
	)
{
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

	appInfo.internalDataPath = activity->internalDataPath;
	appInfo.externalDataPath = activity->externalDataPath;
	appInfo.assetManager = activity->assetManager;
	if(savedState){
		//copy saved state data because no guarantee that the data will be kept alive after returning from this function
		appInfo.savedState.Init(savedStateSize);
		memcpy(appInfo.savedState.Begin(), savedState, savedStateSize);
	}

//	ANativeActivity_setWindowFlags(activity, 1024, 1024); //set fullscreen flag
}

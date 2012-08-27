//This file contains implementations of platform dependent methods from App class.


#include "../App.hpp"


#include <android/native_activity.h>
#include <ting/Array.hpp>


using namespace morda;



namespace morda{


inline void SetEGLStuff(App* app, EGLDisplay display, EGLContext context, EGLSurface surface){
	app->eglDisplay = display;
	app->eglContext = context;
	app->eglSurface = surface;
}



inline void UpdateWindowDimensions(App* app, const tride::Vec2f& newWinDim){
	app->UpdateWindowDimensions(newWinDim);
}



}//~namespace



namespace{

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



void OnDestroy(ANativeActivity* activity){
	delete static_cast<morda::App*>(activity->instance);
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
	
	//initialize OpenGL ES and EGL

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(display, 0, 0);
	
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
	EGLConfig config;
	EGLint numConfigs;
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	//EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	//guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	//As soon as we picked a EGLConfig, we can safely reconfigure the
	//ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
	EGLint format;
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(window, 0, 0, format);

	EGLSurface surface = eglCreateWindowSurface(display, config, window, NULL);
	EGLContext context = eglCreateContext(display, config, NULL, NULL);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		TRACE(<< "eglMakeCurrent(): failed" << std::endl)
		return;
	}

	EGLint w, h;
	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	morda::App* app = static_cast<morda::App*>(activity->instance);
	
	SetEGLStuff(app, display, context, surface);
	UpdateWindowDimensions(app, tride::Vec2f(w, h));

	//call Init() after OGL is initialized
	app->Init(appInfo.savedState);
}



void OnNativeWindowResized(ANativeActivity* activity, ANativeWindow* window){
//    TRACE(<< "OnNativeWindowResized(): invoked" << std::endl)
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnNativeWindowResized(window);
}



void OnNativeWindowRedrawNeeded(ANativeActivity* activity, ANativeWindow* window){
//    TRACE(<< "OnNativeWindowRedrawNeeded(): invoked" << std::endl)
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnNativeWindowRedrawNeeded(window);
}



void OnNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window){
//    TRACE(<< "OnNativeWindowDestroyed(): invoked" << std::endl)

	if(this->display != EGL_NO_DISPLAY){
		eglMakeCurrent(this->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if(this->context != EGL_NO_CONTEXT){
			eglDestroyContext(this->display, this->context);
		}
		if(this->surface != EGL_NO_SURFACE){
			eglDestroySurface(this->display, this->surface);
		}
		eglTerminate(this->display);
	}

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

	morda::App* app = morda::CreateApp(0, 0).Extract();
	ASSERT(app)
	activity->instance = app;

	appInfo.internalDataPath = activity->internalDataPath;
	appInfo.externalDataPath = activity->externalDataPath;
	appInfo.assetManager = activity->assetManager;
	if(savedState){
		//copy saved state data because no guarantee that the data will be kept alive after returning from this function
		appInfo.savedState.Init(savedStateSize);
		memcpy(appInfo.savedState.Begin(), savedState, savedStateSize);
	}

	ANativeActivity_setWindowFlags(activity, 1024, 1024); //set fullscreen flag
}

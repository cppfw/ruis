//This file contains implementations of platform dependent methods from App class.


#include "../App.hpp"


#include <android/native_activity.h>

#include <ting/Array.hpp>

#include "AssetFile.hpp"



using namespace morda;



namespace{

ANativeWindow* androidWindow = 0;

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



ting::Ptr<ting::fs::File> App::CreateResourceFileInterface(const std::string& path)const{
	return AssetFile::New(appInfo.assetManager, path);
}



inline void UpdateWindowDimensions(App* app, const tride::Vec2f& newWinDim){
	app->UpdateWindowDimensions(newWinDim);
}



inline void Render(App* app){
	app->Render();
}



void HandleInputEvents(){
	morda::App& app = morda::App::Inst();
	
	//Read and handle input events
	AInputEvent* event;
	while(AInputQueue_getEvent(curInputQueue, &event) >= 0){
		TRACE(<< "New input event: type = " << AInputEvent_getType(event) << std::endl)
		if(AInputQueue_preDispatchEvent(curInputQueue, event)){
			continue;
		}

		int32_t eventType = AInputEvent_getType(event);
		int32_t eventAction = AMotionEvent_getAction(event);
		
		bool consume = false;
		
		switch(eventType){
			case AINPUT_EVENT_TYPE_MOTION:	
				switch(eventAction & AMOTION_EVENT_ACTION_MASK){
					case AMOTION_EVENT_ACTION_DOWN:
						{
							size_t numPointers = AMotionEvent_getPointerCount(event);
							ASSERT(numPointers >= 1)
							for(size_t pointerNum = 0; pointerNum < numPointers; ++pointerNum){
								TRACE(<< "Action down" << std::endl)
								if(app.rootContainer.IsValid()){
									app.rootContainer->OnMouseButtonDown(
											tride::Vec2f(AMotionEvent_getX(event, pointerNum), app.curWinDim.y - AMotionEvent_getY(event, pointerNum) - 1.0f),
											morda::Widget::LEFT,
											pointerNum
										);
								}
							}//~for(every pointer)
						}
						break;
					case AMOTION_EVENT_ACTION_UP:
						TRACE(<< "Action up" << std::endl)
						{
							size_t numPointers = AMotionEvent_getPointerCount(event);
							ASSERT(numPointers >= 1)
							for(size_t pointerNum = 0; pointerNum < numPointers; ++pointerNum){
								TRACE(<< "Action down" << std::endl)
								if(app.rootContainer.IsValid()){
									app.rootContainer->OnMouseButtonUp(
											tride::Vec2f(AMotionEvent_getX(event, pointerNum), app.curWinDim.y - AMotionEvent_getY(event, pointerNum) - 1.0f),
											morda::Widget::LEFT,
											pointerNum
										);
								}
							}//~for(every pointer)
						}
						break;
					case AMOTION_EVENT_ACTION_MOVE:
						TRACE(<< "Action move" << std::endl)
						{
							size_t numPointers = AMotionEvent_getPointerCount(event);
							ASSERT(numPointers >= 1)
							for(size_t pointerNum = 0; pointerNum < numPointers; ++pointerNum){
								TRACE(<< "Action down" << std::endl)
								if(app.rootContainer.IsValid()){
									app.rootContainer->OnMouseMove(
											tride::Vec2f(AMotionEvent_getX(event, pointerNum), app.curWinDim.y - AMotionEvent_getY(event, pointerNum) - 1.0f),
											pointerNum
										);
								}
							}//~for(every pointer)
						}
						break;
					case AMOTION_EVENT_ACTION_POINTER_DOWN:
						TRACE(<< "Pointer down" << std::endl)
						//TODO:
						break;
					case AMOTION_EVENT_ACTION_POINTER_UP:
						TRACE(<< "Pointer up" << std::endl)
						//TODO:
						break;
					default:
						TRACE(<< "unknown eventAction" << std::endl)
						break;
				}//~switch(event action)
				consume = true;
				break;
			case AINPUT_EVENT_TYPE_KEY:
				//TODO:
				break;
			default:
				break;
		}//~switch(event type)
		
		AInputQueue_finishEvent(
				curInputQueue,
				event,
				consume
			);
	}//~while()
	
	app.Render();
}



}//~namespace



namespace{



void OnDestroy(ANativeActivity* activity){
	TRACE(<< "OnDestroy(): invoked" << std::endl)
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
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnConfigurationChanged();
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



void OnNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window){
	TRACE(<< "OnNativeWindowCreated(): invoked" << std::endl)
	
	//save window in a static var, so it is accessible for OGL initializers from morda::App class
	androidWindow = window;
	
	ASSERT(!activity->instance)
	try{
		activity->instance = morda::CreateApp(0, 0, appInfo.savedState).Extract();
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
	
	morda::UpdateWindowDimensions(
			static_cast<morda::App*>(activity->instance),
			tride::Vec2f(
					float(ANativeWindow_getWidth(window)),
					float(ANativeWindow_getHeight(window))
				)
		);
}



void OnNativeWindowRedrawNeeded(ANativeActivity* activity, ANativeWindow* window){
	TRACE(<< "OnNativeWindowRedrawNeeded(): invoked" << std::endl)
	
	morda::Render(static_cast<morda::App*>(activity->instance));
}



void OnNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window){
	TRACE(<< "OnNativeWindowDestroyed(): invoked" << std::endl)
	
	//TODO: need to destroy app right before window is destroyed, i.e. OGL de-initialized
	//destroy app object
	delete static_cast<morda::App*>(activity->instance);
	activity->instance = 0;
}



int OnInputEventsReadyForReadingFromQueue(int fd, int events, void* data){
	TRACE(<< "OnInputEventsReadyForReadingFromQueue(): invoked" << std::endl)

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



void OnContentRectChanged(ANativeActivity* activity, const ARect* rect){
	TRACE(<< "OnContentRectChanged(): invoked" << std::endl)
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

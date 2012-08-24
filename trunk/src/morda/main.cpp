#include "App.hpp"


#include <ting/Array.hpp>



#if M_OS == M_OS_LINUX

#	ifdef __ANDROID__

#	include <android/native_activity.h>
#include <string.h>

namespace{

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
//    TRACE(<< "OnNativeWindowCreated(): invoked" << std::endl)
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnNativeWindowCreated(window);
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
	//TODO:
//    static_cast<morda::App*>(activity->instance)->OnNativeWindowDestroyed(window);
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

	activity->instance = morda::CreateApp(0, 0).Extract();
	ASSERT(activity->instance)

	morda::App* app = static_cast<morda::App*>(activity->instance);

	appInfo.internalDataPath = activity->internalDataPath;
	appInfo.externalDataPath = activity->externalDataPath;
	appInfo.assetManager = activity->assetManager;
	
	if(savedState){
		//copy saved state data because no guarantee that the data will be kept alive after returning from this function
		appInfo.savedState.Init(savedStateSize);
		memcpy(appInfo.savedState.Begin(), savedState, savedStateSize);
	}

	ANativeActivity_setWindowFlags(activity, 1024, 1024); //set fullscreen flag

	//TODO: call it after OGL is initialized
	app->Init(ting::Buffer<const ting::u8>(savedState, savedStateSize));
}

#	else //generic linux

namespace morda{
inline void Main(int argc, char** argv){
	ting::Ptr<morda::App> app = morda::CreateApp(argc, argv);

	app->Init();

	app->Exec();
}
}//~namespace


int main(int argc, char** argv){
	morda::Main(argc, argv);

	return 0;
}
#	endif

#else
#	error "unsupported OS"
#endif

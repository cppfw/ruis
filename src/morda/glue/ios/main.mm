//
//  main.m
//  morda_test
//
//  Created by Ivan on 28/11/15.
//  Copyright (c) 2015 org.igagis. All rights reserved.
//

#include "../../App.hpp"
#include "../../AppFactory.hpp"


#import <UIKit/UIKit.h>

#include "ViewController.h"


@interface AppDelegate : UIResponder <UIApplicationDelegate>{
	morda::App* app;
}

@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	self->app = morda::createApp(0, nullptr, utki::wrapBuf<std::uint8_t>(nullptr, 0)).release();
	
	return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
	// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
	ASSERT(self->app)
	delete self->app;
}

@end



int main(int argc, char * argv[]){
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	int retVal = UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
	[pool release];
	return retVal;
}



void morda::App::postToUiThread_ts(std::function<void()>&& f){
	//TODO:
	/*
	NSEvent* e = [NSEvent
				  otherEventWithType: NSApplicationDefined
				  location: NSMakePoint(0, 0)
				  modifierFlags:0
				  timestamp:0
				  windowNumber:0
				  context: nil
				  subtype: 0
				  data1: reinterpret_cast<NSInteger>(new std::function<void()>(std::move(f)))
				  data2: 0
				  ];
	
	NSApplication *applicationObject = (NSApplication*)this->applicationObject.id;
	[applicationObject postEvent:e atStart:NO];
	*/
}



void morda::App::setFullscreen(bool enable){
	//TODO:
}


void morda::App::quit()noexcept{
	//TODO:
}


morda::App::App(const morda::App::WindowParams& wp) :
		windowObject(wp)
{
	//TODO:
}


morda::App::DotsPerInchWrapper::DotsPerInchWrapper(){
	//TODO:
}


morda::App::WindowObject::WindowObject(const morda::App::WindowParams& wp){
	UIWindow *window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	
	if(!window){
		throw morda::Exc("failed to create a UIWindow");
	}
	
	this->id = window;
	
	window.screen = [UIScreen mainScreen];
	
	window.backgroundColor = [UIColor redColor];
	window.rootViewController = [[ViewController alloc] init];
	
	[window makeKeyAndVisible];
}

morda::App::WindowObject::~WindowObject()noexcept{
	UIWindow* window = (UIWindow*)this->id;
	[window release];
}


void morda::App::showVirtualKeyboard()noexcept{
	//TODO:
}

void morda::App::hideVirtualKeyboard()noexcept{
	//TODO:
}

std::unique_ptr<papki::File> morda::App::createResourceFileInterface(const std::string& path)const{
	auto dir = [[[NSBundle mainBundle] resourcePath] fileSystemRepresentation];
	
	TRACE(<< "res path = " << dir << std::endl)
	
	//TODO;
	return nullptr;
}

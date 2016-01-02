//
//  main.m
//  morda_test
//
//  Created by Ivan on 28/11/15.
//  Copyright (c) 2015 org.igagis. All rights reserved.
//

#include "../../App.hpp"
#include "../../AppFactory.hpp"

#include <papki/FSFile.hpp>

#include <sstream>

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>


namespace morda{
	
	void ios_render(){
		morda::App::inst().render();
	}
	
	std::uint32_t ios_update(){
		return morda::App::inst().updater.update();
	}
	
	void ios_updateWindowRect(morda::Vec2r dim){
		morda::App::inst().updateWindowRect(
											morda::Rectr(
														 morda::Vec2r(0),
														 dim
														 )
											);
	}
	
	void ios_handleMouseMove(const morda::Vec2r& pos, unsigned id){
		morda::App::inst().handleMouseMove(
										   morda::Vec2r(pos.x, pos.y),
										   id
										   );
	}
	
	void ios_handleMouseButton(bool isDown, const morda::Vec2r& pos, morda::Widget::EMouseButton button, unsigned id){
//		TRACE(<< "mouse pos = " << morda::Vec2r(pos.x, pos.y) << std::endl)
		morda::App::inst().handleMouseButton(
											 isDown,
											 morda::Vec2r(pos.x, pos.y),
											 button,
											 id
											 );
	}
}


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
	auto p = reinterpret_cast<NSInteger>(new std::function<void()>(std::move(f)));
	
	dispatch_async(dispatch_get_main_queue(), ^{
		std::unique_ptr<std::function<void()>> m(reinterpret_cast<std::function<void()>*>(p));
		(*m)();
	});
}



void morda::App::setFullscreen(bool enable){
	UIWindow* w = (UIWindow*)this->windowObject.id;
	
	float scale = [[UIScreen mainScreen] scale];
	
	if(enable){
		if( [[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0f ) {
			CGRect rect = w.frame;
			w.rootViewController.view.frame = rect;
		}
		morda::ios_updateWindowRect(morda::Vec2r(
												 std::round(w.frame.size.width * scale),
												 std::round(w.frame.size.height * scale)
			));
		w.windowLevel = UIWindowLevelStatusBar;
	}else{
		CGSize statusBarSize = [[UIApplication sharedApplication] statusBarFrame].size;
		
		if( [[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0f ) {
				CGRect rect = w.frame;
				rect.origin.y += statusBarSize.height;
				rect.size.height -= statusBarSize.height;
				w.rootViewController.view.frame = rect;
		}
		
		morda::ios_updateWindowRect(morda::Vec2r(
												 std::round(w.frame.size.width * scale),
												 std::round((w.frame.size.height - statusBarSize.height) * scale)
			));
		w.windowLevel = UIWindowLevelNormal;
	}
}


void morda::App::quit()noexcept{
	//TODO:
}


namespace{

morda::real getDotsPerInch(){
	float scale = [[UIScreen mainScreen] scale];

	morda::real value;
	
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
		value = 132 * scale;
	} else if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) {
		value = 163 * scale;
	} else {
		value = 160 * scale;
	}
	TRACE(<< "dpi = " << value << std::endl)
	return value;
}

morda::real getDotsPerPt(){
	float scale = [[UIScreen mainScreen] scale];

	return morda::real(320.0f * scale) / morda::screenSizePt;
}

}//~namespace

morda::App::App(const morda::App::WindowParams& wp) :
		windowObject(wp),
		units(getDotsPerInch(), getDotsPerPt())
{
	this->setFullscreen(false);//this will intialize the viewport
}



@interface ViewController : GLKViewController{

}

@property (strong, nonatomic) EAGLContext *context;
@end

@implementation ViewController

- (void)viewDidLoad{
	[super viewDidLoad];
	
	self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
	if (self.context == nil) {
		self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	}
	
	if (!self.context) {
		NSLog(@"Failed to create ES context");
	}
	
	GLKView *view = (GLKView *)self.view;
	view.context = self.context;
	view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
	
	[EAGLContext setCurrentContext:self.context];
	
	view.multipleTouchEnabled = YES;
	
	if ([self respondsToSelector:@selector(edgesForExtendedLayout)])
 {
	 self.edgesForExtendedLayout = UIRectEdgeNone;
 }
}

- (void)dealloc{
	[super dealloc];
	
	if ([EAGLContext currentContext] == self.context) {
		[EAGLContext setCurrentContext:nil];
	}
	[self.context release];
}

- (void)didReceiveMemoryWarning{
	// Dispose of any resources that can be recreated.
}


- (void)update{
	//TODO: adapt to nothingto update, lower frame rate
	morda::ios_update();
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect{
	morda::ios_render();
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event{
	float scale = [UIScreen mainScreen].scale;
	
	for(UITouch * touch in touches ){
		CGPoint p = [touch locationInView:self.view ];
		
//		TRACE(<< "touch began = " << morda::Vec2r(p.x * scale, p.y * scale).rounded() << std::endl)
		morda::ios_handleMouseButton(
				true,
				morda::Vec2r(p.x * scale, p.y * scale).rounded(),
				morda::Widget::EMouseButton::LEFT,
				0//TODO: id
			);
	}
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event{
	float scale = [UIScreen mainScreen].scale;
	
	for(UITouch * touch in touches ){
		CGPoint p = [touch locationInView:self.view ];
		
//		TRACE(<< "touch moved = " << morda::Vec2r(p.x * scale, p.y * scale).rounded() << std::endl)
		morda::ios_handleMouseMove(
									 morda::Vec2r(p.x * scale, p.y * scale).rounded(),
									 0 //TODO: id
			);
	}
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event{
	float scale = [UIScreen mainScreen].scale;
	
	for(UITouch * touch in touches ){
		CGPoint p = [touch locationInView:self.view ];
		
//		TRACE(<< "touch ended = " << morda::Vec2r(p.x * scale, p.y * scale).rounded() << std::endl)
		morda::ios_handleMouseButton(
									 false,
									 morda::Vec2r(p.x * scale, p.y * scale).rounded(),
									 morda::Widget::EMouseButton::LEFT,
									 0//TODO: id
			);
	}
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event{
	//TODO:
}

@end












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
	std::string dir([[[NSBundle mainBundle] resourcePath] fileSystemRepresentation]);
	
//	TRACE(<< "res path = " << dir << std::endl)

	std::stringstream ss;
	ss << dir << "/" << path;
	
	return utki::makeUnique<papki::FSFile>(ss.str());
}

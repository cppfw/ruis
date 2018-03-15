#include "../../App.hpp"
#include "../../AppFactory.hpp"

#include <papki/FSFile.hpp>
#include <papki/RootDirFile.hpp>

#include <sstream>

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include <mordaren/OpenGLES2Renderer.hpp>


using namespace mordavokne;

#include "../createAppUnix.cppinc"
#include "../friendAccessors.cppinc"



@interface AppDelegate : UIResponder <UIApplicationDelegate>{
	App* app;
}

@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	self->app = createAppUnix(0, nullptr, utki::Buf<std::uint8_t>()).release();
	
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


@interface ViewController : GLKViewController{

}

@property (strong, nonatomic) EAGLContext *context;

@end





namespace{
	App::WindowParams windowParams(0);
	
	struct WindowWrapper : public utki::Unique{
		UIWindow *window;
		
		WindowWrapper(const App::WindowParams& wp){
			windowParams = wp;
			
			this->window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
			
			if(!this->window){
				throw morda::Exc("failed to create a UIWindow");
			}
			
			utki::ScopeExit scopeExitWindow([this](){
				[this->window release];
			});
			
			
			this->window.screen = [UIScreen mainScreen];
			
			this->window.backgroundColor = [UIColor redColor];
			this->window.rootViewController = [[ViewController alloc] init];
			
			[this->window makeKeyAndVisible];
			
			scopeExitWindow.reset();
		}
		~WindowWrapper()noexcept{
			[this->window release];
		}
	};
	
	WindowWrapper& getImpl(const std::unique_ptr<utki::Unique>& pimpl){
		ASSERT(pimpl)
		ASSERT(dynamic_cast<WindowWrapper*>(pimpl.get()))
		return static_cast<WindowWrapper&>(*pimpl);
	}
}






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
	view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;

	{
		const App::WindowParams& wp = windowParams;
		if(wp.buffers.get(App::WindowParams::Buffer_e::DEPTH)){
			view.drawableDepthFormat = GLKViewDrawableDepthFormat16;
		}else{
			view.drawableDepthFormat = GLKViewDrawableDepthFormatNone;
		}
		if(wp.buffers.get(App::WindowParams::Buffer_e::STENCIL)){
			view.drawableStencilFormat = GLKViewDrawableStencilFormat8;
		}else{
			view.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
		}
	}
	
	[EAGLContext setCurrentContext:self.context];
	
	view.multipleTouchEnabled = YES;
	
	if([self respondsToSelector:@selector(edgesForExtendedLayout)]){
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
	//TODO: adapt to nothing to update, lower frame rate
	mordavokne::inst().gui.update();
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect{
	render(mordavokne::inst());
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event{
	float scale = [UIScreen mainScreen].scale;
	
	for(UITouch * touch in touches ){
		CGPoint p = [touch locationInView:self.view ];
		
//		TRACE(<< "touch began = " << morda::Vec2r(p.x * scale, p.y * scale).rounded() << std::endl)
		handleMouseButton(
				mordavokne::inst(),
				true,
				morda::Vec2r(p.x * scale, p.y * scale).rounded(),
				morda::MouseButton_e::LEFT,
				0 //TODO: id
			);
	}
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event{
	float scale = [UIScreen mainScreen].scale;
	
	for(UITouch * touch in touches ){
		CGPoint p = [touch locationInView:self.view ];
		
//		TRACE(<< "touch moved = " << morda::Vec2r(p.x * scale, p.y * scale).rounded() << std::endl)
		handleMouseMove(
				mordavokne::inst(),
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
		handleMouseButton(
				mordavokne::inst(),
				false,
				morda::Vec2r(p.x * scale, p.y * scale).rounded(),
				morda::MouseButton_e::LEFT,
				0//TODO: id
			);
	}
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event{
	//TODO:
}

@end





void App::setFullscreen(bool enable){
	auto& ww = getImpl(this->windowPimpl);
	UIWindow* w = ww.window;
	
	float scale = [[UIScreen mainScreen] scale];
	
	if(enable){
		if( [[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0f ) {
			CGRect rect = w.frame;
			w.rootViewController.view.frame = rect;
		}
		updateWindowRect(
						 morda::Rectr(
									  morda::Vec2r(0),
									  morda::Vec2r(
												   std::round(w.frame.size.width * scale),
												   std::round(w.frame.size.height * scale)
												   )
									  )
						 );
		w.windowLevel = UIWindowLevelStatusBar;
	}else{
		CGSize statusBarSize = [[UIApplication sharedApplication] statusBarFrame].size;
		
		if( [[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0f ) {
			CGRect rect = w.frame;
			rect.origin.y += statusBarSize.height;
			rect.size.height -= statusBarSize.height;
			w.rootViewController.view.frame = rect;
		}
		
		updateWindowRect(
						 morda::Rectr(
									  morda::Vec2r(0),
									  morda::Vec2r(
												   std::round(w.frame.size.width * scale),
												   std::round((w.frame.size.height - statusBarSize.height) * scale)
												   )
									  )
						 );
		w.windowLevel = UIWindowLevelNormal;
	}
}


void App::quit()noexcept{
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
		
		//TODO: use findDotsPerPt() function from morda util
		
		return morda::real(scale);
	}
	
}//~namespace

App::App(const App::WindowParams& wp) :
		windowPimpl(utki::makeUnique<WindowWrapper>(wp)),
		gui(
				std::make_shared<mordaren::OpenGLES2Renderer>(),
				getDotsPerInch(),
				getDotsPerPt(),
				[this](std::function<void()>&& a){
					auto p = reinterpret_cast<NSInteger>(new std::function<void()>(std::move(a)));
	
					dispatch_async(dispatch_get_main_queue(), ^{
						std::unique_ptr<std::function<void()>> m(reinterpret_cast<std::function<void()>*>(p));
						(*m)();
					});
				}
			)
{
	this->setFullscreen(false);//this will intialize the viewport
}


void App::swapFrameBuffers(){
	//do nothing
}

void App::showVirtualKeyboard()noexcept{
	//TODO:
}

void App::hideVirtualKeyboard()noexcept{
	//TODO:
}


std::unique_ptr<papki::File> App::getResFile(const std::string& path)const{
	std::string dir([[[NSBundle mainBundle] resourcePath] fileSystemRepresentation]);
	
//	TRACE(<< "res path = " << dir << std::endl)

	auto rdf = utki::makeUnique<papki::RootDirFile>(utki::makeUnique<papki::FSFile>(), dir + "/");
	rdf->setPath(path);
	
	return std::move(rdf);
}

void App::setMouseCursorVisible(bool visible){
	//do nothing
}

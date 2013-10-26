#include "../App.hpp"

#include <ting/fs/FSFile.hpp>

#import <Cocoa/Cocoa.h>





@interface MordaApplication : NSApplication{
	bool shouldKeepRunning;
}

- (void)run;
- (void)terminate:(id)sender;

@end


ting::Ptr<ting::fs::File> morda::App::CreateResourceFileInterface(const std::string& path)const{
	ting::Ptr<ting::fs::FSFile> fi = ting::fs::FSFile::New(path);
	fi->SetRootDir("res/");
	return fi;
}

void morda::App::ShowVirtualKeyboard()throw(){
	//do nothing
}

morda::App::ApplicationObject::ApplicationObject(){
	NSApplication *applicationObject = [[MordaApplication alloc] init];
	this->id = applicationObject;
	
	if(!this->id){
		throw morda::Exc("morda::App::ApplicationObject::ApplicationObject(): failed to create application object");
	}
}

morda::App::ApplicationObject::~ApplicationObject()throw(){
	NSApplication *applicationObject = (NSApplication*)this->id;
	[applicationObject release];
}

morda::App::WindowObject::WindowObject(){
	NSWindow* window = [[NSWindow alloc]
			initWithContentRect:NSMakeRect(0, 0, 200, 200)
			styleMask:NSTitledWindowMask
			backing:NSBackingStoreBuffered
			defer:NO
		];
	this->id = window;
	
	if(!this->id){
		throw morda::Exc("morda::App::WindowObject::WindowObject(): failed to create Window object");
	}
}

morda::App::WindowObject::~WindowObject()throw(){
	NSWindow* window = (NSWindow*)this->id;
	[window release];
}



morda::App::OpenGLContext::OpenGLContext(void* window){
	NSWindow *wnd = (NSWindow*)window;
	
	static NSOpenGLPixelFormatAttribute attributes[] = {
		NSOpenGLPFAAccelerated,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFADepthSize, 16,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFASupersample,
		0
	};
	
	NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
	if(pixelFormat == nil){
		throw morda::Exc("morda::App::OpenGLContext::OpenGLContext(): failed to create pixel format");
	}
	
	NSOpenGLContext *openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
	[pixelFormat release];
	this->id = openGLContext;
	if(!this->id){
		throw morda::Exc("morda::App::OpenGLContext::OpenGLContext(): failed to create OpenGL context");
	}
	
	[openGLContext setView:[wnd contentView]];
	[openGLContext makeCurrentContext];
}

morda::App::OpenGLContext::~OpenGLContext()throw(){
	NSOpenGLContext *openGLContext = (NSOpenGLContext*)this->id;
	[openGLContext release];
}



morda::App::App(const morda::App::WindowParams& wp) :
		openGLContext(windowObject.id)
{
	this->UpdateWindowRect(
			morda::Rect2f(
					0,
					0,
					float(wp.dim.x),
					float(wp.dim.y)
				)
		);
}



void morda::App::SwapGLBuffers(){
	NSOpenGLContext *openGLContext = (NSOpenGLContext*)this->openGLContext.id;
	[openGLContext flushBuffer];
}


void morda::App::Exec(){
	//	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

//    NSDictionary *infoDictionary = [[NSBundle mainBundle] infoDictionary];
//    Class principalClass = NSClassFromString([infoDictionary objectForKey:@"NSPrincipalClass"]);
//	NSApplication *applicationObject = [[MordaApplication alloc] init];//[principalClass sharedApplication];
	NSApplication *applicationObject = (NSApplication*)this->applicationObject.id;

//    NSString *mainNibName = [infoDictionary objectForKey:@"NSMainNibFile"];
//    NSNib *mainNib = [[NSNib alloc] initWithNibNamed:mainNibName bundle:[NSBundle mainBundle]];
//    [mainNib instantiateNibWithOwner:applicationObject topLevelObjects:nil];

//	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
//	id menubar = [[NSMenu new] autorelease];
//	id appMenuItem = [[NSMenuItem new] autorelease];
//	[menubar addItem:appMenuItem];
//	[NSApp setMainMenu:menubar];
//	id appMenu = [[NSMenu new] autorelease];
//	id appName = [[NSProcessInfo processInfo] processName];
//	id quitTitle = [@"Quit " stringByAppendingString:appName];
//	id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
//		action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
//	[appMenu addItem:quitMenuItem];
//	[appMenuItem setSubmenu:appMenu];

//	NSWindow* window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 200, 200)
	NSWindow* window = (NSWindow*)this->windowObject.id;
//		styleMask:NSTitledWindowMask backing:NSBackingStoreBuffered defer:NO];
//	[window cascadeTopLeftFromPoint:NSMakePoint(20, 20)];
	[window setTitle:[[NSProcessInfo processInfo] processName]];
	[window makeKeyAndOrderFront:nil];
//	[NSApp activateIgnoringOtherApps:YES];

	if ([applicationObject respondsToSelector:@selector(run)]){
		[applicationObject
				performSelectorOnMainThread:@selector(run)
				withObject:nil
				waitUntilDone:YES
			];
	}

	[window release];
//	[mainNib release];
//	[applicationObject release];
//	[pool release];
}



namespace morda{

void Main(int argc, const char** argv){
	ting::Ptr<morda::App> app = morda::CreateApp(argc, argv, ting::Buffer<const ting::u8>(0, 0));

	app->Exec();
}

}



@implementation MordaApplication

- (void)run{
//	[self finishLaunching];
	[[NSNotificationCenter defaultCenter]
		postNotificationName:NSApplicationWillFinishLaunchingNotification
		object:NSApp];
	[[NSNotificationCenter defaultCenter]
		postNotificationName:NSApplicationDidFinishLaunchingNotification
		object:NSApp];

	shouldKeepRunning = YES;
	do{
		NSEvent *event =
			[self
				nextEventMatchingMask:NSAnyEventMask
				untilDate:[NSDate distantFuture]
				inMode:NSDefaultRunLoopMode
				dequeue:YES];

		[self sendEvent:event];
		[self updateWindows];
	}while(shouldKeepRunning);
}

- (void)terminate:(id)sender{
	shouldKeepRunning = NO;
}

@end



int main (int argc, const char** argv){
	morda::Main(argc, argv);

	return 0;
}

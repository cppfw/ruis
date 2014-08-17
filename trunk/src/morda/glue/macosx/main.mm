#include "../../App.hpp"
#include "../../AppFactory.hpp"

#include <ting/fs/FSFile.hpp>

#import <Cocoa/Cocoa.h>



//@interface MordaApplication : NSApplication{
//	bool shouldKeepRunning;
//}
//
//- (void)run;
//- (void)terminate:(id)sender;
//
//@end


morda::App::ApplicationObject::ApplicationObject(){
	NSApplication *applicationObject = [[NSApplication alloc] init];
	this->id = applicationObject;
	
	if(!this->id){
		throw morda::Exc("morda::App::ApplicationObject::ApplicationObject(): failed to create application object");
	}
}

morda::App::ApplicationObject::~ApplicationObject()NOEXCEPT{
	NSApplication *applicationObject = (NSApplication*)this->id;
	[applicationObject release];
}

morda::App::WindowObject::WindowObject(const morda::App::WindowParams& wp){
	NSWindow* window = [[NSWindow alloc]
			initWithContentRect:NSMakeRect(0, 0, wp.dim.x, wp.dim.y)
			styleMask:(/*NSResizableWindowMask | NSMiniaturizableWindowMask |*/ NSClosableWindowMask | NSTitledWindowMask)
			backing:NSBackingStoreBuffered
			defer:NO
		];
	this->id = window;
	
	if(!this->id){
		throw morda::Exc("morda::App::WindowObject::WindowObject(): failed to create Window object");
	}
	
	[window setAcceptsMouseMovedEvents:YES];
	[window becomeFirstResponder];
}

morda::App::WindowObject::~WindowObject()NOEXCEPT{
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
	
	if(glewInit() != GLEW_OK){
		this->Destroy();
		throw morda::Exc("GLEW initialization failed");
	}
}

void morda::App::OpenGLContext::Destroy()NOEXCEPT{
	NSOpenGLContext *openGLContext = (NSOpenGLContext*)this->id;
	[openGLContext release];
}



morda::App::App(const morda::App::WindowParams& wp) :
		windowObject(wp),
		openGLContext(windowObject.id)
{
	{
		NSScreen *screen = [NSScreen mainScreen];
		NSDictionary *description = [screen deviceDescription];
		NSSize displayPixelSize = [[description objectForKey:NSDeviceSize] sizeValue];
		CGSize displayPhysicalSize = CGDisplayScreenSize(
				[[description objectForKey:@"NSScreenNumber"] unsignedIntValue]
			);

		this->dotsPerCm = float(((displayPixelSize.width * 10.0f / displayPhysicalSize.width) +
				(displayPixelSize.height * 10.0f / displayPhysicalSize.height)) / 2.0f);
	}
	
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


void morda::App::PostToUIThread_ts(std::function<void()>&& f){	
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
}


void morda::App::Exec(){
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

	NSWindow* window = (NSWindow*)this->windowObject.id;
	
	NSWindowController * windowController = [[NSWindowController alloc] initWithWindow:window];
	
	[window setTitle:[[NSProcessInfo processInfo] processName]];
	[window makeKeyAndOrderFront:nil];

	
	bool quitFlag = false;
	do{
		this->Render();
		
		std::uint32_t millis = this->updater.Update();
		
		NSEvent *event = [applicationObject
				nextEventMatchingMask:NSAnyEventMask
				untilDate:[NSDate dateWithTimeIntervalSinceNow:(double(millis) / 1000.0)]
				inMode:NSDefaultRunLoopMode
				dequeue:YES
			];

		if(event){
//			TRACE_ALWAYS(<< "Event: type = "<< [event type] << std::endl)
	
			switch([event type]){
				case NSApplicationDefined:
					{
						std::unique_ptr<std::function<void()>> m(reinterpret_cast<std::function<void()>*>([event data1]));
						(*m)();
					}
					break;
				case NSLeftMouseDown:
					{
						NSPoint pos = [event locationInWindow];
						TRACE(<< "pos = " << pos.x << ", " << pos.y << std::endl)
						this->HandleMouseButton(
								true,
								morda::Vec2f(pos.x, this->curWinRect.d.y - pos.y),
								morda::Widget::LEFT,
								0
							);
					}
					break;
				case NSLeftMouseUp:
					{
						NSPoint pos = [event locationInWindow];
						this->HandleMouseButton(
								false,
								morda::Vec2f(pos.x, this->curWinRect.d.y - pos.y),
								morda::Widget::LEFT,
								0
							);
					}
					break;
				case NSRightMouseDown:
					{
						NSPoint pos = [event locationInWindow];
						TRACE(<< "pos = " << pos.x << ", " << pos.y << std::endl)
						this->HandleMouseButton(
								true,
								morda::Vec2f(pos.x, this->curWinRect.d.y - pos.y),
								morda::Widget::RIGHT,
								0
							);
					}
					break;
				case NSRightMouseUp:
					{
						NSPoint pos = [event locationInWindow];
						this->HandleMouseButton(
								false,
								morda::Vec2f(pos.x, this->curWinRect.d.y - pos.y),
								morda::Widget::RIGHT,
								0
							);
					}
					break;
				case NSOtherMouseDown:
					{
						NSPoint pos = [event locationInWindow];
						TRACE(<< "pos = " << pos.x << ", " << pos.y << std::endl)
						this->HandleMouseButton(
								true,
								morda::Vec2f(pos.x, this->curWinRect.d.y - pos.y),
								morda::Widget::MIDDLE,
								0
							);
					}
					break;
				case NSOtherMouseUp:
					{
						NSPoint pos = [event locationInWindow];
						this->HandleMouseButton(
								false,
								morda::Vec2f(pos.x, this->curWinRect.d.y - pos.y),
								morda::Widget::MIDDLE,
								0
							);
					}
					break;
					
				case NSOtherMouseDragged:
				case NSRightMouseDragged:
				case NSLeftMouseDragged:
					{
						NSPoint pos = [event locationInWindow];
						this->HandleMouseMove(
								morda::Vec2f(pos.x, this->curWinRect.d.y - pos.y),
								0
							);
					}
					break;

				//TODO:
				
				default:
					[applicationObject sendEvent:event];
					[applicationObject updateWindows];
					break;
			}
		}//~if(event)
//		[applicationObject updateWindows];
	}while(!quitFlag);
}



namespace morda{

void Main(int argc, const char** argv){
	std::unique_ptr<morda::App> app = morda::CreateApp(argc, argv, ting::Buffer<const std::uint8_t>(0, 0));

	app->Exec();
}

}



int main (int argc, const char** argv){
	morda::Main(argc, argv);

	return 0;
}

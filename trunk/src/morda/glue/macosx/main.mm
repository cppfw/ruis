#include "../../App.hpp"
#include "../../AppFactory.hpp"

#import <Cocoa/Cocoa.h>



namespace morda{

void Macosx_Main(int argc, const char** argv){
	std::unique_ptr<morda::App> app = morda::CreateApp(argc, argv, ting::Buffer<const std::uint8_t>(0, 0));

	app->Exec();
}

void Macosx_HandleMouseMove(const morda::Vec2f& pos, unsigned id){
	morda::App::Inst().HandleMouseMove(
			morda::Vec2f(pos.x, morda::App::Inst().curWinRect.d.y - pos.y),
			id
		);
}

void Macosx_HandleMouseButton(bool isDown, const morda::Vec2f& pos, Widget::EMouseButton button, unsigned id){
	morda::App::Inst().HandleMouseButton(
			isDown,
			morda::Vec2f(pos.x, morda::App::Inst().curWinRect.d.y - pos.y),
			button,
			id
		);
}

void Macosx_UpdateWindowRect(const morda::Rect2f& r){
	NSOpenGLContext *openGLContext = (NSOpenGLContext*)morda::App::Inst().openGLContext.id;
	[openGLContext update];//after resizing window we need to update OpenGL context
	morda::App::Inst().UpdateWindowRect(r);
}

void Macosx_SetQuitFlag(){
	morda::App::Inst().quitFlag = true;
}

}



int main (int argc, const char** argv){
	morda::Macosx_Main(argc, argv);

	return 0;
}



@interface CocoaWindow : NSWindow <NSWindowDelegate>{
	NSTrackingArea* ta;
}

-(id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)windowStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)deferCreation;
-(void)dealloc;

-(BOOL)canBecomeKeyWindow;
-(BOOL)acceptsFirstResponder;

-(void)mouseDown: (NSEvent*)e;
-(void)mouseUp: (NSEvent*)e;
-(void)rightMouseDown: (NSEvent*)e;
-(void)rightMouseUp: (NSEvent*)e;
-(void)otherMouseDown: (NSEvent*)e;
-(void)otherMouseUp: (NSEvent*)e;

-(void)mouseButton: (NSEvent*)e;

-(void)mouseDragged: (NSEvent*)e;
-(void)rightMouseDragged: (NSEvent*)e;
-(void)otherMouseDragged: (NSEvent*)e;
-(void)mouseMoved: (NSEvent*)e;
-(void)mouseEntered: (NSEvent*)e;
-(void)mouseExited: (NSEvent*)e;

-(void)windowDidResize:(NSNotification*)n;
-(BOOL)windowShouldClose:(id)sender;

@end
@implementation CocoaWindow

-(id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)windowStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)deferCreation{
	self->ta = [[NSTrackingArea alloc]
			initWithRect: contentRect
			options: (NSTrackingActiveAlways | NSTrackingInVisibleRect | NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved)
			owner: self
			userInfo: nil
		];
	[[self contentView] addTrackingRect:contentRect owner:self userData:nil assumeInside:NO];
	return [super initWithContentRect:contentRect styleMask:windowStyle backing:bufferingType defer:deferCreation];
}

-(void)dealloc{
	[self->ta release];
	[super dealloc];
}

-(void)windowDidResize:(NSNotification*)n{
	TRACE(<< "window resize!!!!" << std::endl)
	NSWindow* nsw = [n object];
	NSRect frame = [nsw frame];
	NSRect rect = [nsw contentRectForFrameRect:frame];
	morda::Macosx_UpdateWindowRect(morda::Rect2f(0, 0, rect.size.width, rect.size.height));
}

-(BOOL)windowShouldClose:(id)sender{
	TRACE(<< "window wants to close!!!!" << std::endl)
	morda::Macosx_SetQuitFlag();
	return NO;
}

-(BOOL)canBecomeKeyWindow{return YES;}
-(BOOL)acceptsFirstResponder{return YES;}

-(void)mouseDown: (NSEvent*)e{
	[self mouseButton: e];
}

-(void)mouseUp: (NSEvent*)e{
	[self mouseButton: e];
}

-(void)rightMouseDown: (NSEvent*)e{
	[self mouseButton: e];
}

-(void)rightMouseUp: (NSEvent*)e{
	[self mouseButton: e];
}

-(void)otherMouseDown: (NSEvent*)e{
	[self mouseButton: e];
}

-(void)otherMouseUp: (NSEvent*)e{
	[self mouseButton: e];
}

-(void)mouseButton: (NSEvent*)e{
	TRACE(<< "mouseButton event!!!!!" << std::endl)
	morda::Widget::EMouseButton b;
	bool isDown = false;
	switch([e type]){
		case NSLeftMouseDown:
			isDown = true;
			TRACE_ALWAYS(<< "left down!!!!!" << std::endl)
		default:
			TRACE_ALWAYS(<< "default!!!!!" << std::endl)
		case NSLeftMouseUp:
			b = morda::Widget::LEFT;
			break;
		case NSRightMouseDown:
			isDown = true;
			TRACE_ALWAYS(<< "right down!!!!!" << std::endl)
		case NSRightMouseUp:
			b = morda::Widget::RIGHT;
			break;
		case NSOtherMouseDown:
			isDown = true;
		case NSOtherMouseUp:
			b = morda::Widget::MIDDLE;
			break;
	}
	
	NSPoint pos = [e locationInWindow];
	Macosx_HandleMouseButton(
			isDown,
			morda::Vec2f(pos.x, pos.y),
			b,
			0
		);
}

-(void)mouseMoved: (NSEvent*)e{
	TRACE(<< "mouseMoved event!!!!!" << std::endl)
	NSPoint pos = [e locationInWindow];
	Macosx_HandleMouseMove(
			morda::Vec2f(pos.x, pos.y),
			0
		);
}

-(void)mouseDragged: (NSEvent*)e{
	[self mouseMoved:e];
}

-(void)rightMouseDragged: (NSEvent*)e{
	[self mouseMoved:e];
}

-(void)otherMouseDragged: (NSEvent*)e{
	[self mouseMoved:e];
}

-(void)mouseEntered: (NSEvent*)e{
	TRACE(<< "mouseEntered event!!!!!" << std::endl)
}

-(void)mouseExited: (NSEvent*)e{
	TRACE(<< "mouseExited event!!!!!" << std::endl)
}

@end


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
	CocoaWindow* window = [[CocoaWindow alloc]
			initWithContentRect:NSMakeRect(0, 0, wp.dim.x, wp.dim.y)
			styleMask:(NSResizableWindowMask | NSMiniaturizableWindowMask | NSClosableWindowMask | NSTitledWindowMask)
			backing:NSBackingStoreBuffered
			defer:NO
		];
	this->id = window;
	
	if(!this->id){
		throw morda::Exc("morda::App::WindowObject::WindowObject(): failed to create Window object");
	}
	
	[window setDelegate:window];
	
	[window becomeFirstResponder];//this is needed to get mouse move events
	[window setAcceptsMouseMovedEvents:YES];
}

morda::App::WindowObject::~WindowObject()NOEXCEPT{
	NSWindow* window = (CocoaWindow*)this->id;
	[window release];
}



morda::App::OpenGLContext::OpenGLContext(void* window){
	CocoaWindow *wnd = (CocoaWindow*)window;
	
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
	//init dots per cm
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
	NSApplication *applicationObject = (NSApplication*)this->applicationObject.id;

	NSWindow* window = (NSWindow*)this->windowObject.id;
	
	NSWindowController * windowController = [[NSWindowController alloc] initWithWindow:window];
	
	[window setTitle:[[NSProcessInfo processInfo] processName]];
	[window makeKeyAndOrderFront:nil];

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
				case NSMouseMoved:
					TRACE(<< "NSMouseMoved event" << std::endl)
					break;
				default:
					[applicationObject sendEvent:event];
					[applicationObject updateWindows];
					break;
			}
		}//~if(event)
	}while(!this->quitFlag);
}

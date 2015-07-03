#include "../../App.hpp"
#include "../../AppFactory.hpp"
#include "../../util/util.hpp"

#import <Cocoa/Cocoa.h>


using namespace morda;


namespace{
class UnicodeResolver{
	const NSString* nsStr;
public:
	UnicodeResolver(const NSString* nsStr = nullptr) :
			nsStr(nsStr)
	{}
	
	std::vector<std::uint32_t> Resolve()const{
		if(!this->nsStr){
			return std::vector<std::uint32_t>();
		}
		
		NSUInteger len = [this->nsStr length];
		
		std::vector<std::uint32_t> ret(len);
		for(unsigned i = 0; i != len; ++i){
			ret[i] = [this->nsStr characterAtIndex:i];
		}
		
		return std::move(ret);
	}
};
}


namespace morda{

void Macosx_Main(int argc, const char** argv){
	std::unique_ptr<morda::App> app = morda::CreateApp(argc, argv, ting::Buffer<const std::uint8_t>(0, 0));

	app->Exec();
}

void Macosx_HandleMouseMove(const morda::Vec2r& pos, unsigned id){
//	TRACE(<< "Macosx_HandleMouseMove(): pos = " << pos << std::endl)
	morda::App::Inst().HandleMouseMove(
			morda::Vec2r(pos.x, morda::App::Inst().curWinRect.d.y - pos.y),
			id
		);
}

void Macosx_HandleMouseButton(bool isDown, const morda::Vec2r& pos, Widget::EMouseButton button, unsigned id){
	morda::App::Inst().HandleMouseButton(
			isDown,
			morda::Vec2r(pos.x, morda::App::Inst().curWinRect.d.y - pos.y),
			button,
			id
		);
}

void Macosx_HandleMouseHover(bool isHovered){
	morda::App::Inst().HandleMouseHover(isHovered, 0);
}

void Macosx_HandleKeyEvent(bool isDown, EKey keyCode){
	morda::App::Inst().HandleKeyEvent(isDown, keyCode);
}

void Macosx_HandleCharacterInput(const void* nsstring, EKey key){
	morda::App::Inst().HandleCharacterInput(UnicodeResolver(reinterpret_cast<const NSString*>(nsstring)), key);
}

void Macosx_UpdateWindowRect(const morda::Rect2r& r){
	NSOpenGLContext *openGLContext = (NSOpenGLContext*)morda::App::Inst().openGLContext.id;
	[openGLContext update];//after resizing window we need to update OpenGL context
	morda::App::Inst().UpdateWindowRect(r);
}

void morda::App::Quit()noexcept{
	this->quitFlag = true;
}

}

namespace{
void MouseButton(NSEvent* e, bool isDown, morda::Widget::EMouseButton b){
	NSPoint pos = [e locationInWindow];
	Macosx_HandleMouseButton(
			isDown,
			morda::Vec2r(pos.x, pos.y).Rounded(),
			b,
			0
		);
}

const std::array<morda::EKey, std::uint8_t(-1) + 1> keyCodeMap = {
	EKey::A, //0
	EKey::S,
	EKey::D,
	EKey::F,
	EKey::H,
	EKey::G, //5
	EKey::Z,
	EKey::X,
	EKey::C,
	EKey::V,
	EKey::UNKNOWN, //0x0A
	EKey::B,
	EKey::Q,
	EKey::W,
	EKey::E,
	EKey::R, //15
	EKey::Y,
	EKey::T,
	EKey::ONE,
	EKey::TWO,
	EKey::THREE, //20
	EKey::FOUR,
	EKey::SIX,
	EKey::FIVE, //0x17
	EKey::EQUALS,
	EKey::NINE, //25
	EKey::SEVEN,
	EKey::MINUS,
	EKey::EIGHT,
	EKey::ZERO,
	EKey::RIGHT_SQUARE_BRACKET, //30
	EKey::O,
	EKey::U,
	EKey::LEFT_SQUARE_BRACKET,
	EKey::I,
	EKey::P, //35
	EKey::ENTER, //0x24
	EKey::L,
	EKey::J,
	EKey::APOSTROPHE,
	EKey::K, //40
	EKey::SEMICOLON,
	EKey::BACKSLASH,
	EKey::COMMA,
	EKey::SLASH,
	EKey::N, //0x2D, 45
	EKey::M,
	EKey::PERIOD,
	EKey::TAB, //0x30
	EKey::SPACE, //0x31
	EKey::GRAVE, //50
	EKey::BACKSPACE, //0x33
	EKey::UNKNOWN, //0x34
	EKey::ESCAPE, //0x35
	EKey::UNKNOWN, //0x36
	EKey::WINDOWS, //Command, 0x37, 55
	EKey::LEFT_SHIFT, //0x38
	EKey::CAPSLOCK, //0x39
	EKey::UNKNOWN, //Option, 0x3A
	EKey::LEFT_CONTROL, //0x3B
	EKey::RIGHT_SHIFT, //0x3C, 60
	EKey::UNKNOWN, //RightOption, 0x3D
	EKey::RIGHT_CONTROL, //0x3E
	EKey::FUNCTION, //0x3F
	EKey::F17, //0x40
	EKey::UNKNOWN, //KeypadDecimal, 0x41, 65
	EKey::UNKNOWN, //0x42
	EKey::UNKNOWN, //KeypadMultiplym 0x43
	EKey::UNKNOWN, //0x44
	EKey::UNKNOWN, //KeypadPlus, 0x45
	EKey::UNKNOWN, //0x46, 70
	EKey::UNKNOWN, //KeypadClear, 0x47
	EKey::UNKNOWN, //VolumeUp, 0x48
	EKey::UNKNOWN, //VolumeDown, 0x49
	EKey::UNKNOWN, //Mute, 0x4A
	EKey::UNKNOWN, //KeypadDivide, 0x4B, 75
	EKey::UNKNOWN, //KeypadEnter, 0x4C
	EKey::UNKNOWN, //0x4D
	EKey::UNKNOWN, //KeypadMinus
	EKey::F18, //0x4F
	EKey::F19, //0x50, 80
	EKey::UNKNOWN, //KeypadEquals, 0x51
	EKey::UNKNOWN, //Keypad0
	EKey::UNKNOWN, //Keypad1
	EKey::UNKNOWN, //Keypad2
	EKey::UNKNOWN, //Keypad3, 85
	EKey::UNKNOWN, //Keypad4
	EKey::UNKNOWN, //Keypad5
	EKey::UNKNOWN, //Keypad6
	EKey::UNKNOWN, //Keypad7, 0x59
	EKey::F20, //0x5A, 90
	EKey::UNKNOWN, //Keypad8, 0x5B
	EKey::UNKNOWN, //Keypad9, 0x5C
	EKey::UNKNOWN, //0x5D
	EKey::UNKNOWN, //0x5E
	EKey::UNKNOWN, //0x5F, 95
	EKey::F5, //0x60
	EKey::F6, //0x61
	EKey::F7, //0x62
	EKey::F3, //0x63
	EKey::F8, //0x64, 100
	EKey::F9, //0x65
	EKey::UNKNOWN, //0x66
	EKey::F11, //0x67
	EKey::UNKNOWN, //0x68
	EKey::F13, //0x69
	EKey::F16, //0x6A
	EKey::F14, //0x6B
	EKey::UNKNOWN, //0x6C
	EKey::F10, //0x6D
	EKey::UNKNOWN, //0x6E
	EKey::F12, //0x6F
	EKey::UNKNOWN, //0x70
	EKey::F15, //0x71
	EKey::UNKNOWN, //Help, 0x72
	EKey::HOME, //0x73
	EKey::PAGE_UP, //0x74
	EKey::DELETE, //0x75
	EKey::F4, //0x76
	EKey::END, //0x77
	EKey::F2, //0x78
	EKey::PAGE_DOWN, //0x79
	EKey::F1, //0x7A
	EKey::LEFT, //0x7B
	EKey::RIGHT, //0x7C
	EKey::DOWN, //0x7D
	EKey::UP, //0x7E
	EKey::UNKNOWN, //0x7F
	EKey::UNKNOWN, //0x80
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN, //0x90
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN, //0xA0
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN, //0xB0
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN, //0xC0
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN, //0xD0
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN, //0xE0
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN, //0xF0
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN //0xFF
};

}


int main (int argc, const char** argv){
	morda::Macosx_Main(argc, argv);

	return 0;
}


@interface CocoaView : NSView{
	NSTrackingArea* ta;
}

-(id)initWithFrame:(NSRect)rect;
-(void)dealloc;

-(void)mouseDown: (NSEvent*)e;
-(void)mouseUp: (NSEvent*)e;
-(void)rightMouseDown: (NSEvent*)e;
-(void)rightMouseUp: (NSEvent*)e;
-(void)otherMouseDown: (NSEvent*)e;
-(void)otherMouseUp: (NSEvent*)e;

-(void)mouseDragged: (NSEvent*)e;
-(void)rightMouseDragged: (NSEvent*)e;
-(void)otherMouseDragged: (NSEvent*)e;
-(void)mouseMoved: (NSEvent*)e;
-(void)mouseEntered: (NSEvent*)e;
-(void)mouseExited: (NSEvent*)e;

-(void)keyDown:(NSEvent*)e;
-(void)keyUp:(NSEvent*)e;

@end
@implementation CocoaView

-(id)initWithFrame:(NSRect)rect{
	self = [super initWithFrame:rect];
	if(!self){
		return nil;
	}
	self->ta = [[NSTrackingArea alloc]
			initWithRect: rect
			options: (NSTrackingActiveAlways | NSTrackingInVisibleRect | NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved)
			owner: self
			userInfo: nil
		];
	[self addTrackingArea:self->ta];
	return self;
}

-(void)dealloc{
	[self->ta release];
	[super dealloc];
}

-(void)mouseDown: (NSEvent*)e{
//	TRACE(<< "left down!!!!!" << std::endl)
	MouseButton(e, true, morda::Widget::EMouseButton::LEFT);
}

-(void)mouseUp: (NSEvent*)e{
//	TRACE(<< "left up!!!!!" << std::endl)
	MouseButton(e, false, morda::Widget::EMouseButton::LEFT);
}

-(void)rightMouseDown: (NSEvent*)e{
//	TRACE(<< "right down!!!!!" << std::endl)
	MouseButton(e, true, morda::Widget::EMouseButton::RIGHT);
}

-(void)rightMouseUp: (NSEvent*)e{
//	TRACE(<< "right up!!!!!" << std::endl)
	MouseButton(e, false, morda::Widget::EMouseButton::RIGHT);
}

-(void)otherMouseDown: (NSEvent*)e{
//	TRACE(<< "middle down!!!!!" << std::endl)
	MouseButton(e, true, morda::Widget::EMouseButton::MIDDLE);
}

-(void)otherMouseUp: (NSEvent*)e{
//	TRACE(<< "middle up!!!!!" << std::endl)
	MouseButton(e, false, morda::Widget::EMouseButton::MIDDLE);
}


-(void)mouseMoved: (NSEvent*)e{
//	TRACE(<< "mouseMoved event!!!!!" << std::endl)
	NSPoint pos = [e locationInWindow];
//	TRACE(<< "x = " << pos.x << std::endl)
	Macosx_HandleMouseMove(
			morda::Vec2r(pos.x, pos.y).Rounded(),
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
//	TRACE(<< "mouseEntered event!!!!!" << std::endl)
	[[self window] setAcceptsMouseMovedEvents:YES];
	morda::Macosx_HandleMouseHover(true);
}

-(void)mouseExited: (NSEvent*)e{
//	TRACE(<< "mouseExited event!!!!!" << std::endl)
	[[self window] setAcceptsMouseMovedEvents:NO];
	morda::Macosx_HandleMouseHover(false);
}

-(void)keyDown:(NSEvent*)e{
//	TRACE(<< "keyDown event!!!!!" << std::endl)
	std::uint8_t kc = [e keyCode];
	EKey key = keyCodeMap[kc];
	
	if([e isARepeat] == YES){
		Macosx_HandleCharacterInput([e characters], key);
		return;
	}
	
	Macosx_HandleKeyEvent(true, key);
	
	Macosx_HandleCharacterInput([e characters], key);
}

-(void)keyUp:(NSEvent*)e{
//	TRACE(<< "keyUp event!!!!!" << std::endl)
	std::uint8_t kc = [e keyCode];
	Macosx_HandleKeyEvent(false, keyCodeMap[kc]);
}

@end



@interface CocoaWindow : NSWindow <NSWindowDelegate>{
	CocoaView* v;
}

-(id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)windowStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)deferCreation;
-(void)dealloc;

-(BOOL)canBecomeKeyWindow;
-(BOOL)canBecomeMainWindow;
-(BOOL)acceptsFirstResponder;

-(void)windowDidResize:(NSNotification*)n;
-(BOOL)windowShouldClose:(id)sender;
-(NSSize)windowWillResize:(NSWindow*)sender toSize:(NSSize)frameSize;

@end
@implementation CocoaWindow

-(id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)windowStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)deferCreation{
	self = [super initWithContentRect:contentRect styleMask:windowStyle backing:bufferingType defer:deferCreation];
	if(!self){
		return nil;
	}
//	[self setLevel:NSFloatingWindowLevel];
	[self setLevel:NSNormalWindowLevel];
	
	self->v = [[CocoaView alloc] initWithFrame:[self frameRectForContentRect:contentRect]];
	[self setContentView:self->v];
	
	[self makeFirstResponder:self->v];
	
	[self setDelegate:self];
	[self setShowsResizeIndicator:YES];
	[self setMinSize:NSMakeSize(0, 0)];
	[self setMaxSize:NSMakeSize(1000000000, 1000000000)];
	[self setIgnoresMouseEvents:NO];
	[self makeKeyWindow];
	[self makeMainWindow];
	return self;
}

-(void)dealloc{
	[self->v release];
	[super dealloc];
}

-(void)windowDidResize:(NSNotification*)n{
	TRACE(<< "window resize!!!!" << std::endl)
	NSWindow* nsw = [n object];
	NSRect frame = [nsw frame];
	NSRect rect = [nsw contentRectForFrameRect:frame];
	morda::Macosx_UpdateWindowRect(morda::Rect2r(0, 0, rect.size.width, rect.size.height));
}

-(NSSize)windowWillResize:(NSWindow*)sender toSize:(NSSize)frameSize{
	return frameSize;
}

-(BOOL)windowShouldClose:(id)sender{
	TRACE(<< "window wants to close!!!!" << std::endl)
	morda::App::Inst().Quit();
	return NO;
}

-(BOOL)canBecomeKeyWindow{return YES;}
-(BOOL)canBecomeMainWindow{return YES;}
-(BOOL)acceptsFirstResponder{return YES;}

@end


morda::App::ApplicationObject::ApplicationObject(){
	NSApplication *applicationObject = [NSApplication sharedApplication];
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
	
	[window setTitle:[[NSProcessInfo processInfo] processName]];
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
}

void morda::App::OpenGLContext::Destroy()NOEXCEPT{
	NSOpenGLContext *openGLContext = (NSOpenGLContext*)this->id;
	[openGLContext release];
}



App::DotsPerCmWrapper::DotsPerCmWrapper(){
	NSScreen *screen = [NSScreen mainScreen];
	NSDictionary *description = [screen deviceDescription];
	NSSize displayPixelSize = [[description objectForKey:NSDeviceSize] sizeValue];
	CGSize displayPhysicalSize = CGDisplayScreenSize(
			[[description objectForKey:@"NSScreenNumber"] unsignedIntValue]
		);

	this->value = float(((displayPixelSize.width * 10.0f / displayPhysicalSize.width) +
			(displayPixelSize.height * 10.0f / displayPhysicalSize.height)) / 2.0f);
}



morda::App::App(const morda::App::WindowParams& wp) :
		windowObject(wp),
		openGLContext(windowObject.id)
{
	this->UpdateWindowRect(
			morda::Rect2r(
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
	
	[applicationObject activateIgnoringOtherApps:YES];
	
	[window makeKeyAndOrderFront:nil];

	[window orderFrontRegardless];
	
	//in order to get keyboard events we need to be foreground application
	{
		ProcessSerialNumber psn = {0, kCurrentProcess};
		OSStatus status = TransformProcessType(&psn, kProcessTransformToForegroundApplication);
		if(status != errSecSuccess){
			ASSERT(false)
		}
	}
	
	do{
		this->render();
		
		std::uint32_t millis = this->updater.Update();
		
		NSEvent *event = [applicationObject
				nextEventMatchingMask:NSAnyEventMask
				untilDate:[NSDate dateWithTimeIntervalSinceNow:(double(millis) / 1000.0)]
				inMode:NSDefaultRunLoopMode
				dequeue:YES
			];

		if(!event){
			continue;
		}
		
		do{
//			TRACE_ALWAYS(<< "Event: type = "<< [event type] << std::endl)
			switch([event type]){
				case NSApplicationDefined:
					{
						std::unique_ptr<std::function<void()>> m(reinterpret_cast<std::function<void()>*>([event data1]));
						(*m)();
					}
					break;
				default:
					[applicationObject sendEvent:event];
					[applicationObject updateWindows];
					break;
			}
			
			event = [applicationObject
					nextEventMatchingMask:NSAnyEventMask
					untilDate:[NSDate distantPast]
					inMode:NSDefaultRunLoopMode
					dequeue:YES
				];
		}while(event && !this->quitFlag);
	}while(!this->quitFlag);
}

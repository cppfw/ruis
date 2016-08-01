#include "../../App.hpp"
#include "../../AppFactory.hpp"
#include "../../../../../../src/morda/util/util.hpp"

#import <Cocoa/Cocoa.h>


using namespace mordavokne;


namespace{
class MacosxUnicodeProvider : public morda::Morda::UnicodeProvider{
	const NSString* nsStr;
public:
	MacosxUnicodeProvider(const NSString* nsStr = nullptr) :
			nsStr(nsStr)
	{}
	
	std::u32string get()const override{
		if(!this->nsStr){
			return std::u32string();
		}
		
		NSUInteger len = [this->nsStr length];
		
		std::u32string ret(len, 0);
		for(unsigned i = 0; i != len; ++i){
			ret[i] = [this->nsStr characterAtIndex:i];
		}
		
		return ret;
	}
};
}


namespace mordavokne{

void macosx_Main(int argc, const char** argv){
	auto app = createAppUnix(argc, argv, utki::Buf<std::uint8_t>());

	app->exec();
}

void macosx_HandleMouseMove(const morda::Vec2r& pos, unsigned id){
//	TRACE(<< "Macosx_HandleMouseMove(): pos = " << pos << std::endl)
	mordavokne::App::inst().handleMouseMove(
			morda::Vec2r(pos.x, mordavokne::App::inst().curWinRect.d.y - pos.y),
			id
		);
}

void macosx_HandleMouseButton(bool isDown, const morda::Vec2r& pos, morda::Widget::MouseButton_e button, unsigned id){
	mordavokne::App::inst().handleMouseButton(
			isDown,
			morda::Vec2r(pos.x, mordavokne::App::inst().curWinRect.d.y - pos.y),
			button,
			id
		);
}

void macosx_HandleMouseHover(bool isHovered){
	if(!mordavokne::App::inst().mouseCursorIsCurrentlyVisible){
		if(isHovered){
			[NSCursor hide];
		}else if(!isHovered){
			[NSCursor unhide];
		}
	}
	
	mordavokne::App::inst().handleMouseHover(isHovered, 0);
}

void macosx_HandleKeyEvent(bool isDown, morda::Key_e keyCode){
	mordavokne::App::inst().handleKeyEvent(isDown, keyCode);
}

void macosx_HandleCharacterInput(const void* nsstring, morda::Key_e key){
	mordavokne::App::inst().handleCharacterInput(MacosxUnicodeProvider(reinterpret_cast<const NSString*>(nsstring)), key);
}

void macosx_UpdateWindowRect(const morda::Rectr& r){
	NSOpenGLContext *openGLContext = (NSOpenGLContext*)mordavokne::App::inst().openGLContext.id;
	[openGLContext update];//after resizing window we need to update OpenGL context
	mordavokne::App::inst().updateWindowRect(r);
}

void App::quit()noexcept{
	this->quitFlag = true;
}

}

namespace{
void mouseButton(NSEvent* e, bool isDown, morda::Widget::MouseButton_e b){
	NSPoint pos = [e locationInWindow];
	macosx_HandleMouseButton(
			isDown,
			morda::Vec2r(pos.x, pos.y).rounded(),
			b,
			0
		);
}

const std::array<morda::Key_e, std::uint8_t(-1) + 1> keyCodeMap = {{
	morda::Key_e::A, //0
	morda::Key_e::S,
	morda::Key_e::D,
	morda::Key_e::F,
	morda::Key_e::H,
	morda::Key_e::G, //5
	morda::Key_e::Z,
	morda::Key_e::X,
	morda::Key_e::C,
	morda::Key_e::V,
	morda::Key_e::UNKNOWN, //0x0A
	morda::Key_e::B,
	morda::Key_e::Q,
	morda::Key_e::W,
	morda::Key_e::E,
	morda::Key_e::R, //15
	morda::Key_e::Y,
	morda::Key_e::T,
	morda::Key_e::ONE,
	morda::Key_e::TWO,
	morda::Key_e::THREE, //20
	morda::Key_e::FOUR,
	morda::Key_e::SIX,
	morda::Key_e::FIVE, //0x17
	morda::Key_e::EQUALS,
	morda::Key_e::NINE, //25
	morda::Key_e::SEVEN,
	morda::Key_e::MINUS,
	morda::Key_e::EIGHT,
	morda::Key_e::ZERO,
	morda::Key_e::RIGHT_SQUARE_BRACKET, //30
	morda::Key_e::O,
	morda::Key_e::U,
	morda::Key_e::LEFT_SQUARE_BRACKET,
	morda::Key_e::I,
	morda::Key_e::P, //35
	morda::Key_e::ENTER, //0x24
	morda::Key_e::L,
	morda::Key_e::J,
	morda::Key_e::APOSTROPHE,
	morda::Key_e::K, //40
	morda::Key_e::SEMICOLON,
	morda::Key_e::BACKSLASH,
	morda::Key_e::COMMA,
	morda::Key_e::SLASH,
	morda::Key_e::N, //0x2D, 45
	morda::Key_e::M,
	morda::Key_e::PERIOD,
	morda::Key_e::TAB, //0x30
	morda::Key_e::SPACE, //0x31
	morda::Key_e::GRAVE, //50
	morda::Key_e::BACKSPACE, //0x33
	morda::Key_e::UNKNOWN, //0x34
	morda::Key_e::ESCAPE, //0x35
	morda::Key_e::UNKNOWN, //0x36
	morda::Key_e::WINDOWS, //Command, 0x37, 55
	morda::Key_e::LEFT_SHIFT, //0x38
	morda::Key_e::CAPSLOCK, //0x39
	morda::Key_e::UNKNOWN, //Option, 0x3A
	morda::Key_e::LEFT_CONTROL, //0x3B
	morda::Key_e::RIGHT_SHIFT, //0x3C, 60
	morda::Key_e::UNKNOWN, //RightOption, 0x3D
	morda::Key_e::RIGHT_CONTROL, //0x3E
	morda::Key_e::FUNCTION, //0x3F
	morda::Key_e::F17, //0x40
	morda::Key_e::UNKNOWN, //KeypadDecimal, 0x41, 65
	morda::Key_e::UNKNOWN, //0x42
	morda::Key_e::UNKNOWN, //KeypadMultiplym 0x43
	morda::Key_e::UNKNOWN, //0x44
	morda::Key_e::UNKNOWN, //KeypadPlus, 0x45
	morda::Key_e::UNKNOWN, //0x46, 70
	morda::Key_e::UNKNOWN, //KeypadClear, 0x47
	morda::Key_e::UNKNOWN, //VolumeUp, 0x48
	morda::Key_e::UNKNOWN, //VolumeDown, 0x49
	morda::Key_e::UNKNOWN, //Mute, 0x4A
	morda::Key_e::UNKNOWN, //KeypadDivide, 0x4B, 75
	morda::Key_e::UNKNOWN, //KeypadEnter, 0x4C
	morda::Key_e::UNKNOWN, //0x4D
	morda::Key_e::UNKNOWN, //KeypadMinus
	morda::Key_e::F18, //0x4F
	morda::Key_e::F19, //0x50, 80
	morda::Key_e::UNKNOWN, //KeypadEquals, 0x51
	morda::Key_e::UNKNOWN, //Keypad0
	morda::Key_e::UNKNOWN, //Keypad1
	morda::Key_e::UNKNOWN, //Keypad2
	morda::Key_e::UNKNOWN, //Keypad3, 85
	morda::Key_e::UNKNOWN, //Keypad4
	morda::Key_e::UNKNOWN, //Keypad5
	morda::Key_e::UNKNOWN, //Keypad6
	morda::Key_e::UNKNOWN, //Keypad7, 0x59
	morda::Key_e::F20, //0x5A, 90
	morda::Key_e::UNKNOWN, //Keypad8, 0x5B
	morda::Key_e::UNKNOWN, //Keypad9, 0x5C
	morda::Key_e::UNKNOWN, //0x5D
	morda::Key_e::UNKNOWN, //0x5E
	morda::Key_e::UNKNOWN, //0x5F, 95
	morda::Key_e::F5, //0x60
	morda::Key_e::F6, //0x61
	morda::Key_e::F7, //0x62
	morda::Key_e::F3, //0x63
	morda::Key_e::F8, //0x64, 100
	morda::Key_e::F9, //0x65
	morda::Key_e::UNKNOWN, //0x66
	morda::Key_e::F11, //0x67
	morda::Key_e::UNKNOWN, //0x68
	morda::Key_e::F13, //0x69
	morda::Key_e::F16, //0x6A
	morda::Key_e::F14, //0x6B
	morda::Key_e::UNKNOWN, //0x6C
	morda::Key_e::F10, //0x6D
	morda::Key_e::UNKNOWN, //0x6E
	morda::Key_e::F12, //0x6F
	morda::Key_e::UNKNOWN, //0x70
	morda::Key_e::F15, //0x71
	morda::Key_e::UNKNOWN, //Help, 0x72
	morda::Key_e::HOME, //0x73
	morda::Key_e::PAGE_UP, //0x74
	morda::Key_e::DELETE, //0x75
	morda::Key_e::F4, //0x76
	morda::Key_e::END, //0x77
	morda::Key_e::F2, //0x78
	morda::Key_e::PAGE_DOWN, //0x79
	morda::Key_e::F1, //0x7A
	morda::Key_e::LEFT, //0x7B
	morda::Key_e::RIGHT, //0x7C
	morda::Key_e::DOWN, //0x7D
	morda::Key_e::UP, //0x7E
	morda::Key_e::UNKNOWN, //0x7F
	morda::Key_e::UNKNOWN, //0x80
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, //0x90
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, //0xA0
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, //0xB0
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, //0xC0
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, //0xD0
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, //0xE0
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN, //0xF0
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN,
	morda::Key_e::UNKNOWN //0xFF
}};

}


int main (int argc, const char** argv){
	macosx_Main(argc, argv);

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
-(void)scrollWheel:(NSEvent*)e;

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
	mouseButton(e, true, morda::Widget::MouseButton_e::LEFT);
}

-(void)mouseUp: (NSEvent*)e{
//	TRACE(<< "left up!!!!!" << std::endl)
	mouseButton(e, false, morda::Widget::MouseButton_e::LEFT);
}

-(void)rightMouseDown: (NSEvent*)e{
//	TRACE(<< "right down!!!!!" << std::endl)
	mouseButton(e, true, morda::Widget::MouseButton_e::RIGHT);
}

-(void)rightMouseUp: (NSEvent*)e{
//	TRACE(<< "right up!!!!!" << std::endl)
	mouseButton(e, false, morda::Widget::MouseButton_e::RIGHT);
}

-(void)otherMouseDown: (NSEvent*)e{
//	TRACE(<< "middle down!!!!!" << std::endl)
	mouseButton(e, true, morda::Widget::MouseButton_e::MIDDLE);
}

-(void)otherMouseUp: (NSEvent*)e{
//	TRACE(<< "middle up!!!!!" << std::endl)
	mouseButton(e, false, morda::Widget::MouseButton_e::MIDDLE);
}

-(void)scrollWheel: (NSEvent*)e{
//	TRACE(<< "mouse wheel!!!!!" << std::endl)
	
	if([e hasPreciseScrollingDeltas] == NO){
		morda::Widget::MouseButton_e button;
//		TRACE(<< "dy = " << float(dy) << std::endl)
		if([e scrollingDeltaY] < 0){
			button = morda::Widget::MouseButton_e::WHEEL_DOWN;
		}else{
			button = morda::Widget::MouseButton_e::WHEEL_UP;
		}
//		TRACE(<< "button = " << unsigned(button) << std::endl)
		
		mouseButton(e, true, button);
		mouseButton(e, false, button);
	}else{
		TRACE(<< "mouse wheel: precise scrolling deltas, UNIMPLEMENTED!!!!!" << std::endl)
	}
}

-(void)mouseMoved: (NSEvent*)e{
//	TRACE(<< "mouseMoved event!!!!!" << std::endl)
	NSPoint pos = [e locationInWindow];
//	TRACE(<< "x = " << pos.x << std::endl)
	macosx_HandleMouseMove(
			morda::Vec2r(pos.x, pos.y).rounded(),
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
	macosx_HandleMouseHover(true);
}

-(void)mouseExited: (NSEvent*)e{
//	TRACE(<< "mouseExited event!!!!!" << std::endl)
	[[self window] setAcceptsMouseMovedEvents:NO];
	macosx_HandleMouseHover(false);
}

-(void)keyDown:(NSEvent*)e{
//	TRACE(<< "keyDown event!!!!!" << std::endl)
	std::uint8_t kc = [e keyCode];
	morda::Key_e key = keyCodeMap[kc];
	
	if([e isARepeat] == YES){
		macosx_HandleCharacterInput([e characters], key);
		return;
	}
	
	macosx_HandleKeyEvent(true, key);
	
	macosx_HandleCharacterInput([e characters], key);
}

-(void)keyUp:(NSEvent*)e{
//	TRACE(<< "keyUp event!!!!!" << std::endl)
	std::uint8_t kc = [e keyCode];
	macosx_HandleKeyEvent(false, keyCodeMap[kc]);
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

-(void)initStuff;

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
	
	[self initStuff];
	
	[self setShowsResizeIndicator:YES];
	[self setMinSize:NSMakeSize(0, 0)];
	[self setMaxSize:NSMakeSize(1000000000, 1000000000)];
	[self setIgnoresMouseEvents:NO];
	
	return self;
}

-(void)initStuff{
	[self makeFirstResponder:self->v];
	[self setDelegate:self];
	[self makeKeyWindow];
	[self makeMainWindow];
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
	macosx_UpdateWindowRect(morda::Rectr(0, 0, rect.size.width, rect.size.height));
}

-(NSSize)windowWillResize:(NSWindow*)sender toSize:(NSSize)frameSize{
	return frameSize;
}

-(BOOL)windowShouldClose:(id)sender{
	TRACE(<< "window wants to close!!!!" << std::endl)
	App::inst().quit();
	return NO;
}

-(BOOL)canBecomeKeyWindow{return YES;} //This is needed for window without title bar to be able to get key events
-(BOOL)canBecomeMainWindow{return YES;}
-(BOOL)acceptsFirstResponder{return YES;}

-(CocoaView*)view{return self->v;}

@end


App::ApplicationObject::ApplicationObject(){
	NSApplication *applicationObject = [NSApplication sharedApplication];
	this->id = applicationObject;
	
	if(!this->id){
		throw morda::Exc("morda::App::ApplicationObject::ApplicationObject(): failed to create application object");
	}
}

App::ApplicationObject::~ApplicationObject()noexcept{
	NSApplication *applicationObject = (NSApplication*)this->id;
	[applicationObject release];
}

App::WindowObject::WindowObject(const App::WindowParams& wp){
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

App::WindowObject::~WindowObject()noexcept{
	NSWindow* window = (CocoaWindow*)this->id;
	[window release];
}



App::OpenGLContext::OpenGLContext(const App::WindowParams& wp, void* window){
	CocoaWindow *wnd = (CocoaWindow*)window;
	
	std::vector<NSOpenGLPixelFormatAttribute> attributes;
	attributes.push_back(NSOpenGLPFAAccelerated);
	attributes.push_back(NSOpenGLPFAColorSize); attributes.push_back(24);
	if(wp.buffers.get(App::WindowParams::Buffer_e::DEPTH)){
		attributes.push_back(NSOpenGLPFADepthSize); attributes.push_back(16);
	}
	if(wp.buffers.get(App::WindowParams::Buffer_e::STENCIL)){
		attributes.push_back(NSOpenGLPFAStencilSize); attributes.push_back(8);
	}
	attributes.push_back(NSOpenGLPFADoubleBuffer);
	attributes.push_back(NSOpenGLPFASupersample);
	attributes.push_back(0);
	
	NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:&*attributes.begin()];
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

void App::OpenGLContext::Destroy()noexcept{
	NSOpenGLContext *openGLContext = (NSOpenGLContext*)this->id;
	[openGLContext release];
}


namespace{

morda::real getDotsPerInch(){
	NSScreen *screen = [NSScreen mainScreen];
	NSDictionary *description = [screen deviceDescription];
	NSSize displayPixelSize = [[description objectForKey:NSDeviceSize] sizeValue];
	CGSize displayPhysicalSize = CGDisplayScreenSize(
			[[description objectForKey:@"NSScreenNumber"] unsignedIntValue]
		);

	morda::real value = morda::real(((displayPixelSize.width * 10.0f / displayPhysicalSize.width) +
			(displayPixelSize.height * 10.0f / displayPhysicalSize.height)) / 2.0f);
	value *= 2.54f;
	return value;
}

morda::real getDotsPerPt(){
	NSScreen *screen = [NSScreen mainScreen];
	NSDictionary *description = [screen deviceDescription];
	NSSize displayPixelSize = [[description objectForKey:NSDeviceSize] sizeValue];
	CGSize displayPhysicalSize = CGDisplayScreenSize(
			[[description objectForKey:@"NSScreenNumber"] unsignedIntValue]
		);
	
	kolme::Vec2ui resolution(displayPixelSize.width, displayPixelSize.height);
	kolme::Vec2ui screenSizeMm(displayPhysicalSize.width, displayPhysicalSize.height);
	
	return morda::findDotsPerPt(resolution, screenSizeMm);
}

}//~namespace


App::App(const App::WindowParams& wp) :
		windowObject(wp),
		openGLContext(wp, windowObject.id),
		gui(getDotsPerInch(), getDotsPerPt())
{
	this->updateWindowRect(
			morda::Rectr(
					0,
					0,
					float(wp.dim.x),
					float(wp.dim.y)
				)
		);
}



void App::swapFrameBuffers(){
	NSOpenGLContext *openGLContext = (NSOpenGLContext*)this->openGLContext.id;
	[openGLContext flushBuffer];
}


void App::postToUiThread_ts(std::function<void()>&& f){	
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


void App::exec(){
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
		
		std::uint32_t millis = this->gui.update();
		
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

void App::setFullscreen(bool enable){
	if(enable == this->isFullscreen()){
		return;
	}
	
	CocoaWindow* window = (CocoaWindow*)this->windowObject.id;
	
	if(enable){
		//save old window size
		NSRect rect = [window frame];
		this->beforeFullScreenWindowRect.p.x = rect.origin.x;
		this->beforeFullScreenWindowRect.p.y = rect.origin.y;
		this->beforeFullScreenWindowRect.d.x = rect.size.width;
		this->beforeFullScreenWindowRect.d.y = rect.size.height;
		
		[window setStyleMask:([window styleMask] & (~(NSTitledWindowMask | NSResizableWindowMask)))];
		
		[window setFrame:[[NSScreen mainScreen] frame] display:YES animate:NO];
		[window setLevel:NSScreenSaverWindowLevel];
	}else{
		[window setStyleMask:([window styleMask] | NSTitledWindowMask | NSResizableWindowMask)];
		
		NSRect oldFrame;
		oldFrame.origin.x = this->beforeFullScreenWindowRect.p.x;
		oldFrame.origin.y = this->beforeFullScreenWindowRect.p.y;
		oldFrame.size.width = this->beforeFullScreenWindowRect.d.x;
		oldFrame.size.height = this->beforeFullScreenWindowRect.d.y;
		
		[window setFrame:oldFrame display:YES animate:NO];
		[window setLevel:NSNormalWindowLevel];
	}
	
	[window initStuff];
	
	this->isFullscreen_v = enable;
}



void App::setMouseCursorVisible(bool visible){
	if(visible){
		if(!this->mouseCursorIsCurrentlyVisible){
			[NSCursor unhide];
			this->mouseCursorIsCurrentlyVisible = true;
		}
	}else{
		if(this->mouseCursorIsCurrentlyVisible){
			[NSCursor hide];
			this->mouseCursorIsCurrentlyVisible = false;
		}
	}
}

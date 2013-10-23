#import <Cocoa/Cocoa.h>


@interface MordaApplication : NSApplication{
	bool shouldKeepRunning;
}

- (void)run;
- (void)terminate:(id)sender;

@end

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
//	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

//    NSDictionary *infoDictionary = [[NSBundle mainBundle] infoDictionary];
//    Class principalClass = NSClassFromString([infoDictionary objectForKey:@"NSPrincipalClass"]);
	NSApplication *applicationObject = [[MordaApplication alloc] init];//[principalClass sharedApplication];

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

	NSWindow* window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 200, 200)
		styleMask:NSTitledWindowMask backing:NSBackingStoreBuffered defer:NO];
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
	[applicationObject release];
//	[pool release];

	return 0;
}

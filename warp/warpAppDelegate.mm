//
//  warpAppDelegate.m
//  warp
//
//  Created by Nicholas Kostelnik on 11/09/2010.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "warpAppDelegate.h"
#import <Carbon/Carbon.h>

@implementation WarpAppDelegate

@synthesize window;

int port = 6345;

- (void)server_update {
	while (server->receive());
}

-(bool)connect_to:(NSString*)host port:(unsigned int)port {
	if (client->connec([[address stringValue] cStringUsingEncoding:NSASCIIStringEncoding], port))
	{
		[window makeKeyAndOrderFront:self];
		black_hole->attach();
		black_hole->send_input();
		[[NSApplication sharedApplication] activateIgnoringOtherApps : YES];
		
		return true;
	}	
	return false;
}

- (void)recent:(id)sender {
	NSMenuItem* menu_item = sender;
	
	[[recent_menu submenu]removeItem:menu_item];
	[[recent_menu submenu]addItem:menu_item];
	
	[self connect_to:[address stringValue] port:port];
}

- (IBAction)connect:(id)sender {
	[connect_window orderOut:self];
	
	if ([self connect_to:[address stringValue] port:port])
	{	
		NSMenuItem *empty_item = [[recent_menu submenu] itemWithTitle:@"Empty"];
		
		if (empty_item)
		{
			[[recent_menu submenu] removeItem:empty_item];
		}
		
		NSMenuItem *old_item = [[recent_menu submenu] itemWithTitle:[address stringValue]];
		
		if (old_item)
		{
			[[recent_menu submenu] removeItem:old_item];
		}
		
		[[recent_menu submenu] addItemWithTitle:[address stringValue] action:@selector(recent:) keyEquivalent:@""];
		
		if ([menu numberOfItems] > 5)
		{
			[menu removeItemAtIndex:0];
		}
	}
	else 
	{
		[connect_window makeKeyAndOrderFront:self]; 
	}
}

- (IBAction)quit:(id)sender {
	exit(0);
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {	
	
	server = new Server();
	server->start_listening(port);
	
	client = new Client();
	[input_view set_client:client];
	
	black_hole = new BlackHole(client, window);
	
	[NSThread detachNewThreadSelector:@selector(server_update) toTarget:self withObject:nil];
	
	NSStatusItem* statusItem = [[[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength] retain];
	[statusItem setImage:[NSImage imageNamed:@"menu"]];
	[statusItem setHighlightMode:YES];
	[statusItem setEnabled:YES];
	[statusItem setMenu:menu];
}

@end

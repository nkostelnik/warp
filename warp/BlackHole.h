/*
 *  BlackHole.h
 *  warp
 *
 *  Created by Nicholas Kostelnik on 14/09/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>

class BlackHole
{	
	
public:
	
	BlackHole(Client* client, NSWindow* window) : client_(client), window_(window) { };
	
	void send_input() 
	{ 
		CGAssociateMouseAndMouseCursorPosition(false);
		CGDisplayHideCursor(kCGDirectMainDisplay);
		client_->send_input();
	};
	
	void disable()
	{
		CGDisplayShowCursor(kCGDirectMainDisplay);
		CGAssociateMouseAndMouseCursorPosition(true);
		CGEventTapEnable(event_tap_, false);
		client_->disconnect();
		[window_ orderOut:nil];
	}
	
	Client* client() { return client_; };
	
	void attach()
	{
		CFRunLoopSourceRef runLoopSource;
		
		CGEventType eventType = 
		(1 << kCGEventMouseMoved) | 
		(1 << kCGEventKeyDown) | 
		(1 << kCGEventKeyUp) |
		(1 << kCGEventFlagsChanged) | 
		(1 << kCGEventLeftMouseUp) | 
		(1 << kCGEventLeftMouseDown) | 
		(1 << kCGEventLeftMouseDragged) |
		(1 << kCGEventRightMouseUp) | 
		(1 << kCGEventRightMouseDown) |
		(1 << kCGEventRightMouseDragged) |
		(1 << kCGEventScrollWheel);
		
		event_tap_ = CGEventTapCreate(kCGSessionEventTap,  kCGHeadInsertEventTap,  0, eventType, BlackHole::eventcallback, this);
		
		if(!event_tap_)
		{
			std::cerr << "failed to create event tap" << std::endl;
			exit(1);
		}
		
		runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, event_tap_, 0);
		CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
		CGEventTapEnable(event_tap_, true);
	}
	
private:
	
	Client* client_;
	CFMachPortRef event_tap_;
	NSWindow* window_;
	float last_click_;
	
	static CGEventRef eventcallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon)
	{
		BlackHole* black_hole = (BlackHole*)refcon;
		Client* client = black_hole->client();
		
		switch (type) {
				
			case kCGEventKeyDown:
			{
				CGEventFlags flags = CGEventGetFlags(event);
				
				CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
				
				std::clog << "key down" << keycode << std::endl;
				
				if ((flags & kCGEventFlagMaskShift) && (flags & kCGEventFlagMaskCommand) && keycode == 14)
				{
					black_hole->disable();
				}
				
				client->send_key_down(flags, keycode);
				NSLog(@"key down %d", keycode);
				return NULL;
				break;
			}
				
			case kCGEventKeyUp:
			{
				std::clog << "key up" << std::endl;
				CGEventFlags flags = CGEventGetFlags(event);
				CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
				client->send_key_up(flags, keycode);
				return NULL;
				break;
			}
				
			case kCGEventFlagsChanged:
			{
				std::clog << "flags changed" << std::endl;
				CGEventFlags flags = CGEventGetFlags(event);
				CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
				client->send_flags(keycode, flags);
				return NULL;
				break;
			}
				
			case kCGEventLeftMouseUp:
			{
				std::clog << "left mouse up" << std::endl;
				client->send_left_up();
				return NULL;
				break;
			}
				
			case kCGEventLeftMouseDown:
			{
				std::clog << "left mouse down" << std::endl;
				client->send_left_down();
				return NULL;
				break;
			}
				
			case kCGEventLeftMouseDragged:
			{
				std::clog << "left mouse dragged" << std::endl;
				int x = CGEventGetIntegerValueField(event, kCGMouseEventDeltaX);
				int y = CGEventGetIntegerValueField(event, kCGMouseEventDeltaY);
				client->send_left_dragged(x, y);
				return NULL;
				break;
			}
				
			case kCGEventRightMouseUp:
			{
				std::clog << "right mouse up" << std::endl;
				client->send_right_up();
				return NULL;
				break;
			}
				
			case kCGEventRightMouseDown:
			{
				std::clog << "right mouse down" << std::endl;
				client->send_right_down();
				return NULL;
				break;
			}
				
			case kCGEventRightMouseDragged:
			{
				std::clog << "right mouse dragged" << std::endl;
				int x = CGEventGetIntegerValueField(event, kCGMouseEventDeltaX);
				int y = CGEventGetIntegerValueField(event, kCGMouseEventDeltaY);
				client->send_right_dragged(x, y);
				return NULL;
				break;
			}
				
			case kCGEventMouseMoved:
			{
				std::clog << "mouse moved" << std::endl;				
				int x = CGEventGetIntegerValueField(event, kCGMouseEventDeltaX);
				int y = CGEventGetIntegerValueField(event, kCGMouseEventDeltaY);
				client->send_mouse_moved(x, y);
				std::clog << x << std::endl;
				return NULL;
				break;
			}
				
			case kCGEventScrollWheel:
			{
				std::clog << "scroll wheel" << std::endl;
				int x = CGEventGetIntegerValueField(event, kCGScrollWheelEventDeltaAxis2);
				int y = CGEventGetIntegerValueField(event, kCGScrollWheelEventDeltaAxis1);
				client->send_scroll_wheel(x, y);
				return NULL;
				break;
			}
		}
		
		return event;
	}
};
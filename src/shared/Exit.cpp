#include "Exit.h"

#include "Message.h"
#include "IExitCommand.hpp"

#ifdef _WIN32

	#include "WinExitCommands.hpp"
	#include "WinSocket.h"

	Exit::Exit() 
		: socket_(new WinSocket())
	{
		message_types_[MOUSE_MOVE]				= new MouseMovedCommand();
		message_types_[LEFT_UP]					= new LeftUpCommand();
		message_types_[LEFT_DOWN]				= new LeftDownCommand();
		message_types_[RIGHT_UP]				= new RightUpCommand();
		message_types_[RIGHT_DOWN]				= new RightDownCommand();
		message_types_[LEFT_DOUBLE_CLICK]		= new LeftDoubleClickCommand();
	}

#else

	#include "OSXExitCommands.hpp"
	#include "NixSocket.h"

	Exit::Exit() 
		: socket_(new NixSocket())	
	{		
		message_types_[LEFT_UP]						= new LeftUpCommand();
		message_types_[LEFT_DOWN]					= new LeftDownCommand();
		message_types_[RIGHT_UP]					= new RightUpCommand();
		message_types_[RIGHT_DOWN]				= new RightDownCommand();
		message_types_[KEY_UP]						= new KeyUpCommand();
		message_types_[KEY_DOWN]					= new KeyDownCommand();
		message_types_[MOUSE_MOVE]				= new MouseMovedCommand();
		message_types_[LEFT_DRAGGED]			= new LeftDraggedCommand();
		message_types_[RIGHT_DRAGGED]			= new RightDraggedCommand();
		message_types_[FLAGS_CHANGED]			= new FlagsChangedCommand();
		message_types_[SCROLL_WHEEL]			= new ScrollWheelCommand();
		message_types_[LEFT_DOUBLE_CLICK]	= new LeftDoubleClickCommand();
	}

#endif


void Exit::start_listening(unsigned int port)
{  
	socket_->listen_(port);
};
	
bool Exit::receive() 
{ 
	ISocket::received_data* datas = socket_->receive();
	
	if (!datas->empty())
	{		
		for (ISocket::received_data::iterator i = datas->begin(); i != datas->end(); ++i) 
		{			
			Message message;
			memcpy(&message, (*i), sizeof(Message));
			
			if (message_types_.find(message.type) != message_types_.end())
			{							
				message_types_[message.type]->Execute(message);
			}
		}
		
		socket_->dispose(datas);
	}
	
	return true;
};
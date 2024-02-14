#include <stdlib.h>

#include "datatypes.h"
#include "input.h"

#define KEYBOARD_DEVICE_NAME "keyboard.device"

/*
 * Protos
 */
static VOID exit_keyb_handler(VOID);

/*
 * Private objects
 */
static struct MsgPort* keyb_reply_port;
static struct IOStdReq* keyb_io_request;

/*
 * Public
 */
struct MsgPort* init_keyb(struct InputEvent* keyb_event)
{
	atexit(exit_keyb_handler);

	if(!(keyb_reply_port = CreateMsgPort()))
	{
		exit(EXIT_KEYB_CREATE_MSG_PORT);
	}

	if(!(keyb_io_request = (struct IOStdReq *) CreateIORequest(
		keyb_reply_port,
		sizeof(struct IOStdReq))))
	{
		exit(EXIT_KEYB_CREATE_IO_REQUEST);
	}

	if(OpenDevice(
		KEYBOARD_DEVICE_NAME,
		NULL,
		(struct IORequest *) keyb_io_request,
		NULL))
	{
		exit(EXIT_KEYB_OPEN_DEVICE);
	}

	keyb_io_request->io_Command = KBD_READEVENT;
	keyb_io_request->io_Data = (APTR) keyb_event;
	keyb_io_request->io_Length = sizeof(struct InputEvent);

	SendIO((struct IORequest *) keyb_io_request);

	return keyb_reply_port;
}

/*
 * Private
 */
static VOID exit_keyb_handler(VOID)
{
	if(keyb_io_request) {
		if(!CheckIO((struct IORequest *)keyb_io_request)) {
			AbortIO((struct IORequest *)keyb_io_request);
			WaitIO((struct IORequest *)keyb_io_request);
		}
		CloseDevice((struct IORequest *) keyb_io_request);
		DeleteIORequest((struct IORequest *) keyb_io_request);
	}

	if(keyb_reply_port) {
		struct Message* msg;
		while ((msg = GetMsg(keyb_reply_port)) != NULL) {
        ReplyMsg(msg);
    }
		DeleteMsgPort(keyb_reply_port);
	}
}

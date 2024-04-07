#include "input.h"

#include <devices/input.h>
#include <proto/alib.h>
#include <proto/exec.h>

#include <stdlib.h>

/*
 * Private objects
 */
static IOStdReq* input_device_io_request;
static MsgPort* input_device_msg_port;
static Interrupt handler_info;


/*
 * Private protos
 */
static void exit_handler(void);
static ULONG __regargs input_handler(
	InputEvent* event_chain,
	MsgPort* receiver_port);

/*
 * Public functions
 */
MsgPort* setup_input_handler(void)
{
	MsgPort* receiver_port = NULL;

	atexit(exit_handler);

	if(!(input_device_msg_port = CreatePort(NULL, 0)))
	{
		exit(EXIT_INPUT_CREATE_PORT);
	}

	if(!(input_device_io_request =
		(IOStdReq*) CreateExtIO(input_device_msg_port, sizeof(IOStdReq))))
	{
		exit(EXIT_INPUT_CREATE_EXTIO);
	}

	if(OpenDevice(
		"input.device",
		NULL,
		(IORequest*) input_device_io_request,
		NULL))
	{
		exit(EXIT_INPUT_OPEN_DEVICE);
	}

	if(!(receiver_port = CreatePort(NULL, 0)))
	{
		exit(EXIT_INPUT_CREATE_PORT);
	}

	handler_info.is_Code = (void(*)()) input_handler;
	handler_info.is_Data = (APTR) receiver_port;
	handler_info.is_Node.ln_Pri = 127;
	handler_info.is_Node.ln_Name = "app_input_handler";

	input_device_io_request->io_Data = (APTR) &handler_info;
	input_device_io_request->io_Command = IND_ADDHANDLER;

	if(DoIO((IORequest*) input_device_io_request))
	{
		exit(EXIT_INPUT_DO_IO);
	}

	input_device_io_request->io_Command = CMD_FLUSH;

	if(DoIO((IORequest*) input_device_io_request))
	{
		exit(EXIT_INPUT_DO_IO);
	}

	return receiver_port;
}

/*
 * Private functions
 */
static ULONG __regargs input_handler(
	InputEvent* event_chain,
	MsgPort* receiver_msg_port)
{
	InputEvent* current = event_chain;

	while(current)
	{
		if(current->ie_Class == IECLASS_RAWKEY ||
			current->ie_Class == IECLASS_RAWMOUSE)
		{
			CustomInputEvent* msg = AllocMem(
				sizeof(CustomInputEvent),
				MEMF_PUBLIC | MEMF_CLEAR);

			msg->message.mn_Node.ln_Type = NT_MESSAGE;
      msg->message.mn_Length = sizeof(CustomInputEvent);
      msg->class = current->ie_Class;
      msg->sub_class = current->ie_SubClass;
      msg->code = current->ie_Code;
      msg->qualifier = current->ie_Qualifier;

      if(current->ie_Class == IECLASS_RAWMOUSE)
      {
        msg->x = current->ie_X;
        msg->y = current->ie_Y;
      }

      PutMsg(receiver_msg_port, (struct Message*) msg);
		}

		current = current->ie_NextEvent;
	}


	return (ULONG) NULL;
}

static void exit_handler(void)
{
	if(input_device_io_request) {
		input_device_io_request->io_Data = (APTR) &handler_info;
		input_device_io_request->io_Command = IND_REMHANDLER;

		DoIO((IORequest*) input_device_io_request);

		if(CheckIO((IORequest *) input_device_io_request) == NULL) {
			AbortIO((IORequest *) input_device_io_request);
		}
		WaitIO((IORequest *) input_device_io_request);
		CloseDevice((IORequest *) input_device_io_request);
		DeleteExtIO((IORequest *) input_device_io_request);
	}

	if(input_device_msg_port)
	{
		struct Message* msg;
		while ((msg = GetMsg(input_device_msg_port)) != NULL) {
        ReplyMsg(msg);
    }
		DeletePort(input_device_msg_port);
	}
}

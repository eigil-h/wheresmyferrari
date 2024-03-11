#include "input.h"
#include <proto/alib.h>
#include <proto/exec.h>
#include <devices/input.h>

#include <stdlib.h>

/*
 * Private objects
 */
static IOStdReq* io_req;
static MsgPort* msg_port;
static Interrupt handler;


/*
 * Private protos
 */
static ULONG input_handler(void);
static void exit_handler(void);

/*
 * Public functions
 */
void open_input_device(void)
{
	atexit(exit_handler);

	if(!(msg_port = CreatePort(NULL, 0)))
	{
		exit(EXIT_INPUT_CREATE_PORT);
	}

	if(!(io_req = (IOStdReq*) CreateExtIO(msg_port, sizeof(IOStdReq))))
	{
		exit(EXIT_INPUT_CREATE_EXTIO);
	}

	if(OpenDevice(
		"input.device",
		NULL,
		(IORequest*) io_req,
		NULL))
	{
		exit(EXIT_INPUT_OPEN_DEVICE);
	}
}


void add_input_handler(void)
{
	handler.is_Code = (void(*)()) input_handler;
	handler.is_Data = NULL;
	handler.is_Node.ln_Pri = 100;
	handler.is_Node.ln_Name = "app_inph";

	io_req->io_Data = (APTR) &handler;
	io_req->io_Command = IND_ADDHANDLER;

	DoIO((IORequest*) io_req);
}

/*
 * Private functions
 */
static ULONG input_handler(void)
{
	return NULL;
}

static void exit_handler(void)
{
	io_req->io_Data = (APTR) &handler;
	io_req->io_Command = IND_REMHANDLER;

	DoIO((IORequest*) io_req);
	
	if(msg_port)
	{
		DeletePort(msg_port);
	}
}

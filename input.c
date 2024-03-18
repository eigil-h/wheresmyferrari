#include "input.h"
#include <proto/alib.h>
#include <proto/exec.h>
#include <devices/input.h>
#include <devices/timer.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Private objects
 */
static IOStdReq* io_req;
static MsgPort* msg_port;
static Interrupt handler;


typedef struct {
	InputEvent ievent[200];
	int counter;
} HandlerData;

HandlerData handler_data;


/*
 * Private protos
 */
static ULONG __regargs input_handler(InputEvent* event_chain, HandlerData* hd);
static void remove_input_handler(void);
static void exit_handler(void);
static char* map_class(UBYTE class);

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


void add_input_handler()
{
	handler.is_Code = (void(*)()) input_handler;
	handler.is_Data = (APTR) &handler_data;
	handler.is_Node.ln_Pri = 127;
	handler.is_Node.ln_Name = "app_inph";

	io_req->io_Data = (APTR) &handler;
	io_req->io_Command = IND_ADDHANDLER;

	DoIO((IORequest*) io_req);
}

void disable_keyrepeat(void)
{
	MsgPort* mport;

	if(mport = CreatePort(NULL, 0))
	{
		Message* msg;
		TimeReq* treq;

		if(treq = (TimeReq*) CreateExtIO(mport, sizeof(TimeReq)))
		{
			if(!OpenDevice(
				"input.device",
				NULL,
				(IORequest*) treq,
				NULL))
			{
				treq->tr_time.tv_secs = -1L;
				treq->tr_time.tv_micro = 0L;
				treq->tr_node.io_Command = IND_SETTHRESH;

				DoIO((struct IORequest *) treq);
			}

			if(CheckIO((IORequest *) treq) == NULL) {
				AbortIO((IORequest *) treq);
			}
			WaitIO((IORequest *) treq);
			CloseDevice((IORequest *) treq);
			DeleteExtIO((IORequest *) treq);
		}

		while((msg = GetMsg(mport)) != NULL) {
    	ReplyMsg(msg);
    }
		DeletePort(mport);
	}
}

void print_it(void)
{
	int i;

	for(i = 0; i < handler_data.counter ; i++)
	{
		printf("%s, code 0x%04x, p1dc 0x%02x, p1dq 0x%02x, p2dc 0x%02x, p2dq 0x%02x, ts %d\n",
			map_class(handler_data.ievent[i].ie_Class),
			handler_data.ievent[i].ie_Code,
			handler_data.ievent[i].ie_Prev1DownCode,
			handler_data.ievent[i].ie_Prev1DownQual,
			handler_data.ievent[i].ie_Prev2DownCode,
			handler_data.ievent[i].ie_Prev2DownQual,
			handler_data.ievent[i].ie_TimeStamp);
	}

	puts("");
}


/*
 * Private functions
 */
static void remove_input_handler(void)
{
	if(io_req)
	{
		io_req->io_Data = (APTR) &handler;
		io_req->io_Command = IND_REMHANDLER;

		DoIO((IORequest*) io_req);
	}
}

static ULONG __regargs input_handler(
	InputEvent* event_chain,
	HandlerData* hd)
{
	InputEvent* current = event_chain;

	while(current)
	{
		if(current->ie_Class == IECLASS_RAWKEY
		//	|| current->ie_Class == IECLASS_RAWMOUSE
		)
		{
			if(hd->counter < 200)
				memcpy(&hd->ievent[hd->counter++], current, sizeof(InputEvent));
		}

		current = current->ie_NextEvent;
	}


	return (ULONG) NULL;
}

static void exit_handler(void)
{
	remove_input_handler();

	if(io_req) {
		if(CheckIO((IORequest *)io_req) == NULL) {
			AbortIO((IORequest *)io_req);
		}
		WaitIO((IORequest *)io_req);
		CloseDevice((IORequest *)io_req);
		DeleteExtIO((IORequest *)io_req);
	}

	if(msg_port)
	{
		struct Message* msg;
		while ((msg = GetMsg(msg_port)) != NULL) {
        ReplyMsg(msg);
    }
		DeletePort(msg_port);
	}
}

/*
 * Temp utils
 */
static char* map_class(UBYTE class)
{
	switch(class)
	{
		case IECLASS_RAWKEY:
			return "RAWKEY";
		case IECLASS_RAWMOUSE:
			return "RAWMOUSE";
		case IECLASS_POINTERPOS:
			return "POINTERPOS";
		case IECLASS_TIMER:
			return "TIMER";
		default:
			return "other";
	}
}

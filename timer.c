#include <stdlib.h>
#include <proto/alib.h>
#include <proto/exec.h>
#include <devices/timer.h>
#include "timer.h"
#include "datatypes.h"

/*
 * Private objects
 */
struct Library* TimerBase;
TimeRequest_Type* timer_io_request;
TimeVal_Type* time_val;

/*
 * Private protos
 */
static void exit_handler(void);

/*
 * Public functions
 */
VOID open_timer_device(VOID)
{
	atexit(exit_handler);

	if(!(timer_io_request = (TimeRequest_Type*) AllocMem(
		sizeof(TimeRequest_Type), MEMF_PUBLIC | MEMF_CLEAR)))
	{
		exit(EXIT_TIMER_MEMORY);
	}

	if(OpenDevice(
		TIMERNAME,
		UNIT_VBLANK,
		(IORequest*) timer_io_request,
		NULL))
	{
		exit(EXIT_TIMER_OPEN_DEVICE);
	}

	TimerBase = (struct Library *) timer_io_request->tr_node.io_Device;


	if(!(time_val = (TimeVal_Type*) AllocMem(sizeof(TimeVal_Type),
		MEMF_PUBLIC | MEMF_CLEAR)))
	{
		exit(EXIT_TIMER_MEMORY);
	}
}

/*
 * Private functions
 */
static void exit_handler(void)
{
	if(time_val)
	{
		FreeMem(time_val, sizeof(TimeVal_Type));
	}

	if(timer_io_request)
	{
		if(TimerBase)
			CloseDevice((IORequest *) timer_io_request);

		FreeMem(timer_io_request, sizeof(TimeRequest_Type));
	}
}

#ifndef APP_INPUT_H
#define APP_INPUT_H

#include "datatypes.h"
#include <devices/inputevent.h>

typedef struct IORequest IORequest;
typedef struct IOStdReq IOStdReq;
typedef struct InputEvent InputEvent;

#define EXIT_INPUT 0xB0
#define EXIT_INPUT_CREATE_PORT EXIT_INPUT
#define EXIT_INPUT_CREATE_EXTIO EXIT_INPUT + 1
#define EXIT_INPUT_OPEN_DEVICE EXIT_INPUT + 2
#define EXIT_INPUT_DO_IO EXIT_INPUT + 3

MsgPort* setup_input_handler(void);

#endif

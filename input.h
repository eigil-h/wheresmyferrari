#ifndef APP_INPUT_H
#define APP_INPUT_H

#include <exec/io.h>
#include <exec/ports.h>
#include <devices/inputevent.h>

typedef struct IORequest IORequest;
typedef struct IOStdReq IOStdReq;
typedef struct timerequest TimeReq;
typedef struct MsgPort MsgPort;
typedef struct Message Message;
typedef struct Interrupt Interrupt;
typedef struct InputEvent InputEvent;

#define EXIT_INPUT 0xB0
#define EXIT_INPUT_CREATE_PORT EXIT_INPUT
#define EXIT_INPUT_CREATE_EXTIO EXIT_INPUT + 1
#define EXIT_INPUT_OPEN_DEVICE EXIT_INPUT + 2

void open_input_device(void);
void add_input_handler(void);
void disable_keyrepeat(void);
void print_it(void);

#endif

#ifndef APP_TIMER_H
#define APP_TIMER_H

#include "datatypes.h"

#define EXIT_TIMER 0xC0
#define EXIT_TIMER_MEMORY EXIT_TIMER
#define EXIT_TIMER_OPEN_DEVICE EXIT_TIMER + 1

VOID open_timer_device(VOID);

#endif

#ifndef APP_SCREEN_H
#define APP_SCREEN_H

#include <intuition/screens.h>

struct Screen* open_screen(struct BitMap*);
void close_screen(void);

#endif

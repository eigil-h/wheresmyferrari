#ifndef WINDOW_H
#define WINDOW_H

#include <intuition/intuition.h>

struct Window* open_window(struct Screen*);
void close_window(void);

#endif

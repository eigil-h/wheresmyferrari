#ifndef INPUT_H
#define INPUT_H

#include <proto/exec.h>
#include <devices/inputevent.h>
#include <devices/keyboard.h>

struct MsgPort* init_keyb(VOID);
VOID requestKeybEvent(struct InputEvent*);
BOOL pokeKeybEvent(VOID);

#endif

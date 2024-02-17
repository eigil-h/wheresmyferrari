#ifndef INPUT_H
#define INPUT_H

#include <proto/exec.h>

struct MsgPort* init_keyb(VOID);
VOID requestKeybEvent(VOID);
BOOL pokeKeybEvent(VOID);

#endif

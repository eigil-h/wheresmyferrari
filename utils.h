#ifndef APP_UTILS_H
#define APP_UTILS_H

#include <proto/graphics.h>
#include "datatypes.h"

/* V36 AllocBitMap
 */
struct BitMap* alloc_bitmap(UWORD, UWORD, UBYTE, Error*);
void free_bitmap(struct BitMap*, UWORD, UWORD);

BOOL init_library_version(CONST_STRPTR name, LibraryVersion*);

#endif

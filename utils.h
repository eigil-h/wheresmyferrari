#ifndef APP_UTILS_H
#define APP_UTILS_H

#include "datatypes.h"

#define BYTES_PER_ROW(w) ((((ULONG)(w) + 15) >> 3) & 0xFFFE)

PictureData* alloc_picture_data(UWORD width, UWORD height, UBYTE depth);
VOID free_picture_data(PictureData*);

BitMap* alloc_init_bitmap(PictureData*, Error*);
void free_bitmap(BitMap*, UWORD, UWORD);

UWORD* palette32ToRGB4(Palette32*);

int os_version(void);

#endif

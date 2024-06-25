#ifndef APP_UTILS_H
#define APP_UTILS_H

#include "datatypes.h"

#define BYTES_PER_ROW(w) ((((ULONG)(w) + 15) >> 3) & 0xFFFE)

PictureData* alloc_picture_data(UWORD width, UWORD height, UBYTE depth);
VOID free_picture_data(PictureData**);

BitMap* alloc_init_bitmap(PictureData*, Error*);
VOID free_bitmap(BitMap*, UWORD, UWORD);

UWORD* palette32To4Data(Palette32*);
ULONG* palette32ConcatData(Palette32* src1, Palette32* src2);
VOID print_palette32(Palette32*);
VOID print_palette4(Palette4*);

VOID free_view_request(ViewRequest*);

int os_version(void);

#endif

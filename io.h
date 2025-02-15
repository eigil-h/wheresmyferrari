#ifndef APP_IO_H
#define APP_IO_H

#include "datatypes.h"

#define INVALID_FORMAT 1
#define UNKNOWN_FORMAT 2
#define ERROR_ALLOC_IFF 3
#define ERROR_OPEN_FILE 4

typedef struct
{
	BitMapHeader* bmhd;
	UBYTE* cmap;
	ULONG cmap_size;
	UBYTE* body;
	ULONG body_size;
} ILBMData;

BOOL load_picture(CONST_STRPTR path, PictureData*);

#endif

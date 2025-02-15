#ifndef APP_DATATYPES
#define APP_DATATYPES

#include <exec/types.h>
#include <exec/io.h>
#include <exec/ports.h>
#include <graphics/gfx.h>
#include <libraries/iffparse.h>
#include <datatypes/pictureclass.h>


typedef struct MsgPort MsgPort;
typedef struct Message Message;
typedef struct Interrupt Interrupt;
typedef struct IFFHandle IFFHandle;
typedef struct IORequest IORequest;
typedef struct IOStdReq IOStdReq;
typedef struct ViewPort ViewPort;
typedef struct BitMap BitMap;
typedef struct BitMapHeader BitMapHeader;
typedef struct SimpleSprite SimpleSprite;

typedef ULONG* Palette32; //[length][data][terminator]

typedef struct
{
	UWORD width, height;
	UBYTE depth, pad;
	BOOL interleaved;
	BitMap* bitmap;
	Palette32 palette;
} PictureData;

typedef struct
{
	Message message;
	UBYTE class;
	UBYTE sub_class;
	UWORD code;
	UWORD qualifier;
	WORD x;
	WORD y;
} CustomInputEvent;

typedef struct
{
	BYTE h_val;
	BYTE v_val;
	BOOL enter;
} InputState;

#endif

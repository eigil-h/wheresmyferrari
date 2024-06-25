#ifndef APP_DATATYPES
#define APP_DATATYPES

#include <exec/types.h>
#include <exec/io.h>
#include <exec/ports.h>
#include <graphics/gfx.h>

typedef struct MsgPort MsgPort;
typedef struct Message Message;
typedef struct Interrupt Interrupt;
typedef struct ViewPort ViewPort;
typedef struct BitMap BitMap;

typedef struct
{
	UWORD* data;
	WORD length;
} Palette4;

typedef struct
{
	ULONG* data;
	ULONG length;
} Palette32;

typedef struct
{
	UWORD width;
	UWORD height;
	UBYTE depth;
	UBYTE pad;
	BOOL interleaved;
	UBYTE* data;
	Palette32 palette;
} PictureData;

typedef struct
{
  UWORD width;
  UWORD height;
  UBYTE depth;
  UBYTE pad[3];
  Palette4 palette4;
  UWORD pad1;
  BitMap* bg_bitmap;
} ViewRequest;

typedef struct
{
	ULONG code;
	STRPTR msg;
} Error;

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

enum exit_code
{
	EXIT_OPENSCREEN = 1000,
	EXIT_BAD_BG_PICTURE,
	EXIT_CREATE_PORT,
	EXIT_KEYB_CREATE_MSG_PORT,
	EXIT_KEYB_CREATE_IO_REQUEST,
	EXIT_KEYB_OPEN_DEVICE
};

#endif

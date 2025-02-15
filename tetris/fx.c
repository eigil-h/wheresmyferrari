#include <stdlib.h>
#include <stdio.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#include "/datatypes.h"
#include "/io.h"
#include "/view.h"
#include "tetromino.h"
#include "game.h"
#include "fx.h"

/*
 * Protos
 */
static BOOL init(VOID);
static VOID exit_handler(VOID);

/*
 * Private objects
 */
static ViewPort* viewport;
static PictureData bg_pic;
static PictureData brick_pic;
static ViewRequest view_request;

/*
 * Public functions
 */
LONG prepare_fx(VOID)
{
	atexit(exit_handler);

	if(init())
	{
		BOOL bg_ok = load_picture("TetBG", &bg_pic),
			brick_ok = load_picture("brick", &brick_pic);

		if(bg_ok && brick_ok)
		{
			BltBitMap(
				bg_pic.bitmap,
				0,
				0,
				view_request.viewport_bitmap,
				0,
				0,
				view_request.width,
				view_request.height,
				0xC0,
				0x0F,
				NULL
			);

			if(viewport = make_view(&view_request))
			{
				brick_pic.palette[0] |= 16; // load colors 16-19
				LoadRGB32(viewport, brick_pic.palette);

				brick_pic.palette[0] &= 0xFFFF0000;
				brick_pic.palette[0] |= 20; // load colors 20-23
				LoadRGB32(viewport, brick_pic.palette);

				if(init_tetrominos(viewport, &brick_pic))
				{
					init_game(viewport);
					return 0;
				}
			}
		}
	}

	return -1;
}

static BOOL init(VOID)
{
	view_request.width = VIEW_WIDTH;
	view_request.height = VIEW_HEIGHT;
	view_request.depth = VIEW_DEPTH;

	if(!(view_request.viewport_bitmap = AllocBitMap(
		VIEW_WIDTH,
		VIEW_HEIGHT,
		VIEW_DEPTH,
		BMF_INTERLEAVED | BMF_DISPLAYABLE | BMF_CLEAR,
		NULL)))
	{
		return FALSE;
	}

	bg_pic.width = BG_WIDTH;
	bg_pic.height = BG_HEIGHT;
	bg_pic.depth = BG_DEPTH;
	bg_pic.interleaved = TRUE;

	if(!(bg_pic.bitmap = AllocBitMap(
		BG_WIDTH,
		BG_HEIGHT,
		BG_DEPTH,
		BMF_INTERLEAVED | BMF_DISPLAYABLE,
		view_request.viewport_bitmap)))
	{
		return FALSE;
	}

	if(!(bg_pic.palette = AllocVec(sizeof(ULONG) * ((3 * (1 << BG_DEPTH) + 2)), NULL)))
	{
		return FALSE;
	}

	view_request.palette = bg_pic.palette;

	brick_pic.width = BRICK_WIDTH;
	brick_pic.height = BRICK_HEIGHT;
	brick_pic.depth = BRICK_DEPTH;
	brick_pic.interleaved = TRUE;

	if(!(brick_pic.bitmap = AllocBitMap(
		BRICK_WIDTH,
		BRICK_HEIGHT,
		BRICK_DEPTH,
		BMF_INTERLEAVED,
		NULL)))
	{
		return FALSE;
	}

	if(!(brick_pic.palette = AllocVec(sizeof(ULONG) * ((3 * (1 << BRICK_DEPTH) + 2)), NULL)))
	{
		return FALSE;
	}

	return TRUE;
}

static VOID exit_handler(VOID)
{
	WaitBlit();

	if(brick_pic.palette) FreeVec(brick_pic.palette);
	if(brick_pic.bitmap) FreeBitMap(brick_pic.bitmap);
	if(bg_pic.palette) FreeVec(bg_pic.palette);
	if(bg_pic.bitmap) FreeBitMap(bg_pic.bitmap);
	if(view_request.viewport_bitmap) FreeBitMap(view_request.viewport_bitmap);
}

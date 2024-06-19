#include <proto/graphics.h>
#include <proto/exec.h>
#include <stdlib.h>
#include <string.h>
#include "game_tetris.h"
#include "utils.h"

/*
 * Private objects
 */
static ViewPort* viewport;
static struct SimpleSprite tetromino_sprite[NUM_SPRITES];
static BrickImage __chip brick_image[NUM_SPRITES];
static UWORD brick_bitmap[BRICK_LEN];
static GameState game_state = GS_BEFORE;
static TetraminoRenderer Tetramino[7][4];
static Position current_position;

/*
 * Private protos
 */
static BOOL validate_loaded_data(PictureData* bg, PictureData* brick);
static void exit_handler(void);
static void init_sprites(void);
static void render_sprite(UBYTE sp, UBYTE h, UBYTE x, UBYTE y);
static void render_T0(UBYTE x, UBYTE y);
static void render_T1(UBYTE x, UBYTE y);
static void render_T2(UBYTE x, UBYTE y);
static void render_T3(UBYTE x, UBYTE y);

/*
 * Public functions
 */
void init_game(ViewPort* vp)
{
	atexit(exit_handler);

	viewport = vp;

	init_sprites();

	Tetramino[0][0] = render_T0;
	Tetramino[0][1] = render_T1;
	Tetramino[0][2] = render_T2;
	Tetramino[0][3] = render_T3;
}

void render_frame(InputState* input_state)
{
	switch(game_state)
	{
		case GS_BEFORE:
			Tetramino[0][0](current_position.x_pos, current_position.y_pos);
			game_state = GS_PLAY;
			break;

		case GS_PLAY:
			current_position.x_pos += input_state->h_val;
			current_position.y_pos += input_state->v_val;
			Tetramino[0][0](current_position.x_pos, current_position.y_pos);
			break;

		case GS_PAUSE:
			break;

		case GS_AFTER:
			break;
	}
}

ViewRequest* prepare_gfx(PictureData* bg, PictureData* brick, Error* err)
{
	ViewRequest* view_request = NULL;

	if(validate_loaded_data(bg, brick))
	{
		int h;
		UWORD* bbm;
		UBYTE* bd;
		ULONG tmp_brick_palette_data[16];
		Palette32 tmp_brick_palette;
		Palette32 tmp_palette;

		memcpy(tmp_brick_palette_data, brick->palette.data, 4 * sizeof(ULONG));
		memcpy(&tmp_brick_palette_data[4], brick->palette.data, 4 * sizeof(ULONG));
		memcpy(&tmp_brick_palette_data[8], brick->palette.data, 4 * sizeof(ULONG));
		memcpy(&tmp_brick_palette_data[12], brick->palette.data, 4 * sizeof(ULONG));

		tmp_brick_palette.data = tmp_brick_palette_data;
		tmp_brick_palette.length = 16;

		tmp_palette.data = palette32ConcatData(&bg->palette, &tmp_brick_palette);
		tmp_palette.length = bg->palette.length + 16;

		view_request = AllocMem(sizeof(ViewRequest), NULL);

		if(view_request)
		{
			view_request->width = bg->width;
			view_request->height = bg->height;
			view_request->depth = bg->depth;
			view_request->palette4.data = palette32To4Data(&tmp_palette);
			view_request->palette4.length = tmp_palette.length;
			view_request->bg_bitmap = alloc_init_bitmap(bg, err);
		}

		if(tmp_palette.data)
			FreeMem(tmp_palette.data, sizeof(ULONG) * tmp_palette.length);

		// init brick_bitmap[BRICK_LEN], todo -> alloc_init_sprite
		bbm = brick_bitmap;
		bd = brick->data;
		for(h = 0; h < brick->height; h++)
		{
			*bbm++ = (*bd++) << 8 | *bd++;
			*bbm++ = (*bd++) << 8 | *bd++;
			bd += 2 * (brick->depth - 2);
		}
	}
	else
	{
		err->code = 13;
		err->msg = "Bad files";
	}

	return view_request;
}

/*
 * Private functions
 */
static BOOL validate_loaded_data(PictureData* bg, PictureData* brick)
{
	return (BOOL) (bg->width == 320 &&
		bg->height == 512 &&
		bg->depth == 4 &&
		brick->width == 16 &&
		brick->height == 16 &&
		brick->depth >= 2);
}

static void init_sprites(void)
{
	int i;
	for(i = 0; i < NUM_SPRITES; i++)
	{
		if(GetSprite(&tetromino_sprite[i], i) != i)
			exit(-1);

		memcpy(brick_image[i].data, brick_bitmap, BRICK_SIZ);
		memcpy(&brick_image[i].data[BRICK_LEN], brick_bitmap, BRICK_SIZ);
		memcpy(&brick_image[i].data[2 * BRICK_LEN], brick_bitmap, BRICK_SIZ);
		memcpy(&brick_image[i].data[3 * BRICK_LEN], brick_bitmap, BRICK_SIZ);

		ChangeSprite(viewport, &tetromino_sprite[i], (void*) &brick_image[i]);
	}
}

static void render_sprite(UBYTE sp, UBYTE h, UBYTE x, UBYTE y)
{
	tetromino_sprite[sp].height = h * BRICK_HEIGHT;
	MoveSprite(
		viewport,
		&tetromino_sprite[sp],
		(x * BRICK_WIDTH) << 1, 
		(y * BRICK_HEIGHT) << 1
	);
}

static void render_T0(UBYTE x, UBYTE y)
{
	render_sprite(0, 1, x, y);
	render_sprite(1, 2, x+1, y);
	render_sprite(2, 1, x+2, y);
}

static void render_T1(UBYTE x, UBYTE y)
{
	render_sprite(0, 3, x, y);
	render_sprite(1, 1, x+1, y+1);
}

static void render_T2(UBYTE x, UBYTE y)
{
	render_sprite(0, 1, x, y+1);
	render_sprite(1, 2, x+1, y);
	render_sprite(2, 1, x+2, y+1);
}

static void render_T3(UBYTE x, UBYTE y)
{
	render_sprite(0, 1, x, y+1);
	render_sprite(1, 3, x+1, y);
}

static void exit_handler(void)
{
	int i = NUM_SPRITES;
	while(--i >= 0)
	{
		FreeSprite(i);
	}
}

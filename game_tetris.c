#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/timer.h>
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
static Tetromino current = {{3,1}, {0}, {0}};
static TimeVal_Type prev_tick;
static TimeVal_Type prev_move;
static const TimeVal_Type move_tv = {0L, 200000};
static Game game;
static const Level level[] = {
	{{1L, 0L}, 1000},
	{{0L, 800000}, 2000},
	{{0L, 650000}, 3000},
	{{0L, 500000}, 4000}
};
static const TetrominoData tetromino[] = {
	{{
		{{{1,1},{1,2},{1,1},{0,0}},0,2},
		{{{1,1},{0,3},{0,0},{0,0}},0,1},
		{{{1,1},{0,2},{1,1},{0,0}},0,2},
		{{{0,0},{0,3},{1,1},{0,0}},1,2}
	}}
};

/*
 * Private protos
 */
static BOOL validate_loaded_data(PictureData* bg, PictureData* brick);
static VOID exit_handler(VOID);
static VOID init_sprites(VOID);
static VOID render_sprite(UBYTE sp, UBYTE h, UBYTE x, UBYTE y);
static VOID render_tetromino(VOID);

/*
 * Public functions
 */
void init_game(ViewPort* vp)
{
	atexit(exit_handler);

	viewport = vp;

	init_sprites();
}

void render_frame(InputState* input_state)
{
	TimeVal_Type ts;

	switch(game_state)
	{
		case GS_BEFORE:
			render_tetromino();
			game_state = GS_PLAY;
			break;

		case GS_PLAY:
			if(input_state->h_val != 0 ||
				input_state->v_val != 0 ||
				input_state->enter != 0)
			{
				GetSysTime(&ts);
				SubTime(&ts, &prev_move);

				if(CmpTime(&move_tv, &ts) > 0)
				{
					// horizontal move
					current.p.x += input_state->h_val;

					// rotation
					if(input_state->v_val < 0)
					{
						current.o -= input_state->v_val;
						current.o &= 3;
					}
					// vertical move
					else if(input_state->v_val > 0)
					{
						current.p.y += input_state->v_val;
						if(current.p.y >= PLG_HEIGHT)
							current.p.y -= 1;
					}

					// wall collision
					if(current.p.x + tetromino[current.t].o[current.o].first < 0)
					{
						current.p.x += 1;
					}
					else if(current.p.x + tetromino[current.t].o[current.o].last >=
						PLG_WIDTH)
					{
						current.p.x -= 1;
					}

					render_tetromino();

					AddTime(&prev_move, &ts);
				}
			}
			else
			{
				prev_move.tv_secs = 0L;
				prev_move.tv_micro = 0L;
			}

			GetSysTime(&ts);
			SubTime(&ts, &prev_tick);

			if(CmpTime(&level[game.level].speed, &ts) > 0)
			{
				current.p.y += 1;
				if(current.p.y >= PLG_HEIGHT)
					current.p.y -= 1;

				render_tetromino();

				AddTime(&prev_tick, &ts);
			}

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

static VOID render_tetromino(VOID)
{
	int i;
	for(i = 0; i < 4; i++)
	{
		render_sprite(i,
			tetromino[current.t].o[current.o].c[i].height,
			current.p.x + i,
			current.p.y + tetromino[current.t].o[current.o].c[i].start
		);
	}
}

static void exit_handler(void)
{
	int i = NUM_SPRITES;
	while(--i >= 0)
	{
		FreeSprite(i);
	}
}

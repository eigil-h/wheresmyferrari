#include "game_tetris.h"
#include <proto/graphics.h>
#include <stdlib.h>
#include <string.h>

/*
 * Private objects
 */
static ViewPort* viewport;
static struct SimpleSprite tetromino_sprite[NUM_SPRITES];
static BrickImage __chip brick_image[NUM_SPRITES];
static UWORD brick_bitmap[BRICK_LEN] = {
	0xffff, 0x0000,
	0x8001, 0x0000,
	0x8001, 0x0000,
	0x8001, 0x0000,
	0x8001, 0x0000,
	0x8001, 0x0000,
	0x8001, 0x0000,
	0x8001, 0x0000,
	0x8001, 0x0000,
	0x8001, 0x0000,
	0x8001, 0x0000,
	0x8001, 0x0000,
	0x8001, 0x0000,
	0x8001, 0x0000,
	0x8001, 0x0000,
	0xffff, 0x0000
};
static GameState game_state = GS_BEFORE;
static TetraminoRenderer Tetramino[7][4];
static Position current_position;

/*
 * Private protos
 */
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

/*
 * Private functions
 */
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

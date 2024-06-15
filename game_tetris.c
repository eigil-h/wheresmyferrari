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

/*
 * Private protos
 */
static void exit_handler(void);
static void init_sprites(void);
static void render_sprite(UBYTE sp, UBYTE h, UBYTE x, UBYTE y);
// static void render_tetramino();

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
	switch(game_state)
	{
		case GS_BEFORE:
			render_sprite(0, 1, 0, 1);
			render_sprite(1, 2, 1, 1);
			render_sprite(2, 1, 2, 1);

			game_state = GS_PLAY;

			break;

		case GS_PLAY:
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

static void exit_handler(void)
{
	int i = NUM_SPRITES;
	while(--i >= 0)
	{
		FreeSprite(i);
	}
}

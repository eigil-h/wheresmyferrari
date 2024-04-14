#include "game_tetris.h"
#include <proto/graphics.h>
#include <stdlib.h>
#include <string.h>


/*
 * Private objects
 */
static struct SimpleSprite tetromino_sprite[NUM_SPRITES];
static BrickImageH1 __chip brick_image_h1;
static BrickImageH2 __chip brick_image_h2;
static BrickImageH3 __chip brick_image_h3;
static BrickImageH4 __chip brick_image_h4;
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

/*
 * Public functions
 */
void init_game(void)
{
	atexit(exit_handler);

	init_sprites();
}

void render_frame(struct ViewPort* view_port, InputState* input_state)
{

	switch(game_state)
	{
		case GS_BEFORE:
			tetromino_sprite[0].height = 16;
			ChangeSprite(view_port, &tetromino_sprite[0], (void*) &brick_image_h1);
			MoveSprite(view_port, &tetromino_sprite[0], 0, 0);

			tetromino_sprite[1].height = 32;
			ChangeSprite(view_port, &tetromino_sprite[1], (void*) &brick_image_h2);
			MoveSprite(view_port, &tetromino_sprite[1], 16, 0);

			tetromino_sprite[2].height = 16;
			ChangeSprite(view_port, &tetromino_sprite[2], (void*) &brick_image_h1);
			MoveSprite(view_port, &tetromino_sprite[2], 32, 0);

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
	}

	memcpy(brick_image_h1.data, brick_bitmap, BRICK_SIZ);

	memcpy(brick_image_h2.data, brick_bitmap, BRICK_SIZ);
	memcpy(&brick_image_h2.data[BRICK_LEN], brick_bitmap, BRICK_SIZ);

	memcpy(brick_image_h3.data, brick_bitmap, BRICK_SIZ);
	memcpy(&brick_image_h3.data[BRICK_LEN], brick_bitmap, BRICK_SIZ);
	memcpy(&brick_image_h3.data[2 * BRICK_LEN], brick_bitmap, BRICK_SIZ);

	memcpy(brick_image_h4.data, brick_bitmap, BRICK_SIZ);
	memcpy(&brick_image_h4.data[BRICK_LEN], brick_bitmap, BRICK_SIZ);
	memcpy(&brick_image_h4.data[2 * BRICK_LEN], brick_bitmap, BRICK_SIZ);
	memcpy(&brick_image_h4.data[3 * BRICK_LEN], brick_bitmap, BRICK_SIZ);
}

static void exit_handler(void)
{
	int i = NUM_SPRITES;
	while(--i >= 0)
	{
		FreeSprite(i);
	}
}

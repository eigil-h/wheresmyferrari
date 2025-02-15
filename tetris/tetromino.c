#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <proto/graphics.h>
#include <utility/tagitem.h>
#include "/datatypes.h"
#include "tetromino.h"

/*
 * Public objects
 */
Tetromino current = {{3,1}, {0}, {0}}; // todo init at game.c
TetrominoShapes tetromino[] = {
	{{
		{{{1,1},{1,2},{1,1},{0,0}}, {3,3,0,0}},
		{{{1,1},{0,3},{0,0},{0,0}}, {3,3,0,0}},
		{{{1,1},{0,2},{1,1},{0,0}}, {3,3,0,0}},
		{{{0,0},{0,3},{1,1},{0,0}}, {3,3,0,0}}
	}}
};

/*
 * Private objects
 */
static ViewPort* viewport;
static SimpleSprite sprite[NUM_SPRITES];
static SpriteImage __chip sprite_image[NUM_SPRITES];
static VOID print_shapes(VOID);

/*
 * Private protos
 */
static VOID calc_boundaries(VOID);
static VOID render_sprite(UBYTE index, UBYTE num, UBYTE column, UBYTE row);

/*
 * Public functions
 */
BOOL init_tetrominos(ViewPort* vp, PictureData* brick_pic)
{
	unsigned i;

	atexit(free_sprites);

	viewport = vp;

	for(i = 0; i < NUM_SPRITES; i++)
	{
		if(GetSprite(&sprite[i], i) != i)
			exit(-1);

		memcpy(sprite_image[i].data, brick_pic->bitmap->Planes[0], BRICK_SIZE);
		memcpy(&sprite_image[i].data[BRICK_LENGTH], brick_pic->bitmap->Planes[0], BRICK_SIZE);
		memcpy(&sprite_image[i].data[2 * BRICK_LENGTH], brick_pic->bitmap->Planes[0], BRICK_SIZE);
		memcpy(&sprite_image[i].data[3 * BRICK_LENGTH], brick_pic->bitmap->Planes[0], BRICK_SIZE);

		ChangeSprite(viewport, &sprite[i], (void*) &sprite_image[i]);
	}

	calc_boundaries();

	return TRUE;
}

VOID render_tetromino(VOID)
{
	int i;
	for(i = 0; i < 4; i++)
	{
		Column column = tetromino[current.t].o[current.o].c[i];

		render_sprite(i,
			column.height,
			current.p.c + i,
			current.p.r + column.start
		);
	}
}

VOID render_all_sprites(VOID)
{
	render_sprite(0, 4, 2, 1);
	render_sprite(1, 4, 3, 1);
	render_sprite(2, 4, 4, 1);
	render_sprite(3, 4, 5, 1);
}

VOID free_sprites(VOID)
{
	int i = NUM_SPRITES;
	while(--i >= 0)
	{
		FreeSprite(i);
	}
}

/*
 * Private functions
 */
static VOID render_sprite(UBYTE i, UBYTE n, UBYTE column, UBYTE row)
{
	// render a lo-res sprite on a hi-res laced playfield
	sprite[i].height = n * BRICK_HEIGHT;
	MoveSprite(viewport, &sprite[i], VP_X_FACTOR * column, VP_Y_FACTOR * row);
}

static VOID calc_boundaries(VOID)
{
	int n, m, l;
	for(n = 0; n < Name_sizeof; n++)
	{
		for(m = 0; m < NUM_ORIENTATIONS; m++)
		{
			Orientation* o = &tetromino[n].o[m];

			for(l = 0; l < NUM_COLUMNS; l++)
			{
				if(o->c[l].height > 0)
				{
					if(l < o->b.left)
						o->b.left = l;
					if(l > o->b.right)
						o->b.right = l;
					if(o->c[l].start < o->b.top)
						o->b.top = o->c[l].start;
					if(o->c[l].start + o->c[l].height > o->b.bottom)
						o->b.bottom = o->c[l].start + o->c[l].height - 1;
				}
			}
		}
	}
}

static VOID print_shapes(VOID)
{
	TetrominoShapes tet = tetromino[0];
	int i;

	for(i=0; i<NUM_ORIENTATIONS; i++)
	{
		printf("(%d, %d, %d, %d)\n[%d %d], [%d %d], [%d %d], [%d %d]\n\n",
			tet.o[i].b.left,
			tet.o[i].b.top,
			tet.o[i].b.right,
			tet.o[i].b.bottom,
			tet.o[i].c[0].start,
			tet.o[i].c[0].height,
			tet.o[i].c[1].start,
			tet.o[i].c[1].height,
			tet.o[i].c[2].start,
			tet.o[i].c[2].height,
			tet.o[i].c[3].start,
			tet.o[i].c[3].height
		);
	}
}

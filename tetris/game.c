#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/timer.h>
#include <stdlib.h>
#include <string.h>
#include </datatypes.h>
#include "/utils.h"
#include "tetromino.h"
#include "game.h"

/*
 * Private objects
 */
static ViewPort* viewport;
static GameState game_state = GS_BEFORE;
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

/*
 * Private protos
 */
static VOID exit_handler(VOID);

/*
 * Public functions
 */
void init_game(ViewPort* vp)
{
	atexit(exit_handler);

	viewport = vp;
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
					current.p.c += input_state->h_val;

					// rotation
					if(input_state->v_val < 0)
					{
						current.o -= input_state->v_val;
						current.o &= 3;
					}
					// vertical move
					else if(input_state->v_val > 0)
					{
						current.p.r += input_state->v_val;
						if(current.p.r >= PLAYGROUND_ROWS)
							current.p.r -= 1;
					}

					// wall collision
					if(current.p.c + tetromino[current.t].o[current.o].b.left < 0)
					{
						current.p.c += 1;
					}
					else if(current.p.c + tetromino[current.t].o[current.o].b.right >=
						PLAYGROUND_COLUMNS)
					{
						current.p.c -= 1;
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
				current.p.r += 1;
				if(current.p.r >= PLAYGROUND_ROWS)
					current.p.r -= 1;

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

static void exit_handler(void)
{
}

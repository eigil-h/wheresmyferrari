#include "game_tetris.h"

static GameState game_state = GS_PLAY;

void render_frame(InputState* input_state)
{
	switch(game_state)
	{
		case GS_BEFORE:
			break;

		case GS_PLAY:
			break;

		case GS_PAUSE:
			break;

		case GS_AFTER:
			break;
	}
}

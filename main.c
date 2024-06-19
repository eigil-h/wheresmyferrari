#include "datatypes.h"
#include "utils.h"
#include "view.h"
#include "io.h"
#include "input.h"
#include "game_tetris.h"

#include <libraries/keymap.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>

#include <stdio.h>
#include <stdlib.h>

/*
 * Protos
 */
static void main_turbo(struct MsgPort*);
static void exit_handler(void);

/*
 * Private objects
 */
static Error error;

/*
 * Public
 */
int main(void)
{
	struct MsgPort* input_port;
	ViewPort* viewport;
	ViewRequest* view_request;

	atexit(exit_handler);

	if(!(input_port = setup_input_handler()))
	{
		exit(EXIT_FAILURE);
	}

	if(!(view_request = prepare_gfx(
		load_picture("TetBG", &error),
		load_picture("brick", &error),
		&error
	)))
	{
		exit(EXIT_FAILURE);
	}

	viewport = make_view(view_request);

	free_view_request(view_request);

	if(!viewport)
	{
		exit(EXIT_FAILURE);
	}

	init_game(viewport);

	main_turbo(input_port);

	exit(EXIT_SUCCESS);
}

/*
 * Private
 */
static void main_turbo(struct MsgPort* input_port)
{
	BOOL not_done = TRUE;
	InputState ist = {0};

	while(not_done)
	{
		CustomInputEvent* iev;

		while(iev = (CustomInputEvent*) GetMsg(input_port))
		{
			if(iev->class == 1)
			{
				switch(iev->code)
				{
					case RAWKEY_SPACE:
						ist.enter = TRUE;
						break;
							
					case RAWKEY_ESC:
						not_done = FALSE;
						break;

					case RAWKEY_CRSRUP:
						ist.v_val = -1;
						break;

					case RAWKEY_CRSRDOWN:
						ist.v_val = 1;
						break;

					case RAWKEY_CRSRRIGHT:
						ist.h_val = 1;
						break;

					case RAWKEY_CRSRLEFT:
						ist.h_val = -1;
						break;
							
					case RAWKEY_SPACE + 0x80:
						ist.enter = FALSE;
						break;
							
					case RAWKEY_CRSRUP + 0x80:
					case RAWKEY_CRSRDOWN + 0x80:
						ist.v_val = 0;
						break;

					case RAWKEY_CRSRRIGHT + 0x80:
					case RAWKEY_CRSRLEFT + 0x80:
						ist.h_val = 0;
						break;
				}
			}

			FreeMem(iev, sizeof(CustomInputEvent));
		}

		render_frame(&ist);

		WaitTOF();
	}
}

static void exit_handler(void)
{
	if(error.code)
		printf("%d, %s\n", error.code, error.msg);
}

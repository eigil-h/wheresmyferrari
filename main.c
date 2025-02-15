#include <proto/exec.h>
#include <proto/graphics.h>
#include <libraries/keymap.h>
#include <stdlib.h>
#include "datatypes.h"
#include "input.h"
#include "timer.h"
#include "tetris/fx.h"
#include "tetris/game.h"

/*
 * Protos
 */
static void main_turbo(MsgPort*);

/*
 * Public
 */
int main(void)
{
	MsgPort* input_port;
	LONG error;

	if(!(input_port = setup_input_handler()))
	{
		exit(EXIT_FAILURE);
	}

	open_timer_device();

	if(error = prepare_fx())
	{
		exit(error);
	}

	main_turbo(input_port);
}

/*
 * Private
 */
static void main_turbo(MsgPort* input_port)
{
	BOOL game_on = TRUE;
	InputState ist = {0};

	while(game_on)
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
						game_on = FALSE;
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

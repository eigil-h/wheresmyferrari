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
static void verify_bg_picture(BOOL loaded);
static void exit_handler(void);

/*
 * Private objects
 */
static Error error;
static Picture bg_picture;

/*
 * Public
 */
int main(void)
{
	ViewRequest vreq = {
		320, 512, 5, NULL
	};
	BOOL ok;
	struct MsgPort* input_port;
	ViewPort* viewport;


	atexit(exit_handler);

	if(!(SysBase->LibNode.lib_Version >= 39)) {
		puts("Cant verify AmigaOS 3.0+");
		exit(EXIT_FAILURE);
	}
	
	if(!(input_port = setup_input_handler()))
	{
		exit(EXIT_FAILURE);
	}

	ok = load_picture("TetBG", &bg_picture, &error);

	verify_bg_picture(ok);
	
	vreq.palette4 = bg_picture.palette4;
	vreq.bitmap = bg_picture.bitmap;

	viewport = make_view(&vreq);
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

static void verify_bg_picture(BOOL loaded)
{
	UWORD required_width = 320;
	UWORD required_height = 512;

	if(loaded &&
		bg_picture.width == required_width &&
		bg_picture.height == required_height) {
		return;
	}

	if(!loaded)
		printf("\nERROR: %d - \"%s\"\n", error.code, error.msg);
	else
		printf("Incorrect BG picture dimensions, %d x %d. Should be 320x512",
			bg_picture.width,
			bg_picture.height);

	exit(EXIT_BAD_BG_PICTURE);
}

static void exit_handler(void)
{
	FreeVec(bg_picture.palette4);

	free_bitmap(bg_picture.bitmap, 0, 0);
}

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
static void prepare_view_request(
	PictureData* bg,
	PictureData* brick);
static void main_turbo(struct MsgPort*);
static void exit_handler(void);

/*
 * Private objects
 */
static ViewRequest view_request;
static Error error;

/*
 * Public
 */
int main(void)
{
	struct MsgPort* input_port;
	ViewPort* viewport;

	atexit(exit_handler);

	if(!(input_port = setup_input_handler()))
	{
		exit(EXIT_FAILURE);
	}

	prepare_view_request(
		load_picture("TetBG", &error),
		load_picture("brick", &error)
	);

	viewport = make_view(&view_request);

	init_game(viewport);

	main_turbo(input_port);

	exit(EXIT_SUCCESS);
}

/*
 * Private
 */
static void prepare_view_request(
	PictureData* bg,
	PictureData* brick)
{
	if(bg && brick)
	{
		if(validate_loaded_data(bg, brick))
		{
			view_request.width = bg->width;
			view_request.height = bg->height;
			view_request.depth = bg->depth;
			view_request.palette4.data = palette32ToRGB4(&bg->palette);
			view_request.palette4.length = bg->palette.length;
			view_request.bg_bitmap = alloc_init_bitmap(bg, &error);
		}
		else
		{
			error.code = 13;
			error.msg = "Bad files";
		}
	}

	free_picture_data(brick);
	free_picture_data(bg);
}

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
	if(view_request.palette4.data) {
		FreeMem(view_request.palette4.data, view_request.palette4.length);
	}
	if(view_request.bg_bitmap) {
		free_bitmap(
			view_request.bg_bitmap,
			view_request.width,
			view_request.height
		);
	}

	if(error.code)
		printf("%d, %s\n", error.code, error.msg);
}

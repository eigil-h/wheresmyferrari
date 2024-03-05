#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include "datatypes.h"
#include "utils.h"
#include "view.h"
#include "io.h"
#include "keyb.h"

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
static LibraryVersion library_version;

/*
 * Public
 */
int main(void)
{
	ViewRequest vreq = {
		320, 512, 4, NULL
	};
	BOOL ok;
	struct MsgPort* keyb_port;


	atexit(exit_handler);

	ok = init_library_version("exec.library", &library_version);

	if(!(ok && library_version.major >= 39)) {
		puts("Cant verify AmigaOS 3.0+");
		exit(EXIT_FAILURE);
	}

	ok = load_picture("TetBG", &bg_picture, &error);

	verify_bg_picture(ok);
	
	vreq.palette4 = bg_picture.palette4;
	vreq.bitmap = bg_picture.bitmap;

	make_view(&vreq);

	keyb_port = init_keyb();

	main_turbo(keyb_port);

	exit(EXIT_SUCCESS);
}

/*
 * Private
 */
static void main_turbo(struct MsgPort* keyb_port)
{
	BOOL loop = TRUE;
	struct Message* keyb_msg;
	ULONG keyb_sig = 1L << keyb_port->mp_SigBit;
	ULONG signals;

	while (loop) {
		requestKeybEvent();
		signals = Wait(keyb_sig);

		if(signals & keyb_sig) {
			while(loop && (keyb_msg = GetMsg(keyb_port)) != NULL) {
				loop = FALSE;
			}
		} else {
			puts("WTF");
		}
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

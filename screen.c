#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <stdlib.h>
#include "datatypes.h"
#include "screen.h"

static struct TagItem screentags[] = {
	SA_DisplayID, HIRESLACE_KEY,
	SA_Left, 160,
	SA_Top, 0,
	SA_Width, 320,
	SA_Height, 512,
	SA_Depth, 4,
	SA_Title, (ULONG) "Wheresmyferarri",
	SA_Type, CUSTOMSCREEN,
	SA_BitMap, NULL,
	SA_SysFont, 1,
	TAG_DONE
};

struct Screen* screen = NULL;

struct Screen* open_screen(struct BitMap* bg_bitmap)
{
	FindTagItem(SA_BitMap, screentags)->ti_Data = (ULONG) bg_bitmap;

	screen = OpenScreenTagList(NULL, &screentags[0]);

	if(!screen) {
		exit(EXIT_OPENSCREEN);
	}

	return screen;
}

void close_screen(void)
{
	if(screen)
		CloseScreen(screen);
}

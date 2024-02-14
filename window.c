#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/utility.h>
#include "window.h"

static struct TagItem wintags[] = {  
	WA_Left, 20,
	WA_Top, 20,
	WA_Width, 160,
	WA_Height, 300,
	WA_IDCMP, IDCMP_CLOSEWINDOW,
	WA_Flags, WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_ACTIVATE,
	WA_NewLookMenus, TRUE,
	WA_Title, (ULONG) "My Ferrari",
	WA_CustomScreen, (ULONG) NULL,
	TAG_DONE
};

static struct NewMenu new_menu[] = {
	{ NM_TITLE, "project", NULL, 0, 0L, NULL },
	{ NM_ITEM, "Quit", "Q", 0, 0L, NULL },
	{ NM_END, NULL, NULL, 0, 0L, NULL }
};

static struct Window* window;
static struct Menu* menu_strip = NULL;
static APTR vis_info = NULL;

static struct Window* clean_up(void)
{
	if(menu_strip != NULL) {
		ClearMenuStrip(window);
		FreeMenus(menu_strip);
	}

	if(vis_info != NULL) {
		FreeVisualInfo(vis_info);
	}

	CloseWindow(window);

	return NULL;
}


struct Window* open_window(struct Screen* screen)
{
	FindTagItem(WA_CustomScreen, wintags)->ti_Data = (ULONG) screen;

	if((window = OpenWindowTagList(NULL, &wintags[0])) == NULL)
		return NULL;


	if((vis_info = GetVisualInfo(window->WScreen, GTMN_NewLookMenus, TRUE, TAG_END)) == NULL)
		return clean_up();


	if((menu_strip = CreateMenus(new_menu, TAG_END)) == NULL)
		return clean_up();

	if(!LayoutMenus(menu_strip, vis_info, TAG_END))
		return clean_up();

	if(!SetMenuStrip(window, menu_strip))
		return clean_up();


	return window;
}

void close_window(void)
{
	clean_up();
}

#include <stdlib.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <graphics/videocontrol.h>
#include <hardware/custom.h>
#include "view.h"

extern struct Custom __far custom;

/*
 * Private objects
 */
static struct View* oldview = NULL;
static struct ViewExtra* vextra = NULL;
static struct ViewPortExtra* vpextra = NULL;
static struct MonitorSpec* monspec = NULL;
static struct ColorMap* colormap = NULL;
static struct View view;
static struct ViewPort viewport;
static struct DimensionInfo dimquery;

/*
 * Private Protos
 */
static VOID store_oldview(VOID);
static VOID takeSprite0(VOID);
static VOID exit_handler(VOID);

/*
 * Public functions
 */
ViewPort* make_view(ViewRequest* view_request)
{
	ULONG mode_id = DEFAULT_MONITOR_ID | HIRESLACE_KEY;
	struct RasInfo rasinfo = {0};
	DisplayInfoHandle dinfoh = NULL;

	atexit(exit_handler);

	store_oldview();

	InitView(&view);
	view.Modes |= LACE | SPRITES;

	if(!(vextra = GfxNew(VIEW_EXTRA_TYPE))) {
		exit(VIEW_ERROR_VIEW_EXTRA);
	}

	GfxAssociate(&view, vextra);
	view.Modes |= EXTEND_VSTRUCT;

	if(!(monspec = OpenMonitor(NULL, mode_id))) {
		exit(VIEW_ERROR_OPEN_MONITOR);
	}

	rasinfo.BitMap = view_request->bg_bitmap;

	InitVPort(&viewport);
	view.ViewPort = &viewport;
	viewport.RasInfo = &rasinfo;
	viewport.DWidth = view_request->width;
	viewport.DHeight = view_request->height;
	viewport.DxOffset = 160;
	viewport.Modes = HIRES | LACE | SPRITES;

	if(!(vpextra = GfxNew(VIEWPORT_EXTRA_TYPE))) {
		exit(VIEW_ERROR_VIEWPORT_EXTRA);
	}
	
	if(!GetDisplayInfoData(
		NULL,
		(UBYTE *) &dimquery,
		sizeof(dimquery),
		DTAG_DIMS,
		mode_id
	)) {
		exit(VIEW_ERROR_DIMQUERY);
	}

	vpextra->DisplayClip = dimquery.Nominal;

	if(!(dinfoh = FindDisplayInfo(mode_id))) {
		exit(VIEW_ERROR_DINFOH);
	}

	// viewport.Modes = (UWORD) (mode_id & 0x0000ffff); - 1.3 stuff??

	if(!(colormap = GetColorMap(1L << view_request->depth))) {
		exit(VIEW_ERROR_COLORMAP);
	}

	if(VideoControlTags(colormap,
		VTAG_ATTACH_CM_SET, (ULONG) &viewport,
		VTAG_VIEWPORTEXTRA_SET, (ULONG) vpextra,
		VTAG_NORMAL_DISP_SET, (ULONG) dinfoh,
		VTAG_END_CM, NULL
	)) {
		exit(VIEW_ERROR_VIDEOCONTROL);
	}

	LoadRGB4(&viewport,
		view_request->palette4.data,
		view_request->palette4.length
	);

	if(MakeVPort(&view, &viewport) != MVP_OK) {
		exit(VIEW_ERROR_MAKEVPORT);
	}

	if(MrgCop(&view) != MCOP_OK) {
		exit(VIEW_ERROR_MRGCOP);
	}

	LoadView(&view);

	takeSprite0();

	return &viewport;
}

/*
 * Private functions
 */
static VOID store_oldview(VOID)
{
	struct GfxBase* gfx_base = (struct GfxBase*) OpenLibrary(
		"graphics.library",
		39L
	);

	if(!gfx_base) {
		exit(VIEW_ERROR_GFX_BASE);
	}

	oldview = gfx_base->ActiView;

	CloseLibrary((struct Library*) gfx_base);
}

static VOID takeSprite0(VOID)
{
	FreeSprite(0);
	FreeSprite(1);
}

static VOID exit_handler(VOID)
{
	LoadView(oldview);
	WaitTOF();

	FreeCprList(view.LOFCprList);

	if(view.SHFCprList)
		FreeCprList(view.SHFCprList);

	FreeVPortCopLists(&viewport);

	if(colormap)
		FreeColorMap(colormap);

	if(vpextra)
		GfxFree(vpextra);

	if(monspec)
		CloseMonitor(monspec);

	if(vextra)
		GfxFree(vextra);
}

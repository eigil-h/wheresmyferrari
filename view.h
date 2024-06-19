#ifndef VIEW_H
#define VIEW_H

#include "datatypes.h"

//Checking chipset OCS, ECS, AGA
//(GfxBase->ChipRevBits0 & GFXF_HR_AGNUS)
//(GfxBase->ChipRevBits0 & GFXF_HR_DENISE)

ViewPort* make_view(ViewRequest*);

#define VIEW_ERROR                0xA0
#define VIEW_ERROR_VIEW_EXTRA     VIEW_ERROR
#define VIEW_ERROR_OPEN_MONITOR   VIEW_ERROR + 1
#define VIEW_ERROR_GFX_BASE       VIEW_ERROR + 2
#define VIEW_ERROR_ALLOC_BITMAP   VIEW_ERROR + 3
#define VIEW_ERROR_VIEWPORT_EXTRA VIEW_ERROR + 4
#define VIEW_ERROR_DIMQUERY       VIEW_ERROR + 5
#define VIEW_ERROR_DINFOH         VIEW_ERROR + 6
#define VIEW_ERROR_COLORMAP       VIEW_ERROR + 7
#define VIEW_ERROR_VIDEOCONTROL   VIEW_ERROR + 8
#define VIEW_ERROR_MAKEVPORT      VIEW_ERROR + 9
#define VIEW_ERROR_MRGCOP         VIEW_ERROR + 10

#endif

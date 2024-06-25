#include <proto/exec.h>
#include <proto/graphics.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

PictureData* alloc_picture_data(UWORD width, UWORD height, UBYTE depth)
{
	PictureData* pd = AllocMem(sizeof(PictureData), NULL);

	pd->width = width;
	pd->height = height;
	pd->depth = depth;
	pd->interleaved = TRUE;
	pd->data = AllocMem(depth * height * BYTES_PER_ROW(width), NULL);
	pd->palette.data = AllocMem(sizeof(ULONG) * (1L << depth), NULL);
	pd->palette.length = 1L << depth;

	return pd;
}

VOID free_picture_data(PictureData** pd_ptr)
{
	PictureData* pd = *pd_ptr;

	if(pd) {
		FreeMem(pd->palette.data, sizeof(ULONG) * (1L << pd->depth));
		FreeMem(pd->data, pd->depth * pd->height * BYTES_PER_ROW(pd->width));
		FreeMem(pd, sizeof(PictureData));

		*pd_ptr = NULL;
	}
}

BitMap* alloc_init_bitmap(PictureData* pd, Error* error)
{
	BitMap* bm;

	if(os_version() >= 39) {
		bm = AllocBitMap(
			pd->width,
			pd->height,
			pd->depth,
			BMF_DISPLAYABLE | BMF_INTERLEAVED,
			NULL);

		if(!bm) {
			error->code = -113;
			error->msg = "@Alloc bitplanes";
		} else {
			memcpy(bm->Planes[0],
				pd->data,
				pd->depth * pd->height * BYTES_PER_ROW(pd->width));
		}
	} else {
		bm = AllocMem(sizeof(BitMap), NULL);

		if(bm) {
			int i;

			InitBitMap(bm, pd->depth, pd->width, pd->height);

			for(i = 0; i < pd->depth; i++) {
				bm->Planes[i] = AllocRaster(pd->width, pd->height);

				if(!bm->Planes[i]) {
					error->code = -113;
					error->msg = "@Alloc bitplanes";

					free_bitmap(bm, pd->width, pd->height);

					return NULL;
				}
			}
		} else {
			error->code = -113;
			error->msg = "@Alloc bitmap";
		}
	}

	return bm;
}

void free_bitmap(BitMap* bm, UWORD width, UWORD height)
{
	if(os_version() >= 39) {
		FreeBitMap(bm);
	} else {
		int i;

		for(i = bm->Depth - 1; i >= 0 ; --i) {
			if(bm->Planes[i]) {
				FreeRaster(bm->Planes[i], width, height);
			}
		}

		FreeMem(bm, sizeof(BitMap));
	}
}

UWORD* palette32To4Data(Palette32* pal32)
{
	UWORD* pal4data = AllocMem(pal32->length * sizeof(UWORD), NULL);
	int i;

	for(i = 0; i < pal32->length; i++)
	{
		UBYTE red = (pal32->data[i] >> 20) & 0xF;
    UBYTE green = (pal32->data[i] >> 12) & 0xF;
    UBYTE blue = (pal32->data[i] >> 4) & 0xF;

    pal4data[i] = (red << 8) | (green << 4) | blue;
	}

	return pal4data;
}

ULONG* palette32ConcatData(Palette32* src1, Palette32* src2)
{
	ULONG* dest_data = AllocMem(
		sizeof(ULONG) * (src1->length + src2->length),
		NULL);

	memcpy(dest_data, src1->data, sizeof(ULONG) * src1->length);
	memcpy(&dest_data[src1->length], src2->data, sizeof(ULONG) * src2->length);

	return dest_data;
}

VOID free_view_request(ViewRequest* view_request)
{
	if(view_request)
	{
		if(view_request->palette4.data) {
			FreeMem(view_request->palette4.data, 
				sizeof(UWORD) * view_request->palette4.length);
		}

		if(view_request->bg_bitmap) {
			free_bitmap(
				view_request->bg_bitmap,
				view_request->width,
				view_request->height
			);
		}

		FreeMem(view_request, sizeof(ViewRequest));
	}
}

VOID print_palette32(Palette32* pal)
{
	int i;
	for(i = 0; i < pal->length; i++)
	{
		printf("%2d: #%08x\n", i, pal->data[i]);
	}
}

VOID print_palette4(Palette4* pal)
{
	int i;
	for(i = 0; i < pal->length; i++)
	{
		printf("%2d: #%04x\n", i, pal->data[i]);
	}
}

int os_version(void)
{
	return SysBase->LibNode.lib_Version;
}

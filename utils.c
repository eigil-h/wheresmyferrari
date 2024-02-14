#include <proto/exec.h>
#include "utils.h"

struct BitMap* alloc_bitmap(
	UWORD width,
	UWORD height,
	UBYTE depth,
	Error* error)
{
	struct BitMap* bm = AllocBitMap(
		width,
		height,
		depth,
		BMF_DISPLAYABLE | BMF_INTERLEAVED,
		NULL);

	if(!bm) {
		error->code = -113;
		error->msg = "@Alloc bitplanes";
	}

	return bm;

/*
	struct BitMap* bm = AllocVec(sizeof(struct BitMap), NULL);
	int i;

	if(!bm) {
		error->code = -113;
		error->msg = "@Alloc bitmap";
		return NULL;
	}

	InitBitMap(bm, depth, width, height);

	for(i = 0; i < depth; i++) {
		bm->Planes[i] = AllocRaster(width, height);

		if(!bm->Planes[i]) {
			error->code = -113;
			error->msg = "@Alloc bitplanes";

			free_bitmap(bm, width, height);

			return NULL;
		}
	}


	return bm;
*/
}

void free_bitmap(struct BitMap* bm, UWORD width, UWORD height)
{
	FreeBitMap(bm);

/*
	int i;

	for(i = 0; i < bm->Depth; i++) {
		if(bm->Planes[i]) {
			FreeRaster(bm->Planes[i], width, height);
		}
	}

	FreeVec(bm);
*/
}

BOOL init_library_version(CONST_STRPTR name, LibraryVersion* ver)
{
	struct Library* lib_base;
	BOOL success = FALSE;

	if(lib_base = OpenLibrary(name, 0)) {
		ver->major = lib_base->lib_Version;
		ver->minor = lib_base->lib_Revision;
		ver->display_value = (STRPTR) lib_base->lib_IdString;

		CloseLibrary(lib_base);

		success = TRUE;
	}

	return success;
}

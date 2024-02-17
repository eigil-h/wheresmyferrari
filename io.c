#include <proto/exec.h>
#include <proto/iffparse.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <datatypes/pictureclass.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "io.h"

/*
 * Protos
 */
static Palette4* mk_palette4(const UBYTE* cmap_data, const ULONG size);
static void decompress(
	UBYTE* source,
	PLANEPTR destination[],
	const size_t compressed_size,
	const size_t decompressed_size);
static void free_io(struct IFFHandle*);
static void print_bmhd(const struct BitMapHeader*);
static void print_palette4(const Palette4*);
static void print_bitmap_data(struct BitMap*);

/*
 * Public
 */
BOOL load_picture(CONST_STRPTR file_name, Picture* pic, Error* err)
{
	struct IFFHandle* iff_handle = NULL;
	struct BitMapHeader* bmhd;
	UBYTE* cmap;
	UBYTE* body;
	ULONG body_length;
	BOOL body_compressed;
	struct StoredProperty* sp;
	LONG error;

	if(!(iff_handle = AllocIFF())) {
		err->code = -101;
		err->msg = "@AllocIFF";
		return FALSE;
	}

	if(!(iff_handle->iff_Stream = Open(file_name, MODE_OLDFILE))) {
		free_io(iff_handle);
		err->code = -102;
		err->msg = "@Open";
		return FALSE;
	}

	InitIFFasDOS(iff_handle);

	if(error = OpenIFF(iff_handle, IFFF_READ)) {
		free_io(iff_handle);
		err->code = error;
		err->msg = "@OpenIFF";
		return FALSE;
	}

	if(error = PropChunk(iff_handle, ID_ILBM, ID_BMHD)) {
		free_io(iff_handle);
		err->code = error;
		return FALSE;
	}

	if(error = PropChunk(iff_handle, ID_ILBM, ID_CMAP)) {
		free_io(iff_handle);
		err->code = error;
		return FALSE;
	}

	if(error = PropChunk(iff_handle, ID_ILBM, ID_CAMG)) {
		free_io(iff_handle);
		err->code = error;
		return FALSE;
	}

	if(error = PropChunk(iff_handle, ID_ILBM, ID_BODY)) {
		free_io(iff_handle);
		err->code = error;
		return FALSE;
	}

	StopOnExit(iff_handle, ID_ILBM, ID_FORM);

	if((error = ParseIFF(iff_handle, IFFPARSE_SCAN)) == IFFERR_EOC) {
		if(sp = FindProp(iff_handle, ID_ILBM, ID_BMHD)) {
			bmhd = (struct BitMapHeader*) sp->sp_Data;
			body_compressed = (BOOL) bmhd->bmh_Compression;

			pic->width = bmhd->bmh_Width;
			pic->height = bmhd->bmh_Height;
			pic->depth = bmhd->bmh_Depth;

			//printf("BMHD %ld\n", sp->sp_Size);
			//print_bmhd(bmhd);
		} else {
			puts("No BMHD");
		}

		if(sp = FindProp(iff_handle, ID_ILBM, ID_CMAP)) {
			cmap = (UBYTE *) sp->sp_Data;

			pic->palette4 = mk_palette4(cmap, sp->sp_Size);

			//printf("CMAP size %ld\n", sp->sp_Size);
			//print_palette4(pic->palette4);
		} else {
			puts("No CMAP");
		}

		if(sp = FindProp(iff_handle, ID_ILBM, ID_CAMG)) {
			struct NameInfo ninfo;
			ULONG camg = (ULONG) sp->sp_Data;


			GetDisplayInfoData(
				NULL,
				(UBYTE*) &ninfo,
				sizeof(struct NameInfo),
				DTAG_NAME,
				camg);
		} else {
			puts("No CAMG");
		}

		if(sp = FindProp(iff_handle, ID_ILBM, ID_BODY)) {
			body = (UBYTE *) sp->sp_Data;
			body_length = sp->sp_Size;

			//printf("BODY %ld\n", sp->sp_Size);
		} else {
			puts("No BODY");
		}

		error = 0;
	} else {
		if(error == 0)
			error = -103;
		err->code = error;
		err->msg = "@ParseIFF";
	}

	if(error == 0 && bmhd && body) {
		pic->bitmap = alloc_bitmap(pic->width, pic->height, pic->depth, err);
		if (!pic->bitmap)
			error = 1; // just not 0.. need to look into this

		if(body_compressed) {
			decompress(body, pic->bitmap->Planes, body_length, RASSIZE(pic->width, pic->height));
		} else {
			//print_bitmap_data(pic->bitmap);

			memcpy(pic->bitmap->Planes[0], body, body_length);
		}
	}

	free_io(iff_handle);

	return (BOOL) (error == 0);
}

/*
 * Private
 */
static Palette4* mk_palette4(const UBYTE* cmap_data, const ULONG size)
{
	unsigned int palette_length = size / 3;
	Palette4* palette = AllocVec(sizeof(struct Palette4) * palette_length, MEMF_CHIP);
	int i;

	palette->length = palette_length;

	for (i = 0; i < palette_length; i++) {
		UBYTE red = cmap_data[i * 3] >> 4;
    UBYTE green = cmap_data[i * 3 + 1] >> 4;
    UBYTE blue = cmap_data[i * 3 + 2] >> 4;
    palette->data[i] = (red << 8) | (green << 4) | blue;
  }

	return palette;
}

static void decompress(
	UBYTE* source,
	PLANEPTR destination[],
	const size_t compressed_size,
	const size_t decompressed_size)
{
	size_t src_idx = 0;
	size_t dest_idx = 0;

	while (src_idx < compressed_size && dest_idx < decompressed_size) {
		BYTE byte = source[src_idx++];

    if (byte >= 0) {
      int run_length = byte + 1;
			int i;
      for (i = 0; i < run_length && src_idx < compressed_size && dest_idx < decompressed_size; i++) {
      }
    } else if (byte != -128) {
    	int run_length = -byte;
      // UBYTE value = source[src_idx++];
			int i;
      for (i = 0; i < run_length && dest_idx < decompressed_size; i++) {
      }
    }
  }
}

static void free_io(struct IFFHandle* iff_handle)
{
	if(iff_handle) {
		CloseIFF(iff_handle);
		if(iff_handle->iff_Stream)
			Close(iff_handle->iff_Stream);
		FreeIFF(iff_handle);
	}
}

static void print_bmhd(const struct BitMapHeader* bmhd)
{
	printf(
		"bitmap header:\nW:%d\nH:%d\nL:%d\nT:%d\nD:%d\nM:%d\nC:%d\nTC:%d\nXA:%d\nYA:%d\nPW:%d\nPH:%d\n",
		bmhd->bmh_Width,
		bmhd->bmh_Height,
		bmhd->bmh_Left,
		bmhd->bmh_Top,
		bmhd->bmh_Depth,
		bmhd->bmh_Masking,
		bmhd->bmh_Compression,
		bmhd->bmh_Transparent,
		bmhd->bmh_XAspect,
		bmhd->bmh_YAspect,
		bmhd->bmh_PageWidth,
		bmhd->bmh_PageHeight
	);
}

static void print_palette4(const Palette4* palette)
{
	int i;

	for(i = 0; i < palette->length; i++) {
		printf("0x%03x, ", palette->data[i]);
	}
}

static void print_bitmap_data(struct BitMap* bm)
{
	printf("bpr: %d\nrows: %d\nflags: 0x%08X\ndepth: %d\np0: 0x%08X\np1: 0x%08X\np2: 0x%08X\np3: 0x%08X\n",
		bm->BytesPerRow,
		bm->Rows,
		GetBitMapAttr(bm, BMA_FLAGS),
		bm->Depth,
		bm->Planes[0],
		bm->Planes[1],
		bm->Planes[2],
		bm->Planes[3]);
}

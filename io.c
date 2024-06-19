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
static VOID mk_palette(
  const UBYTE* cmap_data,
  const ULONG size,
  Palette32* palette
);
static void decompress(
	const UBYTE* source,
	UBYTE* destination,
	const size_t compressed_size,
	const size_t decompressed_size);
static void free_io(struct IFFHandle*);
static void print_bmhd(const struct BitMapHeader*);
static void print_cmap(const UBYTE* cmap, const ULONG size);

/*
 * Public
 */
PictureData* load_picture(CONST_STRPTR file_name, Error* err)
{
	PictureData* picture_data = NULL;
	struct IFFHandle* iff_handle = NULL;
	struct BitMapHeader* bmhd = NULL;
	UBYTE* cmap = NULL;
	ULONG cmap_size;
	UBYTE* body = NULL;
	ULONG body_size;
	LONG error;

	if(!(iff_handle = AllocIFF())) {
		err->code = -101;
		err->msg = "@AllocIFF";
		return NULL;
	}

	if(!(iff_handle->iff_Stream = Open(file_name, MODE_OLDFILE))) {
		free_io(iff_handle);
		err->code = -102;
		err->msg = "@Open";
		return NULL;
	}

	InitIFFasDOS(iff_handle);

	if(error = OpenIFF(iff_handle, IFFF_READ)) {
		free_io(iff_handle);
		err->code = error;
		err->msg = "@OpenIFF";
		return NULL;
	}

	if(error = PropChunk(iff_handle, ID_ILBM, ID_BMHD)) {
		free_io(iff_handle);
		err->code = error;
		return NULL;
	}

	if(error = PropChunk(iff_handle, ID_ILBM, ID_CMAP)) {
		free_io(iff_handle);
		err->code = error;
		return NULL;
	}

	if(error = PropChunk(iff_handle, ID_ILBM, ID_BODY)) {
		free_io(iff_handle);
		err->code = error;
		return NULL;
	}

	StopOnExit(iff_handle, ID_ILBM, ID_FORM);

	if((error = ParseIFF(iff_handle, IFFPARSE_SCAN)) == IFFERR_EOC) {
		struct StoredProperty* sp;

		if(sp = FindProp(iff_handle, ID_ILBM, ID_BMHD)) {
			bmhd = (struct BitMapHeader*) sp->sp_Data;
		}

		if(sp = FindProp(iff_handle, ID_ILBM, ID_CMAP)) {
			cmap = (UBYTE *) sp->sp_Data;
			cmap_size = sp->sp_Size;
		}

		if(sp = FindProp(iff_handle, ID_ILBM, ID_BODY)) {
			body = (UBYTE *) sp->sp_Data;
			body_size = sp->sp_Size;
		}
	} else {
		err->code = error;
		err->msg = "@ParseIFF";
	}

	if(bmhd && cmap && body) {
		if(bmhd->bmh_Masking == mskHasMask ||
			bmhd->bmh_Masking == mskHasAlpha) {
			err->code = bmhd->bmh_Masking;
			err->msg = "This mask is not supported";
		} else {
			BOOL body_compressed = (BOOL) bmhd->bmh_Compression;

			picture_data = alloc_picture_data(
				bmhd->bmh_Width,
				bmhd->bmh_Height,
				bmhd->bmh_Depth
			);

			mk_palette(cmap, cmap_size, &picture_data->palette);

			if(body_compressed) {
				decompress(
					body,
					picture_data->data,
					body_size,
					picture_data->depth *
						RASSIZE(picture_data->width, picture_data->height)
				);
			} else {
				memcpy(picture_data->data, body, body_size);
			}
		}

		// print_bmhd(bmhd);
		// print_cmap(cmap, cmap_size);
	}

	free_io(iff_handle);

	return picture_data;
}

/*
 * Private
 */
static VOID mk_palette(
  const UBYTE* cmap_data,
  const ULONG size,
  Palette32* palette
)
{
	ULONG palette_length = size / 3;
	int i;

	for (i = 0; i < palette_length; i++) {
		UBYTE red = cmap_data[i * 3];
    UBYTE green = cmap_data[i * 3 + 1];
    UBYTE blue = cmap_data[i * 3 + 2];
    palette->data[i] = (red << 16) | (green << 8) | blue;
  }
}

static void decompress(
	const UBYTE* source,
	UBYTE* destination,
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
  puts("");
}

static void print_cmap(const UBYTE* data, const ULONG size)
{
	int i;

	for(i = 0; i < size/3; i++) {
		printf("#%02x%02x%02x\n", *data++, *data++, *data++);
	}
  puts("");
}

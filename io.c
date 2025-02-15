#include <proto/exec.h>
#include <proto/iffparse.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatypes.h"
#include "io.h"

#define BYTES_PER_ROW(w,d) (((((ULONG)(w) + 15) >> 3) & 0xFFFE) * (d))

/*
 * Protos
 */
static BOOL validate(ILBMData*, PictureData*);
static VOID cmap_to_palette32(
  const UBYTE* cmap_data,
  const ULONG size,
  Palette32 palette
);
static VOID decompress(
	const UBYTE* source,
	UBYTE* destination,
	const size_t compressed_size,
	const size_t decompressed_size);
static VOID free_io(IFFHandle*);
static BOOL clean_return(IFFHandle*, CONST_STRPTR, LONG);
static VOID print_ilbm_data(ILBMData*);

/*
 * Public
 */
BOOL load_picture(CONST_STRPTR path, PictureData* picture_data)
{
	IFFHandle* iff_handle;
	ILBMData ilbm_data = {0};
	LONG error;

	if(!(iff_handle = AllocIFF()))
	{
		return clean_return(iff_handle, "AllocIFF", ERROR_ALLOC_IFF);
	}

	if(!(iff_handle->iff_Stream = Open(path, MODE_OLDFILE)))
	{
		return clean_return(iff_handle, "Open file", ERROR_OPEN_FILE);
	}

	InitIFFasDOS(iff_handle);

	if(error = OpenIFF(iff_handle, IFFF_READ))
	{
		return clean_return(iff_handle, "OpenIFF", error);
	}

	if(error = PropChunk(iff_handle, ID_ILBM, ID_BMHD))
	{
		return clean_return(iff_handle, "PropChunk bmhd", error);
	}

	if(error = PropChunk(iff_handle, ID_ILBM, ID_CMAP))
	{
		return clean_return(iff_handle, "PropChunk cmap", error);
	}

	if(error = PropChunk(iff_handle, ID_ILBM, ID_BODY))
	{
		return clean_return(iff_handle, "PropChunk body", error);
	}

	StopOnExit(iff_handle, ID_ILBM, ID_FORM);

	if((error = ParseIFF(iff_handle, IFFPARSE_SCAN)) == IFFERR_EOC)
	{
		struct StoredProperty* sp;

		if(sp = FindProp(iff_handle, ID_ILBM, ID_BMHD))
		{
			ilbm_data.bmhd = (BitMapHeader*) sp->sp_Data;
		}

		if(sp = FindProp(iff_handle, ID_ILBM, ID_CMAP))
		{
			ilbm_data.cmap = (UBYTE *) sp->sp_Data;
			ilbm_data.cmap_size = sp->sp_Size;
		}

		if(sp = FindProp(iff_handle, ID_ILBM, ID_BODY))
		{
			ilbm_data.body = (UBYTE *) sp->sp_Data;
			ilbm_data.body_size = sp->sp_Size;
		}
	}
	else
	{
		return clean_return(iff_handle, "ParseIFF", error);
	}

	if(!validate(&ilbm_data, picture_data))
	{
		return clean_return(iff_handle, "validate", INVALID_FORMAT);
	}

	if(ilbm_data.bmhd->bmh_Masking == mskHasMask ||
		ilbm_data.bmhd->bmh_Masking == mskHasAlpha)
	{
		// not supported until I understand what this is
		return clean_return(iff_handle, "Masking", UNKNOWN_FORMAT);
	}

	cmap_to_palette32(
		ilbm_data.cmap,
		ilbm_data.cmap_size,
		picture_data->palette);

	if(ilbm_data.bmhd->bmh_Compression)
	{
		decompress(
			ilbm_data.body,
			picture_data->bitmap->Planes[0],
			ilbm_data.body_size,
			ilbm_data.bmhd->bmh_Depth *
				RASSIZE(ilbm_data.bmhd->bmh_Width,
								ilbm_data.bmhd->bmh_Height)
		);
	}
	else
	{
		memcpy(picture_data->bitmap->Planes[0], ilbm_data.body, ilbm_data.body_size);
	}

	return clean_return(iff_handle, "", 0);
}


/*
 * Private
 */
static BOOL validate(ILBMData* id, PictureData* pd)
{
	return (BOOL) ((id->bmhd && id->cmap && id->body) &&
		(id->bmhd->bmh_Width == pd->width) &&
		(id->bmhd->bmh_Height == pd->height) &&
		(id->bmhd->bmh_Depth == pd->depth) &&
		(id->cmap_size == (3 * (1 << pd->depth))));
}

static VOID cmap_to_palette32(
  const UBYTE* cmap_data,
  const ULONG length,
  Palette32 palette
)
{
	unsigned i;

	*palette++ = (length/3) << 16;

	for (i = 0; i < length; i++) {
		*palette++ = (*cmap_data++ << 24) & 0xFFFFFFFF; //todo: why and (&)?
  }

	*palette = 0L;
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

static VOID free_io(struct IFFHandle* iff_handle)
{
	if(iff_handle)
	{
		CloseIFF(iff_handle);
		if(iff_handle->iff_Stream)
			Close(iff_handle->iff_Stream);
		FreeIFF(iff_handle);
	}
}

static BOOL clean_return(IFFHandle* iff_handle, CONST_STRPTR msg, LONG err)
{
	free_io(iff_handle);

	if(err != 0)
	{
		printf("%s: %d\n", msg, err);
		return FALSE;
	}

	return TRUE;
}

static VOID print_ilbm_data(ILBMData* d)
{
	printf("0x%08lx\n0x%08lx\n%d\n0x%08lx\n%d\n",
		d->bmhd,
		d->cmap,
		d->cmap_size,
		d->body,
		d->body_size);

	if(d->bmhd)
	{
		printf("%d %d %d %d %d\n%d %d %d\n%d %d %d %d\n",
			d->bmhd->bmh_Width,
			d->bmhd->bmh_Height,
			d->bmhd->bmh_Left,
			d->bmhd->bmh_Top,
			d->bmhd->bmh_Depth,
			d->bmhd->bmh_Masking,
			d->bmhd->bmh_Compression,
			d->bmhd->bmh_Transparent,
			d->bmhd->bmh_XAspect,
			d->bmhd->bmh_YAspect,
			d->bmhd->bmh_PageWidth,
			d->bmhd->bmh_PageHeight);
	}
}

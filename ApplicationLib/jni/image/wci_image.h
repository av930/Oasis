/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_image.h

ABSTRACT:    This file contains definitions and data structures for operations with images

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "../wci_codec.h"
#include "wci_image_component.h"
#include "../utils/wci_mem_align.h"

//------------------------------------------------------------------------------

#define EDGE_SIZE  0

//------------------------------------------------------------------------------

typedef struct
{
	uint8_t *y;
	uint8_t *u;
	uint8_t *v;
}
IMAGE;

typedef struct
{
	int16_t *y;
	int16_t *u;
	int16_t *v;
}
DIFF_IMAGE;

typedef struct tagIMAGE_INFO
{
  SIZE            rSize;

  COLOR_SPACE     eColorSpace;

  PIXEL_SAMPLING  ePixelSampling;

  int             iComponents;

  BOOL            bInplaceOneComponent;

} IMAGE_INFO;

typedef struct tagIMAGE_EX
{
  IMAGE_INFO      rInfo;

  IMAGE_COMPONENT arComponent[MAX_IMAGE_COMPONENTS];

} IMAGE_EX;

typedef struct tagDIFF_IMAGE_EX
{
  IMAGE_INFO      rInfo;

  DIFF_IMAGE_COMPONENT arComponent[MAX_IMAGE_COMPONENTS];

} DIFF_IMAGE_EX;

//------------------------------------------------------------------------------

ERROR_CODE wci_image_set_component_size(
  SIZE                 IN     rSize
, COLOR_SPACE          IN     eColorSpace
, SIZE                 OUT    arSize[MAX_IMAGE_COMPONENTS]
);

//------------------------------------------------------------------------------

PIXEL_SAMPLING wci_image_get_pixel_sampling(
  COLOR_SPACE IN eColorSpace
);

//------------------------------------------------------------------------------

ERROR_CODE wci_image_create(
  SIZE                 IN     rSize
, COLOR_SPACE          IN     eColorSpace
, PIXEL_SAMPLING       IN     ePixelSampling
, BYTE *               IN     pbtInPlaceImgBuf
, IMAGE_EX *              OUT prImage
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
);

//------------------------------------------------------------------------------

void wci_image_destroy(
  IMAGE_EX *           IN OUT prImage
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
);

//------------------------------------------------------------------------------

void wci_image_clear(
  IMAGE_EX * IN OUT prImage
);

//------------------------------------------------------------------------------

void wci_image_copy(
  const IMAGE_EX * IN  prSrc
, IMAGE_EX *       OUT prDst
);

//------------------------------------------------------------------------------

int wci_image_get_size(
  SIZE        IN rSize
, COLOR_SPACE IN eColorSpace
);

//------------------------------------------------------------------------------

ERROR_CODE wci_image_input(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, COLOR_SPACE  IN     eColorSpace
, int          IN     iInterlaced
, IMAGE_EX *   IN OUT prImage
);

//------------------------------------------------------------------------------

ERROR_CODE wci_image_clip_output(
  const DIFF_IMAGE_EX * IN  prImage
, COLOR_SPACE           IN  eColorSpace
, int                   IN  iInterlaced
, RECT                  IN  rClipRect
, BYTE *                OUT pbtDst
);

//------------------------------------------------------------------------------

ERROR_CODE wci_image_expand_rect(
  SIZE         IN     rRectSize
, IMAGE_EX *   IN OUT prImage
);

//------------------------------------------------------------------------------

IMAGE wci_image_get_image(
  const IMAGE_EX * IN prImage
);

//------------------------------------------------------------------------------

void wci_image_get_psnr(
  const IMAGE_EX * IN  prOrig  //! Pointer to the 'original' image
, const IMAGE_EX * IN  prRecon //! Pointer to the 'reconstructed' image
, float            OUT afPSNR[MAX_IMAGE_COMPONENTS]
);

//------------------------------------------------------------------------------

ERROR_CODE wci_diffimage_create(
  SIZE                 IN     rSize
, COLOR_SPACE          IN     eColorSpace
, PIXEL_SAMPLING       IN     ePixelSampling
, DIFF_IMAGE_EX *         OUT prImage
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
);

//------------------------------------------------------------------------------

void wci_diffimage_destroy(
  DIFF_IMAGE_EX *      IN OUT prImage
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
);

//------------------------------------------------------------------------------
#ifdef _DEBUG

int wci_image_dump_to_pgm(
  const IMAGE_EX * IN prImage
, const char *     IN szFilename
);

int wci_row_image_dump_to_pgm(
  const BYTE *  IN pbtImage
, SIZE          IN rSize
, const char *  IN szFilename
);

//------------------------------------------------------------------------------

int wci_row_diffimage_dump_to_pgm(
  const short * IN pshImage
, SIZE          IN rSize
, int           IN iBias
, const char *  IN szFilename
);
#endif	// ifdef _DEBUG


#endif	// _IMAGE_H_ 


/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: image.c

ABSTRACT:    This file contains procedures for operations with images

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef _MSC_VER
 #pragma warning(disable: 4100)
#endif

#include "../wci_codec.h"
#include "../wci_global.h"
#include "../utils/wci_mem_align.h"
#include "wci_color_space_table.h"
#include "wci_color_space_out.h"
#include "wci_color_space_in.h"
#include "wci_image.h"

//------------------------------------------------------------------------------

#define SAFETY     64
#define EDGE_SIZE2 (EDGE_SIZE/2)
#define ABS        abs

#define ERR_CHECK(err) if( (err) != ERR_NONE ) return err

//------------------------------------------------------------------------------
ERROR_CODE wci_image_set_component_size(
  SIZE                 IN     rSize
, COLOR_SPACE          IN     eColorSpace
, SIZE                 OUT    arSize[MAX_IMAGE_COMPONENTS]
)
{
  memset( arSize, 0, MAX_IMAGE_COMPONENTS*sizeof(SIZE) );

  switch( eColorSpace )
  {
    case CSP_GRAYSCALE:
      arSize[0] = rSize;
     	break;
    case CSP_YV24:
    case CSP_YV24A:
      arSize[0] = arSize[1] = arSize[2] = arSize[3] = rSize;
      break;
    case CSP_BGR:
      arSize[0] = arSize[1] = arSize[2] = rSize;
      break;
    case CSP_BGRA:
      arSize[0] = arSize[1] = arSize[2] = arSize[3] = rSize;
     	break;
    case CSP_YV12:
      arSize[0] = rSize;
      rSize.cx = (rSize.cx+1) / 2;
      rSize.cy = (rSize.cy+1) / 2;
      arSize[1] = arSize[2] = rSize;
     	break;
    case CSP_YV12A:
      arSize[0] = arSize[3] = rSize;
      rSize.cx = (rSize.cx+1) / 2;
      rSize.cy = (rSize.cy+1) / 2;
      arSize[1] = arSize[2] = rSize;
      break;
    default:
      return ERR_PARAM;
  }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

PIXEL_SAMPLING wci_image_get_pixel_sampling(
  COLOR_SPACE IN eColorSpace
)
{
  switch( eColorSpace )
  {
    default:
    case CSP_GRAYSCALE:
    case CSP_YV24:
    case CSP_YV24A:
    case CSP_BGR: case CSP_BGRA:
      return PS_444;
    case CSP_YV12A:
    case CSP_YV12:
      return PS_420;

  }
}

//------------------------------------------------------------------------------

ERROR_CODE wci_image_create(
  SIZE                 IN     rSize
, COLOR_SPACE          IN     eColorSpace
, PIXEL_SAMPLING       IN     ePixelSampling
, BYTE *               IN     pbtInPlaceImgBuf
, IMAGE_EX *              OUT prImage
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
)
{
  int  i, err;
  SIZE arSize[MAX_IMAGE_COMPONENTS];

  prImage->rInfo.rSize          = rSize         ;
  prImage->rInfo.eColorSpace    = eColorSpace   ;
  prImage->rInfo.ePixelSampling = ePixelSampling;
  prImage->rInfo.bInplaceOneComponent = FALSE;

  prImage->rInfo.iComponents  = wci_get_component_number(eColorSpace);

  err = wci_image_set_component_size( rSize, eColorSpace, arSize );
  ERR_CHECK( err );
  
  if( pbtInPlaceImgBuf != NULL )
  {
    prImage->rInfo.bInplaceOneComponent = TRUE;   
    wci_ic_set( arSize[0], 0, pbtInPlaceImgBuf, &prImage->arComponent[0]);
    i=1;
  }
  else
    i=0;
  for ( ; i<prImage->rInfo.iComponents; ++i )
  {
    err = wci_ic_create( arSize[i], 0, &prImage->arComponent[i], prMemHeap );
    ERR_CHECK( err );
  }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

void wci_image_destroy(
  IMAGE_EX *           IN OUT prImage
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
)
{
  int i = ( prImage->rInfo.bInplaceOneComponent == TRUE ) ? 1 : 0;

  for ( ; i<prImage->rInfo.iComponents; ++i )
  {
    wci_ic_destroy( &prImage->arComponent[i], prMemHeap );
  }
  memset( prImage, 0, sizeof(IMAGE_EX) );
}

//------------------------------------------------------------------------------

#ifdef _DEBUG
int wci_raw_image_dump_to_pgm(
  const BYTE *  IN pbtImage
, SIZE          IN rSize
, const char *  IN szFilename
)
{
  FILE *   f;
  char     hdr[1024];

  f = fopen( szFilename, "wb");
  if (f == NULL)
  {
    return -1;
  }
  else
  {
    int32_t y;

    sprintf(hdr, "P5\n#WICA\n%d %d\n255\n", rSize.cx, rSize.cy );
    fwrite(hdr, strlen(hdr), 1, f);

    for (y = 0; y < rSize.cy; ++y )
    {
      fwrite( pbtImage, rSize.cx, 1, f);
      pbtImage += rSize.cx;
    }
  }

  fclose(f);

  return 0;
}

//------------------------------------------------------------------------------

int wci_raw_diffimage_dump_to_pgm(
  const short * IN pshImage
, SIZE          IN rSize
, int           IN iBias
, const char *  IN szFilename
)
{
  FILE *   f;
  char     hdr[1024];

  f = fopen( szFilename, "wb");
  if (f == NULL)
  {
    return -1;
  }
  else
  {
    int32_t x, y;

    sprintf(hdr, "P5\n#WICA\n%d %d\n255\n", rSize.cx, rSize.cy );
    fwrite(hdr, strlen(hdr), 1, f);

    for (y = 0; y < rSize.cy; ++y )
    {
      for (x = 0; x < rSize.cx; ++x )
      {
        BYTE btV = (BYTE) BOUND( pshImage[x] + iBias, 0, 255 );
        fwrite( &btV, 1, 1, f);
      }
      pshImage += rSize.cx;
    }
  }

  fclose(f);

  return 0;
}

//------------------------------------------------------------------------------

int wci_image_dump_to_pgm(
  const IMAGE_EX * IN prImage
, const char *     IN szFilename
)
{
  FILE *   f;
  char     hdr[1024];
  int32_t  i;
  uint8_t *bmp1;
  uint8_t *bmp2;

  f = fopen( szFilename, "wb");
  if (f == NULL)
  {
    return -1;
  }
  else
  {
    const IMAGE_COMPONENT * prC;
    int nH;

    prC = &prImage->arComponent[0];

    switch( prImage->rInfo.eColorSpace )
    {
    case CSP_YV12:
      nH = 3 * prC->rSize.cy / 2;
      break;
    case CSP_YV24:
      nH = 3 * prC->rSize.cy;
      break;
    case CSP_GRAYSCALE:
      nH = prC->rSize.cy;
      break;
    default:
      fclose(f);
      return -1;
    }

    sprintf(hdr, "P5\n#WICA\n%d %d\n255\n", (int)prC->rSize.cx, nH );
    fwrite(hdr, strlen(hdr), 1, f);

    bmp1 = prC->pbtData;
    for (i = 0; i < prC->rSize.cy; i++)
    {
      fwrite(bmp1, prC->rSize.cx, 1, f);
      bmp1 += prC->rEdgedSize.cx;
    }

    prC = &prImage->arComponent[1];
    bmp1 = prImage->arComponent[1].pbtData;
    bmp2 = prImage->arComponent[2].pbtData;

    switch( prImage->rInfo.eColorSpace )
    {
    case CSP_YV12:
      for (i = 0; i < prC->rSize.cy; i++)
      {
        fwrite(bmp1, prC->rSize.cx, 1, f);
        fwrite(bmp2, prC->rSize.cx, 1, f);
        bmp1 += prC->rEdgedSize.cx;
        bmp2 += prC->rEdgedSize.cx;
      }
    	break;
    case CSP_YV24:
      for (i = 0; i < prC->rSize.cy; i++)
      {
        fwrite(bmp1, prC->rSize.cx, 1, f);
        bmp1 += prC->rEdgedSize.cx;
      }
      for (i = 0; i < prC->rSize.cy; i++)
      {
        fwrite(bmp2, prC->rSize.cx, 1, f);
        bmp2 += prC->rEdgedSize.cx;
      }
    	break;
    }
  }

  fclose(f);

  return 0;
}
#endif //ifdef _DEBUG

//------------------------------------------------------------------------------

int wci_image_get_size(
  SIZE        IN rSize
, COLOR_SPACE IN eColorSpace
)
{
  uint32_t nPels = wci_get_square( rSize );

  switch (eColorSpace & ~CSP_VFLIP)
  {
  case CSP_RGB555:
  case CSP_RGB565:
    return 2*nPels;

  case CSP_YV24A:
  case CSP_YV24:
    return 3*nPels;
    
  case CSP_BGR:
    return 4*((rSize.cx*BGR_SIZE + 3)/4)*rSize.cy;
    
  case CSP_BGRA:
    return 4*nPels;
    
  case CSP_I420:
  case CSP_YV12:
    return (3*nPels)/2;

  case CSP_YV12A:
    return (3*nPels)/2 + nPels;

  case CSP_YUY2:
  case CSP_YVYU:
  case CSP_UYVY:
    return 2*nPels;

  case CSP_GRAYSCALE:
    return 4*((rSize.cx + 3)/4)*rSize.cy;

  case CSP_NULL:
    return 0;

  default:
    assert( !eColorSpace );
    return -1;
  }
}

//------------------------------------------------------------------------------

static float wci_calc_psnr(
  const uint8_t * IN pOrig   //! Pointer to the 'original' image
, const uint8_t * IN pRecon  //! Pointer to the 'reconstructed' image
, uint32_t        IN nStride //! Stride of images
, uint32_t        IN nWidth  //! Image size
, uint32_t        IN nHeight
)
{
// This function calculates PSNR of the 'recon' image compared to the 'original' image.
// Return value is calculated PSNR value.

  int32_t x, y;
  float   fQuad = 0, fPSNR;

  for (y = 0; y < (int32_t)nHeight; y++)
  {
    uint32_t uQuad = 0;

    for (x = 0; x < (int32_t)nWidth; x++)
    {
      int32_t diff = *(pOrig + x) - *(pRecon + x);
      uQuad += diff * diff;
    }
    fQuad += uQuad;

    pOrig += nStride;
    pRecon += nStride;
  }

  fPSNR = fQuad / (nWidth * nHeight);

  if (fPSNR > 0)
  {
    fPSNR = (float) (255 * 255) / fPSNR;
    fPSNR = 10 * (float) log10(fPSNR);
  } else
    fPSNR = 99.999f;

  return fPSNR;
}

//------------------------------------------------------------------------------

void wci_image_get_psnr(
  const IMAGE_EX * IN  prOrig  //! Pointer to the 'original' image
, const IMAGE_EX * IN  prRecon //! Pointer to the 'reconstructed' image
, float            OUT afPSNR[MAX_IMAGE_COMPONENTS]
)
{
// This function calculates PSNR value for images.
// Return value is formally void but in fact PSNR values are retrieved

  int i;

  for ( i=0; i<prOrig->rInfo.iComponents; ++i )
  {
    const IMAGE_COMPONENT * prC = &prOrig->arComponent[i];
    afPSNR[i] = wci_calc_psnr( prC->pbtData, prRecon->arComponent[i].pbtData
                        , prC->rEdgedSize.cx, prC->rSize.cx, prC->rSize.cy );
  }
}

//------------------------------------------------------------------------------

void wci_image_clear(
  IMAGE_EX * IN OUT prImage
)
{
  int i;

  for ( i=0; i<prImage->rInfo.iComponents; ++i )
    wci_ic_clear( &prImage->arComponent[i] );
}

//------------------------------------------------------------------------------

void wci_image_copy(
  const IMAGE_EX * IN  prSrc
, IMAGE_EX *       OUT prDst
)
{
  int i;

  for ( i=0; i<prSrc->rInfo.iComponents; ++i )
    wci_ic_copy( &prSrc->arComponent[i], &prDst->arComponent[i] );
}

//------------------------------------------------------------------------------

ERROR_CODE wci_image_clip_output(
  const DIFF_IMAGE_EX * IN  prImage
, COLOR_SPACE           IN  eColorSpace
, int                   IN  iInterlaced
, RECT                  IN  rClipRect
, BYTE *                OUT pbtDst
)
{
  BOOL bFlip = (eColorSpace & CSP_VFLIP) ? TRUE : FALSE;
  eColorSpace = eColorSpace & ~CSP_VFLIP;
 
  switch( prImage->rInfo.eColorSpace )
  {
  case CSP_YV12:
    switch (eColorSpace)
    {
    case CSP_BGRA:
      wci_yv12_out_to_bgra_diffimg( prImage, rClipRect, pbtDst, bFlip );
      return ERR_NONE;

    case CSP_BGR:
      wci_yv12_out_to_bgr_diffimg( prImage, rClipRect, pbtDst );
      return ERR_NONE;

    case CSP_RGB565:
      wci_yv12_out_to_rgb565_diffimg( prImage, rClipRect, pbtDst );
      return ERR_NONE;
    }
    break;
  case CSP_YV12A:
    switch (eColorSpace)
    {
    case CSP_BGRA:
      wci_yv12a_out_to_bgra_diffimg( prImage, rClipRect, pbtDst, bFlip );
      return ERR_NONE;
    case CSP_RGB565:
      wci_yv12_out_to_rgb565_diffimg( prImage, rClipRect, pbtDst );
      return ERR_NONE;
    }
    break;

  case CSP_YV24:
    switch (eColorSpace)
    {
    case CSP_BGRA:
      wci_yv24_out_to_bgra_diffimg( prImage, rClipRect, pbtDst, bFlip );
      return ERR_NONE;

    case CSP_BGR:
      wci_yv24_out_to_bgr_diffimg( prImage, rClipRect, pbtDst );
      return ERR_NONE;

    case CSP_RGB565:
      wci_yv24_out_to_rgb565_diffimg( prImage, rClipRect, pbtDst );
      return ERR_NONE;
    }
    break;
    
  case CSP_YV24A:
    switch (eColorSpace)
    {
    case CSP_BGRA:
      wci_yv24a_out_to_bgra_diffimg( prImage, rClipRect, pbtDst, bFlip );
      return ERR_NONE;
    case CSP_RGB565:
      wci_yv24_out_to_rgb565_diffimg( prImage, rClipRect, pbtDst );
      return ERR_NONE;
    }
    break;
  case CSP_BGRA:
    switch (eColorSpace)
    {
    case CSP_BGRA:
      wci_bgra_out_to_bgra_diffimg( prImage, rClipRect, pbtDst, bFlip );
      return ERR_NONE;
    }
    break;
  case CSP_BGR:
    switch (eColorSpace)
    {
    case CSP_BGR:
      wci_bgr_out_to_bgr_diffimg( prImage, rClipRect, pbtDst );
      return ERR_NONE;
    case CSP_BGRA:
      wci_bgr_out_to_bgra_diffimg( prImage, rClipRect, pbtDst, bFlip);
      return ERR_NONE;
    case CSP_RGB565:
      wci_bgr_out_to_rgb565_diffimg( prImage, rClipRect, pbtDst );
      return ERR_NONE;
    }
    break;
  case CSP_GRAYSCALE:
    switch (eColorSpace)
    {
    case CSP_BGRA:
      wci_grayscale_out_to_bgra_diffimg( prImage, rClipRect, pbtDst, bFlip );
      return ERR_NONE;
    case CSP_GRAYSCALE:
      wci_grayscale_out_to_grayscale_diffimg( prImage, rClipRect, pbtDst );
      return ERR_NONE;
    }
    break;
  }

  if ( eColorSpace == CSP_NULL )
    return ERR_NONE;

  return ERR_FORMAT;
}

//------------------------------------------------------------------------------
#ifdef ENCODER_SUPPORT
ERROR_CODE wci_image_input(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, COLOR_SPACE  IN     eColorSpace
, int          IN     iInterlaced
, IMAGE_EX *   IN OUT prImage
)
{
  eColorSpace = eColorSpace & ~CSP_VFLIP;

  switch( prImage->rInfo.eColorSpace )
  {
    case CSP_YV12:
      switch (eColorSpace)
      {
        case CSP_BGR:
          wci_yv12_in_from_bgr( pbtSrc, rSrcSize, prImage );
          return ERR_NONE;
        case CSP_RGB565:
          wci_yv12_in_from_rgb565( pbtSrc, rSrcSize, prImage );
          return ERR_NONE;

        case CSP_NULL:
          return ERR_NONE;
      }
  	  break;

    case CSP_YV12A:
      switch (eColorSpace)
      {
      case CSP_BGRA:
        wci_yv12a_in_from_bgra( pbtSrc, rSrcSize, prImage );
        return ERR_NONE;

      case CSP_NULL:
        return ERR_NONE;
      }
      break;

    case CSP_YV24:
      switch (eColorSpace)
      {
        case CSP_BGR:
          wci_yv24_in_from_bgr( pbtSrc, rSrcSize, prImage );
          return ERR_NONE;
  
        case CSP_RGB565:
          wci_yv24_in_from_rgb565( pbtSrc, rSrcSize, prImage );
          return ERR_NONE;

        case CSP_NULL:
          return ERR_NONE;
      }
  	  break;

    case CSP_YV24A:
      switch (eColorSpace)
      {
      case CSP_BGRA:
        wci_yv24a_in_from_bgra( pbtSrc, rSrcSize, prImage );
        return ERR_NONE;
      case CSP_NULL:
        return ERR_NONE;
      }
      break;

    case CSP_BGRA:
      switch (eColorSpace)
      {
#ifdef ENCODER_SUPPORT
        case CSP_BGRA:
          wci_bgra_in_from_bgra( pbtSrc, rSrcSize, prImage );
          return ERR_NONE;
#endif

        case CSP_NULL:
          return ERR_NONE;
      }
  	  break;

    case CSP_BGR:
      switch (eColorSpace)
      {
#ifdef ENCODER_SUPPORT
    case CSP_BGR:
      wci_bgr_in_from_bgr( pbtSrc, rSrcSize, prImage );
      return ERR_NONE;
    case CSP_BGRA:
      wci_bgr_in_from_bgra( pbtSrc, rSrcSize, prImage );
      return ERR_NONE;
    case CSP_RGB565:
      wci_bgr_in_from_rgb565( pbtSrc, rSrcSize, prImage );
      return ERR_NONE;
#endif

    case CSP_NULL:
      return ERR_NONE;
      }
      break;


    case CSP_GRAYSCALE:
      switch (eColorSpace)
      {
      case CSP_BGRA:
        wci_grayscale_in_from_bgra( pbtSrc, rSrcSize, prImage );   
        return ERR_NONE;

      case CSP_GRAYSCALE:
        wci_grayscale_in_from_grayscale( pbtSrc, rSrcSize, prImage );
        return ERR_NONE;
      }
      break;
  }
  return ERR_FORMAT;
}
#endif // #ifdef ENCODER_SUPPORT

//------------------------------------------------------------------------------

ERROR_CODE wci_image_expand_rect(
  SIZE         IN     rRectSize
, IMAGE_EX *   IN OUT prImage
)
{
  if ( rRectSize.cx != prImage->rInfo.rSize.cx || rRectSize.cy != prImage->rInfo.rSize.cy )
  {
    SIZE arSize[MAX_IMAGE_COMPONENTS];
    int  i;

    int  err = wci_image_set_component_size( rRectSize, prImage->rInfo.eColorSpace, arSize );
    ERR_CHECK( err );

    for ( i=0; i<prImage->rInfo.iComponents; ++i )
      wci_ic_expandrect( arSize[i], &prImage->arComponent[i] );
  }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

IMAGE wci_image_get_image(
  const IMAGE_EX * IN prImage
)
{
  IMAGE rImage;

  rImage.y = prImage->arComponent[0].pbtData;
  rImage.u = prImage->arComponent[1].pbtData;
  rImage.v = prImage->arComponent[2].pbtData;

  return rImage;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_diffimage_create(
  SIZE                 IN     rSize
, COLOR_SPACE          IN     eColorSpace
, PIXEL_SAMPLING       IN     ePixelSampling
, DIFF_IMAGE_EX *         OUT prImage
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
)
{
  int  i, err;
  SIZE arSize[MAX_IMAGE_COMPONENTS];

  prImage->rInfo.rSize          = rSize         ;
  prImage->rInfo.eColorSpace    = eColorSpace   ;
  prImage->rInfo.ePixelSampling = ePixelSampling;

  switch(eColorSpace) {
  case CSP_GRAYSCALE:
    prImage->rInfo.iComponents = 1;
    break;
  case CSP_YV12A:
  case CSP_YV24A:
  case CSP_BGRA:
    prImage->rInfo.iComponents = 4;
    break;
  default:
    prImage->rInfo.iComponents = 3;
  }
  err = wci_image_set_component_size( rSize, eColorSpace, arSize );
  ERR_CHECK( err );

  for ( i=0; i<prImage->rInfo.iComponents; ++i )
  {
    err = wci_dic_create( arSize[i], 0, &prImage->arComponent[i], prMemHeap );
    ERR_CHECK( err );
  }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

void wci_diffimage_destroy(
  DIFF_IMAGE_EX *      IN OUT prImage
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
)
{
  int i;

  for ( i=0; i<prImage->rInfo.iComponents; ++i )
  {
    wci_dic_destroy( &prImage->arComponent[i], prMemHeap );
  }
  memset( prImage, 0, sizeof(DIFF_IMAGE_EX) );
}

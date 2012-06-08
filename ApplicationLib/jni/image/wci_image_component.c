/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: image_component.c

ABSTRACT:    This file contains procedures for operations with image component

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include <string.h>

#include "../wci_global.h"
#include "wci_image_component.h"

//------------------------------------------------------------------------------

static int wci_ic_get_offset(
  const IMAGE_COMPONENT * IN prComponent
)
{
  return prComponent->iEdgeSize*(prComponent->rEdgedSize.cy + 1);
}

//------------------------------------------------------------------------------

ERROR_CODE wci_ic_create(
  SIZE                 IN     rSize
, int                  IN     iEdgeSize
, IMAGE_COMPONENT *    OUT    prComponent
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
)
{
  void * pData;
  
  memset( prComponent, 0, sizeof(IMAGE_COMPONENT) );

  prComponent->rSize = rSize;
  
  prComponent->iEdgeSize = iEdgeSize;
  
  prComponent->rEdgedSize.cx = rSize.cx + 2*iEdgeSize;
  prComponent->rEdgedSize.cy = rSize.cy + 2*iEdgeSize;
  
  pData = wci_aligned_malloc( wci_get_square( prComponent->rEdgedSize ), CACHE_LINE, prMemHeap );
  if ( pData == NULL )
  {
    return ERR_MEMORY;
  }
  prComponent->pbtData = (BYTE *)pData + wci_ic_get_offset(prComponent);
  
  return ERR_NONE;
}

//------------------------------------------------------------------------------

void wci_ic_set(
  SIZE                 IN     rSize
, int                  IN     iEdgeSize
, BYTE *               IN     pbtData
, IMAGE_COMPONENT *    OUT    prComponent
)
{
  memset( prComponent, 0, sizeof(IMAGE_COMPONENT) );

  prComponent->rSize = rSize;
  
  prComponent->iEdgeSize = iEdgeSize;
  
  prComponent->rEdgedSize.cx = rSize.cx + 2*iEdgeSize;
  prComponent->rEdgedSize.cy = rSize.cy + 2*iEdgeSize;
  
  prComponent->pbtData = pbtData + wci_ic_get_offset(prComponent);
}

//------------------------------------------------------------------------------

void wci_ic_destroy(
  IMAGE_COMPONENT * IN OUT prComponent
, ALIGNED_MEMMANAGER * IN  prMemHeap
)
{
  if ( !prComponent->bUseExternalBuffer )
  {
    void * pData = prComponent->pbtData - wci_ic_get_offset(prComponent);
    wci_aligned_free( pData, prMemHeap );
  }
  memset( prComponent, 0, sizeof(IMAGE_COMPONENT) );
}

//------------------------------------------------------------------------------

void wci_ic_clear(
  IMAGE_COMPONENT *    IN OUT prComponent
)
{
  void * pbtData = prComponent->pbtData - wci_ic_get_offset(prComponent);

  memset( pbtData, 0, wci_get_square(prComponent->rEdgedSize) );
}

//------------------------------------------------------------------------------

void wci_ic_copy(
  const IMAGE_COMPONENT * IN  prSrc
,       IMAGE_COMPONENT * OUT prDst
)
{
  void * pbtSrc;
  void * pbtDst;
  
  assert( prSrc->rSize.cx == prDst->rSize.cx );
  assert( prSrc->rSize.cy == prDst->rSize.cy );
  assert( prSrc->rEdgedSize.cx == prDst->rEdgedSize.cx );
  assert( prSrc->rEdgedSize.cy == prDst->rEdgedSize.cy );
  
  pbtSrc = prSrc->pbtData - wci_ic_get_offset( prSrc );
  pbtDst = prDst->pbtData - wci_ic_get_offset( prDst );

  memcpy( pbtDst, pbtSrc, wci_get_square(prSrc->rEdgedSize) );
}

//------------------------------------------------------------------------------

void wci_ic_expandrect(
  SIZE              IN     rRectSize
, IMAGE_COMPONENT * IN OUT prC
)
{
  const int w = prC->rSize.cx;
  const int h = prC->rSize.cy;
  int y;

  if ( rRectSize.cx < w )
  {
    int iDiff = w - rRectSize.cx;
    for ( y=0; y<rRectSize.cy; ++y )
    {
      BYTE * pbtLine = &prC->pbtData[y*w];
      memset( &pbtLine[rRectSize.cx], pbtLine[rRectSize.cx-1], iDiff );
    }
  }

  if ( rRectSize.cy < h )
  {
    BYTE * pbtSrcLine = &prC->pbtData[(rRectSize.cy-1)*w];
    BYTE * pbtDstLine = &pbtSrcLine[w];

    for ( y=rRectSize.cy; y<h; ++y )
    {
      memcpy( pbtDstLine, pbtSrcLine, w );
      pbtDstLine += w;
    }
  }
}

//------------------------------------------------------------------------------

static int wci_dic_getoffset(
  const DIFF_IMAGE_COMPONENT * IN prComponent
)
{
  return sizeof(int16_t)*prComponent->iEdgeSize*(prComponent->rEdgedSize.cy + 1);
}

//------------------------------------------------------------------------------

ERROR_CODE wci_dic_create(
  SIZE                   IN     rSize
, int                    IN     iEdgeSize
, DIFF_IMAGE_COMPONENT *    OUT prComponent
, ALIGNED_MEMMANAGER *   IN OUT prMemHeap
)
{
  void * pData;
  
  memset( prComponent, 0, sizeof(DIFF_IMAGE_COMPONENT) );

  prComponent->rSize = rSize;
  
  prComponent->iEdgeSize = iEdgeSize;
  
  prComponent->rEdgedSize.cx = rSize.cx + 2*iEdgeSize;
  prComponent->rEdgedSize.cy = rSize.cy + 2*iEdgeSize;
  
  pData = wci_aligned_malloc( sizeof(int16_t)*wci_get_square( prComponent->rEdgedSize ), CACHE_LINE, prMemHeap );
  if ( pData == NULL )
  {
    return ERR_MEMORY;
  }
  prComponent->piData = (int16_t *)pData + wci_dic_getoffset(prComponent);
  
  return ERR_NONE;
}


//------------------------------------------------------------------------------

void wci_dic_destroy(
  DIFF_IMAGE_COMPONENT * IN OUT prComponent
, ALIGNED_MEMMANAGER *   IN OUT prMemHeap
)
{
  if ( !prComponent->bUseExternalBuffer )
  {
    void * pData = prComponent->piData - wci_dic_getoffset(prComponent);
    wci_aligned_free( pData, prMemHeap );
  }
  memset( prComponent, 0, sizeof(DIFF_IMAGE_COMPONENT) );
}

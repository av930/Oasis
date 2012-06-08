/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_image_component.h

ABSTRACT:    This file contains definitions and data structures for operations with image component

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef IMAGE_COMPONENT_H
#define IMAGE_COMPONENT_H

#include "../wci_codec.h"
#include "../utils/wci_mem_align.h"

//------------------------------------------------------------------------------

typedef struct tagIMAGE_COMPONENT
{
  BYTE * pbtData;

  SIZE   rSize;

  int    iEdgeSize;

  SIZE   rEdgedSize;

  int    iReadyLines;
  
  BOOL   bUseExternalBuffer;

} IMAGE_COMPONENT;


typedef struct tagDIFF_IMAGE_COMPONENT
{
  int16_t * piData;

  SIZE      rSize;

  int       iEdgeSize;

  SIZE      rEdgedSize;

  int       iReadyLines;

  BOOL      bUseExternalBuffer;

} DIFF_IMAGE_COMPONENT;


//------------------------------------------------------------------------------

ERROR_CODE wci_ic_create(
  SIZE                 IN     rSize
, int                  IN     iEdgeSize
, IMAGE_COMPONENT *    OUT    prComponent
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
);

//------------------------------------------------------------------------------

void wci_ic_set(
  SIZE                 IN     rSize
, int                  IN     iEdgeSize
, BYTE *               IN     pbtData
, IMAGE_COMPONENT *    OUT    prComponent
);

//------------------------------------------------------------------------------

void wci_ic_destroy(
  IMAGE_COMPONENT *    IN OUT prComponent
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
);

//------------------------------------------------------------------------------

void wci_ic_clear(
  IMAGE_COMPONENT *    IN OUT prComponent
);

//------------------------------------------------------------------------------

void wci_ic_copy(
  const IMAGE_COMPONENT * IN  prSrc
,       IMAGE_COMPONENT * OUT prDst
);

//------------------------------------------------------------------------------

void wci_ic_expandrect(
  SIZE              IN     rRectSize
, IMAGE_COMPONENT * IN OUT prComponent
);

//------------------------------------------------------------------------------

void wci_ic_expand(
  SIZE                 IN     rBoundary
, IMAGE_COMPONENT *    IN OUT prComponent
);

//------------------------------------------------------------------------------

ERROR_CODE wci_dic_create(
  SIZE                   IN     rSize
, int                    IN     iEdgeSize
, DIFF_IMAGE_COMPONENT *    OUT prComponent
, ALIGNED_MEMMANAGER *   IN OUT prMemHeap
);

//------------------------------------------------------------------------------

void wci_dic_destroy(
  DIFF_IMAGE_COMPONENT * IN OUT prComponent
, ALIGNED_MEMMANAGER *   IN OUT prMemHeap
);

#endif // #ifndef IMAGE_COMPONENT_H

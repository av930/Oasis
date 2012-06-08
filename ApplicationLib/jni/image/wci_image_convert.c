/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: image_convert.c

ABSTRACT:    This file contains procedures for image<->diff convertations

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include "../wci_global.h"
#include "wci_image_convert.h"

//------------------------------------------------------------------------------

#define BOUND_PIXEL(v, i) if (v < 0) { v = 0; i++; } else if (v > 255) { v = 255; i++; }


//------------------------------------------------------------------------------

#ifdef ENCODER_SUPPORT
  CONVERT_IMAGE_TO_DIFF_IMAGE_FUNC_PTR ConvertImageToDiffImage;
  CONVERT_DIFF_IMAGE_TO_IMAGE_FUNC_PTR ConvertDiffImageToImage;
#endif

#ifdef USE_BOUND_ARRAY
  BYTE g_btBoundArray[1280];
#endif                      

//------------------------------------------------------------------------------

#ifdef USE_BOUND_ARRAY
 void wci_init_bound_array( void )
 {
    int i;
    BYTE *pbtBoundArray = g_btBoundArray;

    for(i=768;i<1280;i++)
      pbtBoundArray[i] = 255;
      
    for(i=0;i<512;i++)
      pbtBoundArray[i] = 0;
      
    for(i=512;i<768;i++)
      pbtBoundArray[i] = (BYTE)(i-512);
  }
#endif                      

//------------------------------------------------------------------------------

void wci_image_convert_to_diffimage(
  const IMAGE_EX * IN  prImage
, DIFF_IMAGE_EX *  OUT prDiffImage
)
{
  int iComponent;

  for ( iComponent=0; iComponent<prImage->rInfo.iComponents; ++iComponent )
  {
    const IMAGE_COMPONENT * prC = &prImage->arComponent[ iComponent ];

    BYTE *    pbtImage = prC->pbtData;
    int16_t * piImage = prDiffImage->arComponent[ iComponent ].piData;
    int       nLength = wci_get_square( prC->rEdgedSize ), i;

    for ( i=0; i<nLength; i++ )
    {
      piImage[i] = pbtImage[i];
    }
  }
}

//------------------------------------------------------------------------------

void wci_diffimage_convert_to_image(
  const DIFF_IMAGE_EX * IN  prDiffImage
, IMAGE_EX *            OUT prImage
)
{
  int iComponent;
  int bounding = 0;

  for ( iComponent=0; iComponent<prImage->rInfo.iComponents; ++iComponent )
  {
    const IMAGE_COMPONENT * prC = &prImage->arComponent[ iComponent ];
    BYTE *    pbtImage = prC->pbtData;
    int16_t * piImage = prDiffImage->arComponent[ iComponent ].piData;
    int       nLength = wci_get_square( prC->rEdgedSize ), i;


    for ( i=0; i<nLength; i++ )
    {
      #ifdef USE_BOUND_ARRAY
        pbtImage[i] = g_btBoundArray[ piImage[i] ];
      #else
        int pixel = piImage[i];
        BOUND_PIXEL(pixel, bounding);
        pbtImage[i] = (BYTE)pixel;
      #endif
    }
  }
}

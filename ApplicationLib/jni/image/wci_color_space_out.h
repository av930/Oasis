/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: ColorSpaceOut.c

ABSTRACT:    This file contains definitions and data structures for color space YUV to RGB transformation

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef COLOR_SPACE_OUT_H
#define COLOR_SPACE_OUT_H

#include "../wci_portab.h"
#include "wci_image.h"

//------------------------------------------------------------------------------

typedef void CSP_OUT_FUNCTION_DIFFIMG(
  const DIFF_IMAGE_EX * IN  image
, RECT                  IN  clip_rect
, BYTE *                OUT bitmap
);

typedef void CSP_OUT_FUNCTION_DIFFIMG_FLIP(
   const DIFF_IMAGE_EX * IN  image
 , RECT                  IN  clip_rect
 , BYTE *                OUT bitmap
 , BOOL                  IN  vflip
);


//------------------------------------------------------------------------------

CSP_OUT_FUNCTION_DIFFIMG wci_yv12_out_to_bgr_diffimg;
CSP_OUT_FUNCTION_DIFFIMG wci_yv12_out_to_rgb565_diffimg;
CSP_OUT_FUNCTION_DIFFIMG wci_yv24_out_to_bgr_diffimg;
CSP_OUT_FUNCTION_DIFFIMG wci_yv24_out_to_rgb565_diffimg;
CSP_OUT_FUNCTION_DIFFIMG wci_bgr_out_to_bgr_diffimg;
CSP_OUT_FUNCTION_DIFFIMG wci_bgr_out_to_rgb565_diffimg;
CSP_OUT_FUNCTION_DIFFIMG wci_grayscale_out_to_grayscale_diffimg;


CSP_OUT_FUNCTION_DIFFIMG_FLIP wci_yv12_out_to_bgra_diffimg;
CSP_OUT_FUNCTION_DIFFIMG_FLIP wci_yv12a_out_to_bgra_diffimg;
CSP_OUT_FUNCTION_DIFFIMG_FLIP wci_bgr_out_to_bgra_diffimg;
CSP_OUT_FUNCTION_DIFFIMG_FLIP wci_bgra_out_to_bgra_diffimg;
CSP_OUT_FUNCTION_DIFFIMG_FLIP wci_grayscale_out_to_bgra_diffimg;
CSP_OUT_FUNCTION_DIFFIMG_FLIP wci_yv24_out_to_bgra_diffimg;
CSP_OUT_FUNCTION_DIFFIMG_FLIP wci_yv24a_out_to_bgra_diffimg;


#endif // #ifndef COLOR_SPACE_OUT_H


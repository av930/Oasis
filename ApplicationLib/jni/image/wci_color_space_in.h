/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_color_space_in.h

ABSTRACT:    This file contains definitions and data structures for color space RGB to YUV transformation

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef COLOR_SPACE_IN_H
#define COLOR_SPACE_IN_H

#include "../wci_portab.h"
#include "wci_image.h"

//------------------------------------------------------------------------------

typedef void CSP_IN_FUNCTION(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
);

//------------------------------------------------------------------------------

CSP_IN_FUNCTION wci_yv12_in_from_bgr;
CSP_IN_FUNCTION wci_yv12_in_from_bgra;
CSP_IN_FUNCTION wci_yv12_in_from_rgb565;
CSP_IN_FUNCTION wci_yv12_in_from_yv12;
CSP_IN_FUNCTION wci_yv12a_in_from_bgra;

CSP_IN_FUNCTION wci_yv24_in_from_bgr;
CSP_IN_FUNCTION wci_yv24_in_from_bgra;
CSP_IN_FUNCTION wci_yv24_in_from_rgb565;
CSP_IN_FUNCTION wci_yv24a_in_from_bgra;

CSP_IN_FUNCTION wci_bgr_in_from_bgr;
CSP_IN_FUNCTION wci_bgr_in_from_bgra;
CSP_IN_FUNCTION wci_bgr_in_from_rgb565;
CSP_IN_FUNCTION wci_bgra_in_from_bgra;

CSP_IN_FUNCTION wci_grayscale_in_from_grayscale;
CSP_IN_FUNCTION wci_grayscale_in_from_bgra;

#endif // #ifndef COLOR_SPACE_IN_H


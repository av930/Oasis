/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_image_convert.h

ABSTRACT:    This file contains definitions for image<->diff convertations

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef IMAGE_CONVERT_H
#define IMAGE_CONVERT_H

#include "wci_image.h"

//------------------------------------------------------------------------------

//#define USE_BOUND_ARRAY

//------------------------------------------------------------------------------

void wci_image_convert_to_diffimage(
  const IMAGE_EX * IN  prImage
, DIFF_IMAGE_EX *  OUT prDiffImage
);

//------------------------------------------------------------------------------

void wci_diffimage_convert_to_image(
  const DIFF_IMAGE_EX * IN  prDiffImage
, IMAGE_EX *            OUT prImage
);

//------------------------------------------------------------------------------

typedef void CONVERT_IMAGE_TO_DIFF_IMAGE(
  IMAGE *      prImage
, int          iPels
, int          iLines
, DIFF_IMAGE * prDiffImage);

//------------------------------------------------------------------------------

typedef CONVERT_IMAGE_TO_DIFF_IMAGE * CONVERT_IMAGE_TO_DIFF_IMAGE_FUNC_PTR;

//------------------------------------------------------------------------------

typedef void CONVERT_DIFF_IMAGE_TO_IMAGE(
  DIFF_IMAGE * prDiffImage
, int          iPels
, int          iLines
, IMAGE *      prImage );

//------------------------------------------------------------------------------

typedef CONVERT_DIFF_IMAGE_TO_IMAGE * CONVERT_DIFF_IMAGE_TO_IMAGE_FUNC_PTR;

//------------------------------------------------------------------------------

extern CONVERT_IMAGE_TO_DIFF_IMAGE_FUNC_PTR ConvertImageToDiffImage;
extern CONVERT_DIFF_IMAGE_TO_IMAGE_FUNC_PTR ConvertDiffImageToImage;

#ifdef USE_BOUND_ARRAY
  void wci_init_bound_array( void );
#endif

CONVERT_IMAGE_TO_DIFF_IMAGE ConvertImageToDiffImage_mmx;
CONVERT_DIFF_IMAGE_TO_IMAGE ConvertDiffImageToImage_mmx;

#endif // #ifndef IMAGE_CONVERT_H

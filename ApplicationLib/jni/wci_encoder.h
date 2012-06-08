/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_encoder.h

ABSTRACT:    This file contains definitions and data structures for image encoding

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef ENCODER_H
#define ENCODER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "wci_global.h"
#include "image/wci_image.h"
#include "wav_codec/wci_wav_codec.h"

//------------------------------------------------------------------------------

#define PSNR_STATISTICS

//------------------------------------------------------------------------------

typedef struct
{
  int   iGlobalFlags;        //! Flags of global configuration 

  COLOR_SPACE    eColorSpace;    //! Color space
  PIXEL_SAMPLING ePixelSampling; //! Pixel sampling format

  int   iSpeedOrQuality;     //! Relation between encoding speed and quality 
  int   iQuality;            //! Relation between quality and compression 

  SIZE  rSize;               //! Input frame image size
  SIZE  rMaxEvenSize;        //! Input max frame image evened size

  IMAGE_EX        rImageEx;  //! Image for input and reconstructed operation

  WAV_CODEC_PARAM rWavParam;

  SCALABILITY     rScalability;
  
  WAV_CODEC_DATA  rWavData;  //! Data buffers for wavelet transformation 

#ifdef PSNR_STATISTICS
  IMAGE_EX        rOriginalImage;
#endif

} ENCODER;

//------------------------------------------------------------------------------

int wci_encoder_create(ENC_PARAM * pParam);

//------------------------------------------------------------------------------

int wci_encoder_destroy(ENCODER* pEnc);

//------------------------------------------------------------------------------

int wci_encoder_encode(ENCODER* pEnc, ENC_FRAME* pFrame, ENC_STATS* pResult);

#ifdef __cplusplus
}
#endif

#endif // #ifndef ENCODER_H

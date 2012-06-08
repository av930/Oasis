/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_wav_layer.h

ABSTRACT:    This file contains definitions and data structures for wavelet transform

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef WAV_LAYER_H
#define WAV_LAYER_H

#include "../wci_codec.h"
#include "wci_wav_codec.h"

//------------------------------------------------------------------------------

void wci_wav_inv_trans_image_ex(
  const WAV_CODEC_PARAM * IN     prWavParam //! the structure of wavelet parameters
, DIFF_IMAGE_EX *         IN OUT prImage    //! a pointer to the image array
);

void wci_wav_encode_image(
  WAV_CODEC_PARAM * IN     prWavParam         //! the structure of wavelet parameters
, WAV_CODEC_DATA *  IN OUT prWavData          //! the structure of wavelet data
, IMAGE_EX *        IN OUT prImage            //! to the array of the current image
, BITSTREAM *       IN OUT prBS               //! the structure of all buffers of bitstream
, BOOL              IN     bReconstruct       //! a flag if reconstruction is required or not
, QSP_SIZE *           OUT prQSP_Size         //! QSP size of output bitstream
);

ERROR_CODE wci_wav_decode_image(
  WAV_CODEC_PARAM *    OUT prWavParam         //! the structure of wavelet parameters
, WAV_CODEC_DATA *  IN OUT prWavData          //! the structure of wavelet data
, IMAGE_EX *        IN OUT prImage            //! to the array of the current image
, BITSTREAM *       IN OUT prBS               //! the structure of all buffers of bitstream
);

void wci_wav_encode_diffimage(
  const WAV_CODEC_PARAM * IN     prWavParam   //! the structure of wavelet parameters
, WAV_CODEC_DATA *        IN OUT prWavData    //! the structure of wavelet data
, DIFF_IMAGE_EX *         IN OUT prImage      //! to the array of the current image
, BITSTREAM *             IN OUT prBS         //! the structure of all buffers of bitstream
, BOOL                    IN     bReconstruct //! a flag if reconstruction is required or not
, QSP_SIZE *                 OUT prQSP_Size   //! QSP size of output bitstream
, QSP_BUFFER*             IN OUT prQSP
, QSP_SIZE*                  OUT prBlockSizeQSP
, int32_t*                   OUT piNumberOfBlocks
);

ERROR_CODE wci_wav_decode_diffimage(
  WAV_CODEC_PARAM *    OUT prWavParam         //! the structure of wavelet parameters
, WAV_CODEC_DATA *  IN OUT prWavData          //! the structure of wavelet data
, DIFF_IMAGE_EX *   IN OUT prImage            //! to the array of the current image
, BITSTREAM *       IN OUT prBS               //! the structure of all buffers of bitstream
);
  
#endif // #ifndef WAV_LAYER_H

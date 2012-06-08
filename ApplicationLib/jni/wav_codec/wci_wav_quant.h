/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_wav_quant.h

ABSTRACT:    This file contains definitions and data structures for quantization of wavelet coefficients

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00
             2004.08.27   v1.01 Quantizer range is increased

*******************************************************************************/

#ifndef WAVQUANT_H
#define WAVQUANT_H

#include "../wci_portab.h"
#include "wci_wav_codec.h"
#include "wci_adpcm.h"

//------------------------------------------------------------------------------

#define WAV_MIN_QUANTIZER     1
#define WAV_MAX_I_QUANTIZER   31
#define WAV_MAX_P_QUANTIZER   1

#define WAV_I_QUANTIZER_RANGE ((1<<(MAX_I_WAV_STEPS-1))*WAV_MAX_I_QUANTIZER-WAV_MIN_QUANTIZER+1)
#define WAV_P_QUANTIZER_RANGE ((1<<(MAX_P_WAV_STEPS-1))*WAV_MAX_P_QUANTIZER-WAV_MIN_QUANTIZER+1)

#define WAV_MAX_QUANTIZER     max( WAV_MAX_I_QUANTIZER, WAV_MAX_P_QUANTIZER )
#define WAV_QUANTIZER_RANGE   max( WAV_I_QUANTIZER_RANGE, WAV_P_QUANTIZER_RANGE )

#define WAV_MAX_QUANT_VALUE   min( (1<<(MAX_WAV_STEPS-1))*255, 32767 )

#define QUANT_BLOCK_SIZE      2

#define NORM_DEAD_ZONE        25 // from -100 to 100

#define QUANTIZER_SCALE_FACTOR  100

//------------------------------------------------------------------------------

typedef enum
{
  eemDPCM        = 1,
  eemLPC_DPCM    = 2,
  eemLinearQuant = 4

} enumLL_EncodingMode;

typedef struct tagWAV_QUANTIZER
{
  int   iQuantizer;

  short shDeQuant[256];
  
#ifdef ENCODER_SUPPORT
  QUANT_VALUE arrQuantValue[1 + 2*WAV_MAX_QUANT_VALUE];
#endif

} WAV_QUANTIZER;

//------------------------------------------------------------------------------

extern WAV_QUANTIZER g_arrWavQuantData[WAV_QUANTIZER_RANGE];
extern int * g_pTabAbs;
extern int * g_pTabSqr;

//------------------------------------------------------------------------------

int wci_wav_get_quantizer_by_index( 
  int IN iWavSteps
, int IN index 
);

//------------------------------------------------------------------------------

static int __inline wci_get_dead_zone( int iQuantizer )
{
  return (iQuantizer+1)/2;
}

#ifdef ENCODER_SUPPORT

//------------------------------------------------------------------------------

static QUANT_VALUE __inline * wci_get_quant_value( int iQuantizer )
{
  return &g_arrWavQuantData[ iQuantizer - WAV_MIN_QUANTIZER ].arrQuantValue[WAV_MAX_QUANT_VALUE];
}

#endif

//------------------------------------------------------------------------------

void wci_init_wav_quant_data( void );

void wci_set_quant_scale(
  int iQuantizer
, int iWavSteps
, WAV_QUANT_SCALE rQS[MAX_WAV_STEPS] );

#ifdef ENCODER_SUPPORT

//------------------------------------------------------------------------------

  void wci_wav_encode_block(
    short *psImage, SIZE rBlockSize,
    int iQuantizer_H, int iQuantizer_HH, enumWT_Mode eMode,
    BYTE * pbtPartition, QSP_BUFFER *prQSP, BOOL bReconstruct );

//------------------------------------------------------------------------------

  void wci_wav_encodell_block(
    short *                 IN OUT psImage
  , SIZE                    IN     rBlockSize
  , const WAV_CODEC_PARAM * IN     prWavParam
  , BYTE *                  OUT    pbtPartition
  , QSP_BUFFER *            IN OUT prQSP );

#endif

//------------------------------------------------------------------------------

SIZE wci_wav_get_partition_block(
  SIZE IN rSize
);

//------------------------------------------------------------------------------

void wci_wav_decode_block(
  short *           pshImage
, SIZE              rBlockSize
, WAV_QUANT_SCALE * prQS
, enumWT_Mode       eMode
, BYTE *            pbtPartition
, QSP_BUFFER *      prQSP );

//------------------------------------------------------------------------------

void wci_wav_decodell_block(
  short *                 IN OUT psImage
, SIZE                    IN     rBlockSize
, const WAV_CODEC_PARAM * IN OUT prWavParam
, BYTE *                     OUT pbtPartition
, QSP_BUFFER *            IN OUT prQSP
);

//------------------------------------------------------------------------------

void wci_wav_decodeblock_lh_hl_hh(
  short *      IN OUT pshImage
, SIZE         IN     rBlockSize
, short *      IN     pshDeQuant_H
, short *      IN     pshDeQuant_HH
, BYTE *          OUT pbtPartition
, QSP_BUFFER * IN OUT prQSP );

#endif // ifndef WAVQUANT_H

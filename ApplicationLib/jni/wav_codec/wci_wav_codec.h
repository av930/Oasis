/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_wav_codec.h

ABSTRACT:    This file contains definitions and data structures for initialization and setting of parameters of
             block-oriented wavelet codec

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef WAV_CODEC_H
#define WAV_CODEC_H

#include "../wci_portab.h"
#include "../wci_codec.h"
#include "../wci_global.h"
#include "../image/wci_image.h"
#include "../bitstream/wci_bitstream.h"
#include "../compress/wci_qsp_compress.h"

#include "wci_wavelet.h"

//------------------------------------------------------------------------------

#define MIN_WAV_STEPS         0

#define MAX_I_WAV_STEPS       4
#define MAX_P_WAV_STEPS       2

#define MAX_WAV_STEPS         max( MAX_I_WAV_STEPS, MAX_P_WAV_STEPS )

#define MAX_WAV_LAYERS        2

#define WAV_SPATIAL_LAYERS    0x1

#define MAX_WAV_COMPONENTS    4

//------------------------------------------------------------------------------

typedef enum
{
  LH =  0
, HL =  1
, HH =  2
, LL =  3

} enumWavComponent;

typedef enum {
  ewtHV = 0
, ewtH  = 1
, ewtV  = 2

} enumWT_Mode;                         //! Geometrical transformation mode

typedef struct tagWAV_SIZE
{
  int iLL;
  int iWC[MAX_WAV_STEPS];

} WAV_SIZE;

typedef struct tagQUANT_VALUE
{
  BYTE  btQuant;
  BYTE  btSign;  
  short shDeQuant;

} QUANT_VALUE;

typedef struct tagWAV_QUANT_SCALE
{
  int          iQuantizer_LL;
  int          iBlockDeadZone_LL;
  QUANT_VALUE *prQV_LL;

  int          iQuantizer_H;
  int          iBlockDeadZone_H;
  QUANT_VALUE *prQV_H;

  int          iQuantizer_HH;
  int          iBlockDeadZone_HH;
  QUANT_VALUE *prQV_HH;

} WAV_QUANT_SCALE;

typedef struct tagWAV_LAYER
{
  int   iLayer;                        //! Order number of bitstream layer

  SIZE  rBlockSize;                    //! Block size of wavelet transform step
  SIZE  rInvBlockSize;

  int   iWavSteps;                     //! Sets the number of steps for wavelet transform
  enumWT_Mode eMode;                   //! Geometrical mode of wavelet transform layer

  int   iQuantizer;                    //! Sets the fixed value of the quantizer
  WAV_QUANT_SCALE rQS[MAX_WAV_STEPS];

} WAV_LAYER;

typedef struct tagWAV_SPATIAL_SCALABILITY
{
  int iType;
  int iHorzFactor;
  int iVertFactor;

} WAV_SPATIAL_SCALABILITY;

typedef struct tagWAV_CODEC_PARAM
{
  IMAGE_INFO rImageInfo;               //! Parameters of transformed image

  int   iMaxWavSteps;                  //! Sets the number of steps for wavelet transform

  int   iQuantizer;                    //! Sets the fixed value of the quantizer
  
  int   iQuantizerIndex;               //! Sets the index of the quantizer in quantizer table

  int   iQuantMode;                    //! Mode of lossy compression
  
  int   iCompressMode;                 //! Mode of lossless compression

                                       //! Layer definitions (luma and chrominance)
  WAV_LAYER arWL[MAX_IMAGE_COMPONENTS][MAX_WAV_LAYERS];
  
  int   iLayers;                       //! Number of bitstream layers

  WAV_SPATIAL_SCALABILITY rScalability;//! Spatial scalability

} WAV_CODEC_PARAM;

typedef struct tagWAV_CODEC_DATA
{
  DIFF_IMAGE_EX  rImageEx;             //! Difference image

  QSP_BUFFER     rQSP;                 //! Buffer for writing output data
  QSP_COMPRESSOR rQSP_Compressor;      //! Instance QSP compressor

  BYTE * pbtPartition;                 //! The partition array

  short *pWav_Buf;                     //! Buffer for wavelet transform
  
} WAV_CODEC_DATA;

//------------------------------------------------------------------------------

ERROR_CODE wci_wav_init_data(
  SIZE                 IN     rImageSize     //! Image properies
, COLOR_SPACE          IN     eColorSpace
, PIXEL_SAMPLING       IN     ePixelSampling
, WAV_CODEC_DATA *        OUT prWavData      //! Wavelet codec buffer structure
, ALIGNED_MEMMANAGER * IN OUT prMemHeap      //! Embedded Mem Manager or NULL if system mem manager is used
);

//------------------------------------------------------------------------------

void wci_wav_done_data(
  WAV_CODEC_DATA *     IN OUT prWavData      //! Wavelet codec buffer structure
, ALIGNED_MEMMANAGER * IN OUT prMemHeap      //! Embedded Mem Manager or NULL if system mem manager is used
);

//------------------------------------------------------------------------------

int wci_wav_get_wav_steps(
  SIZE IN rImageSize
);

SIZE wci_wav_get_image_size(
  SIZE IN size
, int  IN wav_steps 
, int  IN version 
);

//------------------------------------------------------------------------------

int wci_wav_get_min_wav_steps(
 const WAV_CODEC_PARAM * IN prWavParam
);

//------------------------------------------------------------------------------

ERROR_CODE wci_wav_set_param(
  const IMAGE_INFO *      IN  prImageInfo       //! Parameters of transformed image
, int                     IN  iQuantizer        //! Sets the fixed value of the quantizer
, int                     IN  iWavSteps         //! Sets the number of steps for wavelet transform
, WAV_SPATIAL_SCALABILITY IN  rScalability
, int                     IN  iQuantMode        //! Mode of lossy compression
, int                     IN  iCompressMode     //! Mode of lossless compression
, const WAV_CODEC_PARAM * IN  prDefaultWavParam
, WAV_CODEC_PARAM *       OUT prWavParam
);

//------------------------------------------------------------------------------

int wci_wav_check_param(
  const WAV_CODEC_PARAM * IN prWavParam
);

//------------------------------------------------------------------------------

void wci_wav_set_layers_param(
  WAV_CODEC_PARAM *prWavParam
, WAV_SPATIAL_SCALABILITY rScalability
, SIZE            rImageSize );

//------------------------------------------------------------------------------

void wci_wav_put_header(
  const WAV_CODEC_PARAM * IN     prWavParam //! the structure of wavelet parameters
, BITSTREAM *             IN OUT prBS       //! output bitstream
);

//------------------------------------------------------------------------------

int wci_wav_get_header(
  BITSTREAM *       IN OUT prBS
, WAV_CODEC_PARAM *    OUT prWavParam
);

//------------------------------------------------------------------------------

static void __inline
wci_down_block_size(
  enumWT_Mode eMode          //! [in]     Geometrical transformation mode
, SIZE *      prBlockSize    //! [in/out] Block size
)
{
  switch( eMode )
  {
  case ewtHV:
    prBlockSize->cx /= 2;
    prBlockSize->cy /= 2;
    break;
  case ewtH:
    prBlockSize->cx /= 2;
    break;
  case ewtV:
    prBlockSize->cy /= 2;
    break;
  }
}

//------------------------------------------------------------------------------

static void __inline
wci_up_block_size(
  enumWT_Mode eMode          //! [in]     Geometrical transformation mode
, SIZE *      prBlockSize    //! [in/out] Block size
)
{
  switch( eMode )
  {
  case ewtHV:
    prBlockSize->cx *= 2;
    prBlockSize->cy *= 2;
    break;
  case ewtH:
    prBlockSize->cx *= 2;
    break;
  case ewtV:
    prBlockSize->cy *= 2;
    break;
  }
}

#endif // WAV_CODEC_H

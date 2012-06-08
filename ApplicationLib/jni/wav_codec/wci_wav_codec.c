/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: WavCodec.c

ABSTRACT:    This file contains procedures for initialization and setting of parameters of
             block-oriented wavelet codec

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00
HISTORY:     2004.08.27   v1.01 Quantization index and scalefactor were added

*******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "../wci_portab.h"
#include "../compress/wci_qsp_compress.h"
#include "../utils/wci_mem_align.h"
#include "wci_wav_quant.h"
#include "wci_wav_codec.h"

//------------------------------------------------------------------------------

typedef struct 
{
  SIZE        rImageSize;
  int         iWavSteps;

} WAV_STEPS;

typedef struct 
{
  SIZE        rImageSize;
  
  WAV_SPATIAL_SCALABILITY rScalability;

  int         iWavSteps;
  enumWT_Mode eMode;
  
} SCALABILTY_MODE;

//------------------------------------------------------------------------------

static const WAV_STEPS g_rWavSteps[] =
{
  // all values of width must be multiple 16
  { {  72,  96 }, 0 },
  { { 128, 128 }, 1 },
  { { 144, 108 }, 1 },
  { { 180, 120 }, 2 },
  { { 180, 144 }, 2 },
  { { 180, 224 }, 2 },
  { { 352, 120 }, 2 },
  { { 352, 240 }, 3 },
  { { 352, 288 }, 3 },
  { { 352, 448 }, 3 },
  { { 368, 120 }, 2 },
  { { 368, 144 }, 3 },
  { { 368, 240 }, 4 },
  { { 368, 288 }, 4 },
  { { 704, 240 }, 4 },
  { { 704, 288 }, 4 },
  { { 704, 480 }, 4 },
  { { 704, 576 }, 4 },
  { { 720, 240 }, 4 },
  { { 720, 288 }, 4 },
  { { 720, 480 }, 4 },
  { { 720, 576 }, 4 },
  { { MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT }, MAX_WAV_STEPS }
};

static const SCALABILTY_MODE g_rScalabiltyMode[] =
{
  { { 352, 240 }, { 1, 2, 2 }, 1, ewtHV },
  { { 352, 288 }, { 1, 2, 2 }, 1, ewtHV },

  { { 360, 240 }, { 1, 2, 2 }, 1, ewtHV },
  { { 360, 288 }, { 1, 2, 2 }, 1, ewtHV },

  { { 704, 240 }, { 1, 2, 2 }, 1, ewtHV },
  { { 704, 288 }, { 1, 2, 2 }, 1, ewtHV },
  { { 704, 480 }, { 1, 2, 2 }, 1, ewtHV },
  { { 704, 576 }, { 1, 2, 2 }, 1, ewtHV },

  { { 720, 240 }, { 1, 2, 2 }, 1, ewtHV },
  { { 720, 288 }, { 1, 2, 2 }, 1, ewtHV },
  { { 720, 480 }, { 1, 4, 4 }, 1, ewtHV },
  { { 720, 576 }, { 1, 4, 4 }, 1, ewtHV }
};

//------------------------------------------------------------------------------

#ifdef ENCODER_SUPPORT

ERROR_CODE wci_wav_init_data(
  SIZE                 IN     rImageSize     //! Image properies
, COLOR_SPACE          IN     eColorSpace
, PIXEL_SAMPLING       IN     ePixelSampling
, WAV_CODEC_DATA *        OUT prWavData      //! Wavelet codec buffer structure
, ALIGNED_MEMMANAGER * IN OUT prMemHeap      //! Embedded Mem Manager or NULL if system mem manager is used
)
{
// This function performs initialization of wavelet codec. Return value is error code.
  SIZE rSize;
  int  iPels = wci_image_get_size( rImageSize, eColorSpace );
  int  iTotalPels = iPels + iPels/3;

  rSize.cx = rImageSize.cx * 2;//WP_MEM_RESERVE ;
  rSize.cy = rImageSize.cy;

  if( wci_diffimage_create( rSize, eColorSpace, ePixelSampling, &prWavData->rImageEx, prMemHeap ) != ERR_NONE )
  {
    return ERR_MEMORY;
  }

  prWavData->pbtPartition = (BYTE*)wci_aligned_malloc( iTotalPels, CACHE_LINE, prMemHeap);
  if ( prWavData->pbtPartition == NULL )
  {
    return ERR_MEMORY;
  }
  memset ( prWavData->pbtPartition, 0, iTotalPels );

  if( wci_qsp_init_buffer( MAX_IMAGE_COMPONENTS*iTotalPels, prMemHeap, &prWavData->rQSP ) != 0 )
  {
    return ERR_MEMORY;
  }

  if( wci_qsp_init_compressor( &prWavData->rQSP_Compressor, 0xFFFF, NULL, 0, prMemHeap) != 0 )
  {
    return ERR_MEMORY;
  }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

void wci_wav_done_data(
  WAV_CODEC_DATA *     IN OUT prWavData      //! Wavelet codec buffer structure
, ALIGNED_MEMMANAGER * IN OUT prMemHeap      //! Embedded Mem Manager or NULL if system mem manager is used
)
{
// This function performs destroying of wavelet codec. Return value is error code.

  wci_diffimage_destroy( &prWavData->rImageEx, prMemHeap );

  wci_aligned_free( prWavData->pbtPartition, prMemHeap );
  prWavData->pbtPartition = NULL;

  wci_qsp_done_buffer( &prWavData->rQSP, prMemHeap );

  wci_qsp_done_compressor( &prWavData->rQSP_Compressor, prMemHeap );
}
#endif

//------------------------------------------------------------------------------

int wci_wav_check_param(
  const WAV_CODEC_PARAM * IN prWavParam
) 
{
// This function checks parameters of wavelet codec. Return value is error code.

  if ( !CHECK_BOUND(prWavParam->iMaxWavSteps, MIN_WAV_STEPS, MAX_WAV_STEPS) )
  {
    return ERR_FORMAT;
  }
  
  if ( !CHECK_BOUND(prWavParam->iQuantizer/QUANTIZER_SCALE_FACTOR, WAV_MIN_QUANTIZER, WAV_MAX_QUANTIZER) )
  {
    return ERR_FORMAT;
  }

  if ( prWavParam->rScalability.iType & WAV_SPATIAL_LAYERS )
  {
    if ( !CHECK_BOUND(prWavParam->iMaxWavSteps, 1, MAX_WAV_STEPS) )
    {
      return ERR_FORMAT;
    }
  }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

static void wci_wl_set_image_size_and_quantizer(
  WAV_CODEC_PARAM * IN OUT prWavParam           //! the structure of codec parameters
, SIZE              IN     rImageSize           //! the image size
, WAV_LAYER         IN OUT arWL[MAX_WAV_LAYERS]
)
{
  int        iLayer, iQuantizer, i;
  WAV_LAYER *prWL;
  SIZE       rBlockSize;

  rBlockSize = rImageSize;

  for ( iLayer=prWavParam->iLayers-1; iLayer>=0; iLayer-- )
  {
    prWL = &arWL[iLayer];

    prWL->rBlockSize = rBlockSize;

    if ( prWL->iWavSteps == 0 )
      prWL->rInvBlockSize = rBlockSize; // Encoding by quant
    else
    {
      for ( i=0; i<prWL->iWavSteps; i++ )
      {
        prWL->rInvBlockSize = rBlockSize;
        wci_down_block_size( prWL->eMode, &rBlockSize );
      }
    }
  }

  if (prWavParam->iMaxWavSteps == 1)
  {
    iQuantizer = prWavParam->iQuantizer;
  }
  else
  {
    iQuantizer = prWavParam->iQuantizer * (1 << (prWavParam->iMaxWavSteps-1));
  }

  for ( iLayer=prWavParam->iLayers-1; iLayer>=0; iLayer-- )
  {
    int  i;

    prWL = &arWL[0];
    
    prWL->iQuantizer = iQuantizer;

    if ( (prWL->iWavSteps != 0) && (prWavParam->iMaxWavSteps > 1) )
    {
      for ( i=0; i<prWL->iWavSteps; i++ )
        iQuantizer /= 2;
    }
  }
}

//------------------------------------------------------------------------------

static void wci_wl_set_quant_scale(
  WAV_CODEC_PARAM * IN OUT prWavParam
, WAV_LAYER         IN OUT arWL[MAX_WAV_LAYERS]
)
{
// This function performs initialization of quantizer for wavelet codec

  int        iLayer;
  WAV_LAYER *prWL;

  if ( prWavParam->iLayers > 1 )
  {
    WAV_QUANT_SCALE rQS[MAX_WAV_STEPS];
    int             nWavSteps, iWavStep;

    for ( nWavSteps=0, iLayer=prWavParam->iLayers-1; iLayer>=0; iLayer-- )
      nWavSteps += arWL[iLayer].iWavSteps;

    wci_set_quant_scale( prWavParam->iQuantizer, nWavSteps, rQS );

    for ( iWavStep=0, iLayer=prWavParam->iLayers-1; iLayer>=0; iLayer-- )
    {
      int i;
      prWL = &prWavParam->arWL[0][iLayer];

      if ( prWL->iWavSteps > 0 )
      {
        for ( i=0; i<prWL->iWavSteps; i++ )
        {
          prWL->rQS[i] = rQS[iWavStep++];
        }
      }
      else
      {
        prWL->rQS[0] = rQS[0];
      }
    }
  }
  else
  {
    wci_set_quant_scale( prWavParam->iQuantizer, arWL[0].iWavSteps, arWL[0].rQS );
  }
}

//------------------------------------------------------------------------------

void wci_wl_set_layer_steps(
  WAV_CODEC_PARAM *       IN OUT prWavParam   //! the structure of codec parameters
, WAV_SPATIAL_SCALABILITY IN     rScalability //! the scalability structure
, SIZE                    IN     rImageSize   //! the image size
, int                     IN     iWavSteps
, WAV_LAYER               IN OUT arWL[MAX_WAV_LAYERS]
)
{
// Setting of layers, wavelet steps and geometrical mode

  BOOL bPresettedMode = FALSE;
  int  iLayer;

  if ( prWavParam->iLayers > 1 )
  {
    int i, iCompare;
    
    for ( i=0; i<sizeof(g_rScalabiltyMode)/sizeof(SCALABILTY_MODE); i++ )
    {
      const SCALABILTY_MODE *prSM = &g_rScalabiltyMode[i];
      
      iCompare = memcmp( &rImageSize, &prSM->rImageSize, sizeof(SIZE) );
      if ( iCompare == 0 )
      {
        iCompare = memcmp( &rScalability, &prSM->rScalability, sizeof(WAV_SPATIAL_SCALABILITY) );
        if ( iCompare == 0 )
        {
          arWL[0].iLayer = 0;
          arWL[0].iWavSteps = iWavSteps - prSM->iWavSteps;
          arWL[0].eMode = ewtHV;

          arWL[1].iLayer = 0;
          arWL[1].iWavSteps = prSM->iWavSteps;
          arWL[1].eMode = ewtHV;

          bPresettedMode = TRUE;
          break;
        }
      }
    }
  }

  if ( !bPresettedMode )
  {
    if ( prWavParam->iLayers > 1 )
    {
      for ( iLayer=prWavParam->iLayers-1; iLayer>=0; iLayer-- )
      {
        int iLayerWavSteps;

        iLayerWavSteps = (iLayer > 0) ? 1 : iWavSteps - prWavParam->iLayers + 1;

        arWL[iLayer].iLayer = iLayer;
        arWL[iLayer].iWavSteps = iLayerWavSteps;
        arWL[iLayer].eMode = ewtHV;
      }
    }
    else
    {
      arWL[0].iLayer = 0;
      arWL[0].iWavSteps = iWavSteps;
      arWL[0].eMode = ewtHV;
    }
  }
}

//------------------------------------------------------------------------------

int wci_wav_get_wav_steps(
  SIZE IN rImageSize //! the transformed image size 
)
{
// This function calculates the number of wavelet transform step.
// Return value is the number of step. Uses global variable: array g_rWavSteps[]

  int iWavSteps;
  int i=0;

  do
  {
    const WAV_STEPS *prWS = &g_rWavSteps[ i++ ];
    
    iWavSteps = prWS->iWavSteps;

    if ( (prWS->rImageSize.cx >= rImageSize.cx)
         && (prWS->rImageSize.cy >= rImageSize.cy) )
    {
      break;
    }
  }
  while ( i<sizeof(g_rWavSteps)/sizeof(WAV_STEPS) );
  
  return iWavSteps;
}

//------------------------------------------------------------------------------

void wci_wav_set_layers_param(
  WAV_CODEC_PARAM *       IN OUT prWavParam   //! the structure of codec parameters
, WAV_SPATIAL_SCALABILITY IN     rScalability //! the scalability structure
, SIZE                    IN     rImageSize   //! the image size
)
{
// This function performs initialization of wavelet codec parameters for
//   scalability mode

  int  iComponent;
  SIZE arSize[MAX_IMAGE_COMPONENTS];
  int  aiWavSteps[MAX_IMAGE_COMPONENTS];

  if ( rScalability.iType & WAV_SPATIAL_LAYERS )
    prWavParam->iLayers = 2;
  else
    prWavParam->iLayers = 1;

  // Setting of quantizer and block size

  wci_image_set_component_size( rImageSize, prWavParam->rImageInfo.eColorSpace, arSize );

  for ( iComponent=0; iComponent<prWavParam->rImageInfo.iComponents; ++iComponent )
    aiWavSteps[iComponent] = prWavParam->iMaxWavSteps;

  if ( prWavParam->rImageInfo.ePixelSampling == PS_420 )
    for ( iComponent=1; iComponent<prWavParam->rImageInfo.iComponents; ++iComponent )
      aiWavSteps[iComponent] = aiWavSteps[iComponent] > 0 ? max( 1, aiWavSteps[iComponent]-1 ) : 0; // 1 - ???

  for ( iComponent=0; iComponent<prWavParam->rImageInfo.iComponents; ++iComponent )
  {
    WAV_LAYER *prWL = prWavParam->arWL[iComponent];

    wci_wl_set_layer_steps( prWavParam, rScalability, arSize[iComponent], aiWavSteps[iComponent], prWL );
    wci_wl_set_image_size_and_quantizer( prWavParam, arSize[iComponent], prWL );
    wci_wl_set_quant_scale( prWavParam, prWL );
  }
}

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
 )
{
// This function performs initialization of wavelet codec.

  if (iQuantizer != 0)
  {
    prWavParam->iQuantizer = iQuantizer;
  }
  else
  {
    prWavParam->iQuantizer = prDefaultWavParam->iQuantizer;
  }

  if (iWavSteps >= 0)
  {
    prWavParam->iMaxWavSteps = iWavSteps;
  }
  else
  {
    prWavParam->iMaxWavSteps = prDefaultWavParam->iMaxWavSteps;
  }
    
  prWavParam->rScalability = rScalability;
  
  prWavParam->rImageInfo = *prImageInfo;

  if ( wci_wav_check_param( prWavParam ) != 0 )
  {
    return ERR_FORMAT;
  }
  
  wci_wav_set_layers_param( prWavParam, rScalability, prImageInfo->rSize ) ;

  {
    int iComponent;
    for(iComponent = 0; iComponent < prWavParam->rImageInfo.iComponents; iComponent++)
    {
      int iSize = prWavParam->arWL[iComponent][0].rInvBlockSize.cx;
      if ( iSize & 0x1  && iWavSteps != 0)
      {
        return ERR_FORMAT;
      }
    }
  }
  
  prWavParam->iQuantMode = iQuantMode;
  if ( prWavParam->iQuantMode == 0 )
  {
    prWavParam->iQuantMode = prDefaultWavParam->iQuantMode;
  }
  
  prWavParam->iCompressMode = iCompressMode;
  if ( prWavParam->iCompressMode == 0 )
  {
    prWavParam->iCompressMode = prWavParam->iCompressMode;
  }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

void wci_wav_put_header(
  const WAV_CODEC_PARAM * IN     prWavParam //! the structure of wavelet parameters
, BITSTREAM *             IN OUT prBS       //! output bitstream
)
{
// This function puts header of frame for wavelet codec

  wci_bitstream_put_bits(prBS, prWavParam->iLayers, 2);

  wci_bitstream_put_bits(prBS, prWavParam->iMaxWavSteps, 3);

  wci_bitstream_put_bits(prBS, prWavParam->iQuantizerIndex, 5);

  wci_bitstream_put_bits(prBS, prWavParam->iQuantMode, 4);

  wci_bitstream_put_bits(prBS, prWavParam->rScalability.iType      , 4);
  wci_bitstream_put_bits(prBS, prWavParam->rScalability.iHorzFactor, 3);
  wci_bitstream_put_bits(prBS, prWavParam->rScalability.iVertFactor, 3);

  wci_bitstream_put_bits(prBS, prWavParam->rImageInfo.rSize.cx, 12);
  wci_bitstream_put_bits(prBS, prWavParam->rImageInfo.rSize.cy, 12);
  {
    int iBit = prWavParam->rImageInfo.eColorSpace & ~(CSP_NULL | CSP_VFLIP);
    int nCode = 0;

    assert( iBit != 0 );

    while ( (iBit & (1 << nCode)) == 0 )
    {
      nCode++;
      if ( nCode == 31 )
        break;
    }
    wci_bitstream_put_bits( prBS, nCode, 5 );
  }
  wci_bitstream_put_bits( prBS, prWavParam->rImageInfo.ePixelSampling, 3 );

  wci_bitstream_pad(prBS);
}

//------------------------------------------------------------------------------

int wci_wav_get_header(
  BITSTREAM *       IN OUT prBS       //! the structure of codec parameters
, WAV_CODEC_PARAM *    OUT prWavParam //! the input bitstream
)
{
// This function gets header of frame for wavelet codec from bitstream

  memset( prWavParam, 0, sizeof(WAV_CODEC_PARAM));

  prWavParam->iLayers = wci_bitstream_get_bits(prBS, 2);

  prWavParam->iMaxWavSteps = wci_bitstream_get_bits(prBS, 3);

  prWavParam->iQuantizerIndex = wci_bitstream_get_bits(prBS, 5);
  prWavParam->iQuantizer= wci_wav_get_quantizer_by_index( prWavParam->iMaxWavSteps, prWavParam->iQuantizerIndex );

  prWavParam->iQuantMode = wci_bitstream_get_bits(prBS, 4);

  prWavParam->rScalability.iType = wci_bitstream_get_bits(prBS, 4);
  prWavParam->rScalability.iHorzFactor = wci_bitstream_get_bits(prBS, 3);
  prWavParam->rScalability.iVertFactor = wci_bitstream_get_bits(prBS, 3);

  prWavParam->rImageInfo.rSize.cx = wci_bitstream_get_bits(prBS, 12);
  prWavParam->rImageInfo.rSize.cy = wci_bitstream_get_bits(prBS, 12);
  prWavParam->rImageInfo.eColorSpace    = 1 << wci_bitstream_get_bits( prBS, 5 );
  prWavParam->rImageInfo.ePixelSampling = wci_bitstream_get_bits( prBS, 3 );

  prWavParam->rImageInfo.iComponents =  wci_get_component_number(prWavParam->rImageInfo.eColorSpace);

  wci_bitstream_byte_align(prBS);

  if ( wci_wav_check_param( prWavParam ) != 0 )
  {
    return ERR_FORMAT;
  }

  wci_wav_set_layers_param( prWavParam, prWavParam->rScalability, prWavParam->rImageInfo.rSize ) ;

  return ERR_NONE;
}

//------------------------------------------------------------------------------

int wci_wav_get_min_wav_steps(
 const WAV_CODEC_PARAM * IN prWavParam
)
{
  int iComponent, iMinWavSteps = MAX_WAV_STEPS;

  for ( iComponent=0; iComponent<prWavParam->rImageInfo.iComponents; ++iComponent )
    iMinWavSteps = min( iMinWavSteps, prWavParam->arWL[iComponent][0].iWavSteps );

  return iMinWavSteps;
}

//------------------------------------------------------------------------------

SIZE wci_wav_get_image_size(
  SIZE IN size
, int  IN wav_steps 
, int  IN version 
)
{
  SIZE even_size;

  assert(wav_steps >= 0 && wav_steps <= MAX_WAV_STEPS);

  switch (version)
  {
  case 1:
    {
      const int aiPaddingTable[1+MAX_WAV_STEPS] = {1, 4, 8, 8, 16};

      int padding_x = (1 << wav_steps) - 1;
      int padding_y = aiPaddingTable[wav_steps] - 1;

      even_size.cx = max( (size.cx + padding_x) & ~padding_x, MB_SIZE );
      even_size.cy = max( (size.cy + padding_y) & ~padding_y, MB_SIZE );
    }
    break;
  default:
    assert(version != STREAM_FORMAT_VERSION);
  case STREAM_FORMAT_VERSION:
    {
      if (wav_steps > 0)
      {
        const int aiPaddingTable[1+MAX_WAV_STEPS] = {1, 4,  8, 16, 32};

        int padding_x = aiPaddingTable[wav_steps] - 1;
        int padding_y = aiPaddingTable[wav_steps] - 1;

        even_size.cx = max( (size.cx + padding_x) & ~padding_x, MB_SIZE );
        even_size.cy = max( (size.cy + padding_y) & ~padding_y, MB_SIZE );
      }
      else
      {
        even_size = size;
      }
    }
    break;
  }
  
  return even_size;
}


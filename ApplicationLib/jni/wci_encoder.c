/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: encoder.c

ABSTRACT:    This file contains procedures for image encoding

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00
             2004.08.27   v1.01 Setting quality scale changed 

*******************************************************************************/

#ifdef ENCODER_SUPPORT

#include <stdlib.h>

#include "wci_portab.h"
#include "wci_codec.h"
#include "wci_frame.h"
#include "bitstream/wci_bitstream.h"
#include "utils/wci_emms.h"
#include "image/wci_image.h"
#include "utils/wci_mem_align.h"
#include "wav_codec/wci_wav_layer.h"
#include "wav_codec/wci_wav_quant.h"
#include "image/wci_image_convert.h"
#include "wci_encoder.h"

//------------------------------------------------------------------------------

//#define OUT_IMAGE // for debug only

#define ENC_CHECK(X)                 if(!(X)) return ERR_FORMAT
#define ENC_CHECK_BOUND(x, min, max) if(((x) < min) || ((x) > max)) return ERR_FORMAT

#define IMAGE_SIZE_THRESH            128

#define DPCM_ENCODING_MODE           eemDPCM // eemLPC_DPCM

#define DEFAULT_FRAME_DURATION       33

//------------------------------------------------------------------------------

typedef struct
{
  int   iGlobalFlags;

  I_FRAME_ENC_PARAM rI;

} SPEED_QUALITY_PARAM;

typedef struct
{
  int iQuality;
  int iI_Quantizer;

} QUALITY_PARAM;

//------------------------------------------------------------------------------

const WAV_CODEC_PARAM rIntraWavParam =
{
  { 352, 288 }, PS_420, 0, 5, eemDPCM, 0x416
};

//------------------------------------------------------------------------------

static void wci_enc_update_stat(
  const ENCODER *   IN     prEnc
, const BITSTREAM * IN     prBS
, QSP_SIZE *        IN     prQSP_Size
, BOOL              IN     bReconstruct
, ENC_STATS *       IN OUT prStat
);

static void wci_set_speed_quality_param( ENC_PARAM * pParam );

static void wci_set_quality_param( int iQuality, int iWavSteps, ENCODER *prEnc );

ERROR_CODE wci_put_decoder_cache(
  const STREAM *       IN     frame_stream
, STREAM *             IN OUT cache_stream
);

//------------------------------------------------------------------------------

//static void wci_set_image_size( ENCODER *prEnc, int iWidth, int iHeight, int iWavSteps )
//{
//  assert(iWavSteps >= 0 && iWavSteps <= MAX_WAV_STEPS);
//  
//  if (iWavSteps > 0)
//  {
//    SIZE      rEvenSize;
//    uint      nPaddingX, nPaddingY;
//    const int aiPaddingTable[MAX_WAV_STEPS+1]= {1, 8, 16, 16, 32};
//
//    nPaddingX = (1<<iWavSteps) - 1;
//    nPaddingY = aiPaddingTable[iWavSteps] - 1;
//
//    prEnc->rSize.cx = iWidth ;
//    prEnc->rSize.cy = iHeight;
//
//    rEvenSize.cx = max( (prEnc->rSize.cx + nPaddingX) & ~nPaddingX, MB_SIZE );
//    rEvenSize.cy = max( (prEnc->rSize.cy + nPaddingY) & ~nPaddingY, MB_SIZE );
//
//    prEnc->rMaxEvenSize.cx = max( rEvenSize.cx, prEnc->rMaxEvenSize.cx );
//    prEnc->rMaxEvenSize.cy = max( rEvenSize.cy, prEnc->rMaxEvenSize.cy );
//  }
//  else
//  {
//    prEnc->rSize.cx = prEnc->rMaxEvenSize.cx = iWidth ;
//    prEnc->rSize.cy = prEnc->rMaxEvenSize.cy = iHeight;
//  }
//}

//------------------------------------------------------------------------------

int wci_encoder_create(ENC_PARAM * pParam)
{
  ENCODER *               prEnc;
  WAV_SPATIAL_SCALABILITY rScalability;

  pParam->pEnc = NULL;

  ENC_CHECK(pParam);
  
  ENC_CHECK(pParam->iWidth > 0 && pParam->iWidth <= MAX_IMAGE_WIDTH);
  ENC_CHECK(pParam->iHeight > 0 && pParam->iHeight <= MAX_IMAGE_HEIGHT);
  
  prEnc = (ENCODER *) wci_aligned_malloc(sizeof(ENCODER), CACHE_LINE, NULL);
  if (prEnc == NULL)
  {
    return ERR_MEMORY;
  }

	// Zero the Encoder Structure 
	memset(prEnc, 0, sizeof(ENCODER));

	// Fill members of Encoder structure 
  prEnc->iGlobalFlags = pParam->iGlobalFlags;
  prEnc->iGlobalFlags |= (STREAM_FORMAT_VERSION & 0xF) << (4*SF_VERSION_DECADE);

  rScalability.iType = pParam->rScalability.rSpatial.eMode;
  rScalability.iHorzFactor = pParam->rScalability.rSpatial.iZoomOut;
  rScalability.iVertFactor = pParam->rScalability.rSpatial.iZoomOut;

  prEnc->eColorSpace    = pParam->eColorSpace   ;
  prEnc->ePixelSampling = pParam->ePixelSampling;

  if ((prEnc->iGlobalFlags & ADVANCED_CONTROL_PARAM) == 0)
  {
    SIZE rSize;

    wci_set_speed_quality_param( pParam );

    rSize.cx  = pParam->iWidth ;
    rSize.cy  = pParam->iHeight;
    pParam->rI.iWavSteps = wci_wav_get_wav_steps( rSize );
    if( pParam->iMaxWavSteps >= 0 )
      pParam->rI.iWavSteps = min( pParam->iMaxWavSteps, pParam->rI.iWavSteps );

    prEnc->iSpeedOrQuality = pParam->iSpeedOrQuality;
    
    wci_set_quality_param( pParam->iQuality, pParam->rI.iWavSteps, prEnc );
    
    pParam->rI.iQuantizer = prEnc->rWavParam.iQuantizer;
  }

  prEnc->rSize.cx     = pParam->iWidth ;
  prEnc->rSize.cy     = pParam->iHeight;
  prEnc->rMaxEvenSize = wci_wav_get_image_size( prEnc->rSize, pParam->rI.iWavSteps, STREAM_FORMAT_VERSION );

//  wci_set_image_size( prEnc, pParam->iWidth, pParam->iHeight, pParam->rI.iWavSteps );

  if (wci_image_create(prEnc->rMaxEvenSize, prEnc->eColorSpace, prEnc->ePixelSampling, NULL, &prEnc->rImageEx, NULL) != ERR_NONE)
  {
    goto err_memory;
  }

#ifdef PSNR_STATISTICS
  if (wci_image_create(prEnc->rMaxEvenSize, prEnc->eColorSpace, prEnc->ePixelSampling, NULL, &prEnc->rOriginalImage, NULL) != ERR_NONE)
  {
    goto err_memory;
  }
#endif

  if( wci_wav_set_param( &prEnc->rImageEx.rInfo, pParam->rI.iQuantizer, pParam->rI.iWavSteps,
                    rScalability, DPCM_ENCODING_MODE, pParam->rI.iCompressMode,
                    &rIntraWavParam, &prEnc->rWavParam ) != 0 )
  {
    goto err_memory;
  }

  if( wci_wav_init_data(prEnc->rMaxEvenSize, prEnc->eColorSpace, prEnc->ePixelSampling, &prEnc->rWavData, NULL ) != 0 )
  {
    goto err_memory;
  }

  pParam->pEnc = (void *)prEnc;

  return ERR_NONE;

// We handle all ERR_MEMORY here, this makes the code lighter
err_memory:

  wci_encoder_destroy( prEnc );

	return ERR_MEMORY;
}

//------------------------------------------------------------------------------

int wci_encoder_destroy(ENCODER * prEnc)
{
  ENC_CHECK(prEnc);

  wci_wav_done_data( &prEnc->rWavData, NULL );

  wci_image_destroy(&prEnc->rImageEx, NULL);

#ifdef PSNR_STATISTICS
  wci_image_destroy(&prEnc->rOriginalImage, NULL);
#endif

  wci_aligned_free(prEnc, NULL);

  return ERR_NONE;
}

//------------------------------------------------------------------------------

static void wci_set_quality_param( int iQuality, int iWavSteps, ENCODER *prEnc )
{
  iQuality = min( max(iQuality, MIN_QUALITY), MAX_QUALITY);
  prEnc->iQuality = iQuality;

  prEnc->rWavParam.iQuantizerIndex =  (MAX_QUALITY - iQuality) / QUALITY_STEP;
  prEnc->rWavParam.iQuantizer = wci_wav_get_quantizer_by_index( iWavSteps, prEnc->rWavParam.iQuantizerIndex );
}

//------------------------------------------------------------------------------

static void wci_set_speed_quality_param(
  ENC_PARAM * pParam )
{
  #define SPEED_RANGE   (MAX_SPEED - LOW_SPEED + 1)
  
  const SPEED_QUALITY_PARAM arrSQP[SPEED_RANGE] =
  {
    { 0x00060000, { 0, 0, 0x416 | 0x5000 } }, // LOW_SPEED
    { 0x00060000, { 0, 0, 0x416 | 0x5000 } }, // 0, NORMAL_SPEED
    { 0x00060000, { 0, 0, 0x416 | 0x5000 } }, // 1, FAST_SPEED
    { 0x00060000, { 0, 0, 0x411 | 0x5000 } }, // 2, 
    { 0x00060000, { 0, 0, 0x311 | 0x5000 } }, // 3, 
    { 0x00060000, { 0, 0, 0x311 | 0x5000 } }, // 4, 
    { 0x00060000, { 0, 0, 0x211 | 0x5000 } }  // 5, MAX_SPEED
  };

  const SPEED_QUALITY_PARAM *prSQP;
  int iIndex;

  pParam->iSpeedOrQuality = max(pParam->iSpeedOrQuality, LOW_SPEED);
  pParam->iSpeedOrQuality = min(pParam->iSpeedOrQuality, MAX_SPEED);
  iIndex = pParam->iSpeedOrQuality - LOW_SPEED;

  prSQP = &arrSQP[iIndex];
  pParam->iGlobalFlags = prSQP->iGlobalFlags;
  
  pParam->rI = prSQP->rI;
}

//------------------------------------------------------------------------------

int wci_enc_change_quality(
  int iQuality
, ENCODER * prEnc
)
{
  WAV_CODEC_PARAM *       prWavParam = &prEnc->rWavParam;
  WAV_SPATIAL_SCALABILITY rSS;

  rSS.iType = prEnc->rScalability.rSpatial.eMode;
  rSS.iHorzFactor = prEnc->rScalability.rSpatial.iZoomOut;
  rSS.iVertFactor = prEnc->rScalability.rSpatial.iZoomOut;

  wci_set_quality_param( iQuality, prWavParam->iMaxWavSteps, prEnc );

  if( wci_wav_set_param( &prWavParam->rImageInfo, prWavParam->iQuantizer, prWavParam->iMaxWavSteps,
    rSS, prWavParam->iQuantMode, prWavParam->iCompressMode, &rIntraWavParam, prWavParam ) != 0 )
  {
    return ERR_FORMAT;
  }
  
  return ERR_NONE;
}

//------------------------------------------------------------------------------

static void wci_update_frame_stat(
  const ENCODER *   IN OUT prEnc
, const BITSTREAM * IN OUT prBS
, const QSP_SIZE *  IN     prQSP_Size
, BOOL              IN     bReconstruct
, FRAME_STATS *     IN OUT prFS
)
{
  prFS->iFrames++;

  prFS->iFrameSize = wci_bitstream_pos(prBS);
  prFS->fMeanFrameSize = ((prFS->iFrames-1)*prFS->fMeanFrameSize + (float)prFS->iFrameSize)/prFS->iFrames;

  DPRINTF( DPRINTF_DEBUG, "FrameSize= %d\n", prFS->iFrameSize );

  prFS->rQSP_Size = *prQSP_Size;
  prFS->rMeanQSP_Size.fQ = ((prFS->iFrames-1)*prFS->rMeanQSP_Size.fQ + (float)prFS->rQSP_Size.iQ)/prFS->iFrames;
  prFS->rMeanQSP_Size.fS = ((prFS->iFrames-1)*prFS->rMeanQSP_Size.fS + (float)prFS->rQSP_Size.iS)/prFS->iFrames;
  prFS->rMeanQSP_Size.fP = ((prFS->iFrames-1)*prFS->rMeanQSP_Size.fP + (float)prFS->rQSP_Size.aiP[0])/prFS->iFrames;

  DPRINTF( DPRINTF_DEBUG, "QSP Size (Q,S,P[3])= %d %d %d %d %d\n"
          , prFS->rQSP_Size.iQ, prFS->rQSP_Size.iS
          , prFS->rQSP_Size.aiP[0], prFS->rQSP_Size.aiP[1], prFS->rQSP_Size.aiP[2] );

  if ( bReconstruct )
  {
  #ifdef PSNR_STATISTICS
    wci_image_get_psnr( &prEnc->rOriginalImage, &prEnc->rImageEx, prFS->rPSNR.afPSNR );
    DPRINTF( DPRINTF_DEBUG, "PSNR (Y,U,V) = %7.3f %7.3f %7.3f\n"
           , prFS->rPSNR.afPSNR[0], prFS->rPSNR.afPSNR[1], prFS->rPSNR.afPSNR[2] );
  #endif  
  }
}

//------------------------------------------------------------------------------

static void wci_enc_update_stat(
  const ENCODER *   IN     prEnc
, const BITSTREAM * IN     prBS
, QSP_SIZE *        IN     prQSP_Size
, BOOL              IN     bReconstruct
, ENC_STATS *       IN OUT prStat)
{
  prStat->iFrameType = I_VOP;

  wci_update_frame_stat(prEnc, prBS, prQSP_Size, bReconstruct, &prStat->rI);
}

//------------------------------------------------------------------------------

static __inline void wci_transform_alpha_component_to_qsp
(
  const int16_t *       piData
, QSP_BUFFER*           prQSP
, int32_t *             piNumberOfBlocks
, QSP_SIZE *            prBlockSizeQSP
, SIZE                  rSize
, WCI_ALPHA_CHANNEL *   prAlphaChannel
)
{
  BYTE* pbtDataQ = (BYTE* )&prQSP->pbtQ[prQSP->iQ_Pos];
  int i, iBlocks = *piNumberOfBlocks;
  int iLength = wci_get_square(rSize);

  for(i = 0; i < iLength; i++)
  {
    *pbtDataQ++ = (BYTE)*piData++;
  }
  prBlockSizeQSP[iBlocks].aiP[0] = prBlockSizeQSP[iBlocks].aiP[1] = prBlockSizeQSP[iBlocks].aiP[2] =  0;
  prBlockSizeQSP[iBlocks].iQ = prBlockSizeQSP[iBlocks-1].iQ + iLength;
  prBlockSizeQSP[iBlocks].iS = prBlockSizeQSP[iBlocks-1].iS + 0;
  prAlphaChannel->iQOffset = prQSP->iQ_Pos;
  prAlphaChannel->iQBlockSize = iLength;
  prQSP->iQ_Pos += iLength; 
  iBlocks += 1;
  prAlphaChannel->iAlphaBlockNumber =  iBlocks;

  *piNumberOfBlocks = iBlocks;
}

//------------------------------------------------------------------------------

void wci_encode_image
(
  ENCODER *    IN OUT prEnc
, BITSTREAM *  IN OUT prBS
, QSP_SIZE *      OUT prQSP_Size
)
{ 
  WAV_CODEC_PARAM * prWavParam  = &prEnc->rWavParam;  
  WAV_CODEC_DATA  * prWavData = &prEnc->rWavData;
  BOOL              bReconstruct = TRUE;
  int32_t           iBlocks = 0;
  IMAGE_EX *        prImage = &prEnc->rImageEx;
  QSP_BUFFER        rQSP;
  QSP_SIZE          rBlockSizeQSP[MAX_IMAGE_COMPONENTS*(MAX_WAV_STEPS+1)];
  WCI_ALPHA_CHANNEL rAlphaChannel;


  wci_image_convert_to_diffimage( &prEnc->rImageEx, &prWavData->rImageEx );

  wci_wav_encode_diffimage( prWavParam, prWavData,
    &prWavData->rImageEx, prBS, bReconstruct, prQSP_Size , &rQSP, rBlockSizeQSP, &iBlocks);

  rAlphaChannel.isAlphaCannel = CSP_IS_THERE_ALPHA(prImage->rInfo.eColorSpace);

  if(rAlphaChannel.isAlphaCannel)
  {
    const int16_t* piData = prWavData->rImageEx.arComponent[ALPHA_COMPONENT].piData;
    wci_transform_alpha_component_to_qsp( piData, &rQSP , &iBlocks, rBlockSizeQSP, prEnc->rMaxEvenSize, &rAlphaChannel );  
  }

  // Compress total layer data and write in bitstream
  wci_qsp_copy_pointers( &prWavData->rQSP, &rQSP );
  {
    QSP_SIZE rSize;
    int      i;
    for ( i=iBlocks-1; i>0; --i )
    {
      rBlockSizeQSP[i].iQ -= rBlockSizeQSP[i-1].iQ;
      rBlockSizeQSP[i].iS -= rBlockSizeQSP[i-1].iS;
    }
    wci_qsp_compress_layer( &prWavData->rQSP_Compressor, &rQSP, prWavParam->iCompressMode,
      iBlocks, rBlockSizeQSP, prBS, &rSize, &rAlphaChannel );

    prQSP_Size->iQ += rSize.iQ;
    prQSP_Size->iS += rSize.iS;
    prQSP_Size->aiP[0] += rSize.aiP[0];
  }
  if ( bReconstruct )
  {
    wci_wav_inv_trans_image_ex( prWavParam, &prWavData->rImageEx );
    wci_diffimage_convert_to_image( &prWavData->rImageEx, &prEnc->rImageEx );
  }
}

//------------------------------------------------------------------------------

int wci_encoder_encode(
  ENCODER *   IN OUT prEnc
, ENC_FRAME * IN OUT prFrame
, ENC_STATS *    OUT prStat
)
{
	BITSTREAM    rBS, rStartBS;
  FRAME_HEADER rFrameHeader;
  QSP_SIZE     rQSP_Size;

  // Check frame parameters
	ENC_CHECK( prEnc  );
  ENC_CHECK( prFrame );
  ENC_CHECK( prFrame->pbBitstream );
  ENC_CHECK( prFrame->pbImage     );
  ENC_CHECK( prFrame->iImageSize > 0 );
  ENC_CHECK( prFrame->iWidth  <= prEnc->rMaxEvenSize.cx );
  ENC_CHECK( prFrame->iHeight <= prEnc->rMaxEvenSize.cy );

  // Load frame image
  if ( prFrame->iWidth == 0 )
    prFrame->iWidth = prEnc->rSize.cx;
  if ( prFrame->iHeight == 0 )
    prFrame->iHeight = prEnc->rSize.cy;

  prFrame->eFrameType = I_VOP;

  if ( (prFrame->iWidth != prEnc->rSize.cx)
       || (prFrame->iHeight != prEnc->rSize.cy) )
  {
    prEnc->rSize.cx     = prFrame->iWidth ;
    prEnc->rSize.cy     = prFrame->iHeight;
    prEnc->rMaxEvenSize = wci_wav_get_image_size( prEnc->rSize, prEnc->rWavParam.iMaxWavSteps, STREAM_FORMAT_VERSION );
  
//    wci_set_image_size( prEnc, prFrame->iWidth, prFrame->iHeight, prEnc->rWavParam.iMaxWavSteps );
  }

	if (wci_image_input( prFrame->pbImage, prEnc->rSize, prFrame->iColorSpace, 0, &prEnc->rImageEx ) != ERR_NONE)
  {
  	return ERR_FORMAT;
  }

  if (wci_image_expand_rect( prEnc->rSize, &prEnc->rImageEx ) != ERR_NONE)
  {
  	return ERR_FORMAT;
  }

#ifdef PSNR_STATISTICS
  wci_image_copy( &prEnc->rImageEx, &prEnc->rOriginalImage );
#endif

#ifdef OUT_IMAGE
  {
    static int i = 0;
    char sz[80];
    sprintf( sz, "o%03d.pgm", i++ );
    wci_image_dump_to_pgm( &prEnc->rImageEx, sz );
  }
#endif
  
  // Form frame header
	wci_bitstream_init(&rBS, prFrame->pbBitstream, prFrame->iBitstreamLength);
  rStartBS = rBS;

  rFrameHeader.iLength = 0;

  rFrameHeader.eFrameType = prFrame->eFrameType;

  rFrameHeader.iRefLayer = 0;

  rFrameHeader.iGlobalFlags = prEnc->iGlobalFlags;

  rFrameHeader.rImageSize = prEnc->rSize;
  rFrameHeader.eColorSpace    = prEnc->rImageEx.rInfo.eColorSpace;
  rFrameHeader.ePixelSampling = prEnc->rImageEx.rInfo.ePixelSampling;

  rFrameHeader.iQuality = prEnc->iQuality;

  rFrameHeader.iDuration = prFrame->iDuration;

  rFrameHeader.rImageResolution.cx = prFrame->iResolutionHorz;
  rFrameHeader.rImageResolution.cy = prFrame->iResolutionVert;

  rFrameHeader.iMaxZoomOut = wci_wav_get_min_wav_steps( &prEnc->rWavParam );

	wci_put_frame_header(&rFrameHeader, &rBS);

  memset( &rQSP_Size, 0, sizeof(rQSP_Size) );
  
  // Encode frame image
  wci_bitstream_flush(&rBS);
  rFrameHeader.iLength = wci_bitstream_pos(&rBS);

  emms();

  wci_encode_image(prEnc, &rBS,  &rQSP_Size);

	emms();

  // Update globals flag
  rFrameHeader.iGlobalFlags = prEnc->iGlobalFlags;

  // Update frame header into bitstream
  wci_bitstream_pad(&rBS);
  wci_bitstream_flush(&rBS);
  // rFrameHeader.iLength = wci_bitstream_pos(&rBS) - rFrameHeader.iLength;
  rFrameHeader.iLength = wci_bitstream_pos(&rBS);
	wci_put_frame_header( &rFrameHeader, &rStartBS );

  // Fill frame parameters
	prFrame->iBitstreamLength = rFrameHeader.iLength;

  // Update frame statistics
  if (prStat != NULL)
  {
    wci_enc_update_stat(prEnc, &rBS, &rQSP_Size, TRUE, prStat);
  }

#ifdef OUT_IMAGE
  {
    static int i = 0;
    char sz[80];
    sprintf( sz, "r%03d.pgm", i++ );
    wci_image_dump_to_pgm( &prEnc->rImageEx, sz );
  }
#endif

  return ERR_NONE;
}

//------------------------------------------------------------------------------

static void wci_set_enc_param(
  const FRAME_IMAGE_INFO * IN  prInfo      //! Image information
, int                      IN  iQuality    //! Encoded image quality
, DECODING_SPEED           IN  eSpeed
, COLOR_SPACE              IN  eColorSpace
, ENC_PARAM *              OUT prParam
)
{
  memset( prParam, 0, sizeof(ENC_PARAM) );

  prParam->iWidth  = prInfo->iWidth ;
  prParam->iHeight = prInfo->iHeight;

  if ( iQuality >= LOSSLESS_QUALITY )
  {
    if(prInfo->eColorSpace == CSP_RGB565)
      prParam->eColorSpace = CSP_BGR;
    else
      prParam->eColorSpace = prInfo->eColorSpace;
    prParam->ePixelSampling = PS_444;
  }
  else
  {
    int  nMaxSize = max( prParam->iWidth, prParam->iHeight );
    BOOL alpha = CSP_IS_THERE_ALPHA(prInfo->eColorSpace);

    switch (eColorSpace)
    {
      case CSP_AUTO:
        if(prInfo->eColorSpace == CSP_GRAYSCALE)
          prParam->eColorSpace = CSP_GRAYSCALE;
        else
        {
          if (alpha)
            prParam->eColorSpace = (nMaxSize < IMAGE_SIZE_THRESH) ? CSP_YV24A : CSP_YV12A;
          else
            prParam->eColorSpace = (nMaxSize < IMAGE_SIZE_THRESH) ? CSP_YV24 : CSP_YV12;
        }
        break;
        
      case CSP_YV12:
        prParam->eColorSpace = alpha ? CSP_YV12A : CSP_YV12;
        break;
      case CSP_YV24:
        prParam->eColorSpace = alpha ? CSP_YV24A : CSP_YV24;
        break;
      case CSP_BGR:
        prParam->eColorSpace = alpha ? CSP_BGRA : CSP_BGR;
        break;

      case CSP_BGRA:
      case CSP_GRAYSCALE:
      case CSP_YV12A:
      case CSP_YV24A:
        prParam->eColorSpace = eColorSpace;
        break;
        
      default:
        break;
    }
    
    switch ( prParam->eColorSpace )
    {
      case CSP_YV12:
      case CSP_YV12A:
        prParam->ePixelSampling = PS_420;
        break;
      case CSP_GRAYSCALE:
      case CSP_YV24:
      case CSP_YV24A:
        prParam->ePixelSampling = PS_444;
        break;
      default:
        break;
    }
  }

  switch(eSpeed)
  {
  default:
  case DS_NORMAL:
    prParam->iSpeedOrQuality = NORMAL_SPEED;
    break;
  case DS_FAST:
    prParam->iSpeedOrQuality = HIGH_SPEED;
    break;
  case DS_MAX:
    prParam->iSpeedOrQuality = MAX_SPEED;
    break;
  }

  prParam->iQuality = iQuality;
  prParam->iMaxWavSteps = -1; //! Maximal wavelet step for encoding image
}

//------------------------------------------------------------------------------

static void wci_set_enc_frame(
  const unsigned char *    IN  pbtImage    //! Pointer to an encoded image
, const FRAME_IMAGE_INFO * IN  prInfo      //! Image information
, const STREAM *           IN  prStream
, DECODING_SPEED           IN  eSpeed
, ENC_FRAME *              OUT prFrame
)
{
  memset( prFrame, 0, sizeof(ENC_FRAME) );

  prFrame->pbImage = (BYTE *)pbtImage;

  switch (eSpeed)
  {
    case DS_FAST:
    case DS_MAX:
      prFrame->iGlobalFlags |= USE_FRAME_CACHE;
      break;
  }

  prFrame->iWidth  = prInfo->iWidth ;
  prFrame->iHeight = prInfo->iHeight;
  prFrame->iColorSpace = prInfo->eColorSpace;
  prFrame->iImageSize = prInfo->iImageSize;
  
  prFrame->iDuration = DEFAULT_FRAME_DURATION;

  prFrame->iResolutionHorz = prInfo->iResolutionHorz;
  prFrame->iResolutionVert = prInfo->iResolutionVert;

  prFrame->pbBitstream      = &prStream->pbtData[ prStream->iOffset ];
  prFrame->iBitstreamLength = prStream->iLength - prStream->iOffset;
}

//------------------------------------------------------------------------------

BOOL wci_validate_param(
  const ENCODE_PARAM *     IN     prParam
)
{
  BOOL valid = TRUE;

  valid = valid && (prParam->iQuality >= MIN_QUALITY) && (prParam->iQuality <= LOSSLESS_QUALITY);
  valid = valid && (  (prParam->eSpeed == DS_NORMAL)
                   || (prParam->eSpeed == DS_FAST)
                   || (prParam->eSpeed == DS_MAX)
                   );
  valid = valid && (  (prParam->eColorSpace == CSP_AUTO)
                   || (prParam->eColorSpace == CSP_BGR)
                   || (prParam->eColorSpace == CSP_BGRA)
                   || (prParam->eColorSpace == CSP_YV12)
                   || (prParam->eColorSpace == CSP_YV24)
                   || (prParam->eColorSpace == CSP_GRAYSCALE)
                   || (prParam->eColorSpace == CSP_YV12A)
                   || (prParam->eColorSpace == CSP_YV24A)
                   );

  return valid;
}

//------------------------------------------------------------------------------

ERROR_CODE wci3_encode_frame
(
  const ENCODE_PARAM *     IN     prParam
, const unsigned char *    IN     pbtImage //! Pointer to an encoded image
, const FRAME_IMAGE_INFO * IN     prInfo   //! Image information
, STREAM *                 IN OUT prStream
)
{
  INIT_PARAM rInitParam;
  ENC_PARAM  rEncParam;
  ENC_FRAME  rEncFrame;
  ENC_STATS  rEncStat[MAX_WAV_STEPS];
  ENCODER *  prEncoder = NULL;
  int        err;

  ENC_CHECK( prParam != NULL );
  ENC_CHECK( pbtImage != NULL );
  ENC_CHECK( prInfo != NULL );
  ENC_CHECK( prStream != NULL );

  DPRINTF( DPRINTF_DEBUG, "WICA3::wci3_encode_frame\n" );

  if( !wci_validate_param(prParam) )
    return ERR_PARAM;

  memset( &rInitParam, 0, sizeof(rInitParam) );
  rInitParam.iCPU_flags = 0x80000000;

  wci_init(NULL, 0, &rInitParam, NULL);

  wci_set_enc_param( prInfo, prParam->iQuality, prParam->eSpeed, prParam->eColorSpace, &rEncParam );

  memset( rEncStat , 0, sizeof(rEncStat ) );

  {
    if ( prParam->iQuality >= LOSSLESS_QUALITY )
      rEncParam.iMaxWavSteps = 0; //! ADPCM encoding
    else
      rEncParam.iMaxWavSteps = 4; //! Maximal wavelet step for encoding image

    DPRINTF( DPRINTF_DEBUG, "Q= %d\n", rEncParam.iQuality );

    if ( (err = wci_encoder_create( &rEncParam ) ) != ERR_NONE )
    {
      return err;
    }
    prEncoder = rEncParam.pEnc;

    DPRINTF( DPRINTF_DEBUG, "WS= %d\n", prEncoder->rWavParam.iMaxWavSteps );

    wci_set_enc_frame( pbtImage, prInfo, prStream, prParam->eSpeed, &rEncFrame );

    err = wci_encoder_encode( prEncoder, &rEncFrame, &rEncStat[0] );
    if (err != ERR_NONE)
    {
      wci_encoder_destroy( prEncoder );
      return err;
    }
    
    err = wci_encoder_destroy( prEncoder );
    if (err != ERR_NONE)
    {
      return err;
    }
  }

  if (rEncFrame.iGlobalFlags & USE_FRAME_CACHE)
  {
    STREAM cache_stream;
    BYTE   cache[65536];
    cache_stream.iLength = sizeof(cache);
    cache_stream.iOffset = 0;
    cache_stream.pbtData = cache;
    wci_put_decoder_cache( prStream, &cache_stream );

    prStream->iOffset = rEncFrame.iBitstreamLength;
    memcpy( &prStream->pbtData[prStream->iOffset], cache_stream.pbtData, cache_stream.iOffset );
    prStream->iOffset += cache_stream.iOffset;
  }
  else
    prStream->iOffset = rEncFrame.iBitstreamLength;
  
  return ERR_NONE;
}

#endif // #ifdef ENCODER_SUPPORT

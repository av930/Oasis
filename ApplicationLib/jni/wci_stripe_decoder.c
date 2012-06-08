/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_stripe_decoder.c

ABSTRACT:    This file contains procedures for image decoding

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00
             2004.08.27   v1.01 #define INPLACE_ONE_IMG_COMP added, 
                                correct index for zoom out calculating added

*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "wci_codec.h"
#include "wci_frame.h"
#include "bitstream/wci_bitstream.h"
#include "utils/wci_mem_align.h"
#include "image/wci_image.h"
#include "image/wci_color_space_table.h"
#include "image/wci_color_space_out.h"
#include "wav_codec/wci_wav_stripe.h"
#include "wav_codec/wci_wav_stripe_trans.h"
#include "wav_codec/wci_wav_block_quant.h"
#include "wav_codec/wci_wav_stripe_quant.h"
#include "wav_codec/wci_adpcm.h"
#include "compress/wci_huff_decompress.h"
#include "compress/wci_rle.h"
#include "compress/wci_vlc.h"
#include "compress/wci_qsp_header.h"
#include "compress/wci_qsp_stripe_decompress.h"
#include "wci_profile.h"

//------------------------------------------------------------------------------

//#define OUT_IMAGE // for debug only

//------------------------------------------------------------------------------

#define DEC_CHECK(x)   if(!(x)) return ERR_FORMAT

#define MEM_CHECK(p)   if( (p) == NULL ) return ERR_MEMORY

#define ERR_CHECK(err) if( (err) != ERR_NONE ) return err

//------------------------------------------------------------------------------

typedef struct tagSTRIPE_DECODER
{
  BITSTREAM                bitstream;
  FRAME_HEADER             frame_header;

  int                      read_frame_cache_size;

  int   iGlobalFlags;      //! Flags of global configuration

  SIZE  rSize;             //! Input frame image size
  SIZE  rEvenSize;         //! Input frame image evened size
  SIZE  rMaxEvenSize;      //! Input max frame image evened size

  int   iComponents;       //! Number of image components
  int   iWavComponents;    //! Number of wavelet image components
  int   iStripes;          //! Number of image stripes

  WAV_CODEC_PARAM         rWavParam;

  SPATIAL_SCALABILITY     rScalability;
  
  QSP_STRIPE_DECOMPRESSOR rSLD;                                 //! Stripe Lossless Decompressor
  
  WAV_STRIPE              stripe[MAX_IMAGE_COMPONENTS];         //! Component image stripe
  
  ADPCM_STATE             arADPCM_State[MAX_IMAGE_COMPONENTS];

  SIZE                    rStripeSize;
  RECT                    rStripeRect;

  RECT                    rOutputImageRect;
  SIZE                    rOutputImageSize;

  DIFF_IMAGE_EX           image_stripe;                         //! Image stripe component
  
  ALIGNED_MEMMANAGER      rMemHeap;                             //! Aligned Memory Manager struct
  ALIGNED_MEMMANAGER *    prMemHeap;

} STRIPE_DECODER;

#ifdef _DEBUG
  STRIPE_DECODER * g_prDec; // for debug only ----------------------------------
#endif

//------------------------------------------------------------------------------

static ERROR_CODE wci_decode_stripe
(
  STRIPE_DECODER * IN OUT prDec
, int              IN     iStripe
);

ERROR_CODE wci_get_decoder_cache(
  STRIPE_DECODER *     IN OUT dec
, const STREAM *       IN OUT stream
);

//------------------------------------------------------------------------------

//static void wci_set_image_size_ex(
//  SIZE             IN  rSize
//, int              IN  iWavSteps 
//, STRIPE_DECODER * OUT prDec
//)
//{
//  uint nPaddingX, nPaddingY;
//  int  iVersion = (prDec->iGlobalFlags >> (4*SF_VERSION_DECADE)) & 0xF;
//
//  assert(iWavSteps >= 0 && iWavSteps <= MAX_WAV_STEPS);
//
//  prDec->rSize = rSize;
//
//  nPaddingX = (1<<iWavSteps) - 1;
//  switch (iVersion)
//  {
//  case 1:
//    {
//      const int aiPaddingTable[MAX_WAV_STEPS+1] = {1, 4, 8, 8, 16};
//      nPaddingY = aiPaddingTable[iWavSteps] - 1;
//
//      prDec->rEvenSize.cx = max( (rSize.cx + nPaddingX) & ~nPaddingX, MB_SIZE );
//      prDec->rEvenSize.cy = max( (rSize.cy + nPaddingY) & ~nPaddingY, MB_SIZE );
//
//      prDec->rMaxEvenSize.cx = max( prDec->rEvenSize.cx, prDec->rMaxEvenSize.cx );
//      prDec->rMaxEvenSize.cy = max( prDec->rEvenSize.cy, prDec->rMaxEvenSize.cy );
//    }
//    break;
//  default:
//  case STREAM_FORMAT_VERSION:
//    {
//      if (iWavSteps > 0)
//      {
//        const int aiPaddingTable[MAX_WAV_STEPS+1] = {1, 4, 16, 16, 32};
//        nPaddingY = aiPaddingTable[iWavSteps] - 1;
//
//        prDec->rEvenSize.cx = max( (rSize.cx + nPaddingX) & ~nPaddingX, MB_SIZE );
//        prDec->rEvenSize.cy = max( (rSize.cy + nPaddingY) & ~nPaddingY, MB_SIZE );
//
//        prDec->rMaxEvenSize.cx = max( prDec->rEvenSize.cx, prDec->rMaxEvenSize.cx );
//        prDec->rMaxEvenSize.cy = max( prDec->rEvenSize.cy, prDec->rMaxEvenSize.cy );
//      }
//      else
//      {
//        prDec->rMaxEvenSize = prDec->rEvenSize = prDec->rSize;
//      }
//    }
//    break;
//  }
//}

//------------------------------------------------------------------------------

static int wci_sld_get_buffer_size(
  SIZE IN rSize
, int  IN iComponents
, int  IN iWavSteps
)
{
  int iBufferSize;

  iBufferSize  = 2 * (sizeof(HUFF_TABLE) + CACHE_LINE);

  iBufferSize += iComponents * (1 + iWavSteps)
                 * 5 // 3P + Q + S
                 * (wci_qsp_get_max_component_context_size() + CACHE_LINE);

  rSize.cx = max(rSize.cx, wci_adpcm_get_buffer_size(rSize.cx) );

  iBufferSize +=  3 * (wci_get_square( rSize ) + CACHE_LINE); // P + Q + S

  iBufferSize +=  iComponents * (sizeof(int16_t) * rSize.cy * iWavSteps / STRIPE_SIZE_STEP + CACHE_LINE);

  return iBufferSize;
}

//------------------------------------------------------------------------------



static ERROR_CODE wci_stripe_decompressor_init
(
  int                         IN     max_stripe_height
, int                         IN     stripes
, int                         IN     components
, const WAV_CODEC_PARAM *     IN     prWavParam
, const QSP_COMPRESS_HEADER * IN     qsp_header
, QSP_STRIPE_DECOMPRESSOR *      OUT decompressor
, ALIGNED_MEMMANAGER      *   IN OUT prMemHeap
)
{
  int           iComponent, iWavStep, nFlags, err;
  QSP_TABLE *   prTable = &decompressor->rTable;
  QSP_CONTEXT * prContext;


  memset( decompressor, 0, sizeof(QSP_STRIPE_DECOMPRESSOR) );

  decompressor->rHeader = *qsp_header;

  if ( QSP_P_FLAG(qsp_header->iCompressMode) == QSP_CMP_HUFFMAN )
  {
    prTable->prP = (HUFF_TABLE*)wci_aligned_malloc( sizeof(HUFF_TABLE), CACHE_LINE, prMemHeap );
    MEM_CHECK(prTable->prP);
  }
  if ( QSP_Q_FLAG(qsp_header->iCompressMode) == QSP_CMP_HUFFMAN )
  {
    prTable->prQ = (HUFF_TABLE*)wci_aligned_malloc( sizeof(HUFF_TABLE), CACHE_LINE, prMemHeap );
    MEM_CHECK(prTable->prQ);
  }

  for ( iComponent=0; iComponent<MAX_PARTITION_COMPONENTS; ++iComponent )
  {
    const WAV_LAYER * prWL = prWavParam->arWL[iComponent];
    for ( iWavStep=0; iWavStep<prWL->iWavSteps; ++iWavStep )
    {
      int nBlock;
      prContext = &decompressor->arContext[iComponent][iWavStep];
      nFlags = QSP_Q_FLAG( qsp_header->iCompressMode );
        err = wci_qsp_init_component_context( nFlags, prTable->prQ, prMemHeap, &prContext->rQ );
      ERR_CHECK( err );

      nFlags = QSP_S_FLAG( qsp_header->iCompressMode );
      err = wci_qsp_init_component_context( nFlags, NULL, prMemHeap, &prContext->rS );
      ERR_CHECK( err );

      nFlags = QSP_P_FLAG( qsp_header->iCompressMode );
      for ( nBlock=LH; nBlock<=HH; ++nBlock )
      {
        err = wci_qsp_init_component_context( nFlags, prTable->prP, prMemHeap, &prContext->rP[nBlock] );
        ERR_CHECK( err );
        err = wci_qsp_add_component_context( QSP_CMP_RLE, NULL, prMemHeap, &prContext->rP[nBlock] );
        ERR_CHECK( err );
      }
    }

    prContext = &decompressor->arContext[iComponent][MAX_WAV_STEPS]; //! Max index for LL component

    nFlags = QSP_Q_FLAG( qsp_header->iCompressMode );
    err = wci_qsp_init_component_context( nFlags, prTable->prQ, prMemHeap, &prContext->rQ );
    ERR_CHECK( err );

    nFlags = QSP_S_FLAG( qsp_header->iCompressMode );
    err = wci_qsp_init_component_context( nFlags, NULL, prMemHeap, &prContext->rS );
    ERR_CHECK( err );

    nFlags = QSP_P_FLAG( qsp_header->iCompressMode );
    err = wci_qsp_init_component_context( nFlags, prTable->prP, prMemHeap, &prContext->rP[LL] );
    ERR_CHECK( err );

    err = wci_qsp_add_component_context( QSP_CMP_RLE, prTable->prP, prMemHeap, &prContext->rP[LL] );
    ERR_CHECK( err );
  }

  {
    int width = prWavParam->rImageInfo.rSize.cx;
    int cx    = wci_adpcm_get_buffer_size(width) / sizeof(short);
    
    wci_qsp_init_buffer( max(width, cx) * max_stripe_height, prMemHeap, &decompressor->rQSP );
  }

  {
    int size = sizeof(int16_t) * components * stripes * prWavParam->arWL[0][0].iWavSteps;

    decompressor->q_size = (int16_t*)wci_aligned_malloc( size, CACHE_LINE, prMemHeap );
    MEM_CHECK(decompressor->q_size);
    memset( decompressor->q_size, 0, size );

    decompressor->s_size = (int16_t*)wci_aligned_malloc( size, CACHE_LINE, prMemHeap );
    MEM_CHECK(decompressor->s_size);
    memset( decompressor->s_size, 0, size );
  }
  
 if(CSP_IS_THERE_ALPHA(prWavParam->rImageInfo.eColorSpace))
 {
   prContext = &decompressor->arContext[ALPHA_COMPONENT][0]; 
   nFlags = QSP_Q_FLAG( qsp_header->iCompressMode );
   err = wci_qsp_init_component_context( QSP_CMP_RLE, prTable->prQ, prMemHeap, &prContext->rQ );
   ERR_CHECK( err );
 }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

static void wci_stripe_decompressor_done(
  QSP_STRIPE_DECOMPRESSOR * IN OUT decompressor,
  ALIGNED_MEMMANAGER *      IN OUT prMemHeap
)
{
  int iComponent, iWavStep;

  for ( iComponent=0; iComponent<MAX_IMAGE_COMPONENTS; ++iComponent )
  {
    QSP_CONTEXT * prContext = decompressor->arContext[iComponent];

    for ( iWavStep=0; iWavStep<MAX_WAV_STEPS; ++iWavStep )
    {
      int nBlock;

      wci_qsp_done_component_context( &prContext[iWavStep].rQ, prMemHeap );
      wci_qsp_done_component_context( &prContext[iWavStep].rS, prMemHeap );

      for ( nBlock=LH; nBlock<=HH; ++nBlock )
      {
        wci_qsp_done_component_context( &prContext[iWavStep].rP[nBlock], prMemHeap );
      }
    }

    prContext = &decompressor->arContext[iComponent][MAX_WAV_STEPS]; //! Max index for LL component

    wci_qsp_done_component_context( &prContext->rQ, prMemHeap );
    wci_qsp_done_component_context( &prContext->rS, prMemHeap );
    wci_qsp_done_component_context( &prContext->rP[LL], prMemHeap );
  }

  wci_aligned_free( decompressor->rTable.prQ, prMemHeap );
  wci_aligned_free( decompressor->rTable.prP, prMemHeap );

  wci_qsp_done_buffer( &decompressor->rQSP, prMemHeap );

  wci_aligned_free( decompressor->q_size, prMemHeap );
  wci_aligned_free( decompressor->s_size, prMemHeap );

  memset( decompressor, 0, sizeof(QSP_STRIPE_DECOMPRESSOR) );
}

//------------------------------------------------------------------------------

static int wci_get_buffer_size(
  SIZE           IN rSize
, COLOR_SPACE    IN eColorSpace
, int            IN iMaxZoomOut
, int            IN iZoomOut
, int            IN iMaxStripeNumber
)
{
  int iComponents;
  SIZE arSize[MAX_IMAGE_COMPONENTS];
  int  err, iComponent, iWavSteps;
  int  iBufferSize = 0;

  iComponents = wci_get_component_number(eColorSpace);
      
  iWavSteps = iMaxZoomOut;
  if ( wci_image_get_pixel_sampling( eColorSpace ) == PS_420 )
    iWavSteps++;

  // for ADPCM decompressor of LL components
  iBufferSize += iComponents * wci_adpcm_get_buffer_size( rSize.cx >> iWavSteps );

  // for lossless decompressor
  iBufferSize += wci_sld_get_buffer_size( rSize, iComponents, iWavSteps );

  // for qsp lengths
  iBufferSize += sizeof(int16_t) * iComponents * (iMaxStripeNumber + 1) * iWavSteps * 2;

 // for stripe image
  rSize.cx >>= iZoomOut;
  rSize.cy = max( rSize.cy >> iZoomOut, STRIPE_MIN_SIZE );

  err = wci_image_set_component_size( rSize, eColorSpace, arSize );
  if ( err != ERR_NONE )
    return -1;

  // for difference component image
  for ( iComponent=0; iComponent<iComponents; ++iComponent )
    iBufferSize += wci_get_square( arSize[iComponent] )*2 + CACHE_LINE;

  // for inverse wavelet transformation
  for ( iComponent=0; iComponent<iComponents; ++iComponent )
    iBufferSize += wci_stripe_get_buffer_size( arSize[iComponent], iWavSteps );

  return iBufferSize;
}

//------------------------------------------------------------------------------

int wci3_get_decoder_buffer_size
(
   const FRAME_IMAGE_INFO * IN prInfo
 , int                      IN iZoomOut
 )
{
  if ( iZoomOut >= 0 && iZoomOut < MAX_WAV_STEPS )
  {
    SIZE rSize;
    int  iBufferSize = sizeof(STRIPE_DECODER) + CACHE_LINE;
    int  iWavSteps =  prInfo->iMaxZoomOut;

    if ( wci_image_get_pixel_sampling( prInfo->eColorSpace ) == PS_420 )
      iWavSteps++;

    rSize.cx = prInfo->iWidth ;
    rSize.cy = prInfo->iHeight;

    rSize = wci_wav_get_image_size( rSize, iWavSteps, STREAM_FORMAT_VERSION);
    {
      int stripes = 1 + rSize.cy/STRIPE_MIN_SIZE + 1;
      rSize.cy = STRIPE_MIN_SIZE;
      iBufferSize += wci_get_buffer_size( rSize, prInfo->eColorSpace, prInfo->iMaxZoomOut, iZoomOut, stripes);
    }
    return iBufferSize;
  }
  return -1;  
}



//------------------------------------------------------------------------------

void wci_destroy_stripe_decoder(
  STRIPE_DECODER * IN OUT prDec
)
{
  int iComponent;

  wci_stripe_decompressor_done( &prDec->rSLD, prDec->prMemHeap );

  for ( iComponent=0; iComponent<prDec->iComponents; ++iComponent )
  {
    wci_adpcm_done_stripe_state( &prDec->arADPCM_State[iComponent], prDec->prMemHeap );
  }
}

//------------------------------------------------------------------------------

static SIZE wci_get_max_stripe_size(
  int            IN iMaxBufferSize
, SIZE           IN rImageSize            
, COLOR_SPACE    IN eColorSpace
, int            IN iMaxZoomOut
, int            IN iZoomOut
)
{
  SIZE rSize, rMaxSize;
  int  iBufferSize;
  int iMaxStripeNumber = rImageSize.cy/STRIPE_MIN_SIZE  + 1;

  memset( &rMaxSize, 0, sizeof(SIZE) );

  rSize.cx = rImageSize.cx;
  rSize.cy = iMaxZoomOut ? STRIPE_MIN_SIZE : min(rImageSize.cy, STRIPE_MIN_SIZE);

  for( ; rSize.cy<=rImageSize.cy>>iZoomOut; rSize.cy += STRIPE_SIZE_STEP )
  {
    iBufferSize = wci_get_buffer_size( rSize, eColorSpace, iMaxZoomOut, iZoomOut, iMaxStripeNumber );
    if ( iBufferSize > iMaxBufferSize )
      break;

    rMaxSize = rSize;
  }
  rMaxSize.cx >>= iZoomOut;
  rMaxSize.cy = min( rMaxSize.cy, rImageSize.cy>>iZoomOut );

  return rMaxSize;
}


//------------------------------------------------------------------------------

ERROR_CODE wci_create_decoder
(
  const FRAME_HEADER *        IN  prFrameHeader
, const WAV_CODEC_PARAM *     IN  prWavParam
, const QSP_COMPRESS_HEADER * IN  prQSP_Header
, STRIPE_DECODER *            OUT prDec
)
{
  int  err;
  int  iMaxMemorySize = wci_aligned_get_freemem( prDec->prMemHeap );
  SIZE rOutSize;
  SIZE rStripeSize;
  int  iComponent, iZoomOut = 0;
  
  // Fill members of Decoder structure

  prDec->rSize        = prFrameHeader->rImageSize;
  prDec->rEvenSize    = wci_wav_get_image_size( prDec->rSize, prWavParam->iMaxWavSteps, (prDec->iGlobalFlags >> (4*SF_VERSION_DECADE)) & 0xF );
  prDec->rMaxEvenSize = prDec->rEvenSize;
  
//  wci_set_image_size_ex( prFrameHeader->rImageSize, prWavParam->iMaxWavSteps, prDec );

  prDec->rWavParam = *prWavParam;

  prDec->iComponents    = wci_get_component_number(prFrameHeader->eColorSpace);
  prDec->iWavComponents = prDec->iComponents - (CSP_IS_THERE_ALPHA(prFrameHeader->eColorSpace) ? 1 : 0);
  
  prDec->rStripeRect.right = prDec->rMaxEvenSize.cx;

  switch(prDec->rScalability.eMode)
  {
    case SSM_SCROLL:
      prDec->rOutputImageRect = prDec->rScalability.rClipRect;
  	  break;

    case SSM_ZOOM:
      iZoomOut = prDec->rScalability.iZoomOut;

    default:
      prDec->rOutputImageRect.left = 0;
      prDec->rOutputImageRect.top  = 0;
      prDec->rOutputImageRect.right  = prDec->rSize.cx >> iZoomOut;
      prDec->rOutputImageRect.bottom = prDec->rSize.cy >> iZoomOut;
  	  break;
  }

  prDec->rOutputImageSize = wci_rect_get_size( prDec->rOutputImageRect );
  prDec->rOutputImageSize.cy = 0; // there are not ready lines

  for ( iComponent=0; iComponent<prDec->iComponents; ++iComponent )
  {
    WAV_LAYER *prWL = &prDec->rWavParam.arWL[iComponent][0];
    SIZE       size;
    int        shift = 1;

    if ( prWL->iWavSteps )
    {
      size  = prWL->rInvBlockSize;
      if ( prWL->iWavSteps > iZoomOut )
        shift = 2;
      wci_down_block_size( ewtHV, &size );
    }
    else
    {
      size = prWL->rBlockSize;
    }

    err = wci_adpcm_init_stripe_state( prDec->rWavParam.iQuantMode, size, shift, prDec->rWavParam.iQuantizer/QUANTIZER_SCALE_FACTOR
                               , &prDec->arADPCM_State[iComponent], prDec->prMemHeap );
    ERR_CHECK( err );
  }

  rOutSize = prDec->rMaxEvenSize;
  rOutSize.cx >>= iZoomOut;
  rOutSize.cy >>= iZoomOut;

  {
    SIZE rMaxStripeSize;

    rMaxStripeSize = wci_get_max_stripe_size( iMaxMemorySize, prDec->rMaxEvenSize, prFrameHeader->eColorSpace, prFrameHeader->iMaxZoomOut, iZoomOut );
    if ( wci_get_square(rMaxStripeSize) <= 0 )
      return ERR_MEMORY;

    rStripeSize.cx = rMaxStripeSize.cx;
    rStripeSize.cy = min(STRIPE_MIN_SIZE, rOutSize.cy); // FIXME

    prDec->rStripeSize = rStripeSize;
  }

  prDec->rMaxEvenSize = rOutSize;

  {
    IMAGE_INFO * info = &prDec->image_stripe.rInfo;

    memset( &prDec->image_stripe, 0, sizeof(DIFF_IMAGE_EX) );

    info->rSize          = rStripeSize;
    info->eColorSpace    = prFrameHeader->eColorSpace;
    info->ePixelSampling = prFrameHeader->ePixelSampling;


    info->iComponents =  wci_get_component_number(info->eColorSpace);

    {
      SIZE component_size[MAX_IMAGE_COMPONENTS];
      int  i, err;

      err = wci_image_set_component_size( rStripeSize, info->eColorSpace, component_size );
      ERR_CHECK( err );

      for ( i=0; i<info->iComponents; ++i )
      {
        DIFF_IMAGE_COMPONENT * component = &prDec->image_stripe.arComponent[i];

        component->rEdgedSize = component_size[i];
        component->rSize      = component_size[i];

        component->bUseExternalBuffer = TRUE;
        component->piData             = NULL;
      }
    }
  }

  if ( rOutSize.cy > rStripeSize.cy )
    prDec->iStripes = 1 + CELLS( rOutSize.cy - (rStripeSize.cy - STRIPE_RESPONSE), rStripeSize.cy );
  else
    prDec->iStripes = 1;

  for ( iComponent=0; iComponent<prDec->iComponents; ++iComponent )
  {
    WAV_LAYER *prWL = &prDec->rWavParam.arWL[iComponent][0];
    SIZE       size = prWL->rBlockSize;

    size.cx >>= iZoomOut;
    size.cy >>= iZoomOut;

    err = wci_stripe_init(  size
                          , prDec->image_stripe.arComponent[iComponent].rSize
                          , prWL->iWavSteps-iZoomOut
                          , prDec->iStripes
                          , prDec->prMemHeap
                          , &prDec->stripe[iComponent] );

    ERR_CHECK( err );
  }

  err = wci_stripe_decompressor_init( rStripeSize.cy, prDec->iStripes, prDec->iComponents
                , prWavParam, prQSP_Header
                , &prDec->rSLD
                , prDec->prMemHeap);
  ERR_CHECK( err );


  return ERR_NONE;
}

//------------------------------------------------------------------------------

static ERROR_CODE wci_prepare_decoder
(
  BITSTREAM *       IN OUT prBS
, STRIPE_DECODER *  IN OUT prDec
)
{
  int iComponent, iWavStep;
  int err;
  QSP_STRIPE_DECOMPRESSOR * prStripeDec = &prDec->rSLD;
  QSP_COMPRESS_HEADER *     prHeader = &prStripeDec->rHeader;
  QSP_BUFFER                rMainQSP = prStripeDec->rQSP;
  
  // Set block sizes
  {
    int iBlock = 0;
    for ( iComponent=0; iComponent<MAX_PARTITION_COMPONENTS; ++iComponent )
    {
      QSP_CONTEXT * prContext = prStripeDec->arContext[iComponent];
      WAV_LAYER *   prWL = &prDec->rWavParam.arWL[iComponent][0];
      SIZE          rBlockSize = prWL->rBlockSize;

      for( iWavStep=0; iWavStep<prWL->iWavSteps; ++iWavStep )
      {
        prContext[iWavStep].rImageSize = rBlockSize;

        if ( iBlock >= prHeader->iBlocks )
          return ERR_FORMAT;
        prContext[iWavStep].rQSP_Size = prHeader->rBlockSize[iBlock];
        prContext[iWavStep].rQSP_CompressedSize = prHeader->rCompressedBlockSize[iBlock];
          iBlock++;
        wci_down_block_size( ewtHV, &rBlockSize );
      }
      prContext[MAX_WAV_STEPS].rImageSize = rBlockSize;

      if ( iBlock >= prHeader->iBlocks )
        return ERR_FORMAT;
      prContext[MAX_WAV_STEPS].rQSP_Size = prHeader->rBlockSize[iBlock]; // LL-component
      prContext[MAX_WAV_STEPS].rQSP_CompressedSize = prHeader->rCompressedBlockSize[iBlock];
      iBlock++;
    }
    if ( iBlock != prHeader->iBlocks )
      return ERR_FORMAT;
  }

  // Decompress total layer data
  if ( prHeader->iCompressMode & QSP_BLOCK_PACK_FLAG )
  {
    QSP_BUFFER *        prQSP = &rMainQSP;
    QSP_TABLE *         prTable = &prDec->rSLD.rTable;

    prQSP->iP_Pos = prHeader->rTotalSize.aiP[0];
    prQSP->iQ_Pos = prHeader->rTotalSize.iQ;
    prQSP->iS_Pos = prHeader->rTotalSize.iS;

    if( prHeader->rTotalSize.aiP[0] > 0 )
    {
      int    nFlags = QSP_P_FLAG( prHeader->iCompressMode );
      BOOL   bTwoCompressSteps = (nFlags == QSP_CMP_RLE_VLC) || (nFlags == QSP_CMP_RLE_HUFFMAN);

      wci_qsp_get_table( QSP_P_FLAG( prHeader->iCompressMode ), prBS, prTable->prP );
      for ( iComponent=0; iComponent<MAX_PARTITION_COMPONENTS; ++iComponent )
      {
        WAV_LAYER *   prWL = &prDec->rWavParam.arWL[iComponent][0];
        QSP_CONTEXT * prC;
        int           nBlock, nUnCompressedSize, nCompressedSize;

        for( iWavStep=0; iWavStep<prWL->iWavSteps; ++iWavStep )
        {
          for ( nBlock=LH; nBlock<=HH; ++nBlock )
          {
            prC = &prStripeDec->arContext[iComponent][iWavStep];

            if( bTwoCompressSteps)
            {
              nUnCompressedSize = prC->rQSP_CompressedSize.aiP[nBlock][COMPRESSION_STEP1];
              nCompressedSize   = prC->rQSP_CompressedSize.aiP[nBlock][COMPRESSION_STEP2];
            }
            else
            {
              nUnCompressedSize = prC->rQSP_Size.aiP[nBlock];
              nCompressedSize   = prC->rQSP_CompressedSize.aiP[nBlock][COMPRESSION_STEP1];
            }

            wci_qsp_prepare_context( prBS, nUnCompressedSize, &prC->rP[nBlock] );
            wci_bitstream_skip( prBS, 8*nCompressedSize );
          }
        }

        prC = &prStripeDec->arContext[iComponent][MAX_WAV_STEPS];

        if( bTwoCompressSteps)
        {
          nUnCompressedSize= prC->rQSP_CompressedSize.aiP[0][COMPRESSION_STEP1];
          nCompressedSize = prC->rQSP_CompressedSize.aiP[0][COMPRESSION_STEP2];
        }
        else
        {
          nUnCompressedSize= prC->rQSP_Size.aiP[0];
          nCompressedSize = prC->rQSP_CompressedSize.aiP[0][COMPRESSION_STEP1];
        }

        wci_qsp_prepare_context( prBS, nUnCompressedSize, &prC->rP[LL] );
        wci_bitstream_skip( prBS, 8*nCompressedSize );
      }
    }

    wci_qsp_get_table( QSP_Q_FLAG( prHeader->iCompressMode ), prBS, prTable->prQ );

    for ( iComponent=0; iComponent<MAX_PARTITION_COMPONENTS; ++iComponent )
    {
      WAV_LAYER *   prWL = &prDec->rWavParam.arWL[iComponent][0];
      QSP_CONTEXT * prContext = prStripeDec->arContext[iComponent];

      for( iWavStep=0; iWavStep<prWL->iWavSteps; ++iWavStep )
      {
        wci_qsp_prepare_context( prBS, prContext[iWavStep].rQSP_CompressedSize.iQ[0], &prContext[iWavStep].rQ );
        wci_bitstream_skip( prBS, 8*prContext[iWavStep].rQSP_CompressedSize.iQ[0] );
      }

      wci_qsp_prepare_context( prBS, prContext[4].rQSP_CompressedSize.iQ[0], &prContext[4].rQ );
      wci_bitstream_skip( prBS, 8*prContext[4].rQSP_CompressedSize.iQ[0] );
    }

    for ( iComponent=0; iComponent<MAX_PARTITION_COMPONENTS; ++iComponent )
    {
      WAV_LAYER *   prWL = &prDec->rWavParam.arWL[iComponent][0];
      QSP_CONTEXT * prContext = prStripeDec->arContext[iComponent];

      for( iWavStep=0; iWavStep<prWL->iWavSteps; ++iWavStep )
      {
        wci_qsp_prepare_context( prBS, prContext[iWavStep].rQSP_CompressedSize.iS[COMPRESSION_STEP1], &prContext[iWavStep].rS );
        wci_bitstream_skip( prBS, 8*prContext[iWavStep].rQSP_CompressedSize.iS[COMPRESSION_STEP1] );
      }

      wci_qsp_prepare_context( prBS, prContext[MAX_WAV_STEPS].rQSP_CompressedSize.iS[COMPRESSION_STEP1], &prContext[MAX_WAV_STEPS].rS );
      wci_bitstream_skip( prBS, 8*prContext[4].rQSP_CompressedSize.iS[0] );
    }
  }
  else
  {
    QSP_BUFFER *        prQSP = &rMainQSP;
    QSP_TABLE *         prTable = &prDec->rSLD.rTable;
    QSP_COMPONENT_CONTEXT * prCC;

    BITSTREAM             rBS;
    VLC_CONTEXT           rVC;
    HUFF_CONTEXT          rHC;
    RLE_BIT_CONTEXT       rRBC;
    QSP_COMPONENT_CONTEXT rCC;

    prQSP->iP_Pos = prHeader->rTotalSize.aiP[0];
    prQSP->iQ_Pos = prHeader->rTotalSize.iQ;
    prQSP->iS_Pos = prHeader->rTotalSize.iS;

    if( prHeader->rTotalSize.aiP[0] > 0 )
    {
      prCC = &rCC;
      wci_qsp_set_component_context( QSP_P_FLAG( prHeader->iCompressMode ), &rBS, &rVC, &rHC, &rRBC, prTable->prP, prCC );

      wci_qsp_get_table( prCC->nFlags, prBS, prTable->prP );
      wci_qsp_prepare_context( prBS, prQSP->iP_Pos, prCC );

      for ( iComponent=0; iComponent<MAX_PARTITION_COMPONENTS; ++iComponent )
      {
        WAV_LAYER *   prWL = &prDec->rWavParam.arWL[iComponent][0];
        QSP_CONTEXT * prContext = prStripeDec->arContext[iComponent];

        for( iWavStep=0; iWavStep<prWL->iWavSteps; ++iWavStep )
        {
          int  nBlock;
          SIZE rP_Size = wci_wav_get_partition_block( prContext[iWavStep].rImageSize );
          int  size    = wci_get_square(rP_Size);

          for ( nBlock=LH; nBlock<=HH; ++nBlock )
          {
            wci_rle_init_context( prCC->prRBC );
            wci_qsp_copy_context_value( prCC, &prContext[iWavStep].rP[nBlock] );

            err = wci_qsp_decompress_data( prHeader->iCompressMode, prCC, size, NULL );
            ERR_CHECK( err );
          }
        }
      }
      wci_qsp_skip_component( prCC, prBS );
    }

    prCC = &rCC;
    wci_qsp_set_component_context( QSP_Q_FLAG( prHeader->iCompressMode ), &rBS, &rVC, &rHC, &rRBC, prTable->prQ, prCC );

    wci_qsp_get_table( prCC->nFlags, prBS, prTable->prQ );
    wci_qsp_prepare_context( prBS, prQSP->iQ_Pos, prCC );

    for ( iComponent=0; iComponent<MAX_PARTITION_COMPONENTS; ++iComponent )
    {
      WAV_LAYER *   prWL = &prDec->rWavParam.arWL[iComponent][0];
      QSP_CONTEXT * prContext = prStripeDec->arContext[iComponent];

      for( iWavStep=0; iWavStep<prWL->iWavSteps; ++iWavStep )
      {
        wci_qsp_copy_context_value( prCC, &prContext[iWavStep].rQ );

        err = wci_qsp_decompress_data( prHeader->iCompressMode, prCC, prContext[iWavStep].rQSP_Size.iQ, NULL );
        ERR_CHECK( err );
      }

      wci_qsp_copy_context_value( prCC, &prContext[MAX_WAV_STEPS].rQ );

      err = wci_qsp_decompress_data( prHeader->iCompressMode, prCC, prContext[MAX_WAV_STEPS].rQSP_Size.iQ, NULL );
      ERR_CHECK( err );
    }
    wci_qsp_skip_component( prCC, prBS );

    prCC = &rCC;
    wci_qsp_set_component_context( QSP_S_FLAG( prHeader->iCompressMode ), &rBS, &rVC, &rHC, &rRBC, NULL, prCC );

    wci_qsp_prepare_context( prBS, prQSP->iS_Pos, prCC );

    for ( iComponent=0; iComponent<MAX_PARTITION_COMPONENTS; ++iComponent )
    {
      WAV_LAYER *   prWL = &prDec->rWavParam.arWL[iComponent][0];
      QSP_CONTEXT * prContext = prStripeDec->arContext[iComponent];

      for( iWavStep=0; iWavStep<prWL->iWavSteps; ++iWavStep )
      {
        wci_qsp_copy_context_value( prCC, &prContext[iWavStep].rS );

        err = wci_qsp_decompress_data( prHeader->iCompressMode, prCC, prContext[iWavStep].rQSP_Size.iS, NULL );
        ERR_CHECK( err );
      }

      wci_qsp_copy_context_value( prCC, &prContext[MAX_WAV_STEPS].rS );

      err = wci_qsp_decompress_data( prHeader->iCompressMode, prCC, prContext[MAX_WAV_STEPS].rQSP_Size.iS, NULL );
      ERR_CHECK( err );
    }
    wci_qsp_skip_component( prCC, prBS );
  }

  if(CSP_IS_THERE_ALPHA(prDec->frame_header.eColorSpace))
  {
    QSP_CONTEXT*           prContext = prStripeDec->arContext[ALPHA_COMPONENT];
    QSP_COMPRESS_HEADER    rQSP_Header;
    WAV_LAYER*             prWL = &prDec->rWavParam.arWL[ALPHA_COMPONENT][0];
    SIZE                   rBlockSize = prWL->rBlockSize;
    int                    iCompressedSize;
 
    err = wci_qsp_get_header( prBS, &rQSP_Header );
    ERR_CHECK(err);
    iCompressedSize = rQSP_Header.rCompressedBlockSize[0].iQ[0];
    wci_qsp_prepare_context( prBS, iCompressedSize, &prContext[0].rQ );
    prContext[0].rQ.pbtData = wci_bitstream_get_pos( prBS );
    wci_bitstream_skip( prBS, 8*iCompressedSize );

    prContext->rQSP_Size = rQSP_Header.rBlockSize[0];
    prContext->rQSP_CompressedSize = rQSP_Header.rCompressedBlockSize[0];
    prContext->rImageSize = rBlockSize;
  }
 
  return ERR_NONE;
}


//------------------------------------------------------------------------------

static ERROR_CODE wci_fill_frame_info
(
  const DECODE_PARAM *   IN     prParam
, const STRIPE_DECODER * IN     prDec
, const FRAME_HEADER *   IN     prFrameHeader
, FRAME_IMAGE_INFO *     IN OUT prInfo
)
{
  SIZE rSize = wci_rect_get_size( prDec->rOutputImageRect );

  memset( prInfo, 0, sizeof(FRAME_IMAGE_INFO) );

  prInfo->iWidth  = rSize.cx;
  prInfo->iHeight = rSize.cy;

  prInfo->iResolutionHorz = prFrameHeader->rImageResolution.cx;
  prInfo->iResolutionVert = prFrameHeader->rImageResolution.cy;

  prInfo->eColorSpace = prParam->eColorSpace;

  prInfo->iImageSize = wci_image_get_size( rSize, prParam->eColorSpace );

  prInfo->iMaxZoomOut = prFrameHeader->iMaxZoomOut;

  prInfo->iQuality = prFrameHeader->iQuality;

  return ERR_NONE;
}

//------------------------------------------------------------------------------

static ERROR_CODE wci_out_stripe(
  const DECODE_PARAM * IN     prParam
, STRIPE_DECODER *     IN OUT prDec )
{
  if ( wci_rect_is_crossed( prDec->rStripeRect, prDec->rOutputImageRect ) )
  {
    BYTE * pbtDst;
    int    err;

    RECT   rIntersectionRect = wci_rect_get_intersection( prDec->rStripeRect, prDec->rOutputImageRect );
    SIZE   rIntersectionSize = wci_rect_get_size( rIntersectionRect );

    rIntersectionRect.bottom -= prDec->rStripeRect.top;
    rIntersectionRect.top    -= prDec->rStripeRect.top;

    if (prParam->pbtImage != NULL)
    {
      int offset = wci_image_get_size( prDec->rOutputImageSize, prParam->eColorSpace & ~CSP_VFLIP);
      if (prParam->eColorSpace & CSP_VFLIP)
      {
        SIZE size = wci_rect_get_size(prDec->rOutputImageRect);
        size.cy -= rIntersectionSize.cy;
        pbtDst = prParam->pbtImage + wci_image_get_size(size, prParam->eColorSpace & ~CSP_VFLIP) - offset;
      }
      else
        pbtDst = prParam->pbtImage + offset;

      err = wci_image_clip_output( &prDec->image_stripe, prParam->eColorSpace, 0, rIntersectionRect, pbtDst );
      ERR_CHECK( err );
    }

   prDec->rOutputImageSize.cy += rIntersectionSize.cy;
  }

  return ERR_NONE;
}      

//------------------------------------------------------------------------------

ERROR_CODE wci_check_decoding_param( const DECODE_PARAM * IN     prParam, int iMaxWavSteps)
{

  if(prParam->rScalability.iZoomOut < 0 || prParam->rScalability.iZoomOut > iMaxWavSteps)
    return ERR_PARAM;
  if(prParam->rScalability.eMode != SSM_NONE && prParam->rScalability.eMode != SSM_ZOOM && prParam->rScalability.eMode != SSM_SCROLL)
    return ERR_PARAM;
  return ERR_NONE;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_create_stripe_decoder(
  const DECODE_PARAM * IN     prParam
, const STREAM *       IN OUT prStream
, STRIPE_DECODER *        OUT dec
)
{
  int                 err;
  QSP_COMPRESS_HEADER rQSP_Header;
  WAV_CODEC_PARAM     rWavParam;


  DPRINTF( DPRINTF_DEBUG, "wci_create_stripe_decoder\n" );
  
  if(!dec->prMemHeap)
    memset(dec, 0, sizeof(STRIPE_DECODER));

  PF_BEGIN_COUNT(PF_UNPACK_HEADER);

  // Get decoding parameters of common scheme and wavelet transformation
  wci_bitstream_init(&dec->bitstream, &prStream->pbtData[ prStream->iOffset ], prStream->iLength);

  wci_get_frame_header( &dec->bitstream, &dec->frame_header );

  if ( wci_check_frame_header( &dec->frame_header ) != ERR_NONE )
  {
    return ERR_FORMAT;
  }

  dec->iGlobalFlags = dec->frame_header.iGlobalFlags;

  if ( dec->frame_header.eFrameType != I_VOP )
  {
    return ERR_FORMAT;
  }

  if ( wci_wav_get_header( &dec->bitstream, &rWavParam ) != 0 )
  {
    return ERR_FORMAT;
  }

  err = wci_qsp_get_header( &dec->bitstream, &rQSP_Header );
  if ( err != ERR_NONE )
  {
    return err;
  }

  PF_STOP_COUNT(PF_UNPACK_HEADER);
  if ( wci_check_decoding_param( prParam, rWavParam.iMaxWavSteps ) )
    return ERR_PARAM;
  PF_BEGIN_COUNT(PF_INITIALIZATION);

#ifndef CSP_CONST_TABLE
  wci_color_space_init_table(); // Initialize internal colorspace transformation tables
#endif 

  // wci_init Embedded Memory Manager
  if( dec->prMemHeap == NULL)
  {
    if(prParam->iProcBufSize && (prParam->pbtProcBuf != NULL) )
    {
      if(wci_aligned_memmanager_init( prParam->pbtProcBuf, prParam->iProcBufSize, &dec->rMemHeap))
        return ERR_MEMORY;
      dec->prMemHeap = &dec->rMemHeap;
    }
    else
      dec->prMemHeap = NULL; // use system memory
  }
  dec->rScalability = prParam->rScalability;

  // Create decoder structures, i.e structure lossless decompressor,
  //   dequantization and image stripe

  if ( (err = wci_create_decoder( &dec->frame_header, &rWavParam, &rQSP_Header, dec )) != ERR_NONE )
  {
    wci_destroy_stripe_decoder( dec );
    return err;
  }
  PF_STOP_COUNT(PF_INITIALIZATION);

  return err;
}


//------------------------------------------------------------------------------

ERROR_CODE wci_stripe_decode(
  STRIPE_DECODER *        OUT dec
, const DECODE_PARAM * IN     prParam
, const STREAM *       IN OUT prStream
, FRAME_IMAGE_INFO *      OUT prInfo         //! Decoded image information 
)
{
  int                 err;
  // Prepare lossless decompressor
  PF_BEGIN_COUNT(PF_PREPARE_DECODER);
  err = wci_prepare_decoder( &dec->bitstream, dec );
  PF_STOP_COUNT(PF_PREPARE_DECODER);

  if ( prParam->rScalability.iZoomOut == 0)
    wci_get_decoder_cache( dec, prStream );

  // Stripe decoding cycle
  if ( err == ERR_NONE )
  {
    int iStripe;

    for ( iStripe = 0; iStripe < dec->iStripes; ++iStripe )
    {
      // Decoding of image stripe

      PF_BEGIN_COUNT(PF_DECODE_STRIPE);
      err = wci_decode_stripe( dec, iStripe );
      PF_STOP_COUNT(PF_DECODE_STRIPE);

      if ( err != ERR_NONE )
        break;

      // Set and check current stripe rect
      dec->rStripeRect.top     = dec->rStripeRect.bottom;
      dec->rStripeRect.bottom += dec->image_stripe.arComponent[0].iReadyLines;

      if ( dec->rStripeRect.top > dec->rOutputImageRect.bottom )
      {
        break; // output pels are absent
      }

      // Output of image components to display
      PF_BEGIN_COUNT(PF_OUT_STRIPE);
      err = wci_out_stripe( prParam, dec );
      PF_STOP_COUNT(PF_OUT_STRIPE);

      if ( err != ERR_NONE )
        break;
    }
  }

  PF_BEGIN_COUNT(PF_DONE);
  // Form frame information

  //assert( (int)wci_bitstream_pos(&dec->bitstream) == dec->frame_header.iLength );
  wci_fill_frame_info( prParam, dec, &dec->frame_header, prInfo );

#ifdef OUT_IMAGE
  {
    static int i = 0;
    char       sz[80];
    sprintf( sz, "d%03d.pgm", i++ );
    DiffImage_DumpToPGM( &dec->image_stripe, sz );
  }
#endif

  DPRINTF( DPRINTF_INFO, "Max used memory size= %3d\n", dec->prMemHeap->nMaxUsedMem );
  PF_STOP_COUNT(PF_DONE);

  return err;
}

//------------------------------------------------------------------------------

#ifdef ENCODER_SUPPORT

ERROR_CODE wci_form_decoder_cache(
  STRIPE_DECODER *        OUT dec
, STREAM *             IN OUT prStream
)
{
  BITSTREAM   bitstream;
  const char *szPreamble = FRAME_CACHE_PREAMBLE;

  int sizes  = dec->rSLD.stripe * sizeof(int16_t);
  int length = PREAMBLE_PREFIX_SIZE + PREAMBLE_SIZE + 2 + 2*sizes;

  prStream->iLength += length;

  wci_bitstream_init(&bitstream, &prStream->pbtData[ prStream->iOffset ], prStream->iLength);

  wci_bitstream_put_bits( &bitstream, length, 32 );

  {
    int i;
    for( i=0; i<8; ++i )
      wci_bitstream_put_bits( &bitstream, szPreamble[i], 8 );
  }

  wci_bitstream_put_bits( &bitstream, dec->rSLD.stripe, 16 );
  wci_bitstream_flush( &bitstream );

  {
    uint8_t * buffer = wci_bitstream_lock_buffer( &bitstream );

    memcpy( buffer, dec->rSLD.q_size, sizes );
    memcpy( &buffer[sizes], dec->rSLD.s_size, sizes );

    wci_bitstream_unlock_buffer( &bitstream, buffer, 2 * sizes );
  }
  prStream->iOffset += wci_bitstream_pos(&bitstream);

  return ERR_NONE;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_put_decoder_cache(
  const STREAM *       IN     frame_stream
, STREAM *             IN OUT cache_stream
)
{
  FRAME_IMAGE_INFO info;
  DECODE_PARAM     param;
  STRIPE_DECODER   decoder;
  int              err;

  wci3_get_frame_info( frame_stream, &info );

  memset( &param, 0, sizeof(param) );
  param.eColorSpace  = CSP_NULL;
  param.iProcBufSize = wci3_get_decoder_buffer_size(&info, 0);
  param.pbtProcBuf   = (unsigned char *)malloc(param.iProcBufSize);
  param.pbtImage     = (unsigned char *)malloc(4*MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT);
  
  decoder.prMemHeap = NULL; 
  err = wci_create_stripe_decoder( &param, frame_stream, &decoder );
  ERR_CHECK( err );

  err = wci_stripe_decode( &decoder, &param, frame_stream, &info );
  ERR_CHECK( err );

  wci_form_decoder_cache( &decoder, cache_stream );

  wci_destroy_stripe_decoder( &decoder );

  free( param.pbtImage );
  free( param.pbtProcBuf );

  return err;
}

#endif // #ifdef ENCODER_SUPPORT

//------------------------------------------------------------------------------

ERROR_CODE wci_get_decoder_cache(
  STRIPE_DECODER *     IN OUT dec
, const STREAM *       IN OUT stream
)
{
  int offset = stream->iOffset + dec->frame_header.iLength + PREAMBLE_PREFIX_SIZE;
  const char * szPreamble = (const char *) &stream->pbtData[ offset ];

  dec->read_frame_cache_size = 0;

  if ( strncmp( szPreamble, FRAME_CACHE_PREAMBLE, PREAMBLE_SIZE ) == 0 )
  {
    uint8_t * buffer = &stream->pbtData[ offset + PREAMBLE_SIZE ];
    int       sizes;

    sizes = (buffer[0] << 8) | buffer[1];
    buffer += 2;

    memcpy( dec->rSLD.q_size, buffer, sizes * sizeof(int16_t) );
    buffer += sizes * sizeof(int16_t);
    memcpy( dec->rSLD.s_size, buffer, sizes * sizeof(int16_t) );
    buffer += sizes * sizeof(int16_t);

    dec->read_frame_cache_size = PREAMBLE_PREFIX_SIZE + PREAMBLE_SIZE + 2 + 2*sizes;

    return ERR_NONE;
  }

  return ERR_FORMAT;
}

//------------------------------------------------------------------------------

ERROR_CODE wci3_decode_frame
(
  const DECODE_PARAM * IN     prParam
, STREAM *             IN OUT prStream
, FRAME_IMAGE_INFO *      OUT prInfo         //! Decoded image information 
)
{
  STRIPE_DECODER *   prDecoder = NULL;
  ALIGNED_MEMMANAGER rMemHeap;                    
  int                err;

  PF_INIT_COUNTERS();  
  PF_BEGIN_COUNT(PF_MAIN);
  if(prParam->iProcBufSize && (prParam->pbtProcBuf != NULL))
  {
    if(wci_aligned_memmanager_init( prParam->pbtProcBuf, prParam->iProcBufSize, &rMemHeap))
      return ERR_MEMORY;
  }
  else
    return ERR_MEMORY;

  prDecoder  = wci_aligned_malloc( sizeof(STRIPE_DECODER), CACHE_LINE, &rMemHeap );
  memset(prDecoder, 0, sizeof(STRIPE_DECODER));
  prDecoder->prMemHeap = &rMemHeap;
  err = wci_create_stripe_decoder( prParam, prStream, prDecoder );
  if( err != ERR_NONE ) 
  {
    wci_destroy_stripe_decoder( prDecoder);
    if(prDecoder)
      wci_aligned_free( prDecoder, &rMemHeap );
    return err;
  }
  err = wci_stripe_decode( prDecoder, prParam, prStream, prInfo );
  if( err != ERR_NONE ) 
  {
    wci_destroy_stripe_decoder( prDecoder);
    if(prDecoder)
      wci_aligned_free( prDecoder, &rMemHeap );
    return err;
  }
 
  wci_destroy_stripe_decoder( prDecoder);
  if(prDecoder)
    wci_aligned_free( prDecoder, &rMemHeap );


  PF_STOP_COUNT(PF_MAIN);
#ifdef _PROFILING_
  PF_GET_RESULTS();
#endif

  return err;
}

//------------------------------------------------------------------------------

static ERROR_CODE wci_qsp_decompress_stripe(
  int                       IN     iComponent
, int                       IN     iWavStep
, int                       IN     iLinesToDecompress
, QSP_STRIPE_DECOMPRESSOR * IN OUT prStripeDec
, QSP_BUFFER *                 OUT prQSP )
{
  QSP_CONTEXT * prContext = &prStripeDec->arContext[iComponent][iWavStep];
  SIZE          rStripeSize;
  QSP_SIZE      rSize;
  int           err;
  int           iCompressedMode= prStripeDec->rHeader.iCompressMode;

  assert( iLinesToDecompress <= prContext->rImageSize.cy - prContext->nCurrLine );

  rStripeSize.cx = prContext->rImageSize.cx;
  rStripeSize.cy = iLinesToDecompress;

  wci_qsp_resetpos( prQSP );

  {
    SIZE   rP_Size = wci_wav_get_partition_block( rStripeSize );
    int    size = wci_get_square( rP_Size );
    int    nBlock, nOffset = 0;

    rSize.aiP[0] = (HH-LH+1)*size;

    prQSP->pbtP = prStripeDec->rQSP.pbtP;
    prQSP->iP_Length = rSize.aiP[0];

    for ( nBlock=LH; nBlock<=HH; ++nBlock )
    {
      err = wci_qsp_decompress_data( prStripeDec->rHeader.iCompressMode
                              , &prContext->rP[nBlock]
                              , size
                              , &prQSP->pbtP[nOffset] );
      ERR_CHECK( err );
      nOffset += size;
    }
  }

  if ( (rSize.iQ = prStripeDec->q_size[prStripeDec->stripe]) == 0 )
  {
    rSize.iQ = wci_qsp_get_length_q( rSize.aiP[0], rStripeSize, prQSP );
    prStripeDec->q_size[prStripeDec->stripe] = (int16_t)rSize.iQ;
  }
  else
  {
    assert( rSize.iQ == wci_qsp_get_length_q( rSize.aiP[0], rStripeSize, prQSP ) );
  }

  prQSP->pbtQ = prStripeDec->rQSP.pbtQ;
  prQSP->iQ_Length = rSize.iQ;

  err = wci_qsp_decompress_data( iCompressedMode, &prContext->rQ, prQSP->iQ_Length, prQSP->pbtQ );
  ERR_CHECK( err );

  if ( (rSize.iS = prStripeDec->s_size[prStripeDec->stripe]) == 0 )
  {
    rSize.iS = wci_qsp_get_length_s( rSize.iQ, prQSP );
    prStripeDec->s_size[prStripeDec->stripe] = (int16_t)rSize.iS;
  }
  else
  {
    assert( rSize.iS == wci_qsp_get_length_s( rSize.iQ, prQSP ) );
  }


  prQSP->pbtS = prStripeDec->rQSP.pbtS;
  prQSP->iS_Length = rSize.iS;

  err = wci_qsp_decompress_data( iCompressedMode, &prContext->rS, prQSP->iS_Length, prQSP->pbtS );
  ERR_CHECK( err );

  prContext->nCurrLine += iLinesToDecompress;

  DPRINTF( DPRINTF_DEBUG, "prStripeDec->stripe=%d\n", prStripeDec->stripe );
  prStripeDec->stripe++;

  return ERR_NONE;
}

//------------------------------------------------------------------------------

static ERROR_CODE wci_qsp_decompress_ll_stripe(
  int                       IN     iComponent
, int                       IN     iLinesToDecompress
, QSP_STRIPE_DECOMPRESSOR * IN OUT prStripeDec
, ADPCM_STATE *             IN OUT prState 
, QSP_BUFFER *                 OUT prQSP )
{
  QSP_CONTEXT * prContext = &prStripeDec->arContext[iComponent][MAX_WAV_STEPS];
  SIZE          rStripeSize, rP_Size;
  QSP_SIZE      rSize;
  int           err;
  int           iCompressedMode= prStripeDec->rHeader.iCompressMode;

  assert( iLinesToDecompress <= prContext->rImageSize.cy - prContext->nCurrLine );

  rStripeSize.cx = prContext->rImageSize.cx;
  rStripeSize.cy = iLinesToDecompress;

  wci_qsp_resetpos( prQSP );

  rP_Size = wci_adpcm_get_partition_block( rStripeSize );

  rSize.aiP[0] = wci_get_square( rP_Size );

  prQSP->pbtP = prStripeDec->rQSP.pbtP;
  prQSP->iP_Length = rSize.aiP[0];

  err = wci_qsp_decompress_data( iCompressedMode, &prContext->rP[LL], prQSP->iP_Length, prQSP->pbtP );
  ERR_CHECK( err );

  rSize.iQ = wci_adpcm_get_length_q( rStripeSize, prQSP, prState );

  prQSP->pbtQ = prStripeDec->rQSP.pbtQ;
  prQSP->iQ_Length = rSize.iQ;

  err = wci_qsp_decompress_data( iCompressedMode, &prContext->rQ, prQSP->iQ_Length, prQSP->pbtQ );
  ERR_CHECK( err );

  rSize.iS = wci_adpcm_get_length_s( rStripeSize, prQSP );

  prQSP->pbtS = prStripeDec->rQSP.pbtS;
  prQSP->iS_Length = rSize.iS;

  err = wci_qsp_decompress_data( iCompressedMode, &prContext->rS, prQSP->iS_Length, prQSP->pbtS );
  ERR_CHECK( err );

  prContext->nCurrLine += iLinesToDecompress;

  return ERR_NONE;
}


//------------------------------------------------------------------------------


ERROR_CODE wci_decompress_alpha_component
(
  STRIPE_DECODER* IN OUT prDec
, int32_t         IN     iComponent
, int32_t         IN     iComponentLines
)
{
  QSP_STRIPE_DECOMPRESSOR * prStripeDec = &prDec->rSLD;
  QSP_CONTEXT *             prContext = &prDec->rSLD.arContext[iComponent][0];
  QSP_COMPONENT_CONTEXT*    prCQ = (QSP_COMPONENT_CONTEXT*)&prContext->rQ;
  WAV_STRIPE_LAYER*         layer = &prDec->stripe[iComponent].layer[0];
  DIFF_IMAGE_COMPONENT *    component = &prDec->image_stripe.arComponent[iComponent];
  BYTE*                     pbSrc  = (BYTE*)prStripeDec->rQSP.pbtQ;
  int16_t*                  piDest;
  int                       i, err, size;

  size = prDec->rStripeSize.cx * iComponentLines;

  if(prDec->rScalability.eMode == 0 || prDec->rScalability.eMode == 2)
  { 
    err = wci_rle_decompress_block_mode(prCQ, size, prStripeDec->rQSP.pbtQ);
    ERR_CHECK(err);
  }
  else
  {
    memset( prStripeDec->rQSP.pbtQ, -1, size );
  }

  component->piData      = &layer->buffer[0];
  piDest = (int16_t*)component->piData;
  for(i = 0; i < size; i++)
  {
    *piDest++ = *pbSrc++;
  }
  component->iReadyLines = iComponentLines;
  layer->ready_line_pos += iComponentLines;

  return ERR_NONE;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_decode_stripe
(
  STRIPE_DECODER * IN OUT prDec
, int              IN     iStripe
)
{
  int err = ERR_NONE;
  int iComponent;
  int component_lines, anNeededComponentLines[MAX_IMAGE_COMPONENTS];

  if ( iStripe == 0 )
  {
    if ( prDec->iStripes == 1 )
      anNeededComponentLines[0] = prDec->rStripeSize.cy;
    else
      anNeededComponentLines[0] = prDec->rStripeSize.cy - STRIPE_RESPONSE;

    if ( prDec->image_stripe.rInfo.ePixelSampling == PS_420 )
      component_lines = max((anNeededComponentLines[0]+1)/2, 1);
    else
      component_lines = anNeededComponentLines[0];
  }
  else
  {
    int nRest = prDec->rMaxEvenSize.cy - prDec->rStripeRect.bottom;
    anNeededComponentLines[0] = min( nRest, prDec->rStripeSize.cy );

    if ( prDec->image_stripe.rInfo.ePixelSampling == PS_420 )
      component_lines = anNeededComponentLines[0]/2;
    else
      component_lines = anNeededComponentLines[0];
  }

  for ( iComponent=1; iComponent<prDec->iComponents; ++iComponent )
    anNeededComponentLines[iComponent] = component_lines;
 

  if(CSP_IS_THERE_ALPHA(prDec->image_stripe.rInfo.eColorSpace))
  {
    err = wci_decompress_alpha_component(prDec, ALPHA_COMPONENT, anNeededComponentLines[0]);
    ERR_CHECK( err );
  }
  
  for ( iComponent=prDec->iWavComponents-1; iComponent>=0; iComponent-- )
  {
    WAV_LAYER *      prWL = &prDec->rWavParam.arWL[iComponent][0];
    WAV_STRIPE *     stripe = &prDec->stripe[iComponent];
    const WAV_SIZE * needed_lines;
    QSP_BUFFER       rQSP;
    int              step;

    PF_BEGIN_COUNT(PF_STRIPE_SET_COUNTERS);
    needed_lines = wci_stripe_set_counters( anNeededComponentLines[iComponent], stripe );
    PF_STOP_COUNT(PF_STRIPE_SET_COUNTERS);  

    if ( needed_lines->iLL > 0 )
    {
      PF_BEGIN_COUNT(PF_DECOMPRESSLL_STRIPE);
      err = wci_qsp_decompress_ll_stripe( iComponent, needed_lines->iLL, &prDec->rSLD
                                        , &prDec->arADPCM_State[iComponent], &rQSP );
      PF_STOP_COUNT(PF_DECOMPRESSLL_STRIPE);  
      ERR_CHECK( err );

      PF_BEGIN_COUNT(PF_ADPCM_DECODE_STRIPE);    
      wci_adpcm_decode_stripe( &rQSP, needed_lines->iLL, &prDec->arADPCM_State[iComponent]
                             , wci_stripe_get_LL_stride( stripe )
                             , wci_stripe_get_LL_block( stripe ) );
      PF_STOP_COUNT(PF_ADPCM_DECODE_STRIPE);    
    }

    for( step = stripe->wav_steps-1; step>=0; --step )
    {
      int zoom_step = step + prWL->iWavSteps - stripe->wav_steps;

      if( stripe->layer[ step ].skip_transformation )
        continue;

      PF_BEGIN_COUNT(PF_QSP_DECOMPRESS_STRIPE);
      err = wci_qsp_decompress_stripe( iComponent, zoom_step
                                     , needed_lines->iWC[step]
                                     , &prDec->rSLD, &rQSP );
      PF_STOP_COUNT(PF_QSP_DECOMPRESS_STRIPE);
      ERR_CHECK( err );

      PF_BEGIN_COUNT(PF_STRIPE_INSERT_LL);
      wci_stripe_insert_ll_lines( step, stripe );
      PF_BEGIN_COUNT(PF_STRIPE_INSERT_LL);

      PF_BEGIN_COUNT(PF_DEQUANTSTEPSTRIPE);
      wci_stripe_dequant( &rQSP, &prWL->rQS[zoom_step], needed_lines->iWC[step], step, stripe );
      PF_STOP_COUNT(PF_DEQUANTSTEPSTRIPE);

      PF_BEGIN_COUNT(PF_STRIPE_INVWAVTRANS);     
      wci_stripe_inv_transformation( step, stripe );
      PF_STOP_COUNT(PF_STRIPE_INVWAVTRANS);     
    }

    PF_BEGIN_COUNT(PF_STRIPE_GETCOMPONENT);
    wci_stripe_get_component( stripe, anNeededComponentLines[iComponent], &prDec->image_stripe.arComponent[iComponent] );
    PF_STOP_COUNT(PF_STRIPE_GETCOMPONENT);

  #ifdef PRINT_STRIPE_COUNTERS
    DPRINTF( DPRINTF_DEBUG, "\n\n" );
  #endif
  }

  return ERR_NONE;
}

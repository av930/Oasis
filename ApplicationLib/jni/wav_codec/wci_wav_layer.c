/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: WavLayer.c

ABSTRACT:    This file contains procedures for forward and inverse 
             wavelet transform of image for Intra Frame Coding

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include <string.h>
#include <stdlib.h>

#include "../image/wci_image_convert.h"
#include "../compress/wci_qsp_compress.h"
#include "wci_wav_quant.h"
#include "wci_wav_layer.h"
#include "../compress/wci_rle.h"
#include "wci_adpcm.h"

//------------------------------------------------------------------------------

//#define OUT_IMAGE // for debug only

//------------------------------------------------------------------------------

short * wci_inv_wav_pyramid(short *Orig, int size_x, int size_y, int Nstep)
{
  int i;
  int Sx = size_x;
  int Sy = size_y;
  int len = size_x*size_y;
  short *ptr_big = Orig;
  short *ptr_small = ptr_big;

  len = Sx*Sy;

  #ifdef OUT_IMAGE
  {
    SIZE rSize;
    static int i = 0;
    char sz[80];
    sprintf( sz, "ll%03d.pgm", i++ );

    rSize.cx = Sx/2;
    rSize.cy = Sy/2;
    wci_row_diffimage_dump_to_pgm( Orig, rSize, 0, sz );
  }
  #endif

  for(i=Nstep-1; i>=0;i--)
  {
    ptr_big = ptr_small - len;

    // The function Inv_Wavlet_mmx_next_LL() is called at all steps
    // This function puts content of the buffer pointed by the 'ptr_small' pointer
    // into cells of LL-frame in the buffer pointed by the'ptr_big' pointer
    // and  then perfoms all the same as wci_inv_lift_wav_block().	
    wci_inv_wavlet_insert_ll_c(ptr_small, 0, 0, Sx, Sy, Sx, ptr_big);
    wci_inv_lift_wav_block(ptr_big, 0, 0, Sx, Sy, Sx, ptr_small);

    #ifdef OUT_IMAGE
    {
      SIZE rSize;
      static int i = 0;
      char sz[80];
      sprintf( sz, "wl%03d.pgm", i++ );

      rSize.cx = Sx;
      rSize.cy = Sy;
      wci_row_diffimage_dump_to_pgm( ptr_big, rSize, 0, sz );
    }
    #endif

    Sx = Sx<<1;
    Sy = Sy<<1;
    len = Sx*Sy;
    ptr_small = ptr_big;
  }

  return ptr_big;
}

//------------------------------------------------------------------------------

void wci_wav_inv_trans_image_ex(
  const WAV_CODEC_PARAM * IN     prWavParam //! the structure of wavelet parameters
, DIFF_IMAGE_EX *         IN OUT prImage    //! a pointer to the image array
)
{
// This function performs inverse wavelet transform of coded color image 
// For this it calls wci_inv_wav_pyramid() function for each component at all layers 

  DIFF_IMAGE_EX rImage = *prImage;
  int           iLayer, iComponent;

  for ( iLayer=0; iLayer<prWavParam->iLayers; iLayer++ )
  {
    for ( iComponent=0; iComponent<prImage->rInfo.iComponents; ++iComponent )
    {
      DIFF_IMAGE_COMPONENT * prC = &rImage.arComponent[ iComponent ];
      const WAV_LAYER *      prWL;
      SIZE                   rBlockSize;
      int                    i;

      prWL = &prWavParam->arWL[iComponent][iLayer];
      rBlockSize = prWL->rBlockSize;

      for( i=0; i<prWL->iWavSteps; i++ )
      {
        prC->piData += rBlockSize.cx*rBlockSize.cy;
        wci_down_block_size( ewtHV, &rBlockSize );
      }
    }
  }

  for ( iLayer=0; iLayer<prWavParam->iLayers; iLayer++ )
  {
    for ( iComponent=0; iComponent<prImage->rInfo.iComponents; ++iComponent )
    {
      DIFF_IMAGE_COMPONENT * prC = &rImage.arComponent[ iComponent ];
      const WAV_LAYER *      prWL;
      SIZE                   rBlockSize;

      prWL = &prWavParam->arWL[iComponent][iLayer];
      rBlockSize = prWL->rInvBlockSize;

      prC->piData = wci_inv_wav_pyramid( prC->piData, rBlockSize.cx, rBlockSize.cy, prWL->iWavSteps );
    }
  }
}

#ifdef ENCODER_SUPPORT

//------------------------------------------------------------------------------

void wci_wav_encode_image(
  WAV_CODEC_PARAM * IN     prWavParam    //! the structure of wavelet parameters
, WAV_CODEC_DATA *  IN OUT prWavData     //! the structure of wavelet data
, IMAGE_EX *        IN OUT prImage       //! to the array of the current image
, BITSTREAM *       IN OUT prBS          //! the structure of all buffers of bitstream
, BOOL              IN     bReconstruct  //! a flag if reconstruction is required or not
, QSP_SIZE *           OUT prQSP_Size    //! QSP size of output bitstream
)
{
// This function performs encoding I frame.
// For this it converts the frame from BYTE representation to short one,
//   performs encoding on the base of wavelet transform,
//   and converts the reconstructed image back to BYTE representation. 

  wci_image_convert_to_diffimage( prImage, &prWavData->rImageEx );

 // wci_wav_encode_diffimage( prWavParam, prWavData,
 //  &prWavData->rImageEx, prBS, bReconstruct, prQSP_Size );

  if ( bReconstruct )
  {
    wci_diffimage_convert_to_image( &prWavData->rImageEx, prImage );
  }
}

//------------------------------------------------------------------------------

short * wci_wav_encode_diffimage_component(
  const WAV_CODEC_PARAM * IN     prWavParam    //! the structure of wavelet parameters
, WAV_CODEC_DATA *        IN OUT prWavData     //! the structure of wavelet data
, short *                 IN OUT psImage       //! a pointer to the array of the current image
, SIZE                    IN     rBlockSize    //! a size of the image
, int                     IN     iWavSteps     //! a number of the steps of wavelet transform
, enumWT_Mode             IN     eMode         //! a coding mode
, const WAV_QUANT_SCALE * IN     prQS          //! a pointer to the quantizing structure
, BOOL                    IN     bReconstruct  //! a flag if reconstruction is required or not
, BOOL                    IN     bEncodeLL_Block //! a flag if encoding LL segment is required
, QSP_BUFFER *            IN     prQSP         //! a pointer to the buffer structure.
, QSP_SIZE *                 OUT prSizeQSP     //! QSP size of output bitstream
)
{
// This function performs iWavSteps steps of the wavelet transform.
// Return value is a pointer to the buffer containing wavelet coefficients.

  int    i;
  short *ptr_big = psImage;	
  short *ptr_small = ptr_big;

  assert( prSizeQSP != NULL );

  for ( i=0; i<iWavSteps; i++ )
  {
    ptr_small = ptr_big + (rBlockSize.cx * rBlockSize.cy);
    wci_lift_wav_block(ptr_big, 0, 0, rBlockSize.cx, rBlockSize.cy, rBlockSize.cx, ptr_small);

    wci_wav_encode_block(
      ptr_big, rBlockSize,
      prQS[i].iQuantizer_H, prQS[i].iQuantizer_HH,
      eMode, &prQSP->pbtP[prQSP->iP_Pos], prQSP, bReconstruct );

    // Store lengts of LH, HH, HL partitions
    {
      SIZE rPartitionSize = wci_wav_get_partition_block( rBlockSize );
      int  nP_Length = wci_get_square( rPartitionSize );

      prSizeQSP->aiP[LH]= prSizeQSP->aiP[HL]= prSizeQSP->aiP[HH]= nP_Length;
      prQSP->iP_Pos += 3*nP_Length;
    }

    prSizeQSP->iQ = prQSP->iQ_Pos;
    prSizeQSP->iS = prQSP->iS_Pos;
    prSizeQSP++;

    wci_down_block_size( eMode, &rBlockSize );
    ptr_big = ptr_small;
  }

  if ( bEncodeLL_Block )
  {
//    SIZE rPartitionSize = wci_adpcm_get_partition_block( rBlockSize );
    SIZE rPartitionSize = wci_adpcm_get_partition_block( rBlockSize );

		wci_wav_encodell_block( ptr_small, rBlockSize, prWavParam, prWavData->pbtPartition, prQSP );

    prSizeQSP->aiP[0] = wci_get_square( rPartitionSize );
    prSizeQSP->aiP[1] = prSizeQSP->aiP[2] = 0;

    prSizeQSP->iQ = prQSP->iQ_Pos;
    prSizeQSP->iS = prQSP->iS_Pos;
    prSizeQSP++;
  }

  return ptr_big;
}

//------------------------------------------------------------------------------


void wci_wav_encode_diffimage
(
  const WAV_CODEC_PARAM * IN     prWavParam    //! the structure of wavelet parameters
 , WAV_CODEC_DATA *        IN OUT prWavData     //! the structure of wavelet data
 , DIFF_IMAGE_EX *         IN OUT prImage       //! to the array of the current image
 , BITSTREAM *             IN OUT prBS          //! the structure of all buffers of bitstream
 , BOOL                    IN     bReconstruct  //! a flag if reconstruction is required or not
 , QSP_SIZE *                 OUT prQSP_Size    //! QSP size of output bitstream
 , QSP_BUFFER*             IN OUT prQSP
 , QSP_SIZE*                  OUT prBlockSizeQSP
 , int32_t*                   OUT piNumberOfBlocks
 )
{
  int           iLayer = 0;
  DIFF_IMAGE_EX rImage = *prImage;
  const WAV_LAYER * prWL;
  BOOL              bEncodeLL_Block = ( iLayer == 0 );
  int               iBlocks = 0;
  int               iComponent;

  wci_wav_put_header( prWavParam, prBS );
  wci_qsp_duplicate( &prWavData->rQSP, prQSP );

  for ( iComponent=0; iComponent< MAX_WAV_COMPONENTS -1; ++iComponent )
  {
    DIFF_IMAGE_COMPONENT * prC = &rImage.arComponent[ iComponent ];
    prWL = &prWavParam->arWL[iComponent][iLayer];

    prC->piData = wci_wav_encode_diffimage_component( prWavParam, prWavData,
      prC->piData, prWL->rBlockSize, prWL->iWavSteps, prWL->eMode,
      prWL->rQS, bReconstruct, bEncodeLL_Block, prQSP, &prBlockSizeQSP[iBlocks] );
    iBlocks += prWL->iWavSteps + (bEncodeLL_Block ? 1 : 0);
  }
  *piNumberOfBlocks = iBlocks;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_wav_decode_image(
  WAV_CODEC_PARAM *    OUT prWavParam         //! the structure of wavelet parameters
, WAV_CODEC_DATA *  IN OUT prWavData          //! the structure of wavelet data
, IMAGE_EX *        IN OUT prImage            //! to the array of the current image
, BITSTREAM *       IN OUT prBS               //! the structure of all buffers of bitstream
)
{
// The function decodes I-frame. 
// For this it calls the function of wavelet decoding and then converts
//   reconstructed image from short representation to BYTE one.
  int err;
  
  err = wci_wav_decode_diffimage( prWavParam, prWavData, &prWavData->rImageEx, prBS );
  if ( err != ERR_NONE )
  {
    return err;
  }

  wci_diffimage_convert_to_image( &prWavData->rImageEx, prImage );

  return ERR_NONE;
}

//------------------------------------------------------------------------------

short* wci_wav_decode_diffimage_component(
  WAV_CODEC_PARAM *prWavParam,
  WAV_CODEC_DATA *prWavData,
  short *psImage, SIZE rBlockSize,
  int iWavSteps, enumWT_Mode eMode, WAV_QUANT_SCALE *prQS,
  BOOL bDecodeLL_Block, QSP_BUFFER *prQSP )
{
  int i;
  short *ptr_big = psImage;	
  short *ptr_small = ptr_big;				

  for ( i=0; i<iWavSteps; i++ )
  {
    ptr_small = ptr_big + (rBlockSize.cx * rBlockSize.cy);
    memset( ptr_big, 0, (rBlockSize.cx * rBlockSize.cy)*sizeof(short));

    wci_wav_decode_block( ptr_big, rBlockSize, &prQS[i], eMode, prWavData->pbtPartition, prQSP );
    wci_down_block_size( eMode, &rBlockSize );	
    ptr_big = ptr_small;
  }

  if ( bDecodeLL_Block )
  {
    wci_wav_decodell_block( ptr_small, rBlockSize, prWavParam, prWavData->pbtPartition, prQSP );
  }
  
  return ptr_big;	
}

//------------------------------------------------------------------------------

ERROR_CODE wci_wav_decode_diffimage(
  WAV_CODEC_PARAM *    OUT prWavParam //! the structure of wavelet parameters
, WAV_CODEC_DATA *  IN OUT prWavData  //! the structure of wavelet data
, DIFF_IMAGE_EX *   IN OUT prImage    //! to the array of the current image
, BITSTREAM *       IN OUT prBS       //! the structure of all buffers of bitstream
)
{
// This function decodes the color difference image that was encoded by meanse of wavelet transform. 

  int             iLayer;
  WAV_CODEC_PARAM rWavParam;
  DIFF_IMAGE_EX   rImage = *prImage;
  
  if ( wci_wav_get_header( prBS, &rWavParam ) != 0 )
  {
    return ERR_FORMAT;
  }
  *prWavParam = rWavParam;

  for ( iLayer=prWavParam->iLayers-1; iLayer>=0; iLayer-- )
  {
    QSP_BUFFER  rQSP;
    BOOL        bEncodeLL_Block = ( iLayer == 0 );
    int         err;
    int         iComponent;

    // Decompress total layer data
    rQSP = prWavData->rQSP;
    err = wci_qsp_decompress_layer( &prWavData->rQSP_Compressor, prBS, &rQSP );
    if ( err != ERR_NONE )
    {
      return ERR_FORMAT;
    }
    rQSP.iQ_Length = rQSP.iQ_Pos;
    rQSP.iS_Length = rQSP.iS_Pos;
    rQSP.iP_Length = rQSP.iP_Pos;
    wci_qsp_resetpos( &rQSP );

    for ( iComponent=0; iComponent<prImage->rInfo.iComponents; ++iComponent )
    {
      DIFF_IMAGE_COMPONENT * prC = &rImage.arComponent[ iComponent ];
      WAV_LAYER *            prWL = &prWavParam->arWL[iComponent][iLayer];

      prC->piData = wci_wav_decode_diffimage_component( prWavParam, prWavData,
        prC->piData, prWL->rBlockSize, prWL->iWavSteps, prWL->eMode,
        prWL->rQS, bEncodeLL_Block, &rQSP );
    }

    assert( rQSP.iQ_Pos == rQSP.iQ_Length );
    assert( rQSP.iS_Pos == rQSP.iS_Length );
    assert( rQSP.iP_Pos == rQSP.iP_Length );

  } // for ( iLayer=

  wci_wav_inv_trans_image_ex( prWavParam, prImage );

  return ERR_NONE;
}

#endif //#ifdef ENCODER_SUPPORT

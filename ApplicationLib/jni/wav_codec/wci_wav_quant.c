/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: WavQuant.c

ABSTRACT:    This file contains procedures for quantization of wavelet coefficients

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00
HISTORY:     2004.08.27   v1.01 Quantization tables added

*******************************************************************************/

#include <string.h>

#include "../wci_global.h"
#include "../compress/wci_rle.h"
#include "wci_wav_block_quant.h"
#include "wci_wav_quant.h"
#include "wci_adpcm.h"

//------------------------------------------------------------------------------

//#define I_QUANTIZERS_ARE_EQUAL    

#define LL_FACTOR   130/100      // factor increasing q( HL(0) ) compared to q( LL )
#define FACTOR_LH_1 175/100      // factor increasing q( HL(1) ) compared to q( HL(0) )
#define FACTOR_LH_2 185/100      // factor increasing q( HL(2) ) compared to q( HL(1) )
#define FACTOR_LH_3 195/100      // factor increasing q( HL(3) ) compared to q( HL(2) )
#define NEXT_FACTOR 200/100      // factor increasing q( HL(i) ) compared to q( HL(i-1) )

#define AUGMENTATION_HH0      50 // variation from 0 to 100 causes change q( HH(i) )
#define AUGMENTATION_HH1      50 // variation from 0 to 100 causes change q( HH(i) )
#define AUGMENTATION_HH2      50 // variation from 0 to 100 causes change q( HH(i) )
#define AUGMENTATION_NEXT_HH  50 // variation from 0 to 100 causes change q( HH(i) ) 
                                 // from q( HL(i) ) to q( HL(i+1) ), where i is a wavelet step

#define QUANTIZER_TABLE_SIZE  ( MAX_QUALITY/QUALITY_STEP + 1 )

//------------------------------------------------------------------------------

#ifdef ENCODER_SUPPORT

  WAV_QUANTIZER g_arrWavQuantData[WAV_QUANTIZER_RANGE];

  int arrTabAbs[2*WAV_MAX_QUANT_VALUE+1], *g_pTabAbs = &arrTabAbs[WAV_MAX_QUANT_VALUE+1];
  int arrTabSqr[2*WAV_MAX_QUANT_VALUE+1], *g_pTabSqr = &arrTabSqr[WAV_MAX_QUANT_VALUE+1];

#endif

//------------------------------------------------------------------------------

extern void wci_wav_decode_block_lh_hl_hh(
  short *      IN OUT pshImage
, SIZE         IN     rBlockSize
, short *      IN     pshDeQuant_H
, short *      IN     pshDeQuant_HH
, BYTE *          OUT pbtPartition
, QSP_BUFFER * IN OUT prQSP
);

//------------------------------------------------------------------------------

int wci_wav_get_quantizer_by_index( 
  int IN iWavSteps
, int IN nIndex 
)
{
  const int aiQuantizerTable[MAX_WAV_STEPS+1][QUANTIZER_TABLE_SIZE] = 
  {
    { 100, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650 },    // for 0 steps
    { 100, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650 },    // for 1 steps
    { 100, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650 },    // for 2 steps    
    { 100, 200, 225, 250, 275, 300, 325, 350, 375, 400, 425 },    // for 3 steps
    { 100, 150, 175, 200, 225, 250, 275, 300, 325, 350, 375 }     // for 4 steps
  };

  assert( nIndex<QUANTIZER_TABLE_SIZE );
  assert( iWavSteps<=MAX_WAV_STEPS );

  return aiQuantizerTable[iWavSteps][nIndex];
}

//------------------------------------------------------------------------------

SIZE wci_wav_get_partition_block(
  SIZE IN rSize
)
{
  SIZE rBlockSize;
  rBlockSize.cx = 2*(rSize.cx / GROUP_SIZE_X);
  rBlockSize.cy = (rSize.cy / (GROUP_SIZE_Y*QUANT_BLOCK_SIZE));

  return rBlockSize;
}

//------------------------------------------------------------------------------

void wci_set_quant_scale(
  int             IN  iQuantizer         //! a given value of the quantizer parameter
, int             IN  iWavSteps          //! a given value of steps of wavelet transform
, WAV_QUANT_SCALE OUT rQS[MAX_WAV_STEPS] //! an array for quantizing wavelet coefficients
                                         //!   on every step of wavelet representation
)
{
// This function fills an array rQS[MAX_WAV_STEPS] for quantizing wavelet
//   coefficients on every step of wavelet representation.

 int ws;

#ifndef I_QUANTIZERS_ARE_EQUAL
  {
    WAV_QUANT_SCALE rIQS[MAX_WAV_STEPS];
    int iQuantizer_H = iQuantizer;
    int a = AUGMENTATION_NEXT_HH;

    for(ws=0; ws<iWavSteps; ws++)
    {
      rIQS[ws].iQuantizer_LL = iQuantizer/QUANTIZER_SCALE_FACTOR;
      iQuantizer *= 2;
    }

    for(ws=0; ws<iWavSteps; ws++)
    {
      switch( ws )                              
      {
      case 0:  iQuantizer_H =  iQuantizer_H*LL_FACTOR  ; break;
      case 1:  iQuantizer_H =  iQuantizer_H*FACTOR_LH_1; break;
      case 2:  iQuantizer_H =  iQuantizer_H*FACTOR_LH_2; break;
      case 3:  iQuantizer_H =  iQuantizer_H*FACTOR_LH_3; break;
      default: iQuantizer_H =  iQuantizer_H*NEXT_FACTOR; break;
      }
      rIQS[ws].iQuantizer_H = iQuantizer_H/QUANTIZER_SCALE_FACTOR;
    }

    for(ws=0; ws<iWavSteps-1; ws++)
    {
      switch( ws )                              
      {
      case 0: a = AUGMENTATION_HH0; break;
      case 1: a = AUGMENTATION_HH1; break;
      case 2: a = AUGMENTATION_HH2; break;
      default: a = AUGMENTATION_NEXT_HH; break;
      }
      rIQS[ws].iQuantizer_HH = ( (100-a) * rIQS[ws].iQuantizer_H + a * rIQS[ws+1].iQuantizer_H ) / 100;
    }
    rIQS[iWavSteps-1].iQuantizer_HH = rIQS[iWavSteps-1].iQuantizer_H*150 / 100;

    for(ws=0; ws<iWavSteps; ws++)
    {
      rQS[ws] = rIQS[iWavSteps - ws - 1];
    }
  }
#else

  for(ws=iWavSteps-1; ws>=0; ws--)
  {
    rQS[ws].iQuantizer_LL = iQuantizer/QUANTIZER_SCALE_FACTOR;
    rQS[ws].iQuantizer_H  = iQuantizer/QUANTIZER_SCALE_FACTOR;
    rQS[ws].iQuantizer_HH = iQuantizer/QUANTIZER_SCALE_FACTOR;

    iQuantizer *= 2;
  }
  
#endif

  for(ws=iWavSteps-1; ws>=0; ws--)
  {
    rQS[ws].iBlockDeadZone_LL = wci_get_dead_zone( rQS[ws].iQuantizer_LL );
    rQS[ws].iBlockDeadZone_H  = wci_get_dead_zone( rQS[ws].iQuantizer_H );
    rQS[ws].iBlockDeadZone_HH = wci_get_dead_zone( rQS[ws].iQuantizer_HH );

  #ifdef ENCODER_SUPPORT
    rQS[ws].prQV_LL           = wci_get_quant_value( rQS[ws].iQuantizer_LL );
    rQS[ws].prQV_H            = wci_get_quant_value( rQS[ws].iQuantizer_H );
    rQS[ws].prQV_HH           = wci_get_quant_value( rQS[ws].iQuantizer_HH );
  #endif
  }
 
}

//------------------------------------------------------------------------------

#ifdef ENCODER_SUPPORT

void wci_init_quant_value(
  int           IN  iQuantizer //! a given value of the quantizing parameter
, int           IN  iDeadZone  //! a given value of the dead zone
, QUANT_VALUE * OUT prQV       //! a pointer to array of structured values for quantizing/dequantizing
)
{
// This function fills the array of structured values for quantizing/dequantizing
//   wavelet coefficients correspondingly to the given values iQuantizer and iDeadZone.

  int i, j, Low,Up;

  i=0;

  while( i < iDeadZone )
  {
    prQV[i].btQuant   = 0;
    prQV[i].shDeQuant = 0;
    prQV[i].btSign    = 0;
    i++;
  }

  Low = iDeadZone;
  Up  = Low + iQuantizer;
  j   = 1;

  while(i<WAV_MAX_QUANT_VALUE)
  {
    if(i>=Low && i<Up)
    {
      prQV[i].btQuant   = (BYTE)j;
      prQV[i].btSign    = 0;
      prQV[i].shDeQuant = (short)BOUND((Up+Low)>>1,0,WAV_MAX_QUANT_VALUE);
    }
    else
    {
      if(j<255)
      {
        j++;
        Low = Up;
        Up  = Low + iQuantizer;
      }
      prQV[i].btQuant   = (BYTE)j;
      prQV[i].btSign    = 0;
      prQV[i].shDeQuant = (short)BOUND((Up+Low)>>1,0,WAV_MAX_QUANT_VALUE);
    }

    i++;
  }

  for( i=-WAV_MAX_QUANT_VALUE; i<0; i++)
  {
    prQV[i].btQuant   = prQV[-i].btQuant;
    prQV[i].btSign    = prQV[i].btQuant != 0 ? 1 : 0;
    prQV[i].shDeQuant = -prQV[-i].shDeQuant;
  }
}

//------------------------------------------------------------------------------

void wci_init_tab_abs( void )
{
// This function fills the global arrays int arrTabAbs[] and int arrTabSqr[]
//for table functions 'abs' and 'sqr', correspondingly.
  int x;
  int *ptrPos, *ptrNeg;

  *g_pTabAbs = 0;
  ptrPos = g_pTabAbs + 1; ptrNeg = g_pTabAbs - 1;
  for( x=1; x<WAV_MAX_QUANT_VALUE; x++)
  {
      *ptrPos++ = x;
      *ptrNeg-- = x;
  }

  *g_pTabSqr = 0;
  ptrPos = g_pTabSqr + 1; ptrNeg = g_pTabSqr - 1;
  for( x=1; x<WAV_MAX_QUANT_VALUE; x++)
  {
    *ptrPos++ = x*x;
    *ptrNeg-- = x*x;
  }
}

#endif // #ifdef ENCODER_SUPPORT

//------------------------------------------------------------------------------

void wci_init_dequantizer(
  int             IN  iQuantizer //! a given value of the quantizing parameter
, int             IN  iDeadZone  //! a given value of the dead zone
, WAV_QUANTIZER * OUT prQuant    //! a pointer to the structure for quantizing/dequantizing
)
{
// This function fills the 'shDeQuant' array for dequantizing wavelet coefficients
// This 'shDeQuant' array is a member of the structure WAV_QUANTIZER *prQuant.

  int i = iDeadZone + 1;;

  prQuant->shDeQuant[0] = 0;
  
  while(i<WAV_MAX_QUANT_VALUE)
  {
    BYTE btQuant   = (BYTE)BOUND( i / iQuantizer, 0, 255 );
    prQuant->shDeQuant[ btQuant ] = (short)(btQuant*iQuantizer);
    i++;
  }
}

//------------------------------------------------------------------------------

void wci_init_wav_quant_data( void )
{
// This function performs a series of initializations for proper
//  quantizing/dequantizing wavelet coefficients. The subroutines of this function
//  fill the global arrays:
//- WAV_QUANTIZER g_arrWavQuantData[WAV_QUANTIZER_RANGE];
//- int arrTabAbs[2*WAV_MAX_QUANT_VALUE+1];
//- int arrTabSqr[2*WAV_MAX_QUANT_VALUE+1];

#ifdef ENCODER_SUPPORT
  int i;
  int iQuantizer = WAV_MIN_QUANTIZER;
  
  for ( i=0; i<WAV_QUANTIZER_RANGE; i++ )
  {
    WAV_QUANTIZER *prQuant = &g_arrWavQuantData[i];
    int iDeadZone = (iQuantizer+1)/2;
     
    wci_init_quant_value( iQuantizer, iDeadZone,
                    & prQuant->arrQuantValue[WAV_MAX_QUANT_VALUE] );
   
    prQuant->iQuantizer = iQuantizer;
    wci_init_dequantizer( iQuantizer, iDeadZone, prQuant );
    
    iQuantizer++;
  }

  wci_init_tab_abs();
#endif

}

#ifdef ENCODER_SUPPORT

//------------------------------------------------------------------------------

void wci_wav_decode_block(
  short *           IN     pshImage
, SIZE              IN     rBlockSize
, WAV_QUANT_SCALE * IN     prQS
, enumWT_Mode       IN     eMode
, BYTE *               OUT pbtPartition
, QSP_BUFFER *      IN OUT prQSP )
{
// This function is only an envelope for calling the function DeQuant_and_DeCode_HL_LH_HH()
// from the function WAV_EncodeDiffImageComponent().
  WAV_QUANTIZER * prQuant_H = &g_arrWavQuantData[prQS->iQuantizer_H-WAV_MIN_QUANTIZER];
  WAV_QUANTIZER * prQuant_HH = &g_arrWavQuantData[prQS->iQuantizer_HH - WAV_MIN_QUANTIZER];
    
  switch( eMode )
  {
  case ewtHV:
    wci_wav_decode_block_lh_hl_hh( pshImage, rBlockSize, prQuant_H->shDeQuant, prQuant_HH->shDeQuant
                            , pbtPartition, prQSP );
    break;

  default:
    assert( FALSE );
  }
}

//------------------------------------------------------------------------------

void wci_wav_encode_block(
  short *psImage, SIZE rBlockSize,
  int iQuantizer_H, int iQuantizer_HH, enumWT_Mode eMode,   
  BYTE * pbtPartition, QSP_BUFFER *prQSP, BOOL bReconstruct )
// This function is only an envelope for calling the function wci_wav_hl_lh_hh_quant()
// from the function WAV_EncodeDiffImageComponent().
// See detailed description of the wci_wav_hl_lh_hh_quant() function.
{
  switch( eMode )
  {
    case ewtHV:
      wci_wav_hl_lh_hh_quant( psImage, rBlockSize, iQuantizer_H, iQuantizer_HH,   
                        bReconstruct, pbtPartition, prQSP );
    break;
    
    case ewtH:
    case ewtV:
    default:
      assert( FALSE );
  }
}

//------------------------------------------------------------------------------

void wci_wav_encodell_block(
  short *                 IN OUT psImage
, SIZE                    IN     rBlockSize
, const WAV_CODEC_PARAM * IN     prWavParam
, BYTE *                  OUT    pbtPartition
, QSP_BUFFER *            IN OUT prQSP
)
{
// This function is only an envelope for calling the function wci_adpcm_encode()
// for quantizing LL coefficients
  WAV_QUANTIZER * prQuant = &g_arrWavQuantData[prWavParam->iQuantizer/QUANTIZER_SCALE_FACTOR-WAV_MIN_QUANTIZER];
  QUANT_VALUE *   prQV = &prQuant->arrQuantValue[WAV_MAX_QUANT_VALUE];
  pbtPartition= pbtPartition;

  switch(prWavParam->iQuantMode)
  {
    default:
    case eemDPCM:
      wci_adpcm_encode( eDPCM, psImage, rBlockSize, prQV, prQSP );
      break;
    case eemLPC_DPCM:
      wci_adpcm_encode( eLPC_DPCM, psImage, rBlockSize, prQV, prQSP );
      break;
  }
}

//------------------------------------------------------------------------------

void wci_wav_decodell_block(
  short *                 IN OUT psImage
, SIZE                    IN     rBlockSize
, const WAV_CODEC_PARAM * IN OUT prWavParam
, BYTE *                     OUT pbtPartition
, QSP_BUFFER *            IN OUT prQSP
)
{
// This function is only an envelope for calling the function wci_adpcm_decode()
// for dequantizing LL coefficients.

  WAV_QUANTIZER * prQuant = &g_arrWavQuantData[prWavParam->iQuantizer/QUANTIZER_SCALE_FACTOR-WAV_MIN_QUANTIZER];
  pbtPartition= pbtPartition;

  switch(prWavParam->iQuantMode)
  {
  default:
  case eemDPCM:
  
    #ifdef ADPCM_DECODE_TAB
      wci_adpcm_decode( eDPCM, prQSP, rBlockSize, prQuant->shDeQuant, psImage );
    #else
      wci_adpcm_decode( eDPCM, prQSP, rBlockSize, prQuant->iQuantizer, psImage );
    #endif
      
    break;
    
  case eemLPC_DPCM:
  
    #ifdef ADPCM_DECODE_TAB
      wci_adpcm_decode( eLPC_DPCM, prQSP, rBlockSize, prQuant->shDeQuant, psImage );
    #else
      wci_adpcm_decode( eLPC_DPCM, prQSP, rBlockSize, prQuant->iQuantizer, psImage );
    #endif
      
    break;

  case eemLinearQuant:
    assert( FALSE );
    break;
  }
}

#endif // #ifdef ENCODER_SUPPORT


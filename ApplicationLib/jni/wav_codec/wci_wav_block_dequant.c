/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: WavBlockDequant.c

ABSTRACT:    This file contains procedures for dequantization of wavelet coefficients

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include "../wci_global.h"
#include "../compress/wci_rle.h"
#include "wci_wav_quant.h"

//------------------------------------------------------------------------------

#define DEQUANT_C_WS(pshDeQuant, dst)                       \
  if((tmpq = *pbtQ++) != 0)                                 \
    dst =(short)((1-(*pbtS++)*2) * pshDeQuant[tmpq])


#define DEQUANT_FOUR_COEFF_WS(pshDeQuant, cur)              \
  DEQUANT_C_WS(pshDeQuant, cur[0]);                         \
  DEQUANT_C_WS(pshDeQuant, cur[1]);                         \
  DEQUANT_C_WS(pshDeQuant, cur[2]);                         \
  DEQUANT_C_WS(pshDeQuant, cur[3])

#define ASK_AND_DEDO_WS(pshDeQuant, ptrDes, cur)            \
  if(*ptrDes++ != 0)                                        \
  {                                                         \
    DEQUANT_C_WS(pshDeQuant, cur[0]);                       \
    DEQUANT_C_WS(pshDeQuant, cur[1]);                       \
    DEQUANT_C_WS(pshDeQuant, cur[bot11]);                   \
    DEQUANT_C_WS(pshDeQuant, cur[bot12]);                   \
  }

//------------------------------------------------------------------------------

static void __inline wci_dequant_lh_hl_hh(
short *       IN OUT pshImage
, SIZE          IN     rBlockSize
, const short * IN     pshDeQuant_H
, const short * IN     pshDeQuant_HH
, const BYTE *  IN     pbtPartition
, QSP_BUFFER *  IN OUT prQSP )
{
  SIZE         rP_Size = wci_wav_get_partition_block( rBlockSize );
  int          nGroupsInRow = rP_Size.cx/2;

  const BYTE * pbtP_LH = pbtPartition;
  const BYTE * pbtP_HL = pbtP_LH + wci_get_square( rP_Size );
  const BYTE * pbtP_HH = pbtP_HL + wci_get_square( rP_Size );

  const BYTE * pbtQ = &prQSP->pbtQ[prQSP->iQ_Pos];
  const BYTE * pbtS = &prQSP->pbtS[prQSP->iS_Pos];

  const int    nRest_x = (rBlockSize.cx % GROUP_SIZE_X);
  const int    nRest_y = (rBlockSize.cy % (GROUP_SIZE_Y*QUANT_BLOCK_SIZE));

  short *   ptr_start = pshImage + GROUP_HALF_SIZE_X;   
  short *   cur;
  short *   bot;

  const int shiftToHL = rBlockSize.cx - GROUP_HALF_SIZE_X;
  const int shiftToHH = rBlockSize.cx;
  const int restToHL  = rBlockSize.cx - nRest_x/2;

  const int bot11 = 2*rBlockSize.cx; 
  const int bot12 = bot11 + 1;

  const int shift2rows  = bot11 + rBlockSize.cx;

  int       i,j,k,tmpq;

  for(i=0; i<rP_Size.cy; i++)
  {
    for(j=0;j<nGroupsInRow;j++)
    {
      cur = ptr_start;           
      ASK_AND_DEDO_WS(pshDeQuant_H, pbtP_LH, cur);
      cur += QUANT_BLOCK_SIZE; 
      ASK_AND_DEDO_WS(pshDeQuant_H, pbtP_LH, cur);

      cur = ptr_start + shiftToHL;           
      ASK_AND_DEDO_WS(pshDeQuant_H, pbtP_HL, cur);
      cur += QUANT_BLOCK_SIZE; 
      ASK_AND_DEDO_WS(pshDeQuant_H, pbtP_HL, cur);

      cur = ptr_start + shiftToHH;           
      ASK_AND_DEDO_WS(pshDeQuant_HH, pbtP_HH, cur);
      cur += QUANT_BLOCK_SIZE; 
      ASK_AND_DEDO_WS(pshDeQuant_HH, pbtP_HH, cur);

      ptr_start += GROUP_SIZE_X;
    }

    if(nRest_x)  
    {
      ptr_start -= (GROUP_SIZE_X - nRest_x)/2;

      for(k=0; k < nRest_x/2; k++)
      {
        cur = ptr_start;                 // LH
        bot = cur + bot11;    
        DEQUANT_C_WS(pshDeQuant_H, *cur);
        DEQUANT_C_WS(pshDeQuant_H, *bot);
        
        cur = ptr_start + restToHL;      // HL
        bot = cur + bot11;        
        DEQUANT_C_WS(pshDeQuant_H, *cur);
        DEQUANT_C_WS(pshDeQuant_H, *bot);
        
        cur = ptr_start + shiftToHH;     // HH
        bot = cur + bot11;
        DEQUANT_C_WS(pshDeQuant_HH, *cur);
        DEQUANT_C_WS(pshDeQuant_HH, *bot);
        
        ptr_start++;
      }  
      ptr_start += GROUP_HALF_SIZE_X;       // shift from LL to LH
    }
    
    ptr_start += shift2rows;
  }

  if(nRest_y)             // nRest_y can be equal to 2, 0
  {
    assert( ptr_start == pshImage + (rBlockSize.cy-GROUP_SIZE_Y)*rBlockSize.cx + GROUP_HALF_SIZE_X );
    
    for(j=0;j<nGroupsInRow;j++)
    {
      cur = ptr_start;                 // LH
      DEQUANT_FOUR_COEFF_WS(pshDeQuant_H, cur);

      cur = ptr_start + shiftToHL;     // HL
      DEQUANT_FOUR_COEFF_WS(pshDeQuant_H, cur);

      cur = ptr_start + shiftToHH;     // HH
      DEQUANT_FOUR_COEFF_WS(pshDeQuant_HH, cur);

      ptr_start += GROUP_SIZE_X;
    }

    if(nRest_x)
    {
      ptr_start -= (GROUP_SIZE_X - nRest_x)/2;

      for(j=0; j < nRest_x/2; j++)
      {
        cur = ptr_start;  
        DEQUANT_C_WS(pshDeQuant_H, *cur);
      
        cur = ptr_start + restToHL;
        DEQUANT_C_WS(pshDeQuant_H, *cur);
      
        cur = ptr_start + shiftToHH;
        DEQUANT_C_WS(pshDeQuant_H, *cur);
      }  
    }
  }

  prQSP->iQ_Pos += pbtQ - &prQSP->pbtQ[prQSP->iQ_Pos];
  prQSP->iS_Pos += pbtS - &prQSP->pbtS[prQSP->iS_Pos];
}

//------------------------------------------------------------------------------

void wci_wav_decode_block_lh_hl_hh(
  short *      IN OUT pshImage
, SIZE         IN     rBlockSize
, short *      IN     pshDeQuant_H
, short *      IN     pshDeQuant_HH
, BYTE *          OUT pbtPartition
, QSP_BUFFER * IN OUT prQSP
)
{
  SIZE rSize = wci_wav_get_partition_block( rBlockSize );
  int  iSize = wci_get_square( rSize );
  
  // Decode partition for LH
  prQSP->iP_Pos += wci_rle_decompress_bits( &prQSP->pbtP[prQSP->iP_Pos], iSize, pbtPartition );

  // Decode partition for HL
  prQSP->iP_Pos += wci_rle_decompress_bits( &prQSP->pbtP[prQSP->iP_Pos], iSize, pbtPartition + iSize );

  // Decode partition for HH
  prQSP->iP_Pos += wci_rle_decompress_bits( &prQSP->pbtP[prQSP->iP_Pos], iSize, pbtPartition + 2*iSize );

  // DeQuant HL_LH_HH coeff
  wci_dequant_lh_hl_hh(  pshImage, rBlockSize, pshDeQuant_H, pshDeQuant_HH, pbtPartition, prQSP );
}

/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: WavBlockQuant.c

ABSTRACT:    This file contains procedures for quantization of wavelet coefficients

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifdef ENCODER_SUPPORT

#include <math.h>

#include "../compress/wci_rle.h"
#include "wci_wav_quant.h"
#include "wci_wav_block_quant.h"

//------------------------------------------------------------------------------

// MacroDefinition for quantizing, persuming inverse transform,
// without counters of quant values and signs
// Fast sign processing
#define QUANT_REC_WS(prQV, rec)			\
  Qstr = prQV[rec];						      \
  rec = Qstr.shDeQuant;					    \
  *ptr_q++ = Qstr.btQuant;				  \
  *ptr_s = Qstr.btSign;					    \
  ptr_s += (Qstr.btQuant != 0);

// MacroDefinition for quantizing, not persuming inverse transform,
// without counters of quant values and signs
// Fast sign processing
#define QUANT_V_WS(prQV, rec)\
  Qstr = prQV[rec];          \
  *ptr_q++ = Qstr.btQuant;   \
  *ptr_s = Qstr.btSign;			 \
  ptr_s += (Qstr.btQuant != 0);

// MacroDefinition for quantizing, persuming inverse transform,
// with counters of signs
#define QUANT_REC(prQV, rec) \
  Qstr = prQV[rec];          \
  rec = Qstr.shDeQuant;      \
  *ptr_q++ = Qstr.btQuant;   \
  if(Qstr.btQuant)           \
  {                          \
    *ptr_s++ = Qstr.btSign;  \
    (LenS)++;                \
  }

// MacroDefinition for quantizing, not persuming inverse transform,
// with counters of signs
#define QUANT_V(prQV, rec)	  	\
  *ptr_q++ = prQV[rec].btQuant;	\
  if(prQV[rec].btQuant)			\
  {           					\
    *ptr_s++ = prQV[rec].btSign;\
    (LenS)++;	        		\
  }

// MacroDefinition for calculating sum of differencies,
// by meanse of either function abs() or a table function tab[]
// that can be either function of absolute value or function of squared value.
#ifdef TAB_ABS
	#define SUM_ABS_BLOCK(sum, cur, bot)	 sum = tab[*cur]+ tab[*(cur+1)] + tab[*bot]+ tab[*(bot+1)]; 
#else
	#define SUM_ABS_BLOCK(sum, cur, bot)	 sum = abs(*cur)+ abs(*(cur+1))+ abs(*bot)+ abs(*(bot+1)); 
#endif

// MacroDefinition for checking condition of partition,
// and quantizing four coefficients,
// persuming inverse transform,
// without counters of quant values and signs.
#define CHECK_AND_DO_REC_PSPQ_WS(sum, dz, prQV, cur, bot, ptrDes)	\
  if(sum>dz)														\
  {																	\
    *ptrDes = 1;													\
    QUANT_REC_WS(prQV, *cur) cur++;  QUANT_REC_WS(prQV, *cur) cur++;\
    QUANT_REC_WS(prQV, *bot) bot++;  QUANT_REC_WS(prQV, *bot) bot++;\
  }																	\
  else																\
  {																	\
    memset(cur, 0, 2*sizeof(short));	cur+=2;						\
    memset(bot, 0, 2*sizeof(short));	bot+=2;						\
  }																	\
  ptrDes++;

// MacroDefinition for checking condition of partition,
// and quantizing four coefficients. 
// not persuming inverse transform,
// without counters of quant values and signs.
#define CHECK_AND_DO_V_PSPQ_WS(sum, dz, prQV, cur, bot, ptrDes)	\
  if(sum>dz)													\
  {																\
    *ptrDes = 1;												\
    QUANT_V_WS(prQV, *cur) cur++;  QUANT_V_WS(prQV, *cur) cur++;\
    QUANT_V_WS(prQV, *bot) bot++;  QUANT_V_WS(prQV, *bot) bot++;\
  }																\
  else														\
  {																\
    cur+=2;												\
    bot+=2;												\
  }																\
  ptrDes++;

// MacroDefinition for checking condition of partition,
// and quantizing four coefficients. 
// persuming inverse transform,
// with counters of quant values and signs.
#define CHECK_AND_DO_REC(sum, dz, prQV, cur, bot, ptrDes)	\
  if(sum>dz)												                      \
  {															                          \
    *ptrDes = 1;											                    \
    QUANT_REC(prQV, *cur)		QUANT_REC(prQV, *(cur+1))	    \
    QUANT_REC(prQV, *bot)	    QUANT_REC(prQV, *(bot+1))	  \
	LenQ+=4;												                        \
  }															                          \
  else														                        \
  {															                          \
    memset(cur, 0, 2*sizeof(short));						          \
    memset(bot, 0, 2*sizeof(short));						          \
  }															                          \
  ptrDes++;

// MacroDefinition for quantizing four coefficients. 
// not persuming inverse transform,
// without counters of quant values and signs.
#define QUANT_V_4C_WS(prQV, cur)							\
    QUANT_V_WS(prQV, *cur)		QUANT_V_WS(prQV, *(cur+1))	\
    QUANT_V_WS(prQV, *(cur+2))	QUANT_V_WS(prQV, *(cur+3))	

// MacroDefinition for quantizing four coefficients. 
// persuming inverse transform,
// without counters of quant values and signs.
#define QUANT_REC_4C_WS(prQV, cur)								\
    QUANT_REC_WS(prQV, *cur)		  QUANT_REC_WS(prQV, *(cur+1))	\
    QUANT_REC_WS(prQV, *(cur+2))	QUANT_REC_WS(prQV, *(cur+3))

//------------------------------------------------------------------------------

void wci_quant_rec_lh_hl_hh(
  short *img, int Sx, int Sy, QUANT_VALUE* prQV_H, QUANT_VALUE* prQV_HH, 
  int DeadZ_H,  int DeadZ_HH,   
  BYTE *DesArr, BYTE *Buf_q, int *len_q, BYTE *Buf_s, int *len_s)
{  
// Below is not self-contained function 
// but only a part of body of wci_wav_hl_lh_hh_quant() function
// See detailed description of this function

  int     nGroupsInRow = (Sx / GROUP_SIZE_X);
  int     nBlocksInCol = (Sy / (GROUP_SIZE_Y*QUANT_BLOCK_SIZE));

  int     lenDes = 2*nGroupsInRow*nBlocksInCol;

  BYTE *  ptrDesUR = DesArr;
  BYTE *  ptrDesBL = DesArr + lenDes;
  BYTE *  ptrDesHH = DesArr + 2*lenDes;

  short * cur, * bot;
  int     res_x = (Sx % GROUP_SIZE_X);
  int     res_y = (Sy % (GROUP_SIZE_Y*QUANT_BLOCK_SIZE));

  short * ptr_start = img + GROUP_HALF_SIZE_X; // shift from LL to LH
  
  const int shiftToBL = Sx - GROUP_HALF_SIZE_X;   
  const int shiftToHH = Sx;                   
  const int bot11     = Sx+Sx; 

  const int shift2rows = 3*Sx;

  int   sum;  
  int   i,j,k;
  BYTE *ptr_q = Buf_q, *ptr_s = Buf_s;
  QUANT_VALUE Qstr;

  #ifdef SQR_APPROXIMATION
    int *tab = g_pTabSqr;  
  #else
    int *tab = g_pTabAbs;  
  #endif

  for(i=0;i<nBlocksInCol;i++)
  {
    for(j=0;j<nGroupsInRow;j++)
    {
      cur = ptr_start;                  // LH
      bot = cur + bot11;    
      SUM_ABS_BLOCK(sum, cur, bot)
      CHECK_AND_DO_REC_PSPQ_WS(sum, DeadZ_H, prQV_H, cur, bot, ptrDesUR)
      SUM_ABS_BLOCK(sum, cur, bot)                      
      CHECK_AND_DO_REC_PSPQ_WS(sum, DeadZ_H, prQV_H, cur, bot, ptrDesUR)    

      cur = ptr_start + shiftToBL;      // HL
      bot = cur + bot11;        
      SUM_ABS_BLOCK(sum, cur, bot)  
      CHECK_AND_DO_REC_PSPQ_WS(sum, DeadZ_H, prQV_H, cur, bot, ptrDesBL)
      SUM_ABS_BLOCK(sum, cur, bot)
      CHECK_AND_DO_REC_PSPQ_WS(sum, DeadZ_H, prQV_H, cur, bot, ptrDesBL)

      cur = ptr_start + shiftToHH;      //  HH
      bot = cur + bot11;
      SUM_ABS_BLOCK(sum, cur, bot)  
      CHECK_AND_DO_REC_PSPQ_WS(sum, DeadZ_HH, prQV_HH, cur, bot, ptrDesHH)  
      SUM_ABS_BLOCK(sum, cur, bot)  
      CHECK_AND_DO_REC_PSPQ_WS(sum, DeadZ_HH, prQV_HH, cur, bot, ptrDesHH)

      ptr_start += GROUP_SIZE_X;       // shift from LL to LH
    }

    if(res_x)
    {
      ptr_start -= ((GROUP_SIZE_X - res_x)/2);

      for(k=0; k < res_x/2; k++)
      {
        cur = ptr_start;                       // LH
        bot = cur + bot11;    
        QUANT_REC_WS(prQV_H, *cur);
        QUANT_REC_WS(prQV_H, *bot);
        
        cur = ptr_start + Sx - res_x/2;        // HL
        bot = cur + bot11;        
        QUANT_REC_WS(prQV_H, *cur);
        QUANT_REC_WS(prQV_H, *bot);
        
        cur = ptr_start + Sx;                  // HH
        bot = cur + bot11;
        QUANT_REC_WS(prQV_HH, *cur);
        QUANT_REC_WS(prQV_HH, *bot);
        
        ptr_start++;
      }  

      ptr_start += GROUP_HALF_SIZE_X;
    }
          
    ptr_start += shift2rows;
  }
                    
  if(res_y)             // res_y can be equal to 2, 0
  {
    assert( ptr_start == img + (Sy-GROUP_SIZE_Y)*Sx + GROUP_HALF_SIZE_X);
    
    for(j=0;j<nGroupsInRow;j++)
    {
      cur = ptr_start;              // LH
      QUANT_REC_4C_WS(prQV_H, cur);

      cur = ptr_start + shiftToBL;  // HL
      QUANT_REC_4C_WS(prQV_H, cur);

      cur = ptr_start + shiftToHH;  // HH
      QUANT_REC_4C_WS(prQV_HH, cur);

      ptr_start += GROUP_SIZE_X;
    }

    if(res_x)
    {
      ptr_start -= ((GROUP_SIZE_X - res_x)/2);

      for(k=0; k < res_x/2; k++)
      {
        cur = ptr_start;  
        QUANT_REC_WS(prQV_H, *cur);
        cur = ptr_start + Sx - res_x/2;
        QUANT_REC_WS(prQV_H, *cur);
        cur = ptr_start + shiftToHH;
        QUANT_REC_WS(prQV_HH, *cur);
        ptr_start++;
      }  
    }
  }

  *len_q += (ptr_q - Buf_q);
  *len_s += (ptr_s - Buf_s);
}

//------------------------------------------------------------------------------

void wci_wav_hl_lh_hh_quant(                        
  short *ptr_big           //! a pointer to the array of coefficients,                      
, SIZE rBlockSize          //! a size of the array,                      
, int iQuantizer_H         //! a quantizing parameter for HL and LH coefficients,
, int iQuantizer_HH        //! a quantizing parameter for HH coefficients,
, BOOL bReconstruct        //! a flag of required inverse reconstruction, 
, BYTE * pbtPartition      //! an array of decisions about 2x2 blocks (zero/non-zero), 
, QSP_BUFFER *prQSP )      //! a buffer structure for quantized coefficients and their signs.
{
/*
This function performs quantization of a rectangular array of coefficients after Wavelet transform.
The main features of this function of the family of used quantizing functions are as follows:
- it processes simultaneously 3 types of coefficients (HL, LH, and HH) that are unsorted,
- the array can be arbitrary size,
- it estimates whether a 2x2 block is near to zero and sets it to zero but other case quantizes its members,
- it uses both SAD and MSA approximation for above mentioned estimation,
- it uses its own procedures RL_Des_EncV() and RL_Des_Enc() for compression the dicision about this approximation
- it uses a combined structure for quantization/dequantization,
- it uses a combined structure for q, s, p buffers.
*/
  SIZE            rPartitionSize = wci_wav_get_partition_block( rBlockSize );
  int             nP_Length = wci_get_square( rPartitionSize );
  QSP_BUFFER      rQSP;
  WAV_QUANTIZER * prQuant_H = &g_arrWavQuantData[iQuantizer_H - WAV_MIN_QUANTIZER]; 
  QUANT_VALUE *   prQV_H = &prQuant_H->arrQuantValue[WAV_MAX_QUANT_VALUE]; 
  WAV_QUANTIZER * prQuant_HH = &g_arrWavQuantData[iQuantizer_HH - WAV_MIN_QUANTIZER]; 
  QUANT_VALUE *   prQV_HH = &prQuant_HH->arrQuantValue[WAV_MAX_QUANT_VALUE]; 

  #ifdef SQR_APPROXIMATION
    int iBlockDeadZone_H  = (iQuantizer_H * iQuantizer_H)* (QUANT_BLOCK_SIZE*QUANT_BLOCK_SIZE/2)*SQR_APPR_FOR_I;
    int iBlockDeadZone_HH = (iQuantizer_HH * iQuantizer_HH) * (QUANT_BLOCK_SIZE*QUANT_BLOCK_SIZE/2)*SQR_APPR_FOR_I;
  #else
    int iBlockDeadZone_H  = iQuantizer_H*QUANT_BLOCK_SIZE*QUANT_BLOCK_SIZE/2; 
    int iBlockDeadZone_HH = iQuantizer_HH*QUANT_BLOCK_SIZE*QUANT_BLOCK_SIZE/2; 
  #endif

  memset( pbtPartition, 0, 3*nP_Length);

  wci_qsp_duplicate( prQSP, &rQSP );

  if (bReconstruct)
  {
    wci_quant_rec_lh_hl_hh(ptr_big, rBlockSize.cx, rBlockSize.cy, 
        prQV_H, prQV_HH, iBlockDeadZone_H, iBlockDeadZone_HH, pbtPartition,
        &prQSP->pbtQ[prQSP->iQ_Pos], &rQSP.iQ_Pos,
        &prQSP->pbtS[prQSP->iS_Pos], &rQSP.iS_Pos );
  }
  else
  {
    //Quant_LH_HL_HH();           
  }
  prQSP->iQ_Pos += rQSP.iQ_Pos;
  prQSP->iS_Pos += rQSP.iS_Pos;
 }

#endif // #ifdef ENCODER_SUPPORT

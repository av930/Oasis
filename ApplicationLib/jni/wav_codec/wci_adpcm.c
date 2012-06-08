/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: ADPCM.c

ABSTRACT:    This file contains procedures for ADPCM codec

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include <string.h>

#include "../wci_global.h"
#include "../utils/wci_mem_align.h"
#include "wci_wav_quant.h"
#include "wci_adpcm.h"

//------------------------------------------------------------------------------

#define DPCM_PARTITION
#define DPCM_MEAN

//#define DIFF_SUM // for debug only

#define DPCM_MIN_MEAN     (-7)
#define DPCM_MAX_MEAN     7
#define DPCM_MEAN_MASK    0xF

#define DPCM_BLOCK_SIZE_X 16
#define DPCM_BLOCK_SIZE_Y 16

#define DPCM_MAX_X_BLOCKS (MAX_IMAGE_WIDTH /DPCM_BLOCK_SIZE_X)
#define DPCM_MAX_Y_BLOCKS (MAX_IMAGE_HEIGHT/DPCM_BLOCK_SIZE_Y)

//------------------------------------------------------------------------------

const SIZE c_rLPC_Blocks = { DPCM_BLOCK_SIZE_X, DPCM_BLOCK_SIZE_Y };

//------------------------------------------------------------------------------

SIZE wci_dpcm_get_blocks(
  SIZE          IN     rSize
)
{
  SIZE rBlocks;
  rBlocks.cx = CELLS( rSize.cx, DPCM_BLOCK_SIZE_X );
  rBlocks.cy = CELLS( rSize.cy, DPCM_BLOCK_SIZE_Y );
  return rBlocks;
}

//------------------------------------------------------------------------------



#ifndef DPCM_MEAN

void wci_dpcm_encode
(
  short *       IN OUT pshLL
, SIZE          IN     rSize
, QUANT_VALUE * IN     prQV
, QSP_BUFFER *  OUT    prQSP
)
{
  int   iPredict = 128;
  int   i, j, iDiff;
  BYTE  btQ;
  BOOL  bSign;

#ifdef DIFF_SUM
  int  iDiffSum = 0, iDiffSqSum = 0;
#endif

  for(i=0;i<rSize.cy;i++)
  {
    if(i)
      iPredict=*(pshLL-rSize.cx);

    for(j=0;j<rSize.cx;j++)
    {
      if(j)
        iPredict=*(pshLL-1);

      iDiff = (short)(*pshLL-iPredict);
      #ifdef DIFF_SUM
        iDiffSum += iDiff;
        iDiffSqSum += iDiff*iDiff;
      #endif

      if(iDiff >= 0)
      {
        btQ=prQV[iDiff].btQuant;
        *pshLL = (short)(iPredict + prQV[iDiff].shDeQuant);
        bSign=0;
      }
      else
      {
        btQ=prQV[-iDiff].btQuant;
        *pshLL = (short)(iPredict-prQV[-iDiff].shDeQuant);
        bSign=1;
      }

    #ifdef DPCM_PARTITION
      if(btQ!=0)
      {
        prQSP->pbtP[ prQSP->iP_Pos++ ] = 1;
        prQSP->pbtQ[ prQSP->iQ_Pos++ ] = btQ-1;
        prQSP->pbtS[ prQSP->iS_Pos++ ] = (BYTE)bSign;
      }
      else
        prQSP->pbtP[ prQSP->iP_Pos++ ] = 0;
    #else
      prQSP->pbtQ[ prQSP->iQ_Pos++ ] = btQ;
      if(btQ!=0)
      {
        prQSP->pbtS[ prQSP->iS_Pos++ ] = (BYTE)bSign;
      }
    #endif

      pshLL++;
    }
  }

#ifdef DIFF_SUM
  DPRINTF( DPRINTF_DEBUG, "Diff Sum= %5d Diff Square Sum= %d\n", iDiffSum, iDiffSqSum );
#endif
}

#endif // #ifndef DPCM_MEAN

//------------------------------------------------------------------------------

#ifdef DPCM_MEAN

void wci_dpcm_encode
(
  short *       IN OUT pshLL
, SIZE          IN     rSize
, QUANT_VALUE * IN     prQV
, QSP_BUFFER *  OUT    prQSP
)
{
  int   iPredict = 128;
  int   i, j, iDiff;
  BOOL  bSign;
  BYTE  btQ;

  int   x, y;
  SIZE  rBlocks = wci_dpcm_get_blocks( rSize );
  BYTE  *pbtMeanRest;
  int   aiMeanRest[DPCM_MAX_X_BLOCKS];

#ifdef DIFF_SUM
  int  iDiffSum = 0, iDiffSqSum = 0;
#endif

  y = 0;

  while( y < rSize.cy )
  {
    int iLines = min(DPCM_BLOCK_SIZE_Y, rSize.cy-y);

    memset( aiMeanRest, 0, DPCM_MAX_X_BLOCKS*sizeof(int) );

    pbtMeanRest = &prQSP->pbtQ[ prQSP->iQ_Pos ];
    prQSP->iQ_Pos += rBlocks.cx;

    for(i=y;i<y+iLines;i++)
    {
      if(i)
        iPredict=*(pshLL-rSize.cx);

      for(j=0;j<rSize.cx;j++)
      {
        if(j)
          iPredict=*(pshLL-1);

        iDiff = (short)(*pshLL-iPredict);
        #ifdef DIFF_SUM
          iDiffSum += iDiff;
          iDiffSqSum += iDiff*iDiff;
        #endif

        if(iDiff>=0)
        {
          btQ = prQV[iDiff].btQuant;
          *pshLL = (short)(iPredict + prQV[iDiff].shDeQuant);

          aiMeanRest[j/DPCM_BLOCK_SIZE_X] += iDiff - prQV[iDiff].shDeQuant;
          bSign = FALSE;
        }
        else
        {
          btQ = prQV[-iDiff].btQuant;
          *pshLL = (short)(iPredict-prQV[-iDiff].shDeQuant);

          aiMeanRest[j/DPCM_BLOCK_SIZE_X] += iDiff + prQV[-iDiff].shDeQuant;
          bSign = TRUE;
        }

        #ifdef DPCM_PARTITION
          if(btQ!=0)
          {
            prQSP->pbtP[ prQSP->iP_Pos++ ] = 1;
            prQSP->pbtQ[ prQSP->iQ_Pos++ ] = btQ-1;
            prQSP->pbtS[ prQSP->iS_Pos++ ] = (BYTE)bSign;
          }
          else
            prQSP->pbtP[ prQSP->iP_Pos++ ] = 0;
        #else
          prQSP->pbtQ[ prQSP->iQ_Pos++ ] = btQ;
          if(btQ!=0)
          {
            prQSP->pbtS[ prQSP->iS_Pos++ ] = (BYTE)bSign;
          }
        #endif

        pshLL++;
      }
    }

    for( x=0; x<rBlocks.cx; ++x )
    {
      int iWidth = min(DPCM_BLOCK_SIZE_Y, rSize.cy-y);
      int iPels = iLines*iWidth;
      int iRest = aiMeanRest[x];

      if (iRest >= 0)
        iRest = (iRest+iPels/2) / iPels;
      else
        iRest = (iRest-iPels/2) / iPels;

      aiMeanRest[x] = BOUND(iRest, DPCM_MIN_MEAN, DPCM_MAX_MEAN);
      pbtMeanRest[x] = (BYTE)(aiMeanRest[x] - DPCM_MIN_MEAN);
    }

    pshLL -= iLines*rSize.cx;
    for(i=y;i<y+iLines;i++)
      for(j=0;j<rSize.cx;j++)
        *pshLL++ += (short)aiMeanRest[j/DPCM_BLOCK_SIZE_X];

    y += iLines;
  }

#ifdef DIFF_SUM
  DPRINTF( DPRINTF_DEBUG, "Diff Sum= %5d Diff Square Sum= %d\n", iDiffSum, iDiffSqSum );
#endif
}

#endif // #ifdef DPCM_MEAN

//------------------------------------------------------------------------------

static __inline void wci_dpcmdequant(
  int          IN     iQuantizer
, int          IN     iPredict
, QSP_BUFFER * IN OUT prQSP
, short *         OUT pshLL
)
{
//  const int iOffset = NORM_DEAD_ZONE*iQuantizer/100;
//  #define DPCM_DEQUANT( iQuantizer,x) (iQuantizer*(x) + iOffset )
#define DPCM_DEQUANT( iQuantizer,x) (iQuantizer*(x))

#ifdef DPCM_PARTITION
  if ( prQSP->pbtP[ prQSP->iP_Pos++ ] )
  {
    BYTE btQ = 1 + prQSP->pbtQ[ prQSP->iQ_Pos++ ];

    if( prQSP->pbtS[ prQSP->iS_Pos++ ] )
      (*pshLL) = (short)(iPredict - DPCM_DEQUANT( iQuantizer, btQ ));
    else
      (*pshLL) = (short)(iPredict + DPCM_DEQUANT( iQuantizer, btQ ));
  }
  else
    (*pshLL) = (short)iPredict;
#else
  {
    BYTE btQ = prQSP->pbtQ[ prQSP->iQ_Pos++ ];

    if(btQ!=0) 
    {
      if( prQSP->pbtS[ prQSP->iS_Pos++ ] )
        (*pshLL) = (short)(iPredict - DPCM_DEQUANT( iQuantizer, btQ ));
      else                                                           
        (*pshLL) = (short)(iPredict + DPCM_DEQUANT( iQuantizer, btQ ));
    }
    else
      (*pshLL) = iPredict;
  }
#endif
}

//------------------------------------------------------------------------------

#ifdef ENCODER_SUPPORT

int __inline wci_lpc_dpcm_predict(
  const short *     IN pshLL
, const short *     IN pshPrevLine
, int               IN nPosX
, int               IN nPosY
, const LPC_COEFF * IN prLPC
, SIZE              IN rBlockLPC
)
{
  int iPredict = 128;
  int nBlock = nPosX/rBlockLPC.cx;
  int al,bt,gm;

  al = prLPC[nBlock].shAL;
  bt = prLPC[nBlock].shBT;

	gm = LPC_MAX_FILTR - al - bt;

	if(nPosX && nPosY)
  { 
    iPredict=(short)BOUND((al*pshLL[nPosX-1]+gm*pshPrevLine[nPosX-1]+bt*pshPrevLine[nPosX]+LPC_HALF_FILTR)>>13,0,255);

    //DPRINTF( DPRINTF_DEBUG, " l=%3d ul=%3d t=%3d p=%3d\n", pshLL[nPosX-1], pshPrevLine[nPosX-1], pshPrevLine[nPosX], iPredict );
  }
  else
  {
	  if(nPosY==0 && nPosX )
    {
      iPredict=pshLL[nPosX-1];

      //DPRINTF( DPRINTF_DEBUG, " l=%3d p=%3d\n", pshLL[nPosX-1], iPredict );
    }
	  else
    {
	    if(nPosY && nPosX==0)
        iPredict = *pshPrevLine;
	    else
        iPredict=125;
      //DPRINTF( DPRINTF_DEBUG, " p=%3d\n", iPredict );
    }
  }

  return iPredict;
}


//------------------------------------------------------------------------------

void wci_lpc_dpcm_encode
(
  short *       IN OUT pshLL
, SIZE          IN     rSize
, QUANT_VALUE * IN     prQV
, QSP_BUFFER *  OUT    prQSP
)
{
  int   iPredict = 128;
  int   i, j, iDiff;
  BOOL  bSign;
  BYTE  btQ;

  int   x, y;
  SIZE  rBlocks = wci_dpcm_get_blocks( rSize );
  int       aiMeanRest[DPCM_MAX_X_BLOCKS];
  BYTE  *   pbtMeanRest;
  LPC_COEFF arLPC[DPCM_MAX_X_BLOCKS];

  LPC_QUANTIZER rQuant;

#ifdef DIFF_SUM
  int  iDiffSum = 0, iDiffSqSum = 0;
#endif

  wci_lpc_init_quantizer( LPC_NUM_AL, LPC_NUM_BT, &rQuant );

  y = 0;

  while( y < rSize.cy )
  {
    int iLines = min(DPCM_BLOCK_SIZE_Y, rSize.cy-y);
    BOOL bLastBand = (y+iLines >= rSize.cy);

    #ifdef DPCM_MEAN
      memset( aiMeanRest, 0, DPCM_MAX_X_BLOCKS*sizeof(int) );

      pbtMeanRest = &prQSP->pbtQ[ prQSP->iQ_Pos ];
      prQSP->iQ_Pos += rBlocks.cx;
    #endif

    wci_lpc_calc_coeff( pshLL, rSize.cx, c_rLPC_Blocks, rBlocks.cx, bLastBand, arLPC );

    prQSP->iQ_Pos += wci_lpc_put_quant_value( arLPC, rBlocks.cx, &rQuant, &prQSP->pbtQ[ prQSP->iQ_Pos ] );

    for(i=y;i<y+iLines;i++)
    {
      short * pshLine = pshLL;

      for(x=0;x<rSize.cx;x++)
      {
        //DPRINTF( DPRINTF_DEBUG, "l= %3d, b=%3d x=%2d", i, x/DPCM_BLOCK_SIZE_X, x );

        iPredict = wci_lpc_dpcm_predict( pshLine, pshLine-rSize.cx, x, i, arLPC, c_rLPC_Blocks );

        iDiff = (short)(*pshLL-iPredict);
        #ifdef DIFF_SUM
          iDiffSum += iDiff;
          iDiffSqSum += iDiff*iDiff;
        #endif

        if(iDiff>=0)
        {
          btQ = prQV[iDiff].btQuant;
          *pshLL = (short)(iPredict + prQV[iDiff].shDeQuant);

          aiMeanRest[x/DPCM_BLOCK_SIZE_X] += iDiff - prQV[iDiff].shDeQuant;
          bSign = FALSE;
        }
        else
        {
          btQ = prQV[-iDiff].btQuant;
          *pshLL = (short)(iPredict-prQV[-iDiff].shDeQuant);

          aiMeanRest[x/DPCM_BLOCK_SIZE_X] += iDiff + prQV[-iDiff].shDeQuant;
          bSign = TRUE;
        }

        #ifdef DPCM_PARTITION
          if(btQ!=0)
          {
            prQSP->pbtP[ prQSP->iP_Pos++ ] = 1;
            prQSP->pbtQ[ prQSP->iQ_Pos++ ] = btQ-1;
            prQSP->pbtS[ prQSP->iS_Pos++ ] = (BYTE)bSign;
          }
          else
            prQSP->pbtP[ prQSP->iP_Pos++ ] = 0;
        #else
          prQSP->pbtQ[ prQSP->iQ_Pos++ ] = btQ;
          if(btQ!=0)
          {
            prQSP->pbtS[ prQSP->iS_Pos++ ] = (BYTE)bSign;
          }
        #endif

        pshLL++;
      }
    }

    #ifdef DPCM_MEAN

      for( x=0; x<rBlocks.cx; ++x )
      {
        int iWidth = min(DPCM_BLOCK_SIZE_Y, rSize.cy-y);
        aiMeanRest[x] /= (iLines*iWidth);
        aiMeanRest[x] = BOUND(aiMeanRest[x], DPCM_MIN_MEAN, DPCM_MAX_MEAN);
        pbtMeanRest[x] = (BYTE)(aiMeanRest[x] - DPCM_MIN_MEAN);
      }

      pshLL -= iLines*rSize.cx;
      for(i=y;i<y+iLines;i++)
        for(j=0;j<rSize.cx;j++)
        {
          *pshLL++ += (short)aiMeanRest[j/DPCM_BLOCK_SIZE_X];
        }

    #endif

    y += iLines;

#ifdef DIFF_SUM
  DPRINTF( DPRINTF_DEBUG, "Diff Sum= %5d Diff Square Sum= %d\n", iDiffSum, iDiffSqSum );
#endif
  }
}
#endif  //ENCODER_SUPPORT


//------------------------------------------------------------------------------

#ifndef DPCM_MEAN

#ifdef ADPCM_DECODE_TAB

void wci_dpcm_decode
(
  QSP_BUFFER * IN  prQSP
, SIZE         IN  rSize
, const short  IN  shDeQuant[256]
, short *      OUT pshLL
)
{
  #define DEQUANT(x) shDeQuant[x]
  
#else

void wci_dpcm_decode
(
  QSP_BUFFER * IN  prQSP
, SIZE         IN  rSize
, int          IN  iQuantizer
, short *      OUT pshLL
)
{
//  const int iOffset = NORM_DEAD_ZONE*iQuantizer/100;
//  #define DEQUANT(x) (short)(iQuantizer*(x) + iOffset )

  #define DEQUANT(x) (short)(iQuantizer*(x))

#endif

  short iPredict = 128;
  int   i, j;

  for(i=0;i<rSize.cy;i++)
  {
    if(i)
      iPredict=*(pshLL-rSize.cx);

    for(j=0;j<rSize.cx;j++)
    {
      if(j)
        iPredict=*(pshLL-1);
      {
        BYTE btQ = prQSP->pbtQ[ prQSP->iQ_Pos++ ];

        if(btQ!=0) 
        {
          if( prQSP->pbtS[ prQSP->iS_Pos++ ] )
            (*pshLL) = iPredict - DEQUANT(btQ);
          else
            (*pshLL) = iPredict + DEQUANT(btQ);
        }
        else
         (*pshLL) = iPredict;
      }
      pshLL++;
    }
  }

#undef DEQUANT
}

#endif // #ifndef DPCM_MEAN

//------------------------------------------------------------------------------

#ifdef DPCM_MEAN

void wci_dpcm_decode
(
  QSP_BUFFER * IN  prQSP
, SIZE         IN  rSize
, int          IN  iQuantizer
, short *      OUT pshLL
)
{
  int   iPredict = 128;
  int   i, j;

  int   x, y;
  SIZE  rBlocks = wci_dpcm_get_blocks( rSize );
  int   aiMeanRest[DPCM_MAX_X_BLOCKS];

  y = 0;

  while( y < rSize.cy )
  {
    int iLines = min(DPCM_BLOCK_SIZE_Y, rSize.cy-y);

    for( x=0; x<rBlocks.cx; ++x )
    {
      BYTE btMeanRest = prQSP->pbtQ[ prQSP->iQ_Pos++ ];
      aiMeanRest[x] = btMeanRest - DPCM_MIN_MEAN;
    }

    for(i=y;i<y+iLines;i++)
    {
      if(i)
        iPredict=*(pshLL-rSize.cx);

      for(j=0;j<rSize.cx;j++)
      {
        if(j)
          iPredict=*(pshLL-1);

        wci_dpcmdequant( iQuantizer, iPredict, prQSP, pshLL );

        pshLL++;
      }
    }

    pshLL -= iLines*rSize.cx;
    for(i=y;i<y+iLines;i++)
      for(j=0;j<rSize.cx;j++)
      {
        *pshLL++ += (short)aiMeanRest[j/DPCM_BLOCK_SIZE_X];
      }

    y += iLines;
  }
}

#endif // #ifdef DPCM_MEAN

//------------------------------------------------------------------------------

ERROR_CODE wci_dpcm_get_last_pos
(
  SIZE         IN     rSize
, QSP_BUFFER * IN OUT prQSP
)
{
  int iQ_Length = wci_get_square( rSize );
  
  prQSP->iQ_Pos += iQ_Length;
  if ( prQSP->iQ_Pos > prQSP->iQ_Length )
  {
    return ERR_MEMORY;
  }
  {
    int i;

    for ( i=prQSP->iQ_Pos-iQ_Length; i<prQSP->iQ_Pos; i++ )
    {
      if ( prQSP->pbtQ[i] )
        ++prQSP->iS_Pos;
    }
  }
  if ( prQSP->iS_Pos > prQSP->iS_Length )
  {
    return ERR_MEMORY;
  }
  return ERR_NONE;
}

//------------------------------------------------------------------------------

SIZE wci_adpcm_get_partition_block(
  SIZE         IN     rSize
)
{
#ifndef DPCM_PARTITION
  rSize.cx = rSize.cy = 0;
#endif
  return rSize;
}

//------------------------------------------------------------------------------

int wci_adpcm_get_length_q(
  SIZE                IN rSize
, const QSP_BUFFER *  IN prQSP
, const ADPCM_STATE * IN prState
)
{
#ifdef DPCM_PARTITION

  int    iP_Length = wci_get_square( rSize );
  int    iQ_Length = 0;
  int    i;
  BYTE * pbtP = &prQSP->pbtP[prQSP->iP_Pos];

  for ( i=0; i<iP_Length; i++ )
  {
    if ( *pbtP++ )
      ++iQ_Length;
  }

  if (rSize.cy > prState->nCoeffLines)
  {
    int nCoeffs = prState->rBlocks.cx * CELLS( rSize.cy-prState->nCoeffLines, DPCM_BLOCK_SIZE_Y );

    #ifdef DPCM_MEAN
      iQ_Length += nCoeffs;
    #endif

    if ( prState->eMode == eLPC_DPCM )
      iQ_Length += 2*nCoeffs;
  }

  return iQ_Length;

#else

  return wci_get_square( rSize );

#endif
}

//------------------------------------------------------------------------------

int wci_adpcm_get_length_s(
  SIZE               IN rSize
, const QSP_BUFFER * IN prQSP
)
{
#ifdef DPCM_PARTITION

  int    iP_Length = wci_get_square( rSize );
  int    iS_Length = 0;
  int    i;
  BYTE * pbtP = &prQSP->pbtP[prQSP->iP_Pos];

  for ( i=0; i<iP_Length; i++ )
  {
    if ( *pbtP++ )
      ++iS_Length;
  }

  return iS_Length;

#else

  int    iQ_Length = wci_get_square( rSize );
  int    iS_Length = 0;
  int    i;
  BYTE * pbtQ = &prQSP->pbtQ[prQSP->iQ_Pos];

  for ( i=0; i<iQ_Length; i++ )
  {
    if ( *pbtQ++ )
      ++iS_Length;
  }

  return iS_Length;

#endif
}

//------------------------------------------------------------------------------

int wci_adpcm_get_buffer_size(
  int IN iImageWidth
)
{
  int  iBufferSize = 0;
  SIZE rBlockSize, rBlocks;

  rBlockSize.cx = iImageWidth;
  rBlockSize.cy = MAX_IMAGE_HEIGHT;

  rBlocks = wci_dpcm_get_blocks( rBlockSize );

  iBufferSize += iImageWidth * sizeof(short) + CACHE_LINE; // for pshPrevLine

#ifdef DPCM_MEAN
  iBufferSize += rBlocks.cx * sizeof(short) + CACHE_LINE;
#endif

  iBufferSize += 2 * rBlocks.cx * sizeof(short) + CACHE_LINE; // for pshLPC

  return iBufferSize;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_adpcm_init_stripe_state
(
  enumDPCM_Mode        IN     eMode
, SIZE                 IN     rBlockSize
, int                  IN     nShift
, int                  IN     iQuantizer
, ADPCM_STATE *        IN OUT prState
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
)
{
  memset( prState, 0, sizeof(ADPCM_STATE) );

  prState->eMode      = eMode;
  prState->rBlockSize = rBlockSize;
  prState->nShift     = nShift;
  prState->rBlocks    = wci_dpcm_get_blocks( rBlockSize );
  prState->iQuantizer = iQuantizer;

  prState->pshPrevLine = (short *) wci_aligned_malloc( rBlockSize.cx * sizeof(short), CACHE_LINE, prMemHeap );
  if (prState->pshPrevLine == NULL)
  {
    return ERR_MEMORY;
  }
  memset( prState->pshPrevLine, 0, rBlockSize.cx * sizeof(short) );

#ifdef DPCM_MEAN
  prState->pshMean = (short *) wci_aligned_malloc( prState->rBlocks.cx * sizeof(short), CACHE_LINE, prMemHeap );
  if (prState->pshMean == NULL)
  {
    return ERR_MEMORY;
  }
#endif

#ifdef ENCODER_SUPPORT
  if (eMode == eLPC_DPCM)
  {
    wci_lpc_init_quantizer( LPC_NUM_AL, LPC_NUM_BT, &prState->rQuant );

    prState->prLPC = (LPC_COEFF *) wci_aligned_malloc( prState->rBlocks.cx * sizeof(LPC_COEFF), CACHE_LINE, prMemHeap );
    if (prState->prLPC == NULL)
    {
      return ERR_MEMORY;
    }
  }
#endif  //ENCODER_SUPPORT

  return ERR_NONE;
}

//------------------------------------------------------------------------------

void wci_adpcm_done_stripe_state
(
  ADPCM_STATE*         IN OUT prState
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
)
{
  wci_aligned_free( prState->pshPrevLine, prMemHeap );

#ifdef DPCM_MEAN
  wci_aligned_free( prState->pshMean, prMemHeap );
#endif

  wci_aligned_free( prState->prLPC, prMemHeap );

  memset( prState, 0, sizeof(ADPCM_STATE) );
}

//------------------------------------------------------------------------------

void wci_dpcm_decodecoeffs(
  QSP_BUFFER * IN OUT prQSP
, ADPCM_STATE* IN OUT prState
)
{

#ifdef DPCM_MEAN

  int x;

  for( x=0; x<prState->rBlocks.cx; ++x )
  {
    BYTE btMeanRest = prQSP->pbtQ[ prQSP->iQ_Pos++ ];
    prState->pshMean[x] = btMeanRest + DPCM_MIN_MEAN;
  }

#endif
  
#ifdef ENCODER_SUPPORT
  if ( prState->eMode == eLPC_DPCM )
  {
    int iOffset = wci_lpc_get_quant_value( &prQSP->pbtQ[ prQSP->iQ_Pos ]
                                   , prState->rBlocks.cx
                                   , &prState->rQuant
                                   , prState->prLPC );
    prQSP->iQ_Pos += iOffset;
  }
#endif  //ENCODER_SUPPORT
}        

//------------------------------------------------------------------------------
#ifdef ENCODER_SUPPORT

void wci_dpcm_decodelines(
  QSP_BUFFER * IN OUT prQSP
, int          IN     iDecodeLines
, ADPCM_STATE* IN OUT prState
, int          IN     iStride
, short *         OUT pshLL
)
{
  const int iQuantizer = prState->iQuantizer;
  const int iWidth = prState->rBlockSize.cx;
  const int iDiff = iStride - prState->rBlockSize.cx;
  int       iLine;
  int       iPredict;
  int       x;

  iLine = 0;

  if (prState->nCurrLine > 0)
  {
    iPredict = *prState->pshPrevLine;
    //DPRINTF( DPRINTF_DEBUG, "l= %3d, p= %d\n", prState->nCurrLine+iLine, iPredict );

    for(x=0; x<iWidth; x++)
    {
      wci_dpcmdequant( iQuantizer, iPredict, prQSP, pshLL );

      #ifdef DPCM_MEAN
        iPredict = *pshLL;
        *pshLL++ += prState->pshMean[x/DPCM_BLOCK_SIZE_X];
      #else
        iPredict = *pshLL++;
      #endif
    }
    pshLL += iDiff;

    iLine++;
  }

  for(; iLine<iDecodeLines; iLine++)
  {
    if( iLine )
      #ifdef DPCM_MEAN
        iPredict = *(pshLL-iStride) - prState->pshMean[0];
      #else
        iPredict = *(pshLL-iStride);
      #endif
    else
      iPredict = 128;

    //DPRINTF( DPRINTF_DEBUG, "l= %3d, p= %d\n", prState->nCurrLine+iLine, iPredict );

    for(x=0; x<iWidth; x++)
    {
      wci_dpcmdequant( iQuantizer, iPredict, prQSP, pshLL );

      #ifdef DPCM_MEAN
        iPredict = *pshLL;
        *pshLL++ += prState->pshMean[x/DPCM_BLOCK_SIZE_X];
      #else
        iPredict = *pshLL++;
      #endif
    }
    pshLL += iDiff;
  }
}

//------------------------------------------------------------------------------

void wci_dpcm_decodestripe
(
  QSP_BUFFER * IN OUT prQSP
, int          IN     iDecodeLines
, ADPCM_STATE* IN OUT prState
, int          IN     iStride
, short *         OUT pshLL
)
{
  //DPRINTF( DPRINTF_DEBUG, "WICA::wci_dpcm_decodestripe\n" );

  assert( iDecodeLines <= prState->rBlockSize.cy - prState->nCurrLine );

  if( iDecodeLines > 0 )
  {
    int nLines;

    for ( nLines=0; nLines<iDecodeLines; )
    {
      int nLineToDecode;

      if ( prState->nCoeffLines <= 0 )
      {
        wci_dpcm_decodecoeffs( prQSP, prState );
        prState->nCoeffLines += DPCM_BLOCK_SIZE_Y;
      }

      nLineToDecode = min( prState->nCoeffLines, iDecodeLines-nLines );

      wci_dpcm_decodelines( prQSP, nLineToDecode, prState, iStride, pshLL );
      nLines += nLineToDecode;
      pshLL += nLineToDecode*iStride;

      prState->nCoeffLines -= nLineToDecode;

      *prState->pshPrevLine = *(pshLL - iStride);
      #ifdef DPCM_MEAN
        if (prState->nCoeffLines > 0)
          *prState->pshPrevLine -= prState->pshMean[0];
      #endif

      prState->nCurrLine += nLineToDecode;
    }
  }
}

//------------------------------------------------------------------------------


void wci_lpc_dpcm_decodelines(
  QSP_BUFFER * IN OUT prQSP
, int          IN     iDecodeLines
, ADPCM_STATE* IN OUT prState
, int          IN     iStride
, short *         OUT pshLL
)
{
  const int iQuantizer = prState->iQuantizer;
  const int iWidth = prState->rBlockSize.cx;
  const int iDiff = iStride - prState->rBlockSize.cx;
  int       iLine;
  int       iPredict;
  int       x;

  short *   pshPrevLine = prState->pshPrevLine;

  for( iLine = 0; iLine<iDecodeLines; iLine++)
  {
    short * pshLine = pshLL;

    for(x=0; x<iWidth; x++)
    {
      //DPRINTF( DPRINTF_DEBUG, "l= %3d, b=%3d x=%2d", prState->nCurrLine+iLine, x/DPCM_BLOCK_SIZE_X, x );

      iPredict = wci_lpc_dpcm_predict( pshLine, pshPrevLine, x, prState->nCurrLine+iLine, prState->prLPC, c_rLPC_Blocks );

      wci_dpcmdequant( iQuantizer, iPredict, prQSP, pshLL );

      pshLL++;
    }
    pshPrevLine = pshLine;
    pshLL += iDiff;
  }

  #ifdef DPCM_MEAN
    pshLL -= iStride*iDecodeLines;
    for( iLine = 0; iLine<iDecodeLines; iLine++)
    {
      for(x=0; x<iWidth; x++)
        *pshLL++ += prState->pshMean[x/DPCM_BLOCK_SIZE_X];
      pshLL += iDiff;
    }
  #endif
}

//------------------------------------------------------------------------------

void wci_lpc_dpcm_decodestripe(
  QSP_BUFFER * IN OUT prQSP
, int          IN     iDecodeLines
, ADPCM_STATE* IN OUT prState
, int          IN     iStride
, short *         OUT pshLL
)
{
  //DPRINTF( DPRINTF_DEBUG, "WICA::wci_lpc_dpcm_decodestripe\n" );

  assert( iDecodeLines <= prState->rBlockSize.cy - prState->nCurrLine );

  if( iDecodeLines > 0 )
  {
    int nLines;

    for ( nLines=0; nLines<iDecodeLines; )
    {
      int nLineToDecode;

      if ( prState->nCoeffLines <= 0 )
      {
        wci_dpcm_decodecoeffs( prQSP, prState );
        prState->nCoeffLines += DPCM_BLOCK_SIZE_Y;
      }

      nLineToDecode = min( prState->nCoeffLines, iDecodeLines-nLines );

      wci_lpc_dpcm_decodelines( prQSP, nLineToDecode, prState, iStride, pshLL );
      nLines += nLineToDecode;
      pshLL += nLineToDecode*iStride;

      prState->nCoeffLines -= nLineToDecode;

      memcpy( prState->pshPrevLine, pshLL - iStride, prState->rBlockSize.cx*sizeof(short) );

      #ifdef DPCM_MEAN
        if (prState->nCoeffLines > 0)
        {
          int x;
          for(x=0; x<prState->rBlockSize.cx; x++)
            prState->pshPrevLine[x] -= prState->pshMean[x/DPCM_BLOCK_SIZE_X];
        }
      #endif

      prState->nCurrLine += nLineToDecode;
    }
  }
}

#endif //ENCODER_SUPPORT


//------------------------------------------------------------------------------

#ifdef ADPCM_ENCODE_TAB

  void wci_adpcm_encode
  (
    enumDPCM_Mode IN     eMode
  , short *       IN OUT pshLL
  , SIZE          IN     rBlockSize
  , QUANT_VALUE * IN     prQV
  , QSP_BUFFER *     OUT prQSP
  )
  {
    switch( eMode )
    {
    default:
    case eDPCM:
      wci_dpcm_encode( pshLL, rBlockSize, prQV, prQSP );
    	break;
#ifdef ENCODER_SUPPORT
   case eLPC_DPCM:
      wci_lpc_dpcm_encode( pshLL, rBlockSize, prQV, prQSP );
#endif  //ENCODER_SUPPORT
    }
  }
  
#else

  void wci_adpcm_encode
  (
    enumDPCM_Mode IN     eMode
  , short *       IN OUT pshLL
  , SIZE          IN     rBlockSize
  , int           IN     iDeadZone
  , int           IN     iQuantizer
  , QSP_BUFFER *     OUT prQSP
  )
  {
    switch( eMode )
    {
    default:
    case eDPCM:
      wci_dpcm_encode( pshLL, rBlockSize, iDeadZone, iQuantizer, prQSP );
    	break;
#ifdef ENCODER_SUPPORT
   case eLPC_DPCM:
      wci_lpc_dpcm_encode( pshLL, rBlockSize, iDeadZone, iQuantizer, prQSP );
#endif  //ENCODER_SUPPORT
    }
  }

#endif

//------------------------------------------------------------------------------

#ifdef ADPCM_DECODE_TAB

  void wci_adpcm_decode
  (
    enumDPCM_Mode IN     eMode
  , QSP_BUFFER *  IN     prQSP
  , SIZE          IN     rSize
  , const short   IN     shDeQuant[256]
  , short *          OUT pshLL
  )
  {
    switch( eMode )
    {
    case eDPCM:
      wci_dpcm_decode( prQSP, rSize, shDeQuant, pshLL );
    	break;
    default:
      assert( eMode != eDPCM );
    }
  }
  
#else

  void wci_adpcm_decode
  (
    enumDPCM_Mode IN     eMode
  , QSP_BUFFER *  IN     prQSP
  , SIZE          IN     rSize
  , int           IN     iQuantizer
  , short *          OUT pshLL
  )
  {
    switch( eMode )
    {
    case eDPCM:
      wci_dpcm_decode( prQSP, rSize, iQuantizer, pshLL );
    	break;
    default:
      assert( eMode != eDPCM );
    }
  }

#endif

//------------------------------------------------------------------------------

#ifdef ENCODER_SUPPORT
void wci_adpcm_decodestripe
(
  QSP_BUFFER * IN     prQSP
, int          IN     iDecodeLines
, ADPCM_STATE* IN OUT prState
, int          IN     iStride
, short *         OUT pshLL
)
{
  switch( prState->eMode )
  {
  default:
  case eDPCM:
    wci_dpcm_decodestripe( prQSP, iDecodeLines, prState, iStride, pshLL );
    break;
#ifdef ENCODER_SUPPORT
case eLPC_DPCM:
    wci_lpc_dpcm_decodestripe( prQSP, iDecodeLines, prState, iStride, pshLL );
    break;
#endif  //ENCODER_SUPPORT
  }
}
#endif  //ENCODER_SUPPORT

//------------------------------------------------------------------------------

#define DPCM_DEQUANT( iQuantizer,x) (iQuantizer*(x))

static __inline short* wci_dpcm_decode_line_zeros_set(int iQuantizer,int iPredict, QSP_BUFFER* prQSP, short* pshLL,short* pshMean,int iWidth)
{
  register int x;
  for(x=0; x<iWidth; x++)
  {
    register short t;
    if ( prQSP->pbtP[ prQSP->iP_Pos++ ] )
    {
      BYTE btQ = 1 + prQSP->pbtQ[ prQSP->iQ_Pos++ ];

      if( prQSP->pbtS[ prQSP->iS_Pos++ ] )
        t = (short)(iPredict - DPCM_DEQUANT( iQuantizer, btQ ));
      else
        t = (short)(iPredict + DPCM_DEQUANT( iQuantizer, btQ ));
    }
    else
      t = (short)iPredict;

    iPredict = t;
    *pshLL++ = t + pshMean[x/DPCM_BLOCK_SIZE_X];
    *pshLL++ = 0;
  }
return  pshLL;
}

static __inline short* wci_dpcm_decode_line(int iQuantizer,int iPredict, QSP_BUFFER* prQSP, short* pshLL,short* pshMean,int iWidth)
{
  register int x;
  for(x=0; x<iWidth; x++)
  {
    register short t;
    if ( prQSP->pbtP[ prQSP->iP_Pos++ ] )
    {
      BYTE btQ = 1 + prQSP->pbtQ[ prQSP->iQ_Pos++ ];

      if( prQSP->pbtS[ prQSP->iS_Pos++ ] )
        t = (short)(iPredict - DPCM_DEQUANT( iQuantizer, btQ ));
      else
        t = (short)(iPredict + DPCM_DEQUANT( iQuantizer, btQ ));
    }
    else
      t = (short)iPredict;

    iPredict = t;
    *pshLL++ = t + pshMean[x/DPCM_BLOCK_SIZE_X];
  }
  return  pshLL;
}

void wci_dpcm_decode_lines(
                           QSP_BUFFER * IN OUT prQSP
                           , int          IN     iDecodeLines
                           , ADPCM_STATE* IN OUT prState
                           , int          IN     iStride
                           , short *         OUT pshLL
                           )
{
  const int iQuantizer = prState->iQuantizer;
  const int iWidth = prState->rBlockSize.cx;
  const int nShift = prState->nShift;
  const int iDiff = iStride - nShift*prState->rBlockSize.cx;
  int       iLine;
  int       iPredict;
  
  iLine = 0;

  if (prState->nCurrLine > 0)
  {
    iPredict = *prState->pshPrevLine;
    if(nShift == 2)
    {   
      pshLL = wci_dpcm_decode_line_zeros_set( iQuantizer,iPredict,  prQSP,  pshLL, prState->pshMean,iWidth);
    }
    else
    {
      pshLL = wci_dpcm_decode_line( iQuantizer,iPredict,  prQSP,  pshLL, prState->pshMean,iWidth);
    }
    pshLL += iDiff;
    iLine++;
  }

  for(; iLine<iDecodeLines; iLine++)
  {
    if( iLine )
#ifdef DPCM_MEAN
      iPredict = *(pshLL-iStride) - prState->pshMean[0];
#else
      iPredict = *(pshLL-iStride);
#endif
    else
      iPredict = 128;

    if(nShift == 2)
    {   
      pshLL = wci_dpcm_decode_line_zeros_set( iQuantizer,iPredict,  prQSP,  pshLL, prState->pshMean,iWidth);
    }
    else
    {
      pshLL = wci_dpcm_decode_line( iQuantizer,iPredict,  prQSP,  pshLL, prState->pshMean,iWidth);
    }
    pshLL += iDiff;
  }
}

//------------------------------------------------------------------------------

void wci_dpcm_decode_stripe
(
  QSP_BUFFER * IN OUT prQSP
, int          IN     iDecodeLines
, ADPCM_STATE* IN OUT prState
, int          IN     iStride
, short *         OUT pshLL
)
{
  //DPRINTF( DPRINTF_DEBUG, "WICA::wci_dpcm_decodestripe\n" );

  assert( iDecodeLines <= prState->rBlockSize.cy - prState->nCurrLine );

  if( iDecodeLines > 0 )
  {
    int nLines;

    for ( nLines=0; nLines<iDecodeLines; )
    {
      int nLineToDecode;

      if ( prState->nCoeffLines <= 0 )
      {
        wci_dpcm_decodecoeffs( prQSP, prState );
        prState->nCoeffLines += DPCM_BLOCK_SIZE_Y;
      }

      nLineToDecode = min( prState->nCoeffLines, iDecodeLines-nLines );

      wci_dpcm_decode_lines( prQSP, nLineToDecode, prState, iStride, pshLL);

      nLines += nLineToDecode;
      pshLL  += nLineToDecode*iStride;

      prState->nCoeffLines -= nLineToDecode;

      *prState->pshPrevLine = *(pshLL - iStride);
      #ifdef DPCM_MEAN
        if (prState->nCoeffLines > 0)
          *prState->pshPrevLine -= prState->pshMean[0];
      #endif

      prState->nCurrLine += nLineToDecode;
    }
  }
}


//------------------------------------------------------------------------------

#ifdef ENCODER_SUPPORT
void wci_lpc_dpcm_decode_stripe(
  QSP_BUFFER * IN OUT prQSP
, int          IN     iDecodeLines
, ADPCM_STATE* IN OUT prState
, int          IN     iStride
, short *         OUT pshLL
)
{
  //DPRINTF( DPRINTF_DEBUG, "WICA::wci_lpc_dpcm_decodestripe\n" );

  assert( iDecodeLines <= prState->rBlockSize.cy - prState->nCurrLine );

  if( iDecodeLines > 0 )
  {
    int nLines;

    for ( nLines=0; nLines<iDecodeLines; )
    {
      int nLineToDecode;

      if ( prState->nCoeffLines <= 0 )
      {
        wci_dpcm_decodecoeffs( prQSP, prState );
        prState->nCoeffLines += DPCM_BLOCK_SIZE_Y;
      }

      nLineToDecode = min( prState->nCoeffLines, iDecodeLines-nLines );

      wci_lpc_dpcm_decodelines( prQSP, nLineToDecode, prState, iStride, pshLL );
      nLines += nLineToDecode;
      pshLL += nLineToDecode*iStride;

      prState->nCoeffLines -= nLineToDecode;

      memcpy( prState->pshPrevLine, pshLL - iStride, prState->rBlockSize.cx*sizeof(short) );

      #ifdef DPCM_MEAN
        if (prState->nCoeffLines > 0)
        {
          int x;
          for(x=0; x<prState->rBlockSize.cx; x++)
            prState->pshPrevLine[x] -= prState->pshMean[x/DPCM_BLOCK_SIZE_X];
        }
      #endif

      prState->nCurrLine += nLineToDecode;
    }
  }
}

#endif  //ENCODER_SUPPORT

//------------------------------------------------------------------------------

void wci_adpcm_decode_stripe
(
  QSP_BUFFER * IN     prQSP
, int          IN     iDecodeLines
, ADPCM_STATE* IN OUT prState
, int          IN     iStride
, short *         OUT pshLL
)
{
  switch( prState->eMode )
  {
  default:
  case eDPCM:
    wci_dpcm_decode_stripe( prQSP, iDecodeLines, prState, iStride, pshLL );
    break;
#ifdef ENCODER_SUPPORT
  case eLPC_DPCM:
    wci_lpc_dpcm_decode_stripe( prQSP, iDecodeLines, prState, iStride, pshLL );
    break;
#endif //ENCODER_SUPPORT
  }
}


/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: WavStripeQuant.c

ABSTRACT:    This file contains procedures for dequant stripe

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include "wci_wavelet.h"
#include "wci_wav_quant.h"
#include "wci_wav_block_quant.h"
#include "wci_wav_stripe_quant.h"

//------------------------------------------------------------------------------

#define DEQUANT_QS( q, s, scale, dst )                     \
  if( (*q) != 0 )                                          \
  {                                                        \
    *dst = (int16_t)( ((0 - *s)^(scale * (*q)) ) + (*s) ); \
    s++;                                                   \
  }                                                        \
  else                                                     \
    *dst = 0;                                              \
  dst++;                                                   \
  q++

#define DEQUANT_BLOCK_QS( p, q, s, scale, dst1, dst2 )     \
  assert( (*p == 1) || (*p == 0) );                        \
  if( (*p++) != 0 )                                        \
  {                                                        \
    DEQUANT_QS( q, s, scale, dst1 );                       \
    DEQUANT_QS( q, s, scale, dst1 );                       \
    DEQUANT_QS( q, s, scale, dst2 );                       \
    DEQUANT_QS( q, s, scale, dst2 );                       \
  }                                                        \
  else                                                     \
  {                                                        \
    *dst1++ = 0; *dst1++ = 0;                              \
    *dst2++ = 0; *dst2++ = 0;                              \
  }

#define DEQUANT_FOUR_QS( q, s, scale, dst )                \
  DEQUANT_QS( q, s, scale, dst );                          \
  DEQUANT_QS( q, s, scale, dst );                          \
  DEQUANT_QS( q, s, scale, dst );                          \
  DEQUANT_QS( q, s, scale, dst )                

//------------------------------------------------------------------------------

#define DEQUANT_QS2( q, s, scale, dst )                    \
  if( (*q) != 0 )                                          \
  {                                                        \
  if(*s)                                                   \
  *dst = (int16_t)(  -scale * (*q) );                      \
  else                                                     \
  *dst = (int16_t)(  scale * (*q) );                       \
  s++;                                                     \
  }                                                        \
  else                                                     \
  *dst = 0;                                                \
  dst += 2;                                                \
  q++

#define DEQUANT_BLOCK_QS2( p, q, s, scale, dst1, dst2 )    \
  assert( (*p == 1) || (*p == 0) );                        \
  if( (*p++) != 0 )                                        \
  {                                                        \
    DEQUANT_QS2( q, s, scale, dst1 );                      \
    DEQUANT_QS2( q, s, scale, dst1 );                      \
    DEQUANT_QS2( q, s, scale, dst2 );                      \
    DEQUANT_QS2( q, s, scale, dst2 );                      \
  }                                                        \
  else                                                     \
  {                                                        \
    *dst1  = 0;                                            \
     dst1 += 2;                                            \
    *dst1  = 0;                                            \
     dst1 += 2;                                            \
    *dst2  = 0;                                            \
     dst2 += 2;                                            \
    *dst2  = 0;                                            \
     dst2 += 2;                                            \
  }

#define DEQUANT_FOUR_QS2( q, s, scale, dst )               \
  DEQUANT_QS2( q, s, scale, dst );                         \
  DEQUANT_QS2( q, s, scale, dst );                         \
  DEQUANT_QS2( q, s, scale, dst );                         \
  DEQUANT_QS2( q, s, scale, dst )                


#define DEQUANT_QS21( q, s, scale, dst )                    \
  if( (*q) != 0 )                                           \
{                                                           \
  if(*s)                                                    \
  *dst = (int16_t)(  -scale * (*q) );                       \
  else                                                      \
  *dst = (int16_t)(  scale * (*q) );                        \
  s++;                                                      \
}                                                           \
  dst += 2;                                                 \
  q++

#define DEQUANT_BLOCK_QS21( p, q, s, scale, dst1, dst2 )    \
  assert( (*p == 1) || (*p == 0) );                         \
  if( (*p++) != 0 )                                         \
{                                                           \
  DEQUANT_QS21( q, s, scale, dst1 );                        \
  DEQUANT_QS21( q, s, scale, dst1 );                        \
  DEQUANT_QS21( q, s, scale, dst2 );                        \
  DEQUANT_QS21( q, s, scale, dst2 );                        \
}                                                           \
  else                                                      \
{                                                           \
  dst1 += 4;                                                \
  dst2 += 4;                                                \
}


//------------------------------------------------------------------------------
#ifdef ENCODER_SUPPORT
void wci_qsp_get_max_block_length(
  SIZE         IN     rSize
, enumWT_Mode  IN     eMode
, QSP_BUFFER * IN OUT prQSP
)
{
  SIZE rPartitionBlock = wci_wav_get_partition_block( rSize );
  
  prQSP->iP_Pos = wci_get_square( rPartitionBlock );
  
  wci_down_block_size( eMode, &rSize );
  {
    int iPels = wci_get_square( rSize );
    prQSP->iQ_Pos = iPels;
    prQSP->iS_Pos = iPels;
  }
}

//------------------------------------------------------------------------------

ERROR_CODE wci_qsp_get_last_pos(
  int          IN     iPartitionLength
, SIZE         IN     rSize
, QSP_BUFFER * IN OUT prQSP
)
{
  int i;
  int iBlockSize = QUANT_BLOCK_SIZE*QUANT_BLOCK_SIZE;
  int iQ_Length = 0;
  
  // Partition block of quant. values
  prQSP->iP_Pos += iPartitionLength;
  if ( prQSP->iP_Pos > prQSP->iP_Length )
  {
    return ERR_MEMORY;
  }
  for ( i=prQSP->iP_Pos-iPartitionLength; i<prQSP->iP_Pos; i++ )
  {
    if ( prQSP->pbtP[ i ] )
      iQ_Length += iBlockSize;
  }

  // Separated quant. values
  {
    SIZE rPartitionSize = wci_wav_get_partition_block( rSize );
    SIZE rRest;
    rRest.cx = rSize.cx % GROUP_SIZE_X;
    rRest.cy = rSize.cy % (GROUP_SIZE_Y*QUANT_BLOCK_SIZE);
    
    iQ_Length += rPartitionSize.cy * 2 * ((HH-LH+1)*rRest.cx/2);
    iQ_Length += ((HH-LH+1)*rSize.cx/2) * (rRest.cy / GROUP_SIZE_Y);
  }
  prQSP->iQ_Pos += iQ_Length;
  
  if ( prQSP->iQ_Pos > prQSP->iQ_Length )
  {
    return ERR_MEMORY;
  }

  for ( i=prQSP->iQ_Pos-iQ_Length; i<prQSP->iQ_Pos; i++ )
  {
    if ( prQSP->pbtQ[i] )
      ++prQSP->iS_Pos;
  }
  if ( prQSP->iS_Pos > prQSP->iS_Length )
  {
    return ERR_MEMORY;
  }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

int wci_qsp_get_length_p(
  SIZE IN rSize
)
{
  SIZE rPartitionBlock = wci_wav_get_partition_block( rSize );
  
  return wci_get_square( rPartitionBlock );
}

#endif  //ifdef ENCODER_SUPPORT

//------------------------------------------------------------------------------

int wci_qsp_get_length_q(
  int                IN iP_Length
, SIZE               IN rSize
, const QSP_BUFFER * IN prQSP
)
{
  // Partition block of quant. values
  int iBlockSize = QUANT_BLOCK_SIZE * QUANT_BLOCK_SIZE;
  int iMaxP_Pos = prQSP->iP_Pos+iP_Length;
  int iQ_Length = 0;
  int i;
  
  for ( i=prQSP->iP_Pos; i<iMaxP_Pos; i++ )
  {
    if ( prQSP->pbtP[ i ] )
      iQ_Length += iBlockSize;
  }

  // Separated quant. values
  {
    SIZE rPartitionSize = wci_wav_get_partition_block( rSize );
    SIZE rRest;
    rRest.cx = rSize.cx % GROUP_SIZE_X;
    rRest.cy = rSize.cy % (GROUP_SIZE_Y*QUANT_BLOCK_SIZE);
    
    iQ_Length += rPartitionSize.cy * 2 * ((HH-LH+1)*rRest.cx/2);
    iQ_Length += ((HH-LH+1)*rSize.cx/2) * (rRest.cy / GROUP_SIZE_Y);
  }

  return iQ_Length;
}

//------------------------------------------------------------------------------

int wci_qsp_get_length_s(
  int                IN iQ_Length
, const QSP_BUFFER * IN prQSP
)
{
  int iS_Length = 0;
  int iMaxQ_Pos = prQSP->iQ_Pos+iQ_Length;
  int i;

  for ( i=prQSP->iQ_Pos; i<iMaxQ_Pos; i++ )
  {
    if ( prQSP->pbtQ[i] )
      ++iS_Length;
  }

  return iS_Length;
}

//------------------------------------------------------------------------------

#ifdef  ENCODER_SUPPORT
void wci_dequant_step_stripe(
  QSP_BUFFER *            IN OUT prQSP
, int                     IN     iDequantLines
, const WAV_QUANT_SCALE * IN     prQS
, int                     IN     iStep
, WAV_STRIPE_NO *         IN OUT Wav_Stripe
)
{
  WAV_STATE * Wav_State;

  int16_t *   Buf;
  SIZE        rSize;

  Wav_State = &Wav_Stripe->Wav_State[iStep];
  Buf = Wav_State->wav_buffer;

  rSize.cx = Wav_State->Size.cx;
  rSize.cy = iDequantLines;
  
  {
    const SIZE   rP_Size = wci_wav_get_partition_block( rSize );
    const int    nGroupsInRow = rP_Size.cx/2;

    const int    nRest_x = (rSize.cx % GROUP_SIZE_X) / 2;
    const int    nRest_y = (Wav_State->Full_Size.cy % (GROUP_SIZE_Y*QUANT_BLOCK_SIZE)) / 2;

    const int    Line_Step = rSize.cx + rSize.cx/2;

    const BYTE * P_LH = &prQSP->pbtP[prQSP->iP_Pos];
    const BYTE * P_HL = P_LH + wci_get_square( rP_Size );
    const BYTE * P_HH = P_HL + wci_get_square( rP_Size );

    const BYTE * Q = &prQSP->pbtQ[prQSP->iQ_Pos];
    const BYTE * S = &prQSP->pbtS[prQSP->iS_Pos];

    const int    scale_LH = prQS->iQuantizer_H ;
    const int    scale_HL = prQS->iQuantizer_H ;
    const int    scale_HH = prQS->iQuantizer_HH;

    int16_t *      LHa = Buf + rSize.cx/2;
    int16_t *      LHb = LHa + rSize.cx;

    int16_t *      HLa = Buf + rSize.cx*rSize.cy/2;
    int16_t *      HLb = HLa + rSize.cx;

    int16_t *      HHa = HLa + rSize.cx/2;
    int16_t *      HHb = HHa + rSize.cx;

    int          x, y;
    
    for(y = 0; y<rP_Size.cy; y++)
    {
      for(x = 0; x<nGroupsInRow; x++)
      {
        DEQUANT_BLOCK_QS( P_LH, Q, S, scale_LH, LHa, LHb )
        DEQUANT_BLOCK_QS( P_LH, Q, S, scale_LH, LHa, LHb )

        DEQUANT_BLOCK_QS( P_HL, Q, S, scale_HL, HLa, HLb )
        DEQUANT_BLOCK_QS( P_HL, Q, S, scale_HL, HLa, HLb )

        DEQUANT_BLOCK_QS( P_HH, Q, S, scale_HH, HHa, HHb )
        DEQUANT_BLOCK_QS( P_HH, Q, S, scale_HH, HHa, HHb )
      }

      for(x=0; x < nRest_x; x++)
      {
        DEQUANT_QS( Q, S, scale_LH, LHa );
        DEQUANT_QS( Q, S, scale_LH, LHb );

        DEQUANT_QS( Q, S, scale_HL, HLa );
        DEQUANT_QS( Q, S, scale_HL, HLb );
        
        DEQUANT_QS( Q, S, scale_HH, HHa );
        DEQUANT_QS( Q, S, scale_HH, HHb );
      }  

      LHa += Line_Step;
      LHb += Line_Step;  

      HLa += Line_Step;
      HLb += Line_Step;  

      HHa += Line_Step;
      HHb += Line_Step;  
    }
      
    if(nRest_y == 1 && Wav_Stripe->Wav_State[iStep].is_last == 1)
    {
      for(x = 0; x<nGroupsInRow; x++)
      {
        DEQUANT_FOUR_QS( Q, S, scale_LH, LHa );
        DEQUANT_FOUR_QS( Q, S, scale_HL, HLa );
        DEQUANT_FOUR_QS( Q, S, scale_HH, HHa );
      }
          
      for(x=0; x < nRest_x; x++)
      {
        DEQUANT_QS( Q, S, scale_LH, LHa );
        DEQUANT_QS( Q, S, scale_HL, HLa );
        DEQUANT_QS( Q, S, scale_HH, HHa );
      }  
    }

    prQSP->iQ_Pos += Q - &prQSP->pbtQ[prQSP->iQ_Pos];
    prQSP->iS_Pos += S - &prQSP->pbtS[prQSP->iS_Pos];
    prQSP->iP_Pos += P_HH - &prQSP->pbtP[prQSP->iP_Pos];

    assert( prQSP->iQ_Length == prQSP->iQ_Pos );
    assert( prQSP->iS_Length == prQSP->iS_Pos );
    assert( prQSP->iP_Length == prQSP->iP_Pos );
  }
}

//------------------------------------------------------------------------------

int16_t * wci_stripe_getll_block(
  const WAV_STRIPE_NO * IN prStripe
)
{
  return prStripe->Wav_State_LL.wav_buffer;
}

#endif  //ifdef ENCODER_SUPPORT

//------------------------------------------------------------------------------


void wci_stripe_dequant(
  QSP_BUFFER *            IN OUT qsp
, const WAV_QUANT_SCALE * IN     scale
, int                     IN     lines_to_dequant
, int                     IN     step
, WAV_STRIPE *            IN OUT stripe
)
{
  WAV_STRIPE_LAYER * layer = &stripe->layer[step];
  SIZE               size = layer->size;

  size.cy = lines_to_dequant;

  // Dequantization
  {
    const SIZE   rP_Size = wci_wav_get_partition_block( size );
    const int    nGroupsInRow = rP_Size.cx/2;

    const int    nRest_x = (size.cx % GROUP_SIZE_X) / 2;
    const int    nRest_y = (size.cy % (GROUP_SIZE_Y*QUANT_BLOCK_SIZE)) / 2;

    const int    Line_Step = 3*size.cx;

    const BYTE * P_LH = &qsp->pbtP[qsp->iP_Pos];
    const BYTE * P_HL = P_LH + wci_get_square( rP_Size );
    const BYTE * P_HH = P_HL + wci_get_square( rP_Size );

    const BYTE * Q = &qsp->pbtQ[qsp->iQ_Pos];
    const BYTE * S = &qsp->pbtS[qsp->iS_Pos];

    const int    scale_LH = scale->iQuantizer_H ;
    const int    scale_HL = scale->iQuantizer_H ;
    const int    scale_HH = scale->iQuantizer_HH;

    int16_t *    image = &layer->buffer[layer->free_line_pos*layer->block_size.cx];
    int16_t *    LHa = image + 1;
    int16_t *    LHb = LHa   + 2*layer->buffer_size.cx;

    int16_t *    HLa = image +   layer->buffer_size.cx;
    int16_t *    HLb = HLa   + 2*layer->buffer_size.cx;

    int16_t *    HHa = HLa   + 1;
    int16_t *    HHb = HHa   + 2*layer->buffer_size.cx;

    int          x, y;

    for(y = 0; y<rP_Size.cy; y++)
    {
      for(x = 0; x<nGroupsInRow; x++)
      {
        DEQUANT_BLOCK_QS21( P_LH, Q, S, scale_LH, LHa, LHb )
        DEQUANT_BLOCK_QS21( P_LH, Q, S, scale_LH, LHa, LHb )

        DEQUANT_BLOCK_QS2( P_HL, Q, S, scale_HL, HLa, HLb )
        DEQUANT_BLOCK_QS2( P_HL, Q, S, scale_HL, HLa, HLb )

        DEQUANT_BLOCK_QS2( P_HH, Q, S, scale_HH, HHa, HHb )
        DEQUANT_BLOCK_QS2( P_HH, Q, S, scale_HH, HHa, HHb )
      }

      for(x=0; x < nRest_x; x++)
      {
        DEQUANT_QS21( Q, S, scale_LH, LHa );
        DEQUANT_QS21( Q, S, scale_LH, LHb );

        DEQUANT_QS2( Q, S, scale_HL, HLa );
        DEQUANT_QS2( Q, S, scale_HL, HLb );

        DEQUANT_QS2( Q, S, scale_HH, HHa );
        DEQUANT_QS2( Q, S, scale_HH, HHb );
      }  

      LHa += Line_Step;
      LHb += Line_Step;  

      HLa += Line_Step;
      HLb += Line_Step;  

      HHa += Line_Step;
      HHb += Line_Step;  
    }

    if( nRest_y == 1 )
    {
      for(x = 0; x<nGroupsInRow; x++)
      {
        DEQUANT_FOUR_QS2( Q, S, scale_LH, LHa );
        DEQUANT_FOUR_QS2( Q, S, scale_HL, HLa );
        DEQUANT_FOUR_QS2( Q, S, scale_HH, HHa );
      }

      for(x=0; x < nRest_x; x++)
      {
        DEQUANT_QS2( Q, S, scale_LH, LHa );
        DEQUANT_QS2( Q, S, scale_HL, HLa );
        DEQUANT_QS2( Q, S, scale_HH, HHa );
      }  
    }

    qsp->iQ_Pos += Q    - &qsp->pbtQ[qsp->iQ_Pos];
    qsp->iS_Pos += S    - &qsp->pbtS[qsp->iS_Pos];
    qsp->iP_Pos += P_HH - &qsp->pbtP[qsp->iP_Pos];

    assert( qsp->iQ_Length == qsp->iQ_Pos );
    assert( qsp->iS_Length == qsp->iS_Pos );
    assert( qsp->iP_Length == qsp->iP_Pos );
  }

  layer->free_line_pos += lines_to_dequant;
  layer->loaded_lines  += lines_to_dequant;

  #ifdef PRINT_STRIPE_COUNTERS
    DPRINTF( DPRINTF_DEBUG, " d(%3d), free %3d\n"
            , layer->block_size.cx
            , layer->free_line_pos
           );
  #endif
}


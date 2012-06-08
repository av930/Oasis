/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: VLC.c

ABSTRACT:    This file contains procedures for VLC compression

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef VLC_H
#define VLC_H

#include "../bitstream/wci_bitstream.h"

//------------------------------------------------------------------------------

#ifdef ENCODER_SUPPORT
  #define VLC_FORWARD
#endif

#define VLC_SIZE                     256
#define VLC_MAX_CODE_LENGTH          17         //! max length of prefix code [bit]

#define VLC_PREFIX_LENGTH_TABLE_SIZE 128

//------------------------------------------------------------------------------

typedef struct tagVLC
{
	int iCode;                           //! prefix code
	int iLength;                         //! length of prefix code [bit]

} VLC;

typedef struct tagVLC_CONTEXT
{
  BYTE * pbtBS;
  int    iUnusedBits;
  int    iSkippedBits;
  uint   uIndex;

} VLC_CONTEXT;

//------------------------------------------------------------------------------

#ifdef VLC_FORWARD
  extern const VLC *g_prForwardVLC;
#endif

#ifdef VLC_INVERSE
  extern const VLC *g_prInverseVLC;
#endif

extern const uint8_t g_vlc_code_length_table[VLC_PREFIX_LENGTH_TABLE_SIZE];

//------------------------------------------------------------------------------

void wci_vlc_init_tables( void );

//------------------------------------------------------------------------------

#ifdef VLC_FORWARD

static void __inline wci_vlc_encode_buffer
(
  BYTE *      IN     pbtData           //! Input buffer of data
, int         IN     iDataLength       //! Length of data [byte]
, BITSTREAM * IN OUT prBS              //! Output bit stream of compressed data
)
{
  int    iIn = 0, iOut = 0;
  BYTE * pbtBS;
  int    iUsedBits = 0;
  uint   uIndex = 0;

  wci_bitstream_pad( prBS );

  pbtBS = wci_bitstream_lock_buffer( prBS );

  for ( iIn=0; iIn<iDataLength; iIn++ )
  {
    const VLC *pVLC = &g_prForwardVLC [ pbtData[iIn] ];
    uIndex = (uIndex << pVLC->iLength) | pVLC->iCode;
    iUsedBits += pVLC->iLength;
    if ( iUsedBits > (32 - VLC_MAX_CODE_LENGTH) )
    {
      do
      {
        pbtBS[iOut++] = (BYTE)(uIndex >> (iUsedBits - 8));
        iUsedBits -= 8;
      } while(iUsedBits > 8);
    }
  }

  while ( iUsedBits > 8 )
  {
    pbtBS[iOut++] = (BYTE)(uIndex >> (iUsedBits - 8));
    iUsedBits -= 8;
  }
  if ( iUsedBits > 0 )
  {
    pbtBS[iOut++] = (BYTE)(uIndex << (8 - iUsedBits));
  }

  wci_bitstream_unlock_buffer( prBS, pbtBS, iOut );
}

#endif // #ifdef VLC_FORWARD


//------------------------------------------------------------------------------

static int __inline wci_vlc_init_context
(
  const BITSTREAM * IN  prBS                //! Input bit stream of compressed data
, VLC_CONTEXT *     OUT prContext        
)
{
  prContext->pbtBS = (BYTE*)prBS->tail + prBS->pos/8;

  prContext->uIndex = *prContext->pbtBS++;
  prContext->uIndex = (prContext->uIndex << 8) | *prContext->pbtBS++;
  prContext->uIndex = (prContext->uIndex << 8) | *prContext->pbtBS++;
  prContext->uIndex = (prContext->uIndex << 8) | *prContext->pbtBS++;

  prContext->iUnusedBits = 32;
  prContext->iSkippedBits = 0;

  return 0;
}


//------------------------------------------------------------------------------

static int32_t __inline wci_vlc_decode_data
(
  VLC_CONTEXT* IN OUT prContext        
, int32_t      IN     iDataLength      //! Length of decompressed data [byte]
, uint8_t *    IN OUT pbtData          //! Output buffer of decompressed data
)
{
  int32_t unused_bits  = prContext->iUnusedBits;
  int32_t skipped_bits = prContext->iSkippedBits;
  uint32_t uIndex        = prContext->uIndex;

  while ( iDataLength--> 0 )
  {
    while ( unused_bits < VLC_MAX_CODE_LENGTH )
    {
      uIndex |= *prContext->pbtBS++ << (32 - unused_bits - 8);
      unused_bits += 8;
    }

    assert(uIndex != 0);

    if ( ((signed)uIndex) < 0 ) // special case, the most probable case
    {
      *pbtData++ = 0;
      unused_bits--;
      skipped_bits++;
      uIndex <<= 1;
    }
    else
    {
      int32_t code_length = g_vlc_code_length_table[uIndex >> 24];
      assert((uIndex >> 24) < VLC_PREFIX_LENGTH_TABLE_SIZE);
    
      *pbtData++ = (uint8_t)( (uIndex >> (32-code_length)) - 1);
      uIndex <<= code_length;

      {
        unused_bits -= code_length;
        skipped_bits += code_length;
      }
    }
  }

  prContext->iUnusedBits  = unused_bits;
  prContext->iSkippedBits = skipped_bits;
  prContext->uIndex       = uIndex;

  return 0;
}


//------------------------------------------------------------------------------

static uint __inline wci_vlc_get_compressed_size
(
  VLC_CONTEXT * IN prContext
)
{
  int iSkippedBytes = (prContext->iSkippedBits + 7)/8;
  return iSkippedBytes;
}

//------------------------------------------------------------------------------

static int __inline wci_vlc_decode_buffer
(
  BITSTREAM * IN OUT prBS              //! Input bit stream of compressed data
, int         IN     iDataLength       //! Length of decompressed data [byte]
, BYTE *      IN OUT pbtData           //! Output buffer of decompressed data
)
{
  VLC_CONTEXT rContext;
  int         err;

  wci_vlc_init_context( prBS, &rContext );

  err = wci_vlc_decode_data( &rContext, iDataLength, pbtData );
  if ( err != 0 )
    return err;

  wci_bitstream_skip_bits( prBS, 8*wci_vlc_get_compressed_size( &rContext ) );

  return 0;
}

//------------------------------------------------------------------------------

#endif // #ifndef VLC_H

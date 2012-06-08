/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: QSP_StripeDecompress.c

ABSTRACT:    This file contains procedures for decompression of QSP buffers

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include "../wci_portab.h"
#include "wci_qsp_header.h"
#include "wci_qsp_stripe_decompress.h"

//------------------------------------------------------------------------------

#define MEM_CHECK(p)   if( (p) == NULL ) return ERR_MEMORY

#define ERR_CHECK(err) if( (err) != ERR_NONE ) return err

//------------------------------------------------------------------------------

ERROR_CODE wci_qsp_get_table
(
  int         IN     nFlags
, BITSTREAM * IN OUT prBS
, HUFF_TABLE *   OUT prTable
)
{
  int err = ERR_NONE;

  if ( nFlags == QSP_CMP_HUFFMAN )
  {
    BYTE * pbtBitstream = (BYTE*)prBS->tail + prBS->pos/8;
    uint   nInSize = wci_bitstream_get_unused_length(prBS);
    uint   nOutLength = 0;
    
    err = wci_huff_decompress_table(pbtBitstream, nInSize, prTable, &nOutLength);

    wci_bitstream_skip_bits( prBS, nOutLength*8 );
  }
  return err;  
}

//------------------------------------------------------------------------------

void wci_qsp_copy_context_value
(
  QSP_COMPONENT_CONTEXT * IN  prSrc
, QSP_COMPONENT_CONTEXT * OUT prDst
)
{
  assert( prSrc->nFlags == prDst->nFlags );

  if ( prDst->prBS != NULL && prSrc->prBS != NULL )
    *prDst->prBS = *prSrc->prBS;
  if ( prDst->prVC != NULL && prSrc->prVC != NULL )
    *prDst->prVC = *prSrc->prVC;
  if ( prDst->prHC != NULL && prSrc->prHC != NULL )
    *prDst->prHC = *prSrc->prHC;
  if ( prDst->prRBC != NULL && prSrc->prRBC != NULL )
    *prDst->prRBC = *prSrc->prRBC;
}

//------------------------------------------------------------------------------

void wci_qsp_prepare_context
(
  const BITSTREAM *       IN     prBS
, int  nBlockSize
, QSP_COMPONENT_CONTEXT * IN OUT prContext
)
{
  switch( prContext->nFlags )
  {
  case QSP_CMP_BIT_RAW:
    *prContext->prBS = *prBS;
    break;

  case QSP_CMP_RAW:
    { 
      uint8_t* pbtData = (uint8_t*)prBS->start;
      prContext->pbtData = (uint8_t*) (pbtData + wci_bitstream_pos(prBS));
    break;
    }
  case QSP_CMP_RLE_VLC:
  case QSP_CMP_VLC:
    wci_vlc_init_context( prBS, prContext->prVC );
    break;

  case QSP_CMP_HUFFMAN:
  case QSP_CMP_RLE_HUFFMAN:
    {
      const BYTE * pbtBitstream = (const BYTE*)prBS->tail + prBS->pos/8;
      wci_huff_init_context( pbtBitstream, prContext->prHC );
    }
    break;
  }

  if ( prContext->prRBC != NULL )
    wci_rle_init_context( prContext->prRBC );

  prContext->nLength= nBlockSize;
  prContext->nPos= 0;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_rle_decompress_block_mode
(
   QSP_COMPONENT_CONTEXT * IN OUT prContext
 , int                     IN     iDataLength 
 , BYTE *                     OUT pbtData     //! The pointer to the output buffer
 )
{
  int nOutOffset = 0;

  while ( nOutOffset < iDataLength )
  {
    if ( prContext->prRBC->nCount == 0 || prContext->prRBC->nCount == -1 )
    {
      uint nBytesToRead = wci_rle_get_free_buffer_length( prContext->prRBC );
      nBytesToRead = min( nBytesToRead, prContext->nLength-prContext->nPos );

      if (nBytesToRead > 0)
      {
        BYTE abtData[RLE_CONTEXT_BUFFER_SIZE];
        memcpy(&abtData, &prContext->pbtData[prContext->nPos], nBytesToRead);
        prContext->nPos += nBytesToRead;

        wci_rle_fill_buffer( prContext->prRBC, abtData, nBytesToRead );  
      }
    }
    nOutOffset += wci_rle_decompress_buffer( prContext->prRBC, iDataLength-nOutOffset, &pbtData[nOutOffset] );
  }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_rle_vlc_decompress_byte_mode
(
 QSP_COMPONENT_CONTEXT * IN OUT prContext
, int                    IN     iDataLength 
, BYTE *                    OUT pbtData     //! The pointer to the output buffer
)
{
  int nOutOffset = 0;

  while ( nOutOffset < iDataLength )
  {
    if ( prContext->prRBC->nCount == 0 || prContext->prRBC->nCount == -1)
    {
       BYTE btData;

       if ( wci_vlc_decode_data( prContext->prVC, 1, &btData ) != 0 )
         return ERR_FORMAT;
       prContext->nPos += 1;
       wci_rle_fill_buffer( prContext->prRBC, &btData, 1 );
    }
    if ( pbtData != NULL )
      nOutOffset += wci_rle_decompress_bit_data( prContext->prRBC, iDataLength-nOutOffset, &pbtData[nOutOffset] );
    else
      nOutOffset += wci_rle_decompress_bit_data( prContext->prRBC, iDataLength-nOutOffset, NULL );
  }
  return ERR_NONE;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_rle_vlc_decompress_block_mode
(
  QSP_COMPONENT_CONTEXT * IN OUT prContext
, int                    IN     iDataLength 
, BYTE *                    OUT pbtData     //! The pointer to the output buffer
)
{
  int nOutOffset = 0;

  while ( nOutOffset < iDataLength )
  {
    if ( prContext->prRBC->nCount == 0 || prContext->prRBC->nCount == -1)
    {
      uint nBytesToRead = wci_rle_get_free_buffer_length( prContext->prRBC );
      nBytesToRead = min( nBytesToRead, prContext->nLength-prContext->nPos );
      if (nBytesToRead > 0)
      {
        BYTE abtData[RLE_CONTEXT_BUFFER_SIZE];

        if ( wci_vlc_decode_data( prContext->prVC, nBytesToRead, abtData ) != 0 )
          return ERR_FORMAT;
        prContext->nPos += nBytesToRead;

        wci_rle_fill_buffer( prContext->prRBC, abtData, nBytesToRead );  
      }
    }
    if ( pbtData != NULL )
      nOutOffset += wci_rle_decompress_bit_data( prContext->prRBC, iDataLength-nOutOffset, &pbtData[nOutOffset] );
    else
      nOutOffset += wci_rle_decompress_bit_data( prContext->prRBC, iDataLength-nOutOffset, NULL );
  }
  return ERR_NONE;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_qsp_decompress_data
(
  int                     IN     iCompressMode
, QSP_COMPONENT_CONTEXT * IN OUT prContext
, int                     IN     iDataLength 
, BYTE *                     OUT pbtData     //! The pointer to the output buffer
)
{
  if ( iDataLength > 0 )
  {
    switch( prContext->nFlags )
    {
    case QSP_CMP_BIT_RAW:
      if ( pbtData != NULL )
      {
        int iCycles = iDataLength/32;
        int iRest = iDataLength - iCycles*32;
        int iBit;

        static const uint32_t bits[16] =
        {
        #ifdef  WCI_BIG_ENDIAN
          0x00000000 //  0
        , 0x00000001 //  1
        , 0x00000100 //  2
        , 0x00000101 //  3
        , 0x00010000 //  4
        , 0x00010001 //  5
        , 0x00010100 //  6
        , 0x00010101 //  7
        , 0x01000000 //  8
        , 0x01000001 //  9
        , 0x01000100 // 10
        , 0x01000101 // 11
        , 0x01010000 // 12
        , 0x01010001 // 13
        , 0x01010100 // 14
        , 0x01010101 // 15
        #else
          0x00000000 //  0
        , 0x01000000 //  1
        , 0x00010000 //  2
        , 0x01010000 //  3
        , 0x00000100 //  4
        , 0x01000100 //  5
        , 0x00010100 //  6
        , 0x01010100 //  7
        , 0x00000001 //  8
        , 0x01000001 //  9
        , 0x00010001 // 10
        , 0x01010001 // 11
        , 0x00000101 // 12
        , 0x01000101 // 13
        , 0x00010101 // 14
        , 0x01010101 // 15
        #endif
        };

        { 
          uint32_t * pT = (uint32_t*)pbtData;
        while ( iCycles-- > 0 )
        {

          uint index = wci_getbits32( prContext->prBS );
            *pT++ = bits[(index >> 28) & 0xF];
            *pT++ = bits[(index >> 24) & 0xF];
            *pT++ = bits[(index >> 20) & 0xF];
            *pT++ = bits[(index >> 16) & 0xF];
            *pT++ = bits[(index >> 12) & 0xF];
            *pT++ = bits[(index >>  8) & 0xF];
            *pT++ = bits[(index >>  4) & 0xF];
            *pT++ = bits[(index      ) & 0xF];
          }
          pbtData = (BYTE*)pT;
        }

        for ( iBit=0; iBit<iRest; iBit++ )
		    {
  			  *pbtData++ = (BYTE)wci_bitstream_get_bit( prContext->prBS );
		    }
      }
      else
      {
        wci_bitstream_skip_bits( prContext->prBS, iDataLength );
      }
      break;

    case QSP_CMP_RAW:
      memcpy( pbtData, prContext->pbtData, iDataLength );
      prContext->pbtData += iDataLength;
      break;

    case QSP_CMP_VLC:
      if ( wci_vlc_decode_data( prContext->prVC, iDataLength, pbtData ) != 0 )
        return ERR_FORMAT;
      break;

    case QSP_CMP_HUFFMAN:
      if ( wci_huff_decompress_data( prContext->prTable, prContext->prHC, iDataLength, pbtData ) != 0 )
        return ERR_FORMAT;
      break;

    case QSP_CMP_RLE_VLC:
        if (iCompressMode & QSP_BLOCK_PACK_FLAG)
            return wci_rle_vlc_decompress_block_mode(prContext, iDataLength, pbtData);
        else
            return wci_rle_vlc_decompress_byte_mode(prContext, iDataLength, pbtData);
    break;

    default:
      return ERR_FORMAT;
    }
  }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

void wci_qsp_skip_component
(
  QSP_COMPONENT_CONTEXT * IN OUT prContext
, BITSTREAM *             IN OUT prBS
)
{
  uint nSize = 0;
  switch( prContext->nFlags )
  {
  case QSP_CMP_VLC:
    nSize = wci_vlc_get_compressed_size( prContext->prVC );
    break;

  case QSP_CMP_HUFFMAN:
    break;
  }
  wci_bitstream_skip_bits( prBS, nSize*8 );
}

//------------------------------------------------------------------------------

int wci_qsp_get_max_component_context_size()
{
  int iSize = 0;

  iSize = max( iSize, sizeof(BITSTREAM) );
  iSize = max( iSize, sizeof(RLE_BIT_CONTEXT) );
  iSize = max( iSize, sizeof(VLC_CONTEXT) );
  iSize = max( iSize, sizeof(VLC_CONTEXT) + sizeof(RLE_BIT_CONTEXT) );
  iSize = max( iSize, sizeof(HUFF_CONTEXT) );

  return iSize;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_qsp_add_component_context
(
  int                     IN     nFlags
, const HUFF_TABLE *      IN     prTable
, ALIGNED_MEMMANAGER *    IN OUT prMemHeap
, QSP_COMPONENT_CONTEXT *    OUT prContext
)
{
// This function allocates context buffer.
// Warning! Check wci_qsp_get_max_component_context_size() function after any changes.

  switch( nFlags )
  {
  case QSP_CMP_BIT_RAW:
    prContext->prBS = (BITSTREAM*) wci_aligned_malloc( sizeof(BITSTREAM), CACHE_LINE, prMemHeap );
    MEM_CHECK( prContext->prBS );
    break;
  case QSP_CMP_RAW:
    break;
  case QSP_CMP_RLE:
    prContext->prRBC = (RLE_BIT_CONTEXT*) wci_aligned_malloc( sizeof(RLE_BIT_CONTEXT), CACHE_LINE, prMemHeap );
    MEM_CHECK( prContext->prRBC );
    wci_rle_init_context( prContext->prRBC );
    break;
  case QSP_CMP_VLC:
    prContext->prVC = (VLC_CONTEXT*) wci_aligned_malloc( sizeof(VLC_CONTEXT), CACHE_LINE, prMemHeap );
    MEM_CHECK( prContext->prVC );
    break;
  case QSP_CMP_RLE_VLC:
    prContext->prVC = (VLC_CONTEXT*) wci_aligned_malloc( sizeof(VLC_CONTEXT), CACHE_LINE, prMemHeap );
    MEM_CHECK( prContext->prVC );
    prContext->prRBC = (RLE_BIT_CONTEXT*) wci_aligned_malloc( sizeof(RLE_BIT_CONTEXT), CACHE_LINE, prMemHeap );
    MEM_CHECK( prContext->prRBC );
    wci_rle_init_context( prContext->prRBC );
    break;
  case QSP_CMP_HUFFMAN:
    prContext->prHC = (HUFF_CONTEXT*) wci_aligned_malloc( sizeof(HUFF_CONTEXT), CACHE_LINE, prMemHeap );
    MEM_CHECK( prContext->prHC );
    prContext->prTable = prTable;
    break;
  }
  return ERR_NONE;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_qsp_init_component_context
(
  int                     IN     nFlags
, const HUFF_TABLE *      IN     prTable
, ALIGNED_MEMMANAGER *    IN OUT prMemHeap
, QSP_COMPONENT_CONTEXT *    OUT prContext
)
{
  memset( prContext, 0, sizeof(QSP_COMPONENT_CONTEXT) );

  prContext->nFlags = nFlags;

  return wci_qsp_add_component_context( nFlags, prTable, prMemHeap, prContext );
}

//------------------------------------------------------------------------------

void wci_qsp_set_component_context
(
  int                     IN     nFlags
, BITSTREAM *             IN     prBS
, VLC_CONTEXT *           IN     prVC
, HUFF_CONTEXT *          IN     prHC
, RLE_BIT_CONTEXT *       IN     prRBC
, const HUFF_TABLE *      IN     prTable
, QSP_COMPONENT_CONTEXT *    OUT prContext
)
{
  memset( prContext, 0, sizeof(QSP_COMPONENT_CONTEXT) );

  prContext->nFlags = nFlags;

  prContext->prBS = prBS;
  prContext->prVC = prVC;
  prContext->prHC = prHC;
  prContext->prRBC   = prRBC;
  prContext->prTable = prTable;
}

//------------------------------------------------------------------------------

void wci_qsp_done_component_context
(
  QSP_COMPONENT_CONTEXT * IN OUT prContext
, ALIGNED_MEMMANAGER *    IN OUT prMemHeap
)
{
  wci_aligned_free( prContext->prBS, prMemHeap );
  prContext->prBS = NULL;
  wci_aligned_free( prContext->prVC, prMemHeap );
  prContext->prVC = NULL;
  wci_aligned_free( prContext->prHC, prMemHeap );
  prContext->prHC = NULL;
  wci_aligned_free( prContext->prRBC, prMemHeap );
  prContext->prRBC = NULL;

  memset( prContext, 0, sizeof(QSP_COMPONENT_CONTEXT) );
}

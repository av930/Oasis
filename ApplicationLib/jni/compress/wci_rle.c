/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: RLE.c

ABSTRACT:    This file contains procedures for RL encoding

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "../wci_global.h"
#include "wci_rle.h"

//------------------------------------------------------------------------------

#define RLE_MAX_BIT_REPEATION 255
#define RLE_REPEATION_MODULE  (RLE_MAX_BIT_REPEATION+1)

//------------------------------------------------------------------------------

static BYTE __inline wci_rle_inverse_bit( BYTE btBit )
{
  if ( btBit != 0 )
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

//------------------------------------------------------------------------------

static void __inline  wci_rle_put_count(
  int   iSequenceLength               //! the RL sequence length
, BYTE *pbtCompressedData             //! the buffer for output data
, int * piOutLength )                 //! The output length
{
// This function puts bytes in the format of RLE
  int iOutLength = *piOutLength;

  if ( iSequenceLength < RLE_MAX_BIT_REPEATION )
  {                                              
    pbtCompressedData[iOutLength++] = (BYTE)iSequenceLength;
  }
  else
  {
    if ( iSequenceLength >= RLE_REPEATION_MODULE*RLE_MAX_BIT_REPEATION )
    {
      int iDecade3 =  iSequenceLength / (RLE_REPEATION_MODULE*RLE_REPEATION_MODULE);
      iSequenceLength -= iDecade3*(RLE_REPEATION_MODULE*RLE_REPEATION_MODULE);

      pbtCompressedData[iOutLength++] = (BYTE)RLE_MAX_BIT_REPEATION;
      pbtCompressedData[iOutLength++] = (BYTE)RLE_MAX_BIT_REPEATION;
      pbtCompressedData[iOutLength++] = (BYTE) iDecade3;
      pbtCompressedData[iOutLength++] = (BYTE)( iSequenceLength / RLE_REPEATION_MODULE );
      pbtCompressedData[iOutLength++] = (BYTE)( iSequenceLength % RLE_REPEATION_MODULE );
    }
    else
    {
      pbtCompressedData[iOutLength++] = (BYTE)RLE_MAX_BIT_REPEATION;
      pbtCompressedData[iOutLength++] = (BYTE)( iSequenceLength / RLE_REPEATION_MODULE );
      pbtCompressedData[iOutLength++] = (BYTE)( iSequenceLength % RLE_REPEATION_MODULE );
    }
  }
  
  *piOutLength = iOutLength;
}

//------------------------------------------------------------------------------

int wci_rle_compress_bit_buffer(
  BYTE *          pbtData              //! [in]  the input data buffer
, int             iDataLength          //! [in]  the length of input data
, BYTE *          pbtCompressedData    //! [out] the output data buffer
)
{
// This function performs RL compression of bit data.

  int  iIndex = 0, iStartIndex;
  int  iOutLength = 0;
  BYTE btBit;

  if (iDataLength > 0)
  {
    btBit = pbtData[ iIndex++ ];
    pbtCompressedData[iOutLength++] = btBit;

    while ( iIndex < iDataLength )
    {
      iStartIndex = iIndex;

      while ( btBit == pbtData[ iIndex ] )
      {
        if (++iIndex >= iDataLength)
        {
          wci_rle_put_count( iIndex - iStartIndex, pbtCompressedData, &iOutLength );
          return iOutLength;
        }
      }
      
      wci_rle_put_count( iIndex - iStartIndex, pbtCompressedData, &iOutLength );

      btBit = pbtData[ iIndex++ ];
    }
    
    pbtCompressedData[iOutLength++] = 0;
  }
  
  return iOutLength;
}

//------------------------------------------------------------------------------

int wci_rle_compress_buffer(
  BYTE *          pbtData              //! [in]  the input data buffer
, int             iDataLength          //! [in]  the length of input data
, BYTE *          pbtCompressedData    //! [out] the output data buffer
)
{
// This function performs RL compression of bit data.

  int  iIndex = 0, iStartIndex;
  int  iOutLength = 0;
  BYTE btChar;

  if (iDataLength > 0)
  {
    while ( iIndex < iDataLength )
    {
      btChar = pbtData[ iIndex++ ];
      pbtCompressedData[iOutLength++] = btChar;

      iStartIndex = iIndex;

      while ( btChar == pbtData[ iIndex ] )
      {
        if (++iIndex >= iDataLength)
        {
          wci_rle_put_count( iIndex - iStartIndex, pbtCompressedData, &iOutLength );
          return iOutLength;
        }
      }
      
      wci_rle_put_count( iIndex - iStartIndex, pbtCompressedData, &iOutLength );
    }
    
    pbtCompressedData[iOutLength++] = 0;
  }
  
  return iOutLength;
}

//------------------------------------------------------------------------------

int wci_rle_decompress_bit_buffer(
  const BYTE * IN  pbtCompressedData        //! Compressed data
, int          IN  iCompressedDataLength    //! Length of the compressed data [byte]
, BYTE *       OUT pbtData                  //! Buffer of uncompressed data.
                                            //! Value set of data buffer contains two values only
)
{
// This function decompress data to the bit buffer using  RL decoding

  int  iIndex = 0;
  int  iOutLength = 0;
  BYTE btChar;
  
  btChar = pbtCompressedData[ iIndex++ ];

  while ( iIndex < iCompressedDataLength )
  {
    BYTE btCount = pbtCompressedData[ iIndex++ ];

    if ( btCount != RLE_MAX_BIT_REPEATION )
    {
      pbtData[iOutLength++] = btChar;
      while (btCount-- > 0)
      {
        pbtData[iOutLength++] = btChar;
      }
    }
    else
    {
      int iCount;
      btCount = pbtCompressedData[ iIndex++ ];
      
      if ( btCount == RLE_MAX_BIT_REPEATION )
      {
        int iDecade3 = pbtCompressedData[ iIndex++ ];
        iCount  = iDecade3 * (RLE_REPEATION_MODULE*RLE_REPEATION_MODULE) ;
        iCount += pbtCompressedData[ iIndex++ ] * RLE_REPEATION_MODULE;
        iCount += pbtCompressedData[ iIndex++ ] + 1;
      }
      else
      {
        iCount  = btCount * RLE_REPEATION_MODULE;
        iCount += pbtCompressedData[ iIndex++ ] + 1;
      }
      
      memset( &pbtData[iOutLength], btChar, iCount );
      iOutLength += iCount;
    }
    
    if ( btChar != 0 )
    {
      btChar = 0;
    }
    else
    {
      btChar = 1;
    }
  }
  return iOutLength;
}

//------------------------------------------------------------------------------

// return used length of the compressed data
int wci_rle_decompress_bits(
  const BYTE * IN  pbtCompressedData        //! Compressed data
, int          IN  iUncompressedDataLength  //! Length of the uncompressed data [byte]
, BYTE *       OUT pbtData                  //! Buffer of uncompressed data.
)
{
  int  iIndex = 0;
  int  iOutLength = 0;
  BYTE btChar;

  btChar = pbtCompressedData[ iIndex++ ];

  while ( iOutLength < iUncompressedDataLength )
  {
    BYTE btCount = pbtCompressedData[ iIndex++ ];

    if ( btCount != RLE_MAX_BIT_REPEATION )
    {
      pbtData[iOutLength++] = btChar;
      while (btCount-- > 0)
      {
        pbtData[iOutLength++] = btChar;
      }
    }
    else
    {
      int iCount;
      btCount = pbtCompressedData[ iIndex++ ];

      if ( btCount == RLE_MAX_BIT_REPEATION )
      {
        int iDecade3 = pbtCompressedData[ iIndex++ ];
        iCount  = iDecade3 * (RLE_REPEATION_MODULE*RLE_REPEATION_MODULE) ;
        iCount += pbtCompressedData[ iIndex++ ] * RLE_REPEATION_MODULE;
        iCount += pbtCompressedData[ iIndex++ ] + 1;
      }
      else
      {
        iCount  = btCount * RLE_REPEATION_MODULE;
        iCount += pbtCompressedData[ iIndex++ ] + 1;
      }

      memset( &pbtData[iOutLength], btChar, iCount );
      iOutLength += iCount;
    }

    if ( btChar != 0 )
    {
      btChar = 0;
    }
    else
    {
      btChar = 1;
    }
  }

  return iIndex;
}

//------------------------------------------------------------------------------

// return length of the compressed data
int wci_rle_compress_bit_block(
  BYTE * pbtBitData                    //! [in]  Data buffer to compress
                                       //        Value set of data buffer contains two values only
, SIZE   rBlockSize                    //! [in]  Size of the bit block [byte]
, BYTE * pbtCompressedData             //! [out] Buffer of compressed data
)
{
  int  iCol, iWidth = rBlockSize.cx;
  int  iSequenceLength = -1;
  int  iOutLength = 0;
  BYTE *pbtBit, btBit;
  BYTE *pbtEnd;

  btBit = pbtBitData[ 0 ];
  pbtCompressedData[iOutLength++] = btBit;
  
  pbtEnd = pbtBitData + wci_get_square( rBlockSize );

  for ( iCol=0; iCol<iWidth; iCol++ )
  {
    pbtBit = pbtBitData + iCol;
    
    while ( pbtBit < pbtEnd )
    {
      if ( btBit != *pbtBit )
      {
        if ( iSequenceLength >= RLE_MAX_BIT_REPEATION )
        {                                              
          wci_rle_put_count( iSequenceLength, pbtCompressedData, &iOutLength );
        }
        else
        {
          pbtCompressedData[iOutLength++] = (BYTE)iSequenceLength;
        }
        iSequenceLength = 0;

        btBit = *pbtBit;
      }
      else
      {
        iSequenceLength++;
      }
      
      pbtBit += iWidth;
    }
  }

  if ( iSequenceLength >= RLE_MAX_BIT_REPEATION )
  {                                              
    wci_rle_put_count( iSequenceLength, pbtCompressedData, &iOutLength );
  }
  else
  {
    pbtCompressedData[iOutLength++] = (BYTE)iSequenceLength;
  }

  return iOutLength;
}

//------------------------------------------------------------------------------

static int __inline wci_rle_get_bit_count
(
  const BYTE * IN     pbtCompressedData
, int *        IN OUT piInLength
)
{
  int iCount = pbtCompressedData[ (*piInLength)++ ];

  if ( iCount == (int)RLE_MAX_BIT_REPEATION )
  {
    iCount = pbtCompressedData[ (*piInLength)++ ];

    if ( iCount == (int)RLE_MAX_BIT_REPEATION )
    {
      int iDecade3 = pbtCompressedData[ (*piInLength)++ ];
      iCount  = iDecade3 * (RLE_REPEATION_MODULE*RLE_REPEATION_MODULE) ;
      iCount += pbtCompressedData[ (*piInLength)++ ] * RLE_REPEATION_MODULE;
      iCount += pbtCompressedData[ (*piInLength)++ ];
    }
    else
    {
      iCount  = iCount * RLE_REPEATION_MODULE;
      iCount += pbtCompressedData[ (*piInLength)++ ];
    }
  }
  
  return iCount;
}

//------------------------------------------------------------------------------

// return used length of the compressed data
int wci_rle_decompress_bit_block(
  const BYTE * IN  pbtCompressedData        //! Compressed data
, SIZE         IN  rBlockSize               //! Size of the bit block [byte]
, BYTE *       OUT pbtBitData               //! Buffer of uncompressed data.
                                            //!    Value set of data buffer contains two values only
)
{
  int  iCol, iWidth = rBlockSize.cx;
  int  iSequenceLength = -1;
  int  iInLength = 0;
  BYTE *pbtBit, btBit;
  BYTE *pbtEnd;

  btBit = wci_rle_inverse_bit( pbtCompressedData[ iInLength++ ] );

  pbtEnd = pbtBitData + wci_get_square( rBlockSize );

  for ( iCol=0; iCol<iWidth; iCol++ )
  {
    pbtBit = pbtBitData + iCol;

    while ( pbtBit < pbtEnd )
    {
      if ( --iSequenceLength < 0 )
      {
        iSequenceLength = wci_rle_get_bit_count( pbtCompressedData, &iInLength );
        btBit = wci_rle_inverse_bit( btBit );
      }

      *pbtBit = btBit;

      pbtBit += iWidth;
    }
  }

  return iInLength;
}

//------------------------------------------------------------------------------

void wci_rle_init_context(
  RLE_BIT_CONTEXT * OUT prC
)
{
  memset( prC, 0, sizeof(RLE_BIT_CONTEXT) );
//  prC->btBit = RLE_MAX_BIT_REPEATION;
 prC->nCount = -1;
}

//------------------------------------------------------------------------------

void wci_rle_fill_buffer(
  RLE_BIT_CONTEXT * IN OUT prC
, const BYTE *      IN     pbtInData                //! Compressed data
, int               IN     nInLength                //! Length of the available compressed data [byte]
)
{
  assert( nInLength <= RLE_CONTEXT_BUFFER_SIZE );

  if ( prC->nInLength > 0 )
  {
    assert( prC->nInLength >= prC->nPos );
    prC->nInLength -= prC->nPos;
    memmove( prC->abtInData, &prC->abtInData[prC->nPos], prC->nInLength );
  }

  prC->nPos = 0;
  memcpy( &prC->abtInData[prC->nInLength], pbtInData, nInLength );
  prC->nInLength += nInLength;
}

//------------------------------------------------------------------------------

// return length of the uncompressed data
int wci_rle_decompress_bit_data(
  RLE_BIT_CONTEXT * IN OUT prC
, int               IN     nOutLength               //! Length of the uncompressed data [byte]
, BYTE *               OUT pbtOutData               //! Buffer of uncompressed data.
)
{
  int nOut = 0;

//  if( prC->btBit == RLE_MAX_BIT_REPEATION )
 if( prC->nCount < 0)
 {
    if ( prC->nPos >= prC->nInLength )
      return RLE_NEED_MORE_DATA;
    prC->btBit = prC->abtInData[ prC->nPos++ ];
    assert( prC->btBit == 0 || prC->btBit == 1 );
  }
  else
    if ( prC->nCount > 0 )
    {
      int nCount = min( nOutLength-nOut, prC->nCount );
      if ( pbtOutData != NULL )
        memset( &pbtOutData[nOut], prC->btBit, nCount );
      nOut += nCount;
      prC->nCount -= nCount;

      if ( prC->nCount == 0 )
        prC->btBit ^= 1;
    }

  while ( nOut < nOutLength )
  {
    int nCount;

    if ( prC->nPos >= prC->nInLength )
      break;
    nCount = prC->abtInData[ prC->nPos++ ];

    if ( nCount != RLE_MAX_BIT_REPEATION )
      nCount += 1;
    else
    {
      if ( prC->nPos+1 >= prC->nInLength )
      {
        prC->nPos -= 1;
        break;
      }
      nCount = prC->abtInData[ prC->nPos++ ];

      if ( nCount == RLE_MAX_BIT_REPEATION )
      {
        BYTE btDecade3;

        if ( prC->nPos+2 >= prC->nInLength )
        {
          prC->nPos -= 2;
          break;
        }

        btDecade3 = prC->abtInData[ prC->nPos++ ];
        nCount  = btDecade3 * (RLE_REPEATION_MODULE*RLE_REPEATION_MODULE);
        nCount += prC->abtInData[ prC->nPos++ ] * RLE_REPEATION_MODULE;
        nCount += prC->abtInData[ prC->nPos++ ] + 1;
      }
      else
      {
        nCount *= RLE_REPEATION_MODULE;
        nCount += prC->abtInData[ prC->nPos++ ] + 1;
      }
    }

    prC->nCount = nCount;
    nCount = min( nOutLength-nOut, prC->nCount );
    if ( pbtOutData != NULL )
      memset( &pbtOutData[nOut], prC->btBit, nCount );
    nOut += nCount;
    prC->nCount -= nCount;

    if ( prC->nCount == 0 )
      prC->btBit ^= 1;
  }

  return nOut;
}

//------------------------------------------------------------------------------

// return length of the uncompressed data
int wci_rle_decompress_buffer(
  RLE_BIT_CONTEXT * IN OUT prC
, int               IN     nOutLength               //! Length of the uncompressed data [byte]
, BYTE *               OUT pbtOutData               //! Buffer of uncompressed data.
)
{
  int nOut = 0;

  if( prC->nCount < 0)
  {
    if ( prC->nPos >= prC->nInLength )
      return RLE_NEED_MORE_DATA;
    prC->btBit = prC->abtInData[ prC->nPos++ ];
  }
  else
    if ( prC->nCount > 0 )
    {
      int nCount = min( nOutLength-nOut, prC->nCount );
      if ( pbtOutData != NULL )
        memset( &pbtOutData[nOut], prC->btBit, nCount );
      nOut += nCount;
      prC->nCount -= nCount;

      if ( prC->nCount == 0 )
      {
        if(prC->nPos == RLE_CONTEXT_BUFFER_SIZE)
        {
          prC->nCount = -1;
        }
        else
         prC->btBit = prC->abtInData[ prC->nPos++ ];
      }
    }

  while ( nOut < nOutLength )
  {
    int nCount;

    if ( prC->nPos >= prC->nInLength )
      break;
    nCount = prC->abtInData[ prC->nPos++ ];

    if ( nCount != RLE_MAX_BIT_REPEATION )
    {      
      nCount += 1;
    }
    else
    {
      if ( prC->nPos+1 >= prC->nInLength )
      {
        prC->nPos -= 1;
        break;
      }
      nCount = prC->abtInData[ prC->nPos++ ];

      if ( nCount == RLE_MAX_BIT_REPEATION )
      {
        BYTE btDecade3;

        if ( prC->nPos+2 >= prC->nInLength )
        {
          prC->nPos -= 2;
          break;
        }

        btDecade3 = prC->abtInData[ prC->nPos++ ];
        nCount  = btDecade3 * (RLE_REPEATION_MODULE*RLE_REPEATION_MODULE);
        nCount += prC->abtInData[ prC->nPos++ ] * RLE_REPEATION_MODULE;
        nCount += prC->abtInData[ prC->nPos++ ] + 1;
      }
      else
      {
        nCount *= RLE_REPEATION_MODULE;
        nCount += prC->abtInData[ prC->nPos++ ] + 1;
      }
    }

    prC->nCount = nCount;
    nCount = min( nOutLength-nOut, prC->nCount );
    if ( pbtOutData != NULL )
      memset( &pbtOutData[nOut], prC->btBit, nCount );
    nOut += nCount;
    prC->nCount -= nCount;

    if ( prC->nCount == 0 )
    {
      if(prC->nPos == RLE_CONTEXT_BUFFER_SIZE)
      {
        prC->nCount = -1;
        break;
      }
      else
        prC->btBit = prC->abtInData[ prC->nPos++ ];
    }

  }
  return nOut;
}

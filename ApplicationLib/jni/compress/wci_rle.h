/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_rle.h

ABSTRACT:    This file contains definitions and data structures for RL encoding

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef RLE_H
#define RLE_H

#include "../wci_portab.h"

//------------------------------------------------------------------------------

#define RLE_PREDEFINED_MARKER   0xFF

#define RLE_NEED_MORE_DATA      (-1)

#define RLE_CONTEXT_BUFFER_SIZE 32

//------------------------------------------------------------------------------

typedef enum
{
  eRLE_PredefinedMarker = 0,
  eRLE_MinMarker,
  eRLE_MaxMarker,
  eRLE_OnlyRepeatCounter,
  eRLE_BinaryData
  
} enumRLE_Mode;

typedef struct tagRLE_BIT_CONTEXT
{
  BYTE   btBit;

  int    nCount;

  BYTE   abtInData[RLE_CONTEXT_BUFFER_SIZE+1];
  int    nInLength;
  int    nPos;

} RLE_BIT_CONTEXT;

//------------------------------------------------------------------------------

BOOL wci_rle_find_min_marker_value(
  BYTE * pbtData                       //! [in]  Data buffer to compress
, int    iDataLength                   //! [in]  Length of the data [byte]
, BYTE * pbtMarker                     //! [out] Min marker value of sequence chunk 
);

//------------------------------------------------------------------------------

BOOL wci_rle_find_max_marker_value(
  BYTE * pbtData                       //! [in]  Data buffer to compress
, int    iDataLength                   //! [in]  Length of the data [byte]
, BYTE * pbtMarker                     //! [out] Max marker value of sequence chunk 
);

//------------------------------------------------------------------------------

// return length of the compressed data
/*int wci_rle_compress_buffer(
  BYTE * pbtData                       //! [in]  Data buffer to compress
, int    iDataLength                   //! [in]  Length of the data [byte]
, BYTE   btMarker                      //! [in]  Marker value of sequence chunk 
, BYTE * pbtCompressedData             //! [out] Buffer of compressed data
);
*/

int wci_rle_compress_buffer
(
  BYTE *          pbtData              //! [in]  the input data buffer
, int             iDataLength          //! [in]  the length of input data
, BYTE *          pbtCompressedData    //! [out] the output data buffer
);

//------------------------------------------------------------------------------

// return length of the compressed data
int wci_rle_compress_bit_buffer(
  BYTE * pbtBitData                    //! [in]  Data buffer to compress
                                       //        Value set of data buffer contains two values only
, int    iBitDataLength                //! [in]  Length of the data [byte]
, BYTE * pbtCompressedData             //! [out] Buffer of compressed data
);

//------------------------------------------------------------------------------

// return length of the uncompressed data
int wci_rle_decompress_buffer(
  RLE_BIT_CONTEXT * IN OUT prC
, int               IN     nOutLength               //! Length of the uncompressed data [byte]
, BYTE *               OUT pbtOutData               //! Buffer of uncompressed data.
);


//------------------------------------------------------------------------------

// return length of the uncompressed data
int wci_rle_decompress_bit_buffer(
  const BYTE * IN  pbtCompressedData        //! Compressed data
, int          IN  iCompressedDataLength    //! Length of the compressed data [byte]
, BYTE *       OUT pbtData                  //! Buffer of uncompressed data.
                                            //!    Value set of data buffer contains two values only
);

//------------------------------------------------------------------------------

// return used length of the compressed data
int wci_rle_decompress_bits(
  const BYTE * IN  pbtCompressedData        //! Compressed data
, int          IN  iUncompressedDataLength  //! Length of the uncompressed data [byte]
, BYTE *       OUT pbtData                  //! Buffer of uncompressed data.
                                            //!    Value set of data buffer contains two values only
);

//------------------------------------------------------------------------------

// return length of the compressed data
int wci_rle_compress_bit_block(
  BYTE * pbtBitData                    //! [in]  Data buffer to compress
                                       //        Value set of data buffer contains two values only
, SIZE   rBlockSize                    //! [in]  Size of the bit block [byte]
, BYTE * pbtCompressedData             //! [out] Buffer of compressed data
);

//------------------------------------------------------------------------------

// return used length of the compressed data
int wci_rle_decompress_bit_block(
  const BYTE * IN  pbtCompressedData        //! Compressed data
, SIZE         IN  rBlockSize               //! Size of the bit block [byte]
, BYTE *       OUT pbtBitData               //! Buffer of uncompressed data.
                                            //!    Value set of data buffer contains two values only
);

//------------------------------------------------------------------------------

void wci_rle_init_context(
  RLE_BIT_CONTEXT * OUT prC
);

//------------------------------------------------------------------------------

void wci_rle_fill_buffer(
  RLE_BIT_CONTEXT * IN OUT prC
, const BYTE *      IN     pbtInData                //! Compressed data
, int               IN     nInLength                //! Length of the available compressed data [byte]
);

//------------------------------------------------------------------------------

// return length of the uncompressed data
int wci_rle_decompress_bit_data(
  RLE_BIT_CONTEXT * IN OUT prC
, int               IN     nOutLength               //! Length of the uncompressed data [byte]
, BYTE *               OUT pbtOutData               //! Buffer of uncompressed data.
);

//------------------------------------------------------------------------------

 static int __inline wci_rle_get_free_buffer_length(
  RLE_BIT_CONTEXT * IN OUT prC
)
{
  return prC->nPos + RLE_CONTEXT_BUFFER_SIZE - prC->nInLength;
}

#endif // #ifndef RLE_H

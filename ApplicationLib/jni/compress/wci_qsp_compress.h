/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_qsp_compress.h

ABSTRACT:    This file contains definitions and data structures for compression of QSP buffers

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef QSP_COMPRESS_H
#define QSP_COMPRESS_H

#include "../wci_codec.h"
#include "../wci_global.h"
#include "../bitstream/wci_bitstream.h"
#include "wci_qsp.h"

//------------------------------------------------------------------------------

#define QSP_CMP_NONE        0x0
#define QSP_CMP_BIT_RAW     0x1
#define QSP_CMP_RAW         0x2
#define QSP_CMP_VLC         0x3
#define QSP_CMP_HUFFMAN     0x4
#define QSP_CMP_RLE         0x5
#define QSP_CMP_RLE_VLC     0x6
//#define QSP_CMP_ARITH     0x7
//#define QSP_CMP_ARITH0    0x8
//#define QSP_CMP_RANGE     0x9
#define QSP_CMP_RLE_HUFFMAN 0xA

#define QSP_CMP_MODE_MASK   0xF

#define QSP_Q_FLAG_SHIFT  8
#define QSP_S_FLAG_SHIFT  4
#define QSP_P_FLAG_SHIFT  0

//#define QSP_DEFAULT_MODE  0x416

#define QSP_Q_FLAG(field) ((field >> QSP_Q_FLAG_SHIFT) & QSP_CMP_MODE_MASK)
#define QSP_S_FLAG(field) ((field >> QSP_S_FLAG_SHIFT) & QSP_CMP_MODE_MASK)
#define QSP_P_FLAG(field) ((field >> QSP_P_FLAG_SHIFT) & QSP_CMP_MODE_MASK)

//------------------------------------------------------------------------------

typedef struct tagQSP_COMPRESSOR
{
  int    iMode;                  //! Compression mode

  BOOL   bExternalBuffer;        //! Memory allocation in external buffer
  BYTE * pbtBuffer;              //! Pointer to auxiliary buffer
  void * prCompressTable;        //! Pointer to the compress table   
} QSP_COMPRESSOR;

//------------------------------------------------------------------------------

int wci_qsp_init_compressor(
  QSP_COMPRESSOR *     OUT prCompressor      //! Instance pointer
, int                  IN  iMode             //! Compression mode
, BYTE *               IN  pbtExternalBuffer //! External buffer pointer
                                             //!   If it equals NULL then memory
                                             //!   allocation is internal.
, int                  IN  iBufferLength     //! External buffer length
, ALIGNED_MEMMANAGER * IN  prMemHeap         //! Embedded Mem Manager or NULL if system mem
);

//------------------------------------------------------------------------------

void wci_qsp_done_compressor(
  QSP_COMPRESSOR *     IN OUT prCompressor //! Instance pointer
, ALIGNED_MEMMANAGER * IN     prMemHeap    //! Embedded Mem Manager or NULL if system mem 
);

//------------------------------------------------------------------------------

void wci_qsp_compress_layer(
  QSP_COMPRESSOR *    IN OUT prCompressor      //! Instance pointer
, QSP_BUFFER *        IN     prQSP             //! QSP buffer to compress
, int                 IN     iMode             //! Compression mode
, int                 IN     iBlocks           //! Number of QSP blocks to compress
, const QSP_SIZE *    IN     prBlockSize       //! Size of QSP block
, BITSTREAM *         IN OUT prBS              //! Output bit stream of compressed data
, QSP_SIZE *             OUT prCompressedSize  //! Compression statistics
, WCI_ALPHA_CHANNEL *  IN     pAlphaChannel    //! The presence of A channel
);

//------------------------------------------------------------------------------

int wci_qsp_decompress_layer(
  QSP_COMPRESSOR *prCompressor                   //! [in]     Instance pointer
, BITSTREAM *     prBS                           //! [in/out] Input bit stream of compressed data
, QSP_BUFFER *    prQSP                          //! [out]    QSP buffer to decompress
);

#endif // QSP_COMPRESS_H


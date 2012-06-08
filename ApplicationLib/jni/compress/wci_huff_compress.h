/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: huff3_compress.h

ABSTRACT:    This file contains definitions and data structures for Huffman compression

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef HUFF3_COMPRESS_H
#define HUFF3_COMPRESS_H

#include "../wci_portab.h"

//------------------------------------------------------------------------------

#define SH_MAXLENGTH (31) 

typedef struct
{
  int typ;
  int n_of_table;
  uint  freq[1024];  
  BYTE	symb[256];
  BYTE	len[256];
  BYTE	code[256];
  BYTE  hz[1];
  BYTE  hnz[1];

} HUFF_PARAM;

//------------------------------------------------------------------------------

//Compress buffer of data
void wci_huff_compress_buffer(HUFF_PARAM *pHP,
  BYTE *pbInBuffer,  uint nInSize,
  BYTE *pbOutBuffer, uint *pnOutSize);

//------------------------------------------------------------------------------

//Build and compress Huffman tree table needed to compress data
//return 0 if OK or -1 if size of compressed Huffman tree table more than Out buffer size
int wci_huff_compress_table
(
  BYTE *       IN  pbtInBuffer //! Input buffer of data
, uint         IN  nInSize     //! Length of data [byte]
, HUFF_PARAM * OUT prTable     //! Huffman table contains data for compress data
, BYTE *       OUT pbtOutBuffer//! Out buffer where to put compressed table
, uint         OUT pnOutBufSize//! Size of Out buffer [byte]
, uint *       OUT pnTreeSize  //! Size of compressed table [byte]
);

//------------------------------------------------------------------------------

//Compress block of data
//return 0 if OK or -1 if size of compressed data size more than Out buffer size
int wci_huff_compress_data
(
  BYTE *       IN  pbtInBuffer //! Input buffer of data
, uint         IN  nInSize     //! Length of data [byte]
, HUFF_PARAM * OUT prTable     //! Huffman table contains data for compress data
, BYTE *       OUT pbtOutBuffer//! Out buffer where to put compressed table
, uint         OUT pnOutBufSize//! Size of Out buffer [byte]
, uint *       OUT pnOutSize   //! Size of compressed data [byte]
);

#endif // #ifndef HUFF3_COMPRESS_H

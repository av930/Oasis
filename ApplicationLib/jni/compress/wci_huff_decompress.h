/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: huff3_decompress.h

ABSTRACT:    This file contains definitions and data structures for Huffman decompression

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef HUFF3_DECOMPRESS_H
#define HUFF3_DECOMPRESS_H

#include "../wci_portab.h"

//------------------------------------------------------------------------------

#define SH_MAXLENGTH (31) 
#define SH_N         (256)

#define SH_CACHEBITS (8)
#define SH_CACHESIZE (1<<SH_CACHEBITS)

#define HUFF_KEYS

//------------------------------------------------------------------------------

typedef struct
{
  BYTE	  symb[SH_N];
  BYTE	  len[SH_N];
  BYTE	  code[SH_N];
  BYTE    cache[SH_CACHESIZE];
  BYTE    base[SH_MAXLENGTH];
  BYTE    offs[SH_MAXLENGTH];

  int32_t max_code_length;

#ifdef HUFF_KEYS
  int16_t key[SH_CACHESIZE];
#endif

} HUFF_TABLE;


typedef struct
{
  uint nUnused;               //! Offset inside Local proc buf
  uint nBuf;                  //! Local proc buf
  const BYTE * pbtBuf;        //! Current input buf pointer
} HUFF_CONTEXT;

//------------------------------------------------------------------------------

//Expand Huffman tree table needed to expand compressed data
//return 0 if OK or -1 if size of compressed data more than Input buffer size
int wci_huff_decompress_table
(
  BYTE *       IN  pbtInBuffer //! Input buffer of data
, uint         IN  nInSize     //! Length of data [byte]
, HUFF_TABLE * OUT prTable     //! Table contains data for expand compressed data
, uint *       OUT pnOutSize   //! Size of expanded table
);

//------------------------------------------------------------------------------

//Initialize context before expanding data
void wci_huff_init_context
(
  const BYTE *   IN  pbtInBuffer //! Input buffer of data
, HUFF_CONTEXT * OUT prContext   //! Context data
);

//------------------------------------------------------------------------------

// Decompress nSize bytes of data
// if pbOutBuffer == NULL Expand and Skip nSize bytes of data
// return 0 if OK or -1 if during processing exceeds Input buffer limit
int wci_huff_decompress_data
(
  const HUFF_TABLE *    OUT  prTable     //! Table contains data for expand compressed data
, HUFF_CONTEXT *     IN OUT  prContext   //! Context data contains current state of expander
, uint               IN      nSize       //! Length of data to expand [byte]
, BYTE         *        OUT  pbOutBuffer //! Output data buffer or NULL for skip
);

//------------------------------------------------------------------------------

// Decompress Buffer
int wci_huff_decompress_buffer
(  
  HUFF_TABLE   * OUT     prTable           //! Table for expand compressed data
, BYTE         * IN      pbtInBuffer       //! Input buffer containing compressed data
, uint           IN      nInSize           //! Length of Input buffer
, BYTE         * OUT     pbtOutBuffer      //! Output data buffer
, uint         * OUT     nOutSize          //! Length of Output data buffer [byte]
);

#endif // #ifndef HUFF3_DECOMPRESS_H

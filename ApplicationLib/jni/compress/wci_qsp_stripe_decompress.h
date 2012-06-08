/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_qsp_stripe_decomress.h

ABSTRACT:    This file contains definitions and data structures for decompression of QSP buffers

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef QSP_STRIPE_DECOMPRESS_H
#define QSP_STRIPE_DECOMPRESS_H

#include "../wci_codec.h"
#include "../utils/wci_mem_align.h"
#include "../bitstream/wci_bitstream.h"
#include "wci_huff_decompress.h"
#include "wci_rle.h"
#include "wci_vlc.h"
#include "wci_qsp.h"
#include "wci_qsp_compress.h"

//------------------------------------------------------------------------------

#define QSP_SUB_BLOCKS 4

//------------------------------------------------------------------------------

typedef struct tagQSP_TABLE
{
  HUFF_TABLE * prP;
  HUFF_TABLE * prQ;

} QSP_TABLE;

typedef struct tagQSP_COMPONENT_CONTEXT
{
  int                nFlags;

  uint8_t *          pbtData;
  BITSTREAM *        prBS;
  VLC_CONTEXT *      prVC;
  HUFF_CONTEXT *     prHC;

  RLE_BIT_CONTEXT *  prRBC;

  const HUFF_TABLE * prTable;
  uint               nLength;
  uint               nPos;

} QSP_COMPONENT_CONTEXT;

typedef struct tagQSP_CONTEXT
{
  SIZE                  rImageSize; //! Image size of QSP block [pel]

  int                   nCurrLine;  //! Current line to decompress

  QSP_SIZE              rQSP_Size;  //! Linear size of QSP block [byte]

  QSP_COMPRESSED_SIZE   rQSP_CompressedSize;  //! Compressed linear size of QSP block [byte]

  QSP_COMPONENT_CONTEXT rQ;
  QSP_COMPONENT_CONTEXT rS;
  QSP_COMPONENT_CONTEXT rP[QSP_SUB_BLOCKS];

} QSP_CONTEXT;

typedef struct tagQSP_STRIPE_DECOMPRESSOR
{
  int                 stripe;

  QSP_COMPRESS_HEADER rHeader;

  QSP_BUFFER          rQSP;

  QSP_TABLE           rTable;

  QSP_CONTEXT         arContext[MAX_IMAGE_COMPONENTS][QSP_MAX_BLOCKS/MAX_IMAGE_COMPONENTS];

  int16_t *           q_size;

  int16_t *           s_size;

} QSP_STRIPE_DECOMPRESSOR;

//------------------------------------------------------------------------------

ERROR_CODE wci_qsp_get_table
(
  int         IN     nFlags
, BITSTREAM * IN OUT prBS
, HUFF_TABLE *   OUT prTable
);

//------------------------------------------------------------------------------

void wci_qsp_copy_context_value
(
  QSP_COMPONENT_CONTEXT * IN  prSrc
, QSP_COMPONENT_CONTEXT * OUT prDst
);

//------------------------------------------------------------------------------

void wci_qsp_prepare_context
(
  const BITSTREAM *       IN OUT prBS
, int  nBlockSize
, QSP_COMPONENT_CONTEXT * IN OUT prContext
);

//------------------------------------------------------------------------------

ERROR_CODE wci_qsp_decompress_data
(
  int                     IN     iCompressMode
, QSP_COMPONENT_CONTEXT * IN OUT prContext
, int                     IN     iDataLength 
, BYTE *                     OUT pbtData     //! The pointer to the output buffer
);

//------------------------------------------------------------------------------

void wci_qsp_skip_component
(
  QSP_COMPONENT_CONTEXT * IN OUT prContext
, BITSTREAM *             IN OUT prBS
);

//------------------------------------------------------------------------------

ERROR_CODE wci_qsp_init_component_context
(
  int                     IN     nFlags
, const HUFF_TABLE *      IN     prTable
, ALIGNED_MEMMANAGER *    IN OUT prMemHeap
, QSP_COMPONENT_CONTEXT *    OUT prContext
);

void wci_qsp_done_component_context
(
  QSP_COMPONENT_CONTEXT * IN OUT prContext
, ALIGNED_MEMMANAGER *    IN OUT prMemHeap
);

//------------------------------------------------------------------------------

int wci_qsp_get_max_component_context_size(void);

//------------------------------------------------------------------------------

ERROR_CODE wci_qsp_add_component_context
(
  int                     IN     nFlags
, const HUFF_TABLE *      IN     prTable
, ALIGNED_MEMMANAGER *    IN OUT prMemHeap
, QSP_COMPONENT_CONTEXT *    OUT prContext
);

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
);

//------------------------------------------------------------------------------

ERROR_CODE wci_rle_decompress_block_mode
(
   QSP_COMPONENT_CONTEXT * IN OUT prContext
 , int                     IN     iDataLength 
 , BYTE *                     OUT pbtData     //! The pointer to the output buffer
);


#endif // #ifndef QSP_STRIPE_DECOMPRESS_H

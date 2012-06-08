/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: QSP_header.h

ABSTRACT:    This file contains definitions and data structures  for processing of QSP header

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef QSP_HEADER_H
#define QSP_HEADER_H

#include "../wci_global.h"
#include "../wci_codec.h"
#include "../bitstream/wci_bitstream.h"
#include "wci_qsp.h"

//------------------------------------------------------------------------------

#define QSP_MAX_BLOCKS      (5*MAX_IMAGE_COMPONENTS)

#define QSP_BLOCK_PACK_FLAG 0x1000
#define QSP_CRC             0x8000

//------------------------------------------------------------------------------

typedef struct tagQSP_COMPRESS_HEADER
{
  int        iCompressedLength;  //! Total length of compressed
                                 //!   data (pbtQ, pbtS, pbtP components) 
  int        iQSP_Length;        //! Total length of uncompressed
                                 //!   data (pbtQ, pbtS, pbtP components) 
  int        iCompressMode;      //! Compression component mode

  QSP_SIZE   rTotalSize;         //! Struct for storing of QSP lengths

  int        iBlocks;
  QSP_SIZE              rBlockSize[QSP_MAX_BLOCKS];            //! Struct for storing of QSP lengths
  QSP_COMPRESSED_SIZE   rCompressedBlockSize[QSP_MAX_BLOCKS];  //! Struct for storing of compressed QSP lengths
  
  int        iCRC;

} QSP_COMPRESS_HEADER;

//------------------------------------------------------------------------------

void wci_qsp_put_header(
  QSP_COMPRESS_HEADER * IN OUT prHeader //! The pointer to header structure
, BITSTREAM *           IN OUT prBS     //! The output bitstream
);

//------------------------------------------------------------------------------

int wci_qsp_get_header
(
  BITSTREAM          *  IN OUT prBS     //! The data bitstream
, QSP_COMPRESS_HEADER * IN OUT prHeader //! The pointer to header structure
);

//------------------------------------------------------------------------------

int wci_qsp_check_header
(
  const QSP_COMPRESS_HEADER * IN prHeader //! The pointer to header structure
, QSP_BUFFER *                IN prQSP    //! output QSP buffer
);

#endif // QSP_HEADER_H


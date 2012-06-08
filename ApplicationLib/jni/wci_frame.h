/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_frame.h

ABSTRACT:    This file contains definitions and data structures for frame/stream headers processing

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef FRAME_H
#define FRAME_H

#include "wci_portab.h"
#include "wci_codec.h"
#include "bitstream/wci_bitstream.h"

//------------------------------------------------------------------------------

#define PREAMBLE_PREFIX_SIZE 4
#define PREAMBLE_SIZE        8

#define PREAMBLE             "WICA"
#define STREAM_INFO_PREAMBLE PREAMBLE "si  "
#define FRAME_PREAMBLE       PREAMBLE "f   "
#define FRAME_CACHE_PREAMBLE PREAMBLE "fc  "
#define FAT_PREAMBLE         PREAMBLE "fat "

typedef enum tagATOM_TYPE
{
  AT_UNKNOWN     = 0
, AT_STREAM_INFO = 1
, AT_FRAME       = 2
, AT_FRAME_CACHE = 3
, AT_FAT         = 4

} ATOM_TYPE;

//------------------------------------------------------------------------------

typedef struct
{
  int      iLength;               //! Frame length          , sizeof = 32 bits

  VOP_TYPE eFrameType;            //! Frame type            , sizeof =  4 bits

  int      iRefLayer;             //! Reference layer       , sizeof =  4 bits

  int      iGlobalFlags;          //! Global flags          , sizeof = 32 bits  

  SIZE     rImageSize;            //! Image size [pixel]    , sizeof = 24 bits

  COLOR_SPACE eColorSpace;        //! Color space           , sizeof =  5 bits

  PIXEL_SAMPLING ePixelSampling;  //! Pixel sampling format , sizeof =  3 bits

                                  
                                  //! Optional fields

  int      iDuration;             //! Frame duration        , sizeof =  8 bits
  
  SIZE     rImageResolution;      //! Image resolution [DPI], sizeof = 24 bits

  int      iQuality;              //! Compression quality   , sizeof =  8 bits
  
  int      iMaxZoomOut;           //! Max zoom out for spatial scalability, sizeof = 8 bits

} FRAME_HEADER;                   //! sizeof = 14[+6] bytes


//------------------------------------------------------------------------------

void wci_put_frame_header
(
  const FRAME_HEADER * IN     prFrameHeader
, BITSTREAM *          IN OUT prBS
);

//------------------------------------------------------------------------------

ERROR_CODE wci_get_frame_header
(
  BITSTREAM *    IN OUT prBS
, FRAME_HEADER * OUT    prFrameHeader
);

//------------------------------------------------------------------------------

ERROR_CODE wci_check_frame_header(
  const FRAME_HEADER * IN prFrameHeader
);

#endif // #ifndef FRAME_H

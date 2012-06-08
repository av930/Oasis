/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: frame.c

ABSTRACT:    This file contains procedures for frame/stream headers processing

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include <string.h>

#include "wav_codec/wci_wav_codec.h"
#include "wci_frame.h"

//------------------------------------------------------------------------------

#define STREAM_INFO_LENGTH   (4+PREAMBLE_SIZE+1+2+2+2+2+1)

#define FRAME_DURATION_STEP  10

#define MAX_FRAMES           32767
#define MAX_FRAME_LENGTH     (16*1024+MAX_IMAGE_COMPONENTS*MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT)

//------------------------------------------------------------------------------
void wci_put_frame_header
(
  const FRAME_HEADER * IN     prFH
, BITSTREAM *          IN OUT prBS
)
{ 
  int i;
  const char *szFramePreamble = FRAME_PREAMBLE;

  wci_bitstream_put_bits( prBS, prFH->iLength   , 32 );

  for( i=0; i<PREAMBLE_SIZE; ++i )
    wci_bitstream_put_bits( prBS, szFramePreamble[i], 8 );

  wci_bitstream_put_bits( prBS, prFH->eFrameType & 0xF, 4 );
  wci_bitstream_put_bits( prBS, prFH->iRefLayer ,  4 );

  wci_bitstream_put_bits(prBS, prFH->iGlobalFlags, 32);

  wci_bitstream_put_bits( prBS, prFH->rImageSize.cx, 12 );
  wci_bitstream_put_bits( prBS, prFH->rImageSize.cy, 12 );

  {
    int iBit = prFH->eColorSpace & ~(CSP_NULL | CSP_VFLIP);
    int nCode = 0;

    assert( iBit != 0 );

    while ( (iBit & (1 << nCode)) == 0 )
    {
      nCode++;
      if ( nCode == 31 )
        break;
    }
    wci_bitstream_put_bits( prBS, nCode, 5 );
  }

  wci_bitstream_put_bits( prBS, prFH->ePixelSampling, 3 );

  wci_bitstream_put_bits( prBS, prFH->iQuality , 8 );

  wci_bitstream_put_bits( prBS, min( 255, prFH->iDuration/FRAME_DURATION_STEP ), 8 );

  wci_bitstream_put_bits( prBS, prFH->rImageResolution.cx, 12 );
  wci_bitstream_put_bits( prBS, prFH->rImageResolution.cy, 12 );

  wci_bitstream_put_bits( prBS, prFH->iMaxZoomOut, 8 );
}


//------------------------------------------------------------------------------

ERROR_CODE wci_get_frame_header
(
  BITSTREAM *    IN OUT prBS
, FRAME_HEADER * OUT    prFH
)
{
  int i;
  const char *szFramePreamble = FRAME_PREAMBLE;
  char        szPreamble[PREAMBLE_SIZE];

  prFH->iLength    = wci_bitstream_get_bits( prBS, 32 );

  for( i=0; i<PREAMBLE_SIZE; ++i )
    szPreamble[i] = (char) wci_bitstream_get_bits( prBS, 8 );

  if ( strncmp( szPreamble, szFramePreamble, PREAMBLE_SIZE ) != 0 )
  {
    return ERR_FORMAT;
  }
  
  prFH->eFrameType = wci_bitstream_get_bits( prBS,  4 );
  prFH->iRefLayer  = wci_bitstream_get_bits( prBS,  4 );

  prFH->iGlobalFlags = wci_bitstream_get_bits( prBS, 32 );

  prFH->rImageSize.cx = wci_bitstream_get_bits( prBS, 12 );
  prFH->rImageSize.cy = wci_bitstream_get_bits( prBS, 12 );

  prFH->eColorSpace   = 1 << wci_bitstream_get_bits( prBS, 5 );
  
  {
    int version = (prFH->iGlobalFlags >> (4*SF_VERSION_DECADE)) & 0xF;
    if (version == 1 && prFH->eColorSpace == CSP_BGRA)
      prFH->eColorSpace = CSP_BGR;
  }
  prFH->ePixelSampling = wci_bitstream_get_bits( prBS, 3 );

  prFH->iQuality            = wci_bitstream_get_bits( prBS,  8 );

  prFH->iDuration           = FRAME_DURATION_STEP*wci_bitstream_get_bits( prBS,  8 );

  prFH->rImageResolution.cx = wci_bitstream_get_bits( prBS, 12 );
  prFH->rImageResolution.cy = wci_bitstream_get_bits( prBS, 12 );

  prFH->iMaxZoomOut         = wci_bitstream_get_bits( prBS, 8 );
  
  return ERR_NONE;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_check_frame_header(
  const FRAME_HEADER * IN prFH
)
{
  int iVersion = (prFH->iGlobalFlags >> (4*SF_VERSION_DECADE)) & 0xF;
  if (iVersion > STREAM_FORMAT_VERSION)
  {
    return ERR_FORMAT;
  }

  if (prFH->iLength > MAX_FRAME_LENGTH)
  {
    return ERR_FORMAT;
  }

  switch ( prFH->eFrameType )
  {
  case I_VOP: case P_VOP: case B_VOP: case D_VOP:
    break;
  default:
    return ERR_FORMAT;
  }

  if ( prFH->iRefLayer > 2 )
  {
    return ERR_FORMAT;
  }

  if ( prFH->rImageSize.cx <= 0 )
  {
    return ERR_FORMAT;
  }

  if ( prFH->rImageSize.cx > MAX_IMAGE_WIDTH )
  {
    return ERR_FORMAT;
  }

  if ( prFH->rImageSize.cy <= 0 )
  {
    return ERR_FORMAT;
  }
  
  if ( prFH->rImageSize.cy > MAX_IMAGE_HEIGHT )
  {
    return ERR_FORMAT;
  }

  switch( prFH->ePixelSampling )
  {
  case PS_444:
  case PS_422: case PS_422_CO_SITED:
  case PS_420: case PS_420_CO_SITED:
  	break;
  default:
    return ERR_FORMAT;
  }

  if ( prFH->iQuality < MIN_QUALITY && prFH->iQuality > MAX_QUALITY )
  {
    return ERR_FORMAT;
  }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

int wci3_find_frame
(
  const STREAM *    IN     prStream
, FRAME_INFO *         OUT prInfo
)
{
  const char *szFramePreamble = FRAME_PREAMBLE;
  int         iOffset = prStream->iOffset;

  while ( iOffset + MAX_HEADER_SIZE <= prStream->iLength )
  {
    const char *szPreamble = (const char *) &prStream->pbtData[ iOffset + 4 ];

    if ( strncmp( szPreamble, szFramePreamble, PREAMBLE_SIZE ) == 0 )
    {
      FRAME_HEADER rFrameHeader;
      BITSTREAM    rBS, *prBS = &rBS;

	    wci_bitstream_init( prBS, &prStream->pbtData[ iOffset ], prStream->iLength-iOffset );
      
      if ( wci_get_frame_header( prBS, &rFrameHeader ) != ERR_NONE )
        return ERR_FORMAT;

      if(prInfo != NULL)
      {
        prInfo->iLength   = rFrameHeader.iLength;
        prInfo->iDuration = rFrameHeader.iDuration;
      }

      return iOffset;
    }

    iOffset++;
  }

  return -1;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_check_stream_info
(
  const STREAM_INFO * IN     prInfo
)
{
  if (prInfo->iMaxWidth < 0 || prInfo->iMaxWidth  > MAX_IMAGE_WIDTH )
    return ERR_PARAM;
  if (prInfo->iMaxHeight < 0 || prInfo->iMaxHeight > MAX_IMAGE_HEIGHT)
    return ERR_PARAM;
  if (prInfo->iFrames < 0 || prInfo->iFrames > MAX_FRAMES)
    return ERR_PARAM;
  if (prInfo->iFrameRate < 0 || prInfo->iFrameRate > MAX_FRAME_RATE)
    return ERR_PARAM;
  if (prInfo->iMaxZoomOut < 0 || prInfo->iMaxZoomOut > MAX_WAV_STEPS)
    return ERR_PARAM;
  return ERR_NONE;
}

//------------------------------------------------------------------------------

ERROR_CODE wci3_put_stream_info
(
  const STREAM_INFO * IN     prInfo
, STREAM *            IN OUT prStream
)
{
  int i;
  const char *szStreamInfoPreamble = STREAM_INFO_PREAMBLE;
  BITSTREAM  rBS, *prBS = &rBS;

  {
    int rc;
    if( (rc = wci_check_stream_info(prInfo)) != ERR_NONE)
      return rc;
  }

	wci_bitstream_init( prBS, prStream->pbtData, prStream->iLength );

  wci_bitstream_put_bits( prBS, STREAM_INFO_LENGTH , 32 );

  for( i=0; i<PREAMBLE_SIZE; ++i )
    wci_bitstream_put_bits( prBS, szStreamInfoPreamble[i], 8 );

  wci_bitstream_put_bits( prBS, prInfo->eFormat    ,  8 );
  wci_bitstream_put_bits( prBS, prInfo->iMaxWidth  , 16 );
  wci_bitstream_put_bits( prBS, prInfo->iMaxHeight , 16 );
  wci_bitstream_put_bits( prBS, prInfo->iFrames    , 16 );
  wci_bitstream_put_bits( prBS, prInfo->iFrameRate , 16 );
  wci_bitstream_put_bits( prBS, prInfo->iMaxZoomOut,  8 );

  wci_bitstream_flush( prBS );
  prStream->iOffset += wci_bitstream_pos( prBS );

  return ERR_NONE;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_scan_stream_info
(
  STREAM *          IN OUT prStream
, STREAM_INFO *     OUT    prInfo
)
{
  STREAM     rStream = *prStream;
  FRAME_INFO rFrameInfo;
  int        nTotalDuration = 0;
  int        iMinZoomOut = 0;

  memset( prInfo, 0, sizeof(STREAM_INFO) );

  while ( (rStream.iOffset = wci3_find_frame( &rStream, &rFrameInfo )) >= 0 )
  {
    FRAME_IMAGE_INFO rImageInfo;

    nTotalDuration += rFrameInfo.iDuration;

    if ( wci3_get_frame_info( &rStream, &rImageInfo ) == ERR_NONE )
    {
      prInfo->iFrames++;
  		prInfo->iMaxWidth  = max( prInfo->iMaxWidth , rImageInfo.iWidth  );
  		prInfo->iMaxHeight = max( prInfo->iMaxHeight, rImageInfo.iHeight );
      if ( iMinZoomOut == 0 )
        iMinZoomOut = rImageInfo.iMaxZoomOut;
      else
        iMinZoomOut = min( iMinZoomOut, rImageInfo.iMaxZoomOut );
    }
    else
    {
      return ERR_FORMAT;
    }

    rStream.iOffset += rFrameInfo.iLength;
  }

  if ( prInfo->iFrames > 0 )
  {
    prInfo->iMaxZoomOut = iMinZoomOut;

    if ( nTotalDuration > 0 )
      prInfo->iFrameRate = (1000*prInfo->iFrames) / (10*nTotalDuration);
    if ( prInfo->iFrames == 1 )
      prInfo->eFormat = SF_STILL_IMAGE_V1;
    else
      prInfo->eFormat = SF_IMAGE_SEQUENCE_V1;
  }
  else
  {
    prInfo->eFormat = SF_UNKNOUWN;
  }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

ATOM_TYPE wci_get_preamble(
  STREAM *          IN OUT prStream
)
{
  int iOffset = prStream->iOffset;

  while ( iOffset + MAX_HEADER_SIZE <= prStream->iLength )
  {
    const char * szPreamble = (const char *)&prStream->pbtData[ iOffset + PREAMBLE_PREFIX_SIZE ];

    if ( strncmp( szPreamble, PREAMBLE, PREAMBLE_PREFIX_SIZE ) == 0 )
    {
      ATOM_TYPE type = AT_UNKNOWN;
      
           if( strncmp( szPreamble, FRAME_PREAMBLE      , PREAMBLE_SIZE ) == 0 ) type = AT_FRAME;
      else if( strncmp( szPreamble, STREAM_INFO_PREAMBLE, PREAMBLE_SIZE ) == 0 ) type = AT_STREAM_INFO;
      else if( strncmp( szPreamble, FRAME_CACHE_PREAMBLE, PREAMBLE_SIZE ) == 0 ) type = AT_FRAME_CACHE;
      else if( strncmp( szPreamble, FAT_PREAMBLE        , PREAMBLE_SIZE ) == 0 ) type = AT_FAT;

      if (type != AT_UNKNOWN)
      {
        prStream->iOffset = iOffset;
      }
      
      return type;
    }

    iOffset++;
  }
  return AT_UNKNOWN;
}

//------------------------------------------------------------------------------

ERROR_CODE wci3_get_stream_info
(
  STREAM *          IN OUT prStream
, STREAM_INFO *     OUT    prInfo
)
{
  if(wci_get_preamble(prStream) == AT_STREAM_INFO)
  {
    int         iOffset = prStream->iOffset;
    BITSTREAM   rBS, *prBS = &rBS;

    wci_bitstream_init( prBS, &prStream->pbtData[ iOffset ], prStream->iLength-iOffset );

    if ( wci_bitstream_get_bits( prBS, 32 ) != STREAM_INFO_LENGTH )
      return ERR_FORMAT;

    wci_bitstream_skip_bits( prBS, 8*8 );

    prInfo->eFormat     = wci_bitstream_get_bits( prBS,  8 );
    prInfo->iMaxWidth   = wci_bitstream_get_bits( prBS, 16 );
    prInfo->iMaxHeight  = wci_bitstream_get_bits( prBS, 16 );
    prInfo->iFrames     = wci_bitstream_get_bits( prBS, 16 );
    prInfo->iFrameRate  = wci_bitstream_get_bits( prBS, 16 );
    prInfo->iMaxZoomOut = wci_bitstream_get_bits( prBS,  8 );

    prStream->iOffset = iOffset + STREAM_INFO_LENGTH - 1;

    return wci_check_stream_info(prInfo);
  }
  return wci_scan_stream_info( prStream, prInfo );
}

//------------------------------------------------------------------------------

ERROR_CODE wci3_get_frame_info
(
  const STREAM *     IN OUT prStream
, FRAME_IMAGE_INFO *    OUT prInfo
)
{
  int             iOffset;
  FRAME_INFO      rInfo;
  BITSTREAM       rBS;
  FRAME_HEADER    rFrameHeader;

  if ( ( iOffset = wci3_find_frame( prStream, &rInfo ) ) < 0 )
    return ERR_FORMAT;
  
  memset( prInfo, 0, sizeof(FRAME_IMAGE_INFO) );

  wci_bitstream_init(&rBS, &prStream->pbtData[iOffset], prStream->iLength);

  wci_get_frame_header( &rBS, &rFrameHeader );

  if ( wci_check_frame_header( &rFrameHeader ) != ERR_NONE )
    return ERR_FORMAT;

  if ( rFrameHeader.eFrameType != I_VOP )
    return ERR_FORMAT;

  prInfo->iWidth      = rFrameHeader.rImageSize.cx;
  prInfo->iHeight     = rFrameHeader.rImageSize.cy;

  prInfo->iResolutionHorz = rFrameHeader.rImageResolution.cx;
  prInfo->iResolutionVert = rFrameHeader.rImageResolution.cy;

  prInfo->eColorSpace = rFrameHeader.eColorSpace;

  prInfo->iImageSize = wci_image_get_size( rFrameHeader.rImageSize, rFrameHeader.eColorSpace );

  prInfo->iMaxZoomOut = rFrameHeader.iMaxZoomOut;

  prInfo->iQuality   = rFrameHeader.iQuality;

  return ERR_NONE;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_check_fat(
  const FAT         * IN     prFAT
)
{
  if (prFAT->iFrames < 0 || prFAT->iFrames  > MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT )
    return ERR_PARAM;
  if (prFAT->iMaxOffsets < prFAT->iFrames ||  prFAT->iMaxOffsets > MAX_IMAGE_WIDTH*MAX_IMAGE_HEIGHT )
    return ERR_PARAM;
  if (prFAT->piOffset == NULL)
    return ERR_MEMORY;
  return ERR_NONE;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_get_fat
(
  STREAM *          IN OUT prStream
, FAT *             IN OUT prFAT
)
{
  ATOM_TYPE type;  

  for( type=wci_get_preamble(prStream); type!=AT_UNKNOWN; type=wci_get_preamble(prStream) )
  {
    if (type == AT_UNKNOWN)
      break;
    if (type == AT_FRAME)
      break;
    {
      BITSTREAM rBS;

      wci_bitstream_init(&rBS, &prStream->pbtData[prStream->iOffset], prStream->iLength-prStream->iOffset);
      prStream->iOffset += wci_bitstream_get_bits( &rBS, 32 ) - 4;
      
      if (type == AT_FAT)
      {
        wci_bitstream_skip_bits( &rBS, PREAMBLE_SIZE*8 );

        prFAT->iFrames = wci_bitstream_get_bits( &rBS, 32 );
        {
          ERROR_CODE rc;
          if( (rc = wci_check_fat(prFAT)) != ERR_NONE)
            return rc;
        }
        {
          int i, offsets = min(prFAT->iFrames, prFAT->iMaxOffsets);
          for (i=0; i<offsets; ++i)
            prFAT->piOffset[i] = wci_bitstream_get_bits( &rBS, 32 );
        }
        return ERR_NONE;
      }
    }
  }

  return ERR_FORMAT;
}

//------------------------------------------------------------------------------

ERROR_CODE wci_put_fat
(
  const FAT         * IN     prFAT
, STREAM *            IN OUT prStream
)
{
  const char * szStreamInfoPreamble = FAT_PREAMBLE;
  BITSTREAM   rBS, *prBS = &rBS;

  {
    ERROR_CODE rc;
    if( (rc = wci_check_fat(prFAT)) != ERR_NONE)
      return rc;
  }

	wci_bitstream_init( prBS, &prStream->pbtData[prStream->iOffset], prStream->iLength-prStream->iOffset );

  {
    int length = 4+PREAMBLE_SIZE+4+prFAT->iFrames*4;
    wci_bitstream_put_bits( prBS, length, 32 );
  }

  {
    int i;
    for( i=0; i<PREAMBLE_SIZE; ++i )
      wci_bitstream_put_bits( prBS, szStreamInfoPreamble[i], 8 );
  }

  wci_bitstream_put_bits( prBS, prFAT->iFrames, 32 );
  {
    int i;
    for (i=0; i<prFAT->iFrames; ++i)
      wci_bitstream_put_bits( prBS, prFAT->piOffset[i], 32 );
  }

  wci_bitstream_flush( prBS );
  prStream->iOffset += wci_bitstream_pos( prBS );

  return ERR_NONE;
}


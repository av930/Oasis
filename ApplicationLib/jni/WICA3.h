/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: WICA.h

ABSTRACT:    This file defines WICA codec API.
             For details see file "WICA API.doc".

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef WICA3_API_H
#define WICA3_API_H

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
//  API constants

#define WICA3_API_VERSION       11

//------------------------------------------------------------------------------
// Global restrictive constants

#define WCI3_MAX_IMAGE_WIDTH    1920
#define WCI3_MAX_IMAGE_HEIGHT   1280

#define WCI3_MAX_HEADER_SIZE    32

#define WCI3_MIN_QUALITY        0       // Image quality constants
#define WCI3_MAX_QUALITY        100

#define WCI3_LOWEST_QUALITY     10
#define WCI3_LOW_QUALITY        30
#define WCI3_NORMAL_QUALITY     50
#define WCI3_HIGH_QUALITY       70
#define WCI3_BEST_QUALITY       90
#define WCI3_LOSSLESS_QUALITY   100

#define WCI3_MAX_ZOOM_OUT       4

#define WCI3_MAX_FRAME_RATE     30

//------------------------------------------------------------------------------
//  WCI3 stream formats

typedef enum tagWCI3_STREAM_FORMAT
{
  WCI3_SF_UNKNOUWN          = -1,
  WCI3_SF_STILL_IMAGE_V1    =  0,
  WCI3_SF_IMAGE_SEQUENCE_V1 =  1,
  WCI3_SF_VIDEO_V1          =  2,
  WCI3_SF_STILL_IMAGE_V2    =  4,
  WCI3_SF_IMAGE_SEQUENCE_V2 =  5

} WCI3_STREAM_FORMAT;

//------------------------------------------------------------------------------
//  Color space constants

#define WCI3_MAX_IMAGE_COMPONENTS 4

typedef enum tagWCI3_COLOR_SPACE
{
  WCI3_CSP_AUTO      = 0
, WCI3_CSP_NULL      = 0

, WCI3_CSP_YV12      = (1<< 2)          // 4:2:0 planar
, WCI3_CSP_YV24      = (1<< 3)          // 4:4:4 planar

, WCI3_CSP_BGRA      = (1<< 7)          // 32-bit BGRA packed
, WCI3_CSP_BGR       = (1<<11)          // 24-bit BGR packed
, WCI3_CSP_RGB565    = (1<<14)          // 16-bit RGB565 packed into 16 bit
, WCI3_CSP_GRAYSCALE = (1<<16)          // only luma component
, WCI3_CSP_RGB56588  = (1<<17)          // 16-bit RGB565 and alpha channel packed into 32 bit
, WCI3_CSP_YV12A     = (1<<23)          // 4:2:0 planar with alpha channel
, WCI3_CSP_YV24A     = (1<<24)          // 4:4:4 planar with alpha channel

, WCI3_CSP_VFLIP     = (1<<30)

} WCI3_COLOR_SPACE;

//------------------------------------------------------------------------------
// Error codes

#define WCI3_ERR_NONE     0             // Operation succeed.

#define WCI3_ERR_MEMORY   1             // Operation failed. Available memory is not enough.

#define WCI3_ERR_FORMAT   2             // Operation failed. The input stream is incorrect.

#define WCI3_ERR_PARAM    3             // Operation failed. The value of the parameters were incorrect.

#define WCI3_ERR_FAIL   (-1)            // Operation failed.

typedef int WCI3_ERROR_CODE;

//------------------------------------------------------------------------------
// WCI3 structs

typedef struct
{
  int   iAPI_version;                  //  Codec API version number
  int   iCoreBuild;                    //  Codec core build number
  int   iStreamFormat;                 //  Codec supported stream format

} WCI3_CODEC_VERSION;

typedef struct tagWCI3_STREAM_INFO
{
  WCI3_STREAM_FORMAT eFormat;           // Stream format, version
  int				iMaxWidth;                 // Image Max width [pixel]
  int				iMaxHeight;                // Image Max height [pixel]
  int       iFrames;                   // Number of frames in the stream
  int       iFrameRate;                // Averaged frame rate [FPS]
  int       iMaxZoomOut;               // Max available value of zoom out for all frames

} WCI3_STREAM_INFO;

typedef struct tagWCI3_FAT
{
  int       iFrames;                   // Number of frames in the stream
  int *     piOffset;                  // Frame offsets
  int       iMaxOffsets;               // Max size of offset buffer

} WCI3_FAT;

typedef struct tagWCI3_IMAGE_INFO
{
  int       iWidth;                    // Image width [pixel]
  int       iHeight;                   // Image height [pixel]
  int       iResolutionHorz;           // Horizontal image resolution [DPI]
  int       iResolutionVert;           // Vertical image resolution [DPI]
  WCI3_COLOR_SPACE eColorSpace;         // Image color space
  int       iImageSize;                // Size of the image [bytes]
  int       iMaxZoomOut;               // Max available value of zoom out
  int       iQuality;                  // Image quality

} WCI3_IMAGE_INFO;

typedef struct tagWCI3_FRAME_INFO
{
  int       iLength;                   // Length of the frame [bytes]
  int       iDuration;                 // Display duration of the frame [ms]

} WCI3_FRAME_INFO;

typedef struct tagWCI3_STREAM
{
  unsigned char * pbtData;             // [in] Input bitstream data buffer pointer
  int       iLength;                   // [in] Input bitstream length [bytes]
  int       iOffset;                   // [in\out] Size of processed data [bytes]

} WCI3_STREAM;

typedef enum tagWCI3_DECODING_SPEED
{
  WCI3_DS_NORMAL = 0
, WCI3_DS_FAST   = 1
, WCI3_DS_MAX    = 5

} WCI3_DECODING_SPEED;

typedef struct tagWCI3_ENCODE_PARAM
{
  int                iQuality;         // Encoding quality
  WCI3_DECODING_SPEED eSpeed;           // Decoding speed
  WCI3_COLOR_SPACE    eColorSpace;      // Color space of encoded image

} WCI3_ENCODE_PARAM;

typedef struct tagWCI3_RECT
{
  int       iLeft;
  int       iTop;
  int       iRight;
  int       iBottom;

} WCI3_RECT;

typedef enum tagWCI3_SPATIAL_SCALABILITY_MODE
{
  WCI3_SSM_NONE   = 0
, WCI3_SSM_ZOOM   = 1
, WCI3_SSM_SCROLL = 2

} WCI3_SPATIAL_SCALABILITY_MODE;

typedef struct tagWCI3_SPATIAL_SCALABILITY
{
  WCI3_SPATIAL_SCALABILITY_MODE eMode;       //! [in] Scalability mode
  int                          iZoomOut;    //! [in] Value of spatial zooming (4^n = 1,4,16,64)
  WCI3_RECT                     rClipRect;   //! [in] Clipping rect of image

} WCI3_SPATIAL_SCALABILITY;

typedef struct tagWCI3_DECODE_PARAM
{
  unsigned char *         pbtImage;         //! [in] Pointer to an decoded image
  WCI3_COLOR_SPACE         eColorSpace;      //! [in] Required color space format of the decoded image
  WCI3_SPATIAL_SCALABILITY rScalability;     //! [in] Spatial scalability parameters
  unsigned char *	        pbtProcBuf;	      //! [in] Temporal processing buffer pointer
  int                     iProcBufSize;     //! [in] Temporal processing buffer size [bytes]

} WCI3_DECODE_PARAM;

//------------------------------------------------------------------------------
// Prototypes of the Exported Functions:

#ifndef IN
  #define IN
#endif

#ifndef OUT
  #define OUT
#endif

#define  WCI3_GetVersion            wci3_get_version
#define  WCI3_GetStreamInfo         wci3_get_stream_info
#define  WCI3_FindFrame             wci3_find_frame
#define  WCI3_GetFrameInfo          wci3_get_frame_info
#define  WCI3_GetDecoderBufferSize  wci3_get_decoder_buffer_size
#define  WCI3_DecodeFrame           wci3_decode_frame
#define  WCI3_PutStreamInfo         wci3_put_stream_info
#define  WCI3_EncodeFrame           wci3_encode_frame

//------------------------------------------------------------------------------

WCI3_ERROR_CODE wci3_get_version
(
  WCI3_CODEC_VERSION * OUT    prCodecVersion
);

//------------------------------------------------------------------------------

WCI3_ERROR_CODE wci3_get_stream_info
(
  WCI3_STREAM *          IN OUT prStream
, WCI3_STREAM_INFO *     OUT    prInfo
);

//------------------------------------------------------------------------------

WCI3_ERROR_CODE wci_get_fat
(
  WCI3_STREAM *          IN OUT prStream
, WCI3_FAT *             IN OUT prFAT
);

//------------------------------------------------------------------------------

int wci3_find_frame
(
  const WCI3_STREAM *    IN     prStream
, WCI3_FRAME_INFO *         OUT prInfo
);

//------------------------------------------------------------------------------

WCI3_ERROR_CODE wci3_get_frame_info
(
  WCI3_STREAM *          IN OUT prStream
, WCI3_IMAGE_INFO *      OUT    prInfo
);

//------------------------------------------------------------------------------

int wci3_get_decoder_buffer_size
(
  const WCI3_IMAGE_INFO * IN prInfo
, int                    IN iZoomOut
);

//------------------------------------------------------------------------------

WCI3_ERROR_CODE wci3_decode_frame
(
  const WCI3_DECODE_PARAM * IN     prParam
, WCI3_STREAM *             IN OUT prStream
, WCI3_IMAGE_INFO *         OUT    prInfo         //! Decoded image information
);

//------------------------------------------------------------------------------

WCI3_ERROR_CODE wci3_put_stream_info
(
  const WCI3_STREAM_INFO * IN     prInfo
, WCI3_STREAM *            IN OUT prStream
);

//------------------------------------------------------------------------------

WCI3_ERROR_CODE wci_put_fat
(
  const WCI3_FAT *         IN     prFAT
, WCI3_STREAM *            IN OUT prStream
);

//------------------------------------------------------------------------------

WCI3_ERROR_CODE wci3_encode_frame
(
  const WCI3_ENCODE_PARAM * IN     prParam
, const unsigned char *    IN     pbtImage       //! Pointer to an encoded image
, const WCI3_IMAGE_INFO *   IN     prInfo         //! Decoded image information
, WCI3_STREAM *             IN OUT prStream
);

//------------------------------------------------------------------------------

int wci_image_get_size
(
  SIZE            IN rSize
, WCI3_COLOR_SPACE IN eColorSpace
);


#ifdef __cplusplus
}
#endif

#endif // #ifndef WICA3_API_H

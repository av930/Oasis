/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: codeh.c

ABSTRACT:    This file contains codec definitions and data structures

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,
					   Alexander Ivanov       <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00
             2004.08.27   v1.01 Changed: Setting quality scale
                                Added:   Quantization tables
                                Fixed:   Index for zoom out calculating
             2004.09.02   v1.02 Changed: Calculation of the number of wavelet transform steps, image size alignment
                                Fixed:   Color space conversion for images with odd width
             2005.04.08   v1.03 Changed: Calculation image size alignment
             2005.04.29   v1.18 Added:   Color space RGB565

*******************************************************************************/

#ifndef CODEC_H
#define CODEC_H

#include "wci_portab.h"

//------------------------------------------------------------------------------
// Version

#define CORE_BUILD            158

#define API_VERSION           11

#define STREAM_FORMAT_VERSION 2

//------------------------------------------------------------------------------
// Global restrictive constants

#define MAX_IMAGE_WIDTH       1920
#define MAX_IMAGE_HEIGHT      1280

#define MAX_HEADER_SIZE       32

#define MIN_QUALITY           0                  //! Image quality constants
#define MAX_QUALITY           100
#define QUALITY_STEP          10

#define LOWEST_QUALITY        10
#define LOW_QUALITY           30
#define NORMAL_QUALITY        50
#define HIGH_QUALITY          70
#define BEST_QUALITY          90
#define LOSSLESS_QUALITY      100

#define MAX_FRAME_RATE        30

//------------------------------------------------------------------------------
//  Stream formats

typedef enum tagSTREAM_FORMAT
{
  SF_UNKNOUWN          = -1,
  SF_STILL_IMAGE_V1    =  0,
  SF_IMAGE_SEQUENCE_V1 =  1,
  SF_VIDEO_V1          =  2,
  SF_AVI               =  3,
  SF_STILL_IMAGE_V2    =  4,
  SF_IMAGE_SEQUENCE_V2 =  5

} STREAM_FORMAT;

#define TOTAL_VOP_TYPES 4

typedef enum tagVOP_TYPE
{
  U_VOP = -1,                // Undefined frame type
  I_VOP =  0,                // Key frame type
  P_VOP =  1,                // Predicted frame type
  B_VOP =  2,
  D_VOP =  3                 // Difference frame on basis of Key frame type

} VOP_TYPE;

typedef enum tagWCI_COMPONENT_COMPOSITION
{
  FIRST_COMPONENT  =  0,                // Undefined frame type
  SECOND_COMPONENT =  1,                // Key frame type
  THIRD_COMPONENT  =  2,                // Predicted frame type
  ALPHA_COMPONENT  =  3,
  PALETTE_COMPONENT = 4                 // Difference frame on basis of Key frame type
} WCI_COMPONENT_COMPOSITION;

//------------------------------------------------------------------------------
//  Color space constants

#define MAX_IMAGE_COMPONENTS  4

typedef enum tagCOLOR_SPACE
{
  CSP_NULL      = 0        //! decoder only: don't output anything
, CSP_AUTO      = 0

, CSP_USER      = (1<< 0)  //! 4:2:0 planar  = (1<<(==I420, except for pointers/strides)
, CSP_I420      = (1<< 1)  //! 4:2:0 planar
, CSP_YV12      = (1<< 2)  //! 4:2:0 planar
, CSP_YV24      = (1<< 3)  //! 4:4:4 planar
, CSP_YUY2      = (1<< 4)  //! 4:2:2 packed
, CSP_UYVY      = (1<< 5)  //! 4:2:2 packed
, CSP_YVYU      = (1<< 6)  //! 4:2:2 packed

, CSP_BGRA      = (1<< 7)  //! 32-bit BGRA packed
, CSP_ABGR      = (1<< 8)  //! 32-bit ABGR packed
, CSP_RGBA      = (1<< 9)  //! 32-bit RGBA packed
, CSP_ARGB      = (1<<10)  //! 32-bit ARGB packed
, CSP_BGR       = (1<<11)  //! 24-bit BGR packed
, CSP_RGB       = (1<<12)  //! 24-bit RGB packed
, CSP_RGB555    = (1<<13)  //! 15-bit RGB555 packed into 16 bit
, CSP_RGB565    = (1<<14)  //! 16-bit RGB565 packed into 16 bit
, CSP_RGB666    = (1<<15)  //! 18-bit RGB666 packed into 24 bit

, CSP_GRAYSCALE = (1<<16)  //! only luma component

, CSP_RGB56588  = (1<<17)  //! 16-bit RGB565 and alpha channel packed into 32 bit
, CSP_RGB6668   = (1<<18)  //! 18-bit RGB666 and alpha channel packed into 32 bit

, CSP_SLICE     = (1<<20)  //! decoder only: 4:2:0 planar, per slice rendering
, CSP_INTERNAL  = (1<<21)  //! decoder only: 4:2:0 planar, returns pointers to internal buffers

, CSP_YV12A     = (1<<23)  //! 4:2:0 planar with alpha channel
, CSP_YV24A     = (1<<24)  //! 4:4:4 planar with alpha channel

, CSP_VFLIP     = (1<<30)  //! vertical flip mask

} COLOR_SPACE;

typedef enum tagPIXEL_SAMPLING
{
  PS_444          = 0,
  PS_422          = 1,
  PS_422_CO_SITED = 2,
  PS_420          = 3,
  PS_420_CO_SITED = 4,
  PS_AUTO         = 5

} PIXEL_SAMPLING;

//------------------------------------------------------------------------------
//  CPU flags

// This section describes all constants that show host cpu available features,
// and allow a client application to force usage of some cpu instructions sets.

#define CPU_MMX      0x00000001  /* use/has MMX instruction set */
#define CPU_MMXEXT   0x00000002  /* use/has MMX-ext (pentium3) instruction set */
#define CPU_SSE      0x00000004  /* use/has SSE (pentium3) instruction set */
#define CPU_SSE2     0x00000008  /* use/has SSE2 (pentium4) instruction set */
#define CPU_3DNOW    0x00000010  /* use/has 3dNOW (k6-2) instruction set */
#define CPU_3DNOWEXT 0x00000020  /* use/has 3dNOW-ext (athlon) instruction set */
#define CPU_TSC      0x00000040  /* has TimeStampCounter instruction */

#define CPU_CHKONLY  0x40000000  /* Check cpu features
                                  *
                                  * When this flag is set, the init function performs just a cpu feature
                                  * checking and then fills the cpu field. This flag is usefull when client
                                  * applications want to know what instruction sets the host cpu supports.
                                  */

#define CPU_FORCE    0x80000000  /* Force input flags to be used
                                  *
                                  * When this flag is set, client application forces codec to use other flags
                                  * set in iCPU_flags. Use this at your own risk.
                                  */

//------------------------------------------------------------------------------
// Error codes

typedef enum tagERROR_CODE
{
  ERR_NONE   =  0,                      //! Operation succeed.
  ERR_MEMORY =  1,                      //! Operation failed.
                                        //! The size of input or output buffers is insufficiently.
  ERR_FORMAT =  2,                      //! Operation failed.
                                        //! The format of input stream is incorrect.
  ERR_PARAM  =  3,                      //! Operation failed.
                                        //! The value of the parameters were incorrect.
  ERR_FAIL   = -1                       //! Operation failed.

} ERROR_CODE;

//------------------------------------------------------------------------------
// Global flag constants

#define ADVANCED_CONTROL_PARAM 0x00000001 // Using of advanced control parameters

#define SF_VERSION_DECADE      7          // Decade of stream format version

#define CHANGE_QUALITY         0x00000001 // Set new quality value

#define USE_FRAME_CACHE        0x00000002 // Use frame cache chunk which is situated after frame

#define RAW_IMAGE              0x00000010 // The way of I-frame coding (default- wavelet) (debug only)
#define WAVELET_IMAGE          0x00000020 //(debug only)

#define NO_FORCE_INTRA_FRAME   0x00000100 /* for debug only */

#define SKIP_DIFF_COMPRESS     0x00001000 /* for debug only */
#define SKIP_MB_COMPRESS       0x00002000 /* for debug only */

#define DIFF_WAVELET           0x00020000 // The usage of wavelet transform for encoding of difference image
                                          // else DCT transformation

#define DIFF_WAVELET_MB        0x00040000 // (submode) The wavelet transformation of diff. image is block wavelet one

//------------------------------------------------------------------------------
// Motion flag constants

#define MB_PREDICTION_ZERO     0x00000001 /* Zero prediction of motion vector */

#define MB_PREDICTION_MEDIAN   0x00000002 /* Median prediction of motion vector */

#define MB_HUFFMAN_COMPRESS    0x00000008 /* Huffman compress macroblocks. */


#define ME_DYNAMIC_RANGE       0x00000010 /* Dynamic change of MV search range */

#define ME_FILL_RECON_BP       0x00000020 /* Filling the reconstracted binary pyramid */

#define ME_ENOUGH_FULL_SPIRAL_SEARCH  0x1 /* Constants list of methods for  */
#define ME_FAST_SEARCH                0x2 /**   MV search of ME layer */
#define ME_ENOUGH_FULL_ROW_SEARCH     0x3
#define ME_FAST_CUP_SEARCH            0x4
#define ME_FAST_CUP_VARSTEP_SEARCH    0xA

#define ME4_FULL_ROW_SEARCH           0x5
#define ME4_FAST_SEARCH               0x6
#define ME4_FAST_CUP_SEARCH           0x7
#define ME4_FAST_CUP_VARSTEP_SEARCH   0xB

#define ME_SEARCH_COMPARISON          0xF

#define ME_HALF_PEL_REFINE      0x00100000 /* Halfpel motion estimation
                                                * informs DVR ME to perform a half pixel  motion estimation. */
#define ME_INTER_4_MVS_REFINE   0x00200000 /* 4 motion vectors per macroblock
                                                * informs DVR ME to perform a 4 motion vectors motion estimation. */

#define ME_FIX_SKIP_THRESH      0x00400000 /* Use fixed threshold for MB skipping,
                                                * else threshold is depended on quantizer */
#define ME_FRACTILE_SKIP_THRESH 0x00800000

#define ME_STATISTICS           0x10000000 /* Calculation of ME */
#define ME_DETECT_SCENE_CHANGE  0x20000000 /* Scene change detection */
#define MV_NORMALIZED           0x40000000 /* Normalized MV values to macroblock size 16x16 */

                                           /* If flag decade of ME layer is empty
                                            *  then ME layer is not used */
#define ME4_FLAG_DECADE        2           /* Flag decade of ME layer 4x4 */
#define ME8_FLAG_DECADE        3           /* Flag decade of ME layer 8x8 */
#define ME16_FLAG_DECADE       4           /* Flag decade of ME layer 16x16 */
#define ME_REFINE_FLAG_DECADE  5           /* Flag decade of ME refine layer */

#define ME4_FLAGS              (0xF << 4*ME4_FLAG_DECADE)
#define ME8_FLAGS              (0xF << 4*ME8_FLAG_DECADE)
#define ME16_FLAGS             (0xF << 4*ME16_FLAG_DECADE)
#define ME_REFINE_FLAGS        (0xF << 4*ME_REFINE_FLAG_DECADE)

//------------------------------------------------------------------------------
//  Initialization structure

typedef struct
{
  int   iCPU_flags;      // [in/out] Filled with desired[in] or available[out]
                         //          CPU instruction sets.

  int   iAPI_version;    // [out] init will initialize this field with
                         //       the API_VERSION used in this core library

  int   iCoreBuild;      // [out] init will initialize this field with
                         //       the CORE_BUILD used in this core library

} INIT_PARAM;

typedef struct
{
  int   iAPI_version;     //!  Codec API version number
  int   iCoreBuild;       //!  Codec core build number
  int   iStreamFormat;    //!  Codec supported stream formats

} CODEC_VERSION;

//------------------------------------------------------------------------------

typedef struct tagSTREAM_INFO
{
  STREAM_FORMAT eFormat;               //! Stream format, version
  int				iMaxWidth;                 //! Image Max width [pixel]
  int				iMaxHeight;                //! Image Max height [pixel]
  int       iFrames;                   //! Number of frames in the stream
  int       iFrameRate;                //! Frame rate [FPS]
  int       iMaxZoomOut;               //! Max available value of zoom out for all frames

} STREAM_INFO;

typedef struct tagFAT
{
  int       iFrames;                   // Number of frames in the stream
  int *     piOffset;                  // Frame offsets
  int       iMaxOffsets;               // Max size of offset buffer

} FAT;

typedef struct tagFRAME_INFO
{
  int       iLength;                   //! Length of the frame [bytes]
  int       iDuration;                 //! Display duration of the frame [10 ms]

} FRAME_INFO;

typedef struct tagFRAME_IMAGE_INFO
{
  int       iWidth;                    //! Image width [pixel]
  int       iHeight;                   //! Image height [pixel]
  int       iResolutionHorz;           //! Horizontal image resolution [DPI]
  int       iResolutionVert;           //! Vertical image resolution [DPI]
  COLOR_SPACE eColorSpace;             //! Image color space
  int       iImageSize;                //! Size of the image [bytes]
  int       iMaxZoomOut;               //! Max available value of zoom out
  int       iQuality;                  //! Image quality

} FRAME_IMAGE_INFO;

typedef struct tagSTREAM
{
  unsigned char * pbtData;             //! [in] Input bitstream data buffer pointer
  int       iLength;                   //! [in] Input bitstream length [bytes]
  int       iOffset;                   //! [in\out] Size of processed data [bytes]

} STREAM;

typedef enum tagDECODING_SPEED
{
  DS_NORMAL = 0
, DS_FAST   = 1
, DS_MAX    = 2

} DECODING_SPEED;

typedef struct tagENCODE_PARAM
{
  int            iQuality;             // Encoding quality
  DECODING_SPEED eSpeed;               // Decoding speed
  COLOR_SPACE    eColorSpace;          // Color space of encoded image

} ENCODE_PARAM;

typedef enum tagSPATIAL_SCALABILITY_MODE
{
  SSM_NONE   = 0
, SSM_ZOOM   = 1
, SSM_SCROLL = 2

} SPATIAL_SCALABILITY_MODE;

typedef struct tagSPATIAL_SCALABILITY
{
  SPATIAL_SCALABILITY_MODE eMode;      // [in] Scalability mode
  int                      iZoomOut;   // [in] Value of spatial zooming (4^n = 1,4,16,64)
  RECT                     rClipRect;  // [in] Clipping rect of image

} SPATIAL_SCALABILITY;

typedef struct tagDECODE_PARAM
{
  unsigned char *     pbtImage;        // [in] Pointer to an decoded image
  COLOR_SPACE         eColorSpace;     // [in] Required color space format of the decoded image
  SPATIAL_SCALABILITY rScalability;    // [in] Spatial scalability parameters
  unsigned char *    	pbtProcBuf;	     // [in] Temporal decoder processing buffer pointer
  int                 iProcBufSize;    // [in] Temporal decoder processing buffer size [bytes]

} DECODE_PARAM;

typedef struct {
                             /* SAD tresholds for early stop of ME with one-pel accuracy */
  int   iAbs16;              /* [in]  for 16x16 macroblocks */
  int   iAbs8;               /* [in]  for 8x8 macroblocks */

  int   iHalfPel;            /* [in] SAD threshold for skip of refined ME with half-pel accuracy */

  int   iSkipDiffMB;         /* [in] SAD threshold for skip of difference MB */

  int   iIntraMB;            /* [in] SAD threshold for intra-mode encoding */

} ME_SAD_THRESH;

typedef struct {

  struct
  {
    int iPredictionType;     /* 0 - "IPPP", 1 - "IDDD", 2 - "IPPD" */

  } rTemporal;

  SPATIAL_SCALABILITY rSpatial;

} SCALABILITY;

//------------------------------------------------------------------------------

#define AUTO_MOTION      (-1)
#define NORMAL_MOTION      0
#define FAST_MOTION        1

#define LOWEST_QUALITY_INDEX   (-2)
#define LOW_QUALITY_INDEX      (-1)
#define NORMAL_QUALITY_INDEX     0
#define HIGH_QUALITY_INDEX       1
#define BEST_QUALITY_INDEX       2

#define LOW_SPEED        (-1)
#define NORMAL_SPEED       0
#define HIGH_SPEED         1
#define MAX_SPEED          5

//------------------------------------------------------------------------------

typedef struct
{
  int   iQuantizer;          /*! [in] Sets the fixed value of the quantizer. */
  int   iWavSteps;           /*! [in] Sets the number of steps for wavelet transform. */
  int   iCompressMode;       /*! [in]  */

} I_FRAME_ENC_PARAM;

typedef struct
{
  int   iQuantizer;          /*! [in] Sets the fixed value of the quantizer. */
  int   iWavSteps;           /*! [in] Sets the number of steps for wavelet transform. */
  int   iCompressMode;       /*! [in]  */
  int   iMotionMode;         /*! [in] Flags of motion estimation modes */

  ME_SAD_THRESH rME_SAD_Thresh;

} P_FRAME_ENC_PARAM;

typedef struct
{
  int   iWidth;              /* [in] Input frame image width */
  int   iHeight;             /* [in] Input frame image height */

  int   iFrameIncr;          /* [in] Time increment (fps = increment/base). */
  int   iFrameBase;          /* [in] Time base (fps = increment/base). */

  int   iContentMotion;      /* Content motion type. There are two types: normal and fast(noise) */

  COLOR_SPACE    eColorSpace;    //! Color space
  PIXEL_SAMPLING ePixelSampling; //! Pixel sampling format

  int   iSpeedOrQuality;         //! Relation between encoding speed and quality
  int   iQuality;                //! Relation between quality and compression

  int   iMaxWavSteps;            //! Max steps of wavelet transformation

  int   iMaxKeyInterval;         //! Interval between I-frames

  BOOL  bSharedMemory;           //! [in]  Use shared memory from global heap

  void *pEnc;                    // [out] Instance of DVR encoder

  SCALABILITY rScalability;

  int   iGlobalFlags;            // [in] Flags of global configuration

  I_FRAME_ENC_PARAM rI;
  P_FRAME_ENC_PARAM rP;

} ENC_PARAM;

typedef struct
{
  BYTE     *pbImage;         /* [in] Pointer to an origin image */
  int      iColorSpace;      /* [in] A colorspace format of the origin image */
  int      iImageSize;       /* [in] Size of the origin image */

  int      iWidth;           /* [in] Input frame image width */
  int      iHeight;          /* [in] Input frame image height */

  int      iResolutionHorz;  //! Horizontal image resolution [DPI]
  int      iResolutionVert;  //! Vertical image resolution [DPI]

  int      iDuration;        //! Display duration of the frame [ms]

  int      iGlobalFlags;     /* [in] Flags of global configuration */

  VOP_TYPE eFrameType;       /* [in/out] :
                             *  to inform the encoder if the frame must be encoded as an I-frame
                             *
                             *  U_VOP : let the encoder decide (based on contents and iMaxKeyInterval).
                             *  I_VOP : forces the encoder to create a I-frame.
                             *  P_VOP : forces the encoder not to create a I-frame.
                             *
                             * When first set to U_VOP, the encoder returns
                             *  the effective I-frame state of the frame.
                             */
  int      iQuality;         /* [in] Relation between quality and compression */

  BYTE     *pbBitstream;     /* [out] Output encoded frames bitstream buffer pointer */
  int      iBitstreamLength; /* [out] size of encoded frame [bytes] */

} ENC_FRAME;

#define MAX_PARTITION_COMPONENTS 3

typedef struct
{
  int iQ;
  int iS;
  int aiP[MAX_PARTITION_COMPONENTS];

} QSP_SIZE;

#define MAX_COMPRESSION_STEPS 2
#define COMPRESSION_STEP1     0
#define COMPRESSION_STEP2     1
typedef struct
{
  int iQ[MAX_COMPRESSION_STEPS];
  int iS[MAX_COMPRESSION_STEPS];
  int aiP[MAX_PARTITION_COMPONENTS][MAX_COMPRESSION_STEPS];

} QSP_COMPRESSED_SIZE;

typedef struct { float fQ, fS, fP; } MEAN_QSP_SIZE;

typedef struct
{
  BOOL     bExistence;                   //! Structure fields are setted
  int      iTag;                         //! Number of frames or selected frame
  float    afPSNR[MAX_IMAGE_COMPONENTS]; //! Component PSNR

} PSNR_INFO;

typedef struct
{
  int      iFrames;                      //! Number of frames

  int      iFrameSize;                   //! Size of current frame
  float    fMeanFrameSize;               //! Mean frame size

  QSP_SIZE      rQSP_Size;               //! QSP component size
  MEAN_QSP_SIZE rMeanQSP_Size;

  PSNR_INFO     rPSNR;                   //! PSNR of current frame
  PSNR_INFO     rMeanPSNR;               //! Mean PSNR of video stream
  PSNR_INFO     rMinPSNR;                //! Min PSNR of video stream
  PSNR_INFO     rMaxPSNR;                //! Max PSNR of video stream

} FRAME_STATS;

typedef struct
{
  int      iTotal;           /* [out] Number of macro blocks  */

  int      iIntra;           /* [out] Number of intra macro blocks  */

  int      iInter0;          /* [out] Number of inter0 macro blocks */
  int      iInter8;          /* [out] Number of inter macro blocks */
  int      iInter16;         /* [out] Number of inter macro blocks */
  int      iInterHP;         /* [out] Number of inter macro blocks with halfpel accuracy */

  int      iSkipped;         /* [out] Number of skipped differential macro blocks */

} MACROBLOCK_STATS;

typedef struct
{
  int   iFrameType;          /* [out] Current encoded VOP frame type */

  FRAME_STATS rI;            /* [in/out] Statistics of I VOP */
  FRAME_STATS rP;            /* [in/out] Statistics of P VOP */
  FRAME_STATS rD;            /* [in/out] Statistics of D VOP */

  MACROBLOCK_STATS rMBS;     /* [out] Macroblocks statistics */

} ENC_STATS;

//------------------------------------------------------------------------------

typedef struct
{
  int   iWidth;              /* [in] Input frame image width */
  int   iHeight;             /* [in] Input frame image height */

  int   iFrameIncr;          /* [in] Time increment (fps = increment/base). */
  int   iFrameBase;          /* [in] Time base (fps = increment/base). */

  BOOL  bSharedMemory;       /* Use shared memory from global heap */

  void  *pDec;               /* [out] Instance of DVR decoder */

} DEC_PARAM;

typedef struct
{
  BYTE  *pbBitstream;        /* [in] Input encoded frames bitstream buffer pointer */
  int   iBitstreamLength;    /* [in\out] :
                              *    [in] - input bitstream length [bytes]
                              *    [out] - size of decoded frame [bytes]
                              */
  BYTE  *pbImage;            /* [in] Pointer to an decoded image */
  int   iColorSpace;         /* [in] Required colorspace format of the decoded image */
  int   iImageSize;          /* [out] Size of the decoded image */

  int   iWidth;              /* [out] Output frame image width */
  int   iHeight;             /* [out] Output frame image height */

} DEC_FRAME;


typedef struct tagWCI_ALPHA_CHANNEL
{
  BOOL isAlphaCannel;
  int  iAlphaBlockNumber;
  int  iQOffset;
  int  iQBlockSize;
} WCI_ALPHA_CHANNEL;


//------------------------------------------------------------------------------
ERROR_CODE  wci3_get_version
(
 CODEC_VERSION * OUT    prCodecVersion
);

//------------------------------------------------------------------------------

int wci_init( void *handle, int opt, void *param1, void *param2 );

//------------------------------------------------------------------------------

ERROR_CODE wci3_get_stream_info
(
  STREAM *          IN OUT prStream
, STREAM_INFO *     OUT    prInfo
);

//------------------------------------------------------------------------------

ERROR_CODE wci_get_fat
(
  STREAM *          IN OUT prStream
, FAT *             IN OUT prFAT
);

//------------------------------------------------------------------------------

int wci3_find_frame
(
  const STREAM *    IN     prStream
, FRAME_INFO *         OUT prInfo
);

//------------------------------------------------------------------------------

ERROR_CODE wci3_get_frame_info
(
  const STREAM *     IN OUT prStream
, FRAME_IMAGE_INFO *    OUT prInfo
);

//------------------------------------------------------------------------------

int wci3_get_decoder_buffer_size
(
  const FRAME_IMAGE_INFO * IN prInfo
, int                      IN iZoomOut
);

//------------------------------------------------------------------------------

ERROR_CODE wci3_decode_frame
(
  const DECODE_PARAM * IN     prParam
, STREAM *             IN OUT prStream
, FRAME_IMAGE_INFO *      OUT prInfo        //! Decoded image information
);

//------------------------------------------------------------------------------

ERROR_CODE wci3_put_stream_info
(
  const STREAM_INFO * IN     prInfo
, STREAM *            IN OUT prStream
);

//------------------------------------------------------------------------------

ERROR_CODE wci_put_fat
(
  const FAT *         IN     prFAT
, STREAM *            IN OUT prStream
);

//------------------------------------------------------------------------------

ERROR_CODE wci3_encode_frame
(
  const ENCODE_PARAM *     IN     prParam
, const unsigned char *    IN     pbtImage //! Pointer to an encoded image
, const FRAME_IMAGE_INFO * IN     prInfo   //! Image information
, STREAM *                 IN OUT prStream
);

//------------------------------------------------------------------------------

int wci_image_get_size
(
  SIZE        IN rSize
, COLOR_SPACE IN eColorSpace
);


#endif // #ifndef CODEC_H

/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: ColorSpaceOut.c

ABSTRACT:    This file contains procedures for color space YUV to RGB transformation

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/


#include <string.h>

#include "../wci_portab.h"
#include "../wci_global.h"
#include "wci_image.h"
#include "wci_color_space_table.h"
#include "wci_color_space_out.h"

//------------------------------------------------------------------------------

const uint8_t g_bound_array[768] ={
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
    96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};  // 

const uint8_t * g_bound = &g_bound_array[256];
const int* g_RGB_Y_tab = &RGB_Y_tab[64];
const int* g_B_U_tab = &B_U_tab[64];
const int* g_G_U_tab = &G_U_tab[64];
const int* g_G_V_tab = &G_V_tab[64];
const int* g_R_V_tab = &R_V_tab[64];


//#define CSP_FLOAT_TRANS


#define BYTE_BOUND( v ) ( (BYTE) max( 0, min(255,(v)) ) )

#define RGB_TO_RGB565(r, g, b) ((((uint8_t)(r) & 0xF8) << 8) | (((uint8_t)(g) & 0xFC) << 3) | ((uint8_t)(b) >> 3))


#define YUV_TO_RGB( tb, tg, tr, rgb_y, b_u0, g_uv0, r_v0)                      \
      {                                                                        \
        tb= g_bound[(rgb_y + b_u0 ) >> SCALEBITS_OUT];                         \
        tg= g_bound[(rgb_y - g_uv0 ) >> SCALEBITS_OUT];                        \
        tr= g_bound[(rgb_y + r_v0 ) >> SCALEBITS_OUT];                         \
      }                                                                        \


#define YUV_TO_RGB565( pbtImage, x, y, iStride, rgb_y, b_u0, g_uv0, r_v0 )     \
      {                                                                        \
        register int  tb, tg, tr;                                              \
        unsigned short aid;                                                    \
        YUV_TO_RGB(tb , tg, tr, rgb_y, b_u0, g_uv0, r_v0);                     \
        aid = RGB_TO_RGB565(tr, tg, tb);                                       \
        pbtImage[(y)*(iStride)+(x)+(1)] = (BYTE)(aid >> 8);                    \
        pbtImage[(y)*(iStride)+(x)+(0)] = (BYTE)(aid);                         \
     }



#define YUV_TO_RGB565_16( pbtImage, x, y, iStride, rgb_y, b_u0, g_uv0, r_v0 )  \
      {                                                                        \
        register int  tb, tg, tr;                                              \
        YUV_TO_RGB(tb , tg, tr, rgb_y, b_u0, g_uv0, r_v0);                     \
        pbtImage[(y)*(iStride)+(x)]= RGB_TO_RGB565(tr, tg, tb);                \
      }  



#ifdef CSP_FLOAT_TRANS

#define YUV_TO_BGR( pbtImage, x, y, iStride, rgb_y, b_u0, r_v0 )                                              \
  pbtImage[(y)*(iStride)+(x)+(0)] = BYTE_BOUND(RGB_Y_OUT*(rgb_y-Y_ADD_OUT) + B_U_OUT*(b_u0-U_ADD_OUT) + 0.5); \
  pbtImage[(y)*(iStride)+(x)+(1)] = BYTE_BOUND(RGB_Y_OUT*(rgb_y-Y_ADD_OUT) - G_V_OUT*(r_v0-V_ADD_OUT) - G_U_OUT*(b_u0-U_ADD_OUT)+ 0.5); \
  pbtImage[(y)*(iStride)+(x)+(2)] = BYTE_BOUND(RGB_Y_OUT*(rgb_y-Y_ADD_OUT) + R_V_OUT*(r_v0-V_ADD_OUT) + 0.5);

#define YUV_TO_BGRA( pbtImage, x, y, iStride, rgb_y, b_u0, r_v0 )              \
  YUV_TO_BGR( pbtImage, x, y, iStride, rgb_y, b_u0, r_v0 )                     \
  pbtImage[(y)*(iStride)+(x)+(3)] = 0

#else

#define YUV_TO_BGR( pbtImage, x, y, iStride, rgb_y, b_u0, g_uv0, r_v0 )        \
      {                                                                        \
        register int t;                                                        \
        t= (rgb_y + b_u0 ) >> SCALEBITS_OUT;                                   \
        pbtImage[(y)*(iStride)+(x)+(0)] = g_bound[t];                          \
        t= (rgb_y - g_uv0 ) >> SCALEBITS_OUT;                                  \
        pbtImage[(y)*(iStride)+(x)+(1)] = g_bound[t];                          \
        t= (rgb_y + r_v0 ) >> SCALEBITS_OUT;                                   \
        pbtImage[(y)*(iStride)+(x)+(2)] = g_bound[t];                          \
      }




#define YUV_TO_BGRA( pbtImage, x, y, iStride, rgb_y, b_u0, g_uv0, r_v0 )       \
  YUV_TO_BGR( pbtImage, x, y,iStride,  rgb_y, b_u0, g_uv0, r_v0 )              \
  pbtImage[(x)+(3)] = 0

#define YUV_TO_BGR2( pbtImage, rgb_y, b_u0, g_uv0, r_v0 )                      \
      {                                                                        \
        register int t;                                                        \
        t= (rgb_y + b_u0 ) >> SCALEBITS_OUT;                                   \
        *pbtImage++= g_bound[t];                                               \
        t= (rgb_y - g_uv0 ) >> SCALEBITS_OUT;                                  \
        *pbtImage++ = g_bound[t];                                              \
        t= (rgb_y + r_v0 ) >> SCALEBITS_OUT;                                   \
        *pbtImage++ = g_bound[t];                                              \
      }


#define YUV_TO_BGRA2( pbtImage, rgb_y, b_u0, g_uv0, r_v0 )                     \
  YUV_TO_BGR2( pbtImage, rgb_y, b_u0, g_uv0, r_v0 )                            \
  *pbtImage++ = 0


#define YUV_TO_BGRA3( piDst, rgb_y, b_u0, g_uv0, r_v0 )                        \
      {                                                                        \
        register int t;                                                        \
        t= g_bound[(rgb_y + b_u0 ) >> SCALEBITS_OUT];                          \
        t = t|(g_bound[(rgb_y - g_uv0 ) >> SCALEBITS_OUT]<<8);                 \
        t = t|(g_bound[(rgb_y + r_v0 ) >> SCALEBITS_OUT]<<16);                 \
        *piDst++ = t;                                                          \
      }
// Alpha channel added
#define YUV_TO_BGRA4( piDst, rgb_y, b_u0, g_uv0, r_v0, a  )                    \
{                                                                              \
  register int t;                                                              \
  t= g_bound[(rgb_y + b_u0 ) >> SCALEBITS_OUT];                                \
  t = t|(g_bound[(rgb_y - g_uv0 ) >> SCALEBITS_OUT]<<8);                       \
  t = t|(g_bound[(rgb_y + r_v0 ) >> SCALEBITS_OUT]<<16);                       \
  t = t | (a  << 24);                                                          \
  *piDst++ = t;                                                                \
}


// 김기훈 2010.10.8.금요일
// 알파값을 무조건 0xff 로 변경
//#define YUV_TO_BGRA_DOUBLE( piDst,  rgb_y, rgb_y1, b_u0, g_uv0, r_v0 , a1, a2 )                        \
//  *piDst++ =  g_bound[(rgb_y + b_u0 ) >> SCALEBITS_OUT]|(g_bound[(rgb_y - g_uv0 ) >> SCALEBITS_OUT]<<8)\
//  |(g_bound[(rgb_y + r_v0 ) >> SCALEBITS_OUT]<<16)|(a1<<24);\
//  *piDst++ =  g_bound[(rgb_y1 + b_u0 ) >> SCALEBITS_OUT]|(g_bound[(rgb_y1 - g_uv0 ) >> SCALEBITS_OUT]<<8)\
//  |(g_bound[(rgb_y1 + r_v0 ) >> SCALEBITS_OUT]<<16)|(a2<<24);

#define YUV_TO_BGRA_DOUBLE( piDst,  rgb_y, rgb_y1, b_u0, g_uv0, r_v0 , a1, a2 )                        \
  *piDst++ =  g_bound[(rgb_y + b_u0 ) >> SCALEBITS_OUT]|(g_bound[(rgb_y - g_uv0 ) >> SCALEBITS_OUT]<<8)\
  |(g_bound[(rgb_y + r_v0 ) >> SCALEBITS_OUT]<<16)|(a1<<24) | 0xFF000000;\
  *piDst++ =  g_bound[(rgb_y1 + b_u0 ) >> SCALEBITS_OUT]|(g_bound[(rgb_y1 - g_uv0 ) >> SCALEBITS_OUT]<<8)\
  |(g_bound[(rgb_y1 + r_v0 ) >> SCALEBITS_OUT]<<16)|(a2<<24)| 0xFF000000;


#endif //#ifdef CSP_FLOAT_TRANS




//------------------------------------------------------------------------------

void wci_yv12_out_to_bgra_diffimg(
  const DIFF_IMAGE_EX * IN  prImage
, RECT             IN  rClipRect
, BYTE *           OUT pbtDst
, BOOL             IN  vflip
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );

  const int nEvenW  = rDstSize.cx & ~1;
  const int nEvenH  = rDstSize.cy & ~1;

  const int nStrideW = BGRA_SIZE * rDstSize.cx;
  const int iDiffW = nStrideW - BGRA_SIZE * nEvenW;

  const int nStrideY  = prImage->arComponent[0].rSize.cx;
  const int nStrideUV = prImage->arComponent[1].rSize.cx;

  int16_t * pbtU = prImage->arComponent[1].piData; 
  int16_t * pbtV = prImage->arComponent[2].piData;
  int16_t * pbtY = prImage->arComponent[0].piData;

  const int nDiffY = nStrideY - nEvenW;
  const int nDiffUV = nStrideUV - nEvenW/2;

  uint32_t * piDst, *piDstS;
  int  j;

  pbtY += rClipRect.top   * nStrideY  + rClipRect.left  ;
  pbtU += rClipRect.top/2 * nStrideUV + rClipRect.left/2;
  pbtV += rClipRect.top/2 * nStrideUV + rClipRect.left/2;
  piDst = (uint32_t*)pbtDst;

  if (vflip)
    piDst += (rDstSize.cy-1)*nStrideW/BGRA_SIZE;

  for (j = 0; j < nEvenH; j+=(2))
  {
    register int i;
    if (vflip)
      piDstS = (uint32_t*)(piDst-nStrideW/BGRA_SIZE);
    else
      piDstS = (uint32_t*)(piDst+nStrideW/BGRA_SIZE);

    for (i = 0; i < nEvenW; i+=(2))
    {
      register int b_u0  = g_B_U_tab[ *pbtU ];
      register int g_uv0 = g_G_U_tab[ *pbtU++ ] + g_G_V_tab[ *pbtV ];
      register int r_v0  = g_R_V_tab[ *pbtV++ ];
      register int rgb_y, rgb_y1;

      rgb_y = *(int*)&g_RGB_Y_tab[ pbtY[0] ];
      rgb_y1 = *(int*)&g_RGB_Y_tab[ pbtY[1] ];     
      YUV_TO_BGRA_DOUBLE( piDst, rgb_y, rgb_y1, b_u0, g_uv0, r_v0, 0, 0);
  
      rgb_y = *(int*)&g_RGB_Y_tab[ pbtY[ nStrideY ] ];
      rgb_y1 = *(int*)&g_RGB_Y_tab[ pbtY[ nStrideY + 1 ] ];     
      YUV_TO_BGRA_DOUBLE( piDstS, rgb_y, rgb_y1, b_u0, g_uv0, r_v0, 0, 0);
      pbtY += 2;
    }

    if ( rDstSize.cx - nEvenW )
    {
      int rgb_y;
      int b_u0 = g_B_U_tab[ pbtU[0] ];
      int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
      int r_v0 = g_R_V_tab[ pbtV[0] ];

      rgb_y = g_RGB_Y_tab[ pbtY[(0)*nStrideY + 0] ];
      YUV_TO_BGRA3( piDst, rgb_y, b_u0, g_uv0, r_v0 );

      rgb_y = g_RGB_Y_tab[ pbtY[(1)*nStrideY + 0] ];
      YUV_TO_BGRA3( piDstS,  rgb_y, b_u0, g_uv0, r_v0 );
    }
    if (vflip)
      piDst -= 3*nStrideW/BGRA_SIZE;
    else
      piDst += nStrideW/BGRA_SIZE;
    
    pbtY += nDiffY + (2-1)*nStrideY;
    pbtU += nDiffUV + ((2/2)-1)*nStrideUV;
    pbtV += nDiffUV + ((2/2)-1)*nStrideUV;
  }

  if ( rDstSize.cy - nEvenH )
  {
    register int i;
    for (i = 0; i < nEvenW; i+=(2))
    {
      int rgb_y;
      int b_u0 = g_B_U_tab[ pbtU[0] ];
      int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
      int r_v0 = g_R_V_tab[ pbtV[0] ];

      rgb_y = g_RGB_Y_tab[ pbtY[0] ];
      YUV_TO_BGRA3( piDst,  rgb_y, b_u0, g_uv0, r_v0 );

      rgb_y = g_RGB_Y_tab[ pbtY[1] ];
      YUV_TO_BGRA3( piDst,  rgb_y, b_u0, g_uv0, r_v0 );

      pbtY += (2);
      pbtU += (2)/2;
      pbtV += (2)/2;
    }
    if ( rDstSize.cx - nEvenW )
    {
      int rgb_y;
      int b_u0 = g_B_U_tab[ pbtU[0] ];
      int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
      int r_v0 = g_R_V_tab[ pbtV[0] ];

      rgb_y = g_RGB_Y_tab[ pbtY[0] ];
      YUV_TO_BGRA3( piDst, rgb_y, b_u0, g_uv0, r_v0 );
    }
  }
}


//------------------------------------------------------------------------------


void wci_yv12a_out_to_bgra_diffimg
(
  const DIFF_IMAGE_EX * IN  prImage
, RECT                  IN  rClipRect
, BYTE *                OUT pbtDst
, BOOL                  IN  vflip
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );

  const int nEvenW  = rDstSize.cx & ~1;
  const int nEvenH  = rDstSize.cy & ~1;

  const int nStrideW = BGRA_SIZE * rDstSize.cx;
  const int iDiffW = nStrideW - BGRA_SIZE * nEvenW;

  const int nStrideY  = prImage->arComponent[0].rSize.cx;
  const int nStrideUV = prImage->arComponent[1].rSize.cx;

  int16_t * pbtU = prImage->arComponent[1].piData; 
  int16_t * pbtV = prImage->arComponent[2].piData;
  int16_t * pbtY = prImage->arComponent[0].piData;
  int16_t * piA = prImage->arComponent[3].piData;

  const int nDiffY = nStrideY - nEvenW;
  const int nDiffUV = nStrideUV - nEvenW/2;

  uint32_t * piDst, *piDstS;
  int  j;

  pbtY += rClipRect.top   * nStrideY  + rClipRect.left  ;
  piA  += rClipRect.top   * nStrideY  + rClipRect.left  ;
  pbtU += rClipRect.top/2 * nStrideUV + rClipRect.left/2;
  pbtV += rClipRect.top/2 * nStrideUV + rClipRect.left/2;

  piDst = (uint32_t*)pbtDst;
  
  if (vflip)
    piDst += (rDstSize.cy-1)*nStrideW/BGRA_SIZE;

  for (j = 0; j < nEvenH; j+=(2))
  {
    register int i;
    if (vflip)
      piDstS = (uint32_t*)(piDst-nStrideW/BGRA_SIZE);
    else
      piDstS = (uint32_t*)(piDst+nStrideW/BGRA_SIZE);

    
    for (i = 0; i < nEvenW; i+=(2))
    {
      register int b_u0  = g_B_U_tab[ *pbtU ];
      register int g_uv0 = g_G_U_tab[ *pbtU++ ] + g_G_V_tab[ *pbtV ];
      register int r_v0  = g_R_V_tab[ *pbtV++ ];
      register int rgb_y, rgb_y1;

      rgb_y = *(int*)&g_RGB_Y_tab[ pbtY[0] ];
      rgb_y1 = *(int*)&g_RGB_Y_tab[ pbtY[1] ];     
      YUV_TO_BGRA_DOUBLE( piDst, rgb_y, rgb_y1, b_u0, g_uv0, r_v0, piA[0], piA[1]);

      rgb_y = *(int*)&g_RGB_Y_tab[ pbtY[ nStrideY ] ];
      rgb_y1 = *(int*)&g_RGB_Y_tab[ pbtY[ nStrideY + 1 ] ];     
      YUV_TO_BGRA_DOUBLE( piDstS, rgb_y, rgb_y1, b_u0, g_uv0, r_v0,  piA[nStrideY], piA[nStrideY + 1]);
      pbtY += 2;
      piA  += 2;
    }

    if ( rDstSize.cx - nEvenW )
    {
      int rgb_y;
      int b_u0 = g_B_U_tab[ pbtU[0] ];
      int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
      int r_v0 = g_R_V_tab[ pbtV[0] ];
      rgb_y = g_RGB_Y_tab[ pbtY[(0)*nStrideY + 0] ];
      YUV_TO_BGRA4( piDst, rgb_y, b_u0, g_uv0, r_v0,  piA[0]);

      rgb_y = g_RGB_Y_tab[ pbtY[(1)*nStrideY + 0] ];
      YUV_TO_BGRA4( piDstS,  rgb_y, b_u0, g_uv0, r_v0,  piA[nStrideY]);
    }
    if (vflip)
      piDst -= 3*nStrideW/BGRA_SIZE;
    else
      piDst += nStrideW/BGRA_SIZE;

    pbtY += nDiffY + (2-1)*nStrideY;
    piA  += nDiffY + (2-1)*nStrideY;
    pbtU += nDiffUV + ((2/2)-1)*nStrideUV;
    pbtV += nDiffUV + ((2/2)-1)*nStrideUV;
  }

  if ( rDstSize.cy - nEvenH )
  {
    register int i;
    for (i = 0; i < nEvenW; i+=(2))
    {
      int rgb_y;
      int b_u0 = g_B_U_tab[ pbtU[0] ];
      int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
      int r_v0 = g_R_V_tab[ pbtV[0] ];

      rgb_y = g_RGB_Y_tab[ pbtY[0] ];
      YUV_TO_BGRA4( piDst,  rgb_y, b_u0, g_uv0, r_v0, piA[0] );

      rgb_y = g_RGB_Y_tab[ pbtY[1] ];
      YUV_TO_BGRA4( piDst,  rgb_y, b_u0, g_uv0, r_v0 , piA[1]);

      piA  +=  2;
      pbtY += (2);
      pbtU += (2)/2;
      pbtV += (2)/2;
    }
    if ( rDstSize.cx - nEvenW )
    {
      int rgb_y;
      int b_u0 = g_B_U_tab[ pbtU[0] ];
      int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
      int r_v0 = g_R_V_tab[ pbtV[0] ];

      rgb_y = g_RGB_Y_tab[ pbtY[0] ];
      YUV_TO_BGRA4( piDst, rgb_y, b_u0, g_uv0, r_v0, piA[0] );
    }
  }
}


//------------------------------------------------------------------------------


void wci_yv12_out_to_bgr_diffimg(
  const DIFF_IMAGE_EX * IN  prImage
, RECT             IN  rClipRect
, BYTE *           OUT pbtDst
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );

  const int nEvenW  = rDstSize.cx & ~1;
  const int nEvenH  = rDstSize.cy & ~1;

  const int nStrideW = 4 * ((rDstSize.cx*BGR_SIZE+3)/4) ;
  const int iDiffW = nStrideW - BGR_SIZE * nEvenW;

  const int nStrideY  = prImage->arComponent[0].rSize.cx;
  const int nStrideUV = prImage->arComponent[1].rSize.cx;

  int16_t * pbtY = prImage->arComponent[0].piData;
  int16_t * pbtU = prImage->arComponent[1].piData; 
  int16_t * pbtV = prImage->arComponent[2].piData;

  const int nDiffY = nStrideY - nEvenW;
  const int nDiffUV = nStrideUV - nEvenW/2;

  int i, j;

  pbtY += rClipRect.top   * nStrideY  + rClipRect.left  ;
  pbtU += rClipRect.top/2 * nStrideUV + rClipRect.left/2;
  pbtV += rClipRect.top/2 * nStrideUV + rClipRect.left/2;

  for (j = 0; j < nEvenH; j+=(2))
  {
    for (i = 0; i < nEvenW; i+=(2))
    {
#ifdef CSP_FLOAT_TRANS
      float rgb_y;
      float b_u0  = pbtU[0];
      float r_v0  = pbtV[0];

      rgb_y = pbtY[(0)*nStrideY + 0];
      YUV_TO_BGR( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, r_v0 );

      rgb_y = pbtY[(0)*nStrideY + 1];
      YUV_TO_BGR( pbtDst, BGR_SIZE, 0, nStrideW, rgb_y, b_u0, r_v0 );

      rgb_y = pbtY[(1)*nStrideY + 0];
      YUV_TO_BGR( pbtDst, 0, 1, nStrideW, rgb_y, b_u0, r_v0 );

      rgb_y = pbtY[(1)*nStrideY + 1];
      YUV_TO_BGR( pbtDst, BGR_SIZE, 1, nStrideW, rgb_y, b_u0, r_v0 );

#else
      int rgb_y;
      int b_u0  = g_B_U_tab[ pbtU[0] ];
      int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
      int r_v0  = g_R_V_tab[ pbtV[0] ];

      rgb_y = g_RGB_Y_tab[ pbtY[(0)*nStrideY + 0] ];
      YUV_TO_BGR( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

      rgb_y = g_RGB_Y_tab[ pbtY[(0)*nStrideY + 1] ];
      YUV_TO_BGR( pbtDst, BGR_SIZE, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

      rgb_y = g_RGB_Y_tab[ pbtY[(1)*nStrideY + 0] ];
      YUV_TO_BGR( pbtDst, 0, 1, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

      rgb_y = g_RGB_Y_tab[ pbtY[(1)*nStrideY + 1] ];
      YUV_TO_BGR( pbtDst, BGR_SIZE, 1, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

#endif // #ifdef CSP_FLOAT_TRANS
      pbtDst += (2) * BGR_SIZE;
      pbtY += (2);
      pbtU += (2)/2;
      pbtV += (2)/2;
    }

    if ( rDstSize.cx - nEvenW )
    {
#ifdef CSP_FLOAT_TRANS
      float rgb_y;
      float b_u0  = pbtU[0];
      float r_v0  = pbtV[0];

      rgb_y = pbtY[(0)*nStrideY + 0];
      YUV_TO_BGR( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, r_v0 );

      rgb_y = pbtY[(1)*nStrideY + 0];
      YUV_TO_BGR( pbtDst, 0, 1, nStrideW, rgb_y, b_u0, r_v0 );
#else
      int rgb_y;
      int b_u0 = g_B_U_tab[ pbtU[0] ];
      int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
      int r_v0 = g_R_V_tab[ pbtV[0] ];

      rgb_y = g_RGB_Y_tab[ pbtY[(0)*nStrideY + 0] ];
      YUV_TO_BGR( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

      rgb_y = g_RGB_Y_tab[ pbtY[(1)*nStrideY + 0] ];
      YUV_TO_BGR( pbtDst, 0, 1, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

#endif // #ifdef CSP_FLOAT_TRANS
    }
    pbtDst += iDiffW + (2-1)*nStrideW;
    pbtY += nDiffY + (2-1)*nStrideY;
    pbtU += nDiffUV + ((2/2)-1)*nStrideUV;
    pbtV += nDiffUV + ((2/2)-1)*nStrideUV;
  }
  
  if ( rDstSize.cy - nEvenH )
  {
    for (i = 0; i < nEvenW; i+=(2))
    {
#ifdef CSP_FLOAT_TRANS
      float rgb_y;
      float b_u0  = pbtU[0];
      float r_v0  = pbtV[0];
      rgb_y = pbtY[0];
      YUV_TO_BGR( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, r_v0 );
      rgb_y = pbtY[1];
      YUV_TO_BGR( pbtDst, BGR_SIZE, 0, nStrideW, rgb_y, b_u0, r_v0 );
#else
      int rgb_y;
      int b_u0 = g_B_U_tab[ pbtU[0] ];
      int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
      int r_v0 = g_R_V_tab[ pbtV[0] ];

      rgb_y = g_RGB_Y_tab[ pbtY[0] ];
      YUV_TO_BGR( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );
      rgb_y = g_RGB_Y_tab[ pbtY[1] ];
      YUV_TO_BGR( pbtDst, BGR_SIZE, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

#endif // #ifdef CSP_FLOAT_TRANS
      pbtDst += (2)*BGR_SIZE;
      pbtY += (2);
      pbtU += (2)/2;
      pbtV += (2)/2;
    }

    if ( rDstSize.cx - nEvenW )
    {
#ifdef CSP_FLOAT_TRANS
      float rgb_y;
      float b_u0  = pbtU[0];
      float r_v0  = pbtV[0];
      rgb_y = pbtY[0];
      YUV_TO_BGR( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, r_v0 );
#else
      int rgb_y;
      int b_u0 = g_B_U_tab[ pbtU[0] ];
      int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
      int r_v0 = g_R_V_tab[ pbtV[0] ];
      rgb_y = g_RGB_Y_tab[ pbtY[0] ];
      YUV_TO_BGR( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

#endif // #ifdef CSP_FLOAT_TRANS
    }
  }
}

//------------------------------------------------------------------------------

void wci_yv24_out_to_bgra_diffimg(
  const DIFF_IMAGE_EX * IN  prImage
, RECT                  IN  rClipRect
, BYTE *                OUT pbtDst
, BOOL                  IN  vflip
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );

  const int nW  = rDstSize.cx;
  const int nStrideW = BGRA_SIZE * nW;
  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY = nStrideY - rDstSize.cx;

  const int nOffset = rClipRect.top * nStrideY + rClipRect.left;

  int16_t * pbtY = prImage->arComponent[0].piData + nOffset;
  int16_t * pbtU = prImage->arComponent[1].piData + nOffset; 
  int16_t * pbtV = prImage->arComponent[2].piData + nOffset;

  uint32_t * piDst;
  int i, j;

  piDst = (uint32_t*)pbtDst;
  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  if (vflip)
    piDst += (rDstSize.cy-1)*nStrideW/BGRA_SIZE;

  for (j = 0; j < rDstSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
#ifdef CSP_FLOAT_TRANS
      float rgb_y = pbtY[0];
      float b_u0  = pbtU[0];
      float r_v0  = pbtV[0];

      YUV_TO_BGRA( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, r_v0 );
#else
      int rgb_y = g_RGB_Y_tab[ *pbtY ];
      int b_u0  = g_B_U_tab[ pbtU[0] ];
      int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
      int r_v0  = g_R_V_tab[ pbtV[0] ];
      YUV_TO_BGRA3( piDst, rgb_y, b_u0, g_uv0, r_v0 );

#endif
      pbtY += (1);
      pbtU += (1);
      pbtV += (1);
    }
    if (vflip)
      piDst -= 2*nStrideW/ BGRA_SIZE;

    pbtY += nDiffY;
    pbtU += nDiffY;
    pbtV += nDiffY;
  }
}

//-----------------------------------------------------------------------------
void wci_yv24a_out_to_bgra_diffimg(
  const DIFF_IMAGE_EX * IN  prImage
, RECT                  IN  rClipRect
, BYTE *                OUT pbtDst
, BOOL                  IN  vflip
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );

  const int nW  = rDstSize.cx;
  const int nStrideW = BGRA_SIZE * nW;
  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY = nStrideY - rDstSize.cx;

  const int nOffset = rClipRect.top * nStrideY + rClipRect.left;

  int16_t * pbtY = prImage->arComponent[0].piData + nOffset;
  int16_t * pbtU = prImage->arComponent[1].piData + nOffset; 
  int16_t * pbtV = prImage->arComponent[2].piData + nOffset;
  int16_t * piA  = prImage->arComponent[3].piData + nOffset;

  uint32_t * piDst;
  int i, j;

  piDst = (uint32_t*)pbtDst;
  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  if (vflip)
    piDst += (rDstSize.cy-1)*nStrideW/BGRA_SIZE;

  for (j = 0; j < rDstSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
#ifdef CSP_FLOAT_TRANS
      float rgb_y = pbtY[0];
      float b_u0  = pbtU[0];
      float r_v0  = pbtV[0];

      YUV_TO_BGRA( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, r_v0 );
#else
      int rgb_y = g_RGB_Y_tab[ *pbtY ];
      int b_u0  = g_B_U_tab[ pbtU[0] ];
      int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
      int r_v0  = g_R_V_tab[ pbtV[0] ];
      YUV_TO_BGRA4( piDst, rgb_y, b_u0, g_uv0, r_v0, piA[0] );

#endif
      pbtY += (1);
      piA  += (1);
      pbtU += (1);
      pbtV += (1);
    }
 
    if (vflip)
      piDst -= 2*nStrideW/ BGRA_SIZE;
    
    pbtY += nDiffY;
    piA  += nDiffY;
    pbtU += nDiffY;
    pbtV += nDiffY;
  }
}

//-----------------------------------------------------------------------------


void wci_yv24_out_to_bgr_diffimg(
  const DIFF_IMAGE_EX * IN  prImage
, RECT             IN  rClipRect
, BYTE *           OUT pbtDst
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );

  const int nW  = rDstSize.cx;
  const int nStrideW = 4 * ((rDstSize.cx*BGR_SIZE+3)/4) ;
  const int iDiffW = nStrideW - BGR_SIZE*nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY = nStrideY - rDstSize.cx;

  const int nOffset = rClipRect.top * nStrideY + rClipRect.left;

  int16_t * pbtY = prImage->arComponent[0].piData + nOffset;
  int16_t * pbtU = prImage->arComponent[1].piData + nOffset; 
  int16_t * pbtV = prImage->arComponent[2].piData + nOffset;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  for (j = 0; j < rDstSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
#ifdef CSP_FLOAT_TRANS
      float rgb_y = pbtY[0];
      float b_u0  = pbtU[0];
      float r_v0  = pbtV[0];

      YUV_TO_BGR( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, r_v0 );
#else
      int rgb_y = g_RGB_Y_tab[ *pbtY ];
      int b_u0  = g_B_U_tab[ pbtU[0] ];
      int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
      int r_v0  = g_R_V_tab[ pbtV[0] ];

      YUV_TO_BGR( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );
#endif
      pbtDst += (BGR_SIZE);
      pbtY += (1);
      pbtU += (1);
      pbtV += (1);
    }
    pbtDst += iDiffW + (1-1)*nStrideW;
    pbtY += nDiffY;
    pbtU += nDiffY;
    pbtV += nDiffY;
  }
}

//------------------------------------------------------------------------------

void wci_grayscale_out_to_grayscale_diffimg(
  const DIFF_IMAGE_EX * IN  prImage
, RECT             IN  rClipRect
, BYTE *           OUT pbtDst
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );
  const int  nW       = rDstSize.cx;
  const int  nStrideW = 4 * ((rDstSize.cx+3)/4) ;
  const int  nStrideY = prImage->arComponent[0].rSize.cx;
  int16_t *  pbtY = prImage->arComponent[0].piData;
  int   j;

  for (j = 0; j < rDstSize.cy; j+=(1))
  {
    int i;
    for ( i=0; i<nW; ++i )
      pbtDst[i] = g_bound[pbtY[i]];
    pbtDst += nStrideW;
    pbtY += nStrideY;
  }
}

//------------------------------------------------------------------------------

void wci_grayscale_out_to_bgra_diffimg(
  const DIFF_IMAGE_EX * IN  prImage
, RECT                  IN  rClipRect
, BYTE *                OUT pbtDst
, BOOL                  IN  vflip
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );
  const int nW  = rDstSize.cx;
  const int nStrideW = BGRA_SIZE * nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY = nStrideY - rDstSize.cx;

  const int nOffset = rClipRect.top * nStrideY + rClipRect.left;

  int16_t * pbtY = prImage->arComponent[0].piData;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx > 0 );

  if (vflip)
    pbtDst += (rDstSize.cy-1)*nStrideW;

  for (j = 0; j < rDstSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
      pbtDst[(0)] = g_bound[*pbtY];
      pbtDst[(1)] = g_bound[*pbtY];
      pbtDst[(2)] = g_bound[*pbtY++];
      pbtDst[(3)] =      255;

      pbtDst += (BGRA_SIZE);
    }
    
    if (vflip)
      pbtDst -= 2*nStrideW;
    pbtY += nDiffY;
  }
}

//------------------------------------------------------------------------------

void wci_bgr_out_to_bgra_diffimg(
  const DIFF_IMAGE_EX * IN  prImage
, RECT                  IN  rClipRect
, BYTE *                OUT pbtDst
, BOOL                  IN  vflip
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );
  const int nW  = rDstSize.cx;
  const int nStrideW = BGRA_SIZE * nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY = nStrideY - rDstSize.cx;

  const int nOffset = rClipRect.top * nStrideY + rClipRect.left;

  int16_t * pbtB = prImage->arComponent[0].piData + nOffset;
  int16_t * pbtG = prImage->arComponent[1].piData + nOffset; 
  int16_t * pbtR = prImage->arComponent[2].piData + nOffset;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  if (vflip)
    pbtDst += (rDstSize.cy-1)*nStrideW;

  for (j = 0; j < rDstSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
      pbtDst[(0)] = g_bound[*pbtB++];
      pbtDst[(1)] = g_bound[*pbtG++];
      pbtDst[(2)] = g_bound[*pbtR++];
      pbtDst[(3)] =       0;

      pbtDst += (BGRA_SIZE);
    }
    if (vflip)
      pbtDst -= 2*nStrideW;

    pbtB += nDiffY;
    pbtG += nDiffY;
    pbtR += nDiffY;
  }
}


//------------------------------------------------------------------------------

void wci_bgra_out_to_bgra_diffimg(
  const DIFF_IMAGE_EX * IN  prImage
, RECT                  IN  rClipRect
, BYTE *                OUT pbtDst
, BOOL                  IN  vflip
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );
  const int nW  = rDstSize.cx;
  const int nStrideW = BGRA_SIZE * nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY = nStrideY - rDstSize.cx;

  const int nOffset = rClipRect.top * nStrideY + rClipRect.left;

  int16_t * pbtB = prImage->arComponent[0].piData + nOffset;
  int16_t * pbtG = prImage->arComponent[1].piData + nOffset; 
  int16_t * pbtR = prImage->arComponent[2].piData + nOffset;
  int16_t * pbtA = prImage->arComponent[3].piData + nOffset;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  if (vflip)
    pbtDst += (rDstSize.cy-1)*nStrideW;

  for (j = 0; j < rDstSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
      pbtDst[(0)] = g_bound[*pbtB++];
      pbtDst[(1)] = g_bound[*pbtG++];
      pbtDst[(2)] = g_bound[*pbtR++];
      pbtDst[(3)] = g_bound[*pbtA++];

      pbtDst += (BGRA_SIZE);
    }

    if (vflip)
      pbtDst -= 2*nStrideW;

    pbtB += nDiffY;
    pbtG += nDiffY;
    pbtR += nDiffY;
    pbtA += nDiffY;
  }
}

//------------------------------------------------------------------------------

void wci_bgr_out_to_bgr_diffimg(
  const DIFF_IMAGE_EX * IN  prImage
, RECT             IN  rClipRect
, BYTE *           OUT pbtDst
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );
  const int nW  = rDstSize.cx;
  const int nStrideW = BGR_SIZE * nW;
  const int iDiffW = nStrideW - BGR_SIZE*nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY = nStrideY - rDstSize.cx;

  const int nOffset = rClipRect.top * nStrideY + rClipRect.left;

  int16_t * pbtB = prImage->arComponent[0].piData + nOffset;
  int16_t * pbtG = prImage->arComponent[1].piData + nOffset; 
  int16_t * pbtR = prImage->arComponent[2].piData + nOffset;
  const int iCorrection = (4 - nStrideW%4)&3;


  int i, j;

  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  for (j = 0; j < rDstSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
      pbtDst[(0)] = g_bound[*pbtB++];
      pbtDst[(1)] = g_bound[*pbtG++];
      pbtDst[(2)] = g_bound[*pbtR++];
      pbtDst += (BGR_SIZE);
    }
    pbtDst += iDiffW + (1-1)*nStrideW + iCorrection;

    pbtB += nDiffY;
    pbtG += nDiffY;
    pbtR += nDiffY;
  }
}

//-----------------------------------------------------------------------------

void wci_bgr_out_to_rgb565_diffimg(
  const DIFF_IMAGE_EX * IN  prImage
, RECT             IN  rClipRect
, BYTE *           OUT pbtDst
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );

  const int nW  = rDstSize.cx;
  const int nStrideW = RGB565_SIZE * nW;
  const int iDiffW = nStrideW - RGB565_SIZE*nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY = nStrideY - rDstSize.cx;

  const int nOffset = rClipRect.top * nStrideY + rClipRect.left;

  int16_t * pbtB = prImage->arComponent[0].piData + nOffset;
  int16_t * pbtG = prImage->arComponent[1].piData + nOffset; 
  int16_t * pbtR = prImage->arComponent[2].piData + nOffset;

  const uint8_t * null = NULL;

  int i, j;

  assert( (iDiffW & 1) == 0 );
  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  if ( (pbtDst - null) & 1 )
  {
    for (j = 0; j < rDstSize.cy; j+=(1))
    {
      for (i = 0; i < nW; i+=(1))
      {
        uint16_t pixel = RGB_TO_RGB565(*pbtR++, *pbtG++, *pbtB++);

        pbtDst[(1)] = (BYTE)(pixel >> 8);                   
        pbtDst[(0)] = (BYTE)(pixel);                        

        pbtDst += (RGB565_SIZE);
      }
      pbtDst += iDiffW + (1-1)*nStrideW;

      pbtB += nDiffY;
      pbtG += nDiffY;
      pbtR += nDiffY;
    }

  }
  else
  {
    uint16_t * image = (uint16_t *)pbtDst;

    for (j = 0; j < rDstSize.cy; j+=(1))
    {
      for (i = 0; i < nW; i+=(1))
      {
        *image = RGB_TO_RGB565(*pbtR++, *pbtG++, *pbtB++);                       
        image += (RGB565_SIZE)/sizeof(uint16_t);
      }
      image += iDiffW/sizeof(uint16_t);

      pbtB += nDiffY;
      pbtG += nDiffY;
      pbtR += nDiffY;
    }
  }
}

//------------------------------------------------------------------------------
#if defined (ENCODER_SUPPORT)  || defined (_WCI_M4410_)

void wci_yv12_out_to_yv12(
  const IMAGE_EX * IN  prImage
, RECT             IN  rClipRect
, BYTE *           OUT pbtDst
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );

  const int nEvenW  = rDstSize.cx & ~1;
  const int nEvenH  = rDstSize.cy & ~1;

  BYTE * pbtY = prImage->arComponent[0].pbtData;
  BYTE * pbtU = prImage->arComponent[1].pbtData; 
  BYTE * pbtV = prImage->arComponent[2].pbtData;

  int iSizeY = prImage->arComponent[0].rSize.cx * prImage->arComponent[0].rSize.cy;
  int iSizeUV; 
  assert( prImage->arComponent[0].rSize.cx == 2*prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  iSizeUV = (nEvenW * nEvenH)/4;
  if ( rDstSize.cx - nEvenW )
    iSizeUV += nEvenH/2;
  if ( rDstSize.cy - nEvenH )
  {
    iSizeUV += nEvenW/2;
    if ( rDstSize.cx - nEvenW )
      iSizeUV += 1;
  }

  memcpy(pbtDst, pbtY, iSizeY );
  memcpy( &pbtDst[iSizeY], pbtU, iSizeUV);
  memcpy(&pbtDst[iSizeY + iSizeUV], pbtV, iSizeUV);


}
#endif //ENCODER_SUPPORT

//------------------------------------------------------------------------------

void wci_yv24_out_to_rgb565_diffimg(
  const DIFF_IMAGE_EX * IN  prImage
, RECT             IN  rClipRect
, BYTE *           OUT pbtDst
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );

  const int nW  = rDstSize.cx;
  const int nStrideW = RGB565_SIZE * nW;
  const int iDiffW = nStrideW - RGB565_SIZE*nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY = nStrideY - rDstSize.cx;
  const int nOffset = rClipRect.top * nStrideY + rClipRect.left;

  int16_t * pbtY = prImage->arComponent[0].piData + nOffset;
  int16_t * pbtU = prImage->arComponent[1].piData + nOffset; 
  int16_t * pbtV = prImage->arComponent[2].piData + nOffset;
  const uint8_t * null = NULL;
  int i, j;

  assert( (iDiffW & 1) == 0 );
  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );
  
  if ( (pbtDst - null) & 1 )
  {
    for (j = 0; j < rDstSize.cy; j+=(1))
    {
      for (i = 0; i < nW; i+=(1))
      {
        int rgb_y = g_RGB_Y_tab[ *pbtY ];
        int b_u0  = g_B_U_tab[ pbtU[0] ];
        int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
        int r_v0  = g_R_V_tab[ pbtV[0] ];

        YUV_TO_RGB565( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );
        pbtDst += (RGB565_SIZE);
        pbtY += (1);
        pbtU += (1);
        pbtV += (1);
      }
      pbtDst += iDiffW + (1-1)*nStrideW;
      pbtY += nDiffY;
      pbtU += nDiffY;
      pbtV += nDiffY;
    }
  }
  else
  {
    uint16_t * image = (uint16_t *)pbtDst;
    const int   nStrideW_p16 = nStrideW / sizeof(uint16_t); 

    for (j = 0; j < rDstSize.cy; j+=(1))
    {
      for (i = 0; i < nW; i+=(1))
      {
        int rgb_y = g_RGB_Y_tab[ *pbtY ];
        int b_u0  = g_B_U_tab[ pbtU[0] ];
        int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
        int r_v0  = g_R_V_tab[ pbtV[0] ];
        YUV_TO_RGB565_16( image, 0, 0, nStrideW_p16, rgb_y, b_u0, g_uv0, r_v0 );
        image += (RGB565_SIZE)/sizeof(uint16_t);
        pbtY += (1);
        pbtU += (1);
        pbtV += (1);
      }
      image += iDiffW/sizeof(uint16_t);
      pbtY += nDiffY;
      pbtU += nDiffY;
      pbtV += nDiffY;
    }
  }
}


//------------------------------------------------------------------------------

void wci_yv12_out_to_rgb565_diffimg(
  const DIFF_IMAGE_EX * IN  prImage
, RECT             IN  rClipRect
, BYTE *           OUT pbtDst
)
{
  const SIZE rDstSize = wci_rect_get_size( rClipRect );

  const int nEvenW  = rDstSize.cx & ~1;
  const int nEvenH  = rDstSize.cy & ~1;
  const int nStrideW = RGB565_SIZE * rDstSize.cx;
  const int iDiffW = nStrideW - RGB565_SIZE * nEvenW;
  const int nStrideY  = prImage->arComponent[0].rSize.cx;
  const int nStrideUV = prImage->arComponent[1].rSize.cx;

  int16_t * pbtY = prImage->arComponent[0].piData;
  int16_t * pbtU = prImage->arComponent[1].piData; 
  int16_t * pbtV = prImage->arComponent[2].piData;

  const int nDiffY = nStrideY - nEvenW;
  const int nDiffUV = nStrideUV - nEvenW/2;
  const uint8_t * null = NULL;

  int i, j;

  pbtY += rClipRect.top   * nStrideY  + rClipRect.left  ;
  pbtU += rClipRect.top/2 * nStrideUV + rClipRect.left/2;
  pbtV += rClipRect.top/2 * nStrideUV + rClipRect.left/2;

  assert( (iDiffW & 1) == 0 );


  if ( (pbtDst - null) & 1 )
  {
    for (j = 0; j < nEvenH; j+=(2))
    {
      for (i = 0; i < nEvenW; i+=(2))
      {
        int rgb_y;
        int b_u0  = g_B_U_tab[ pbtU[0] ];
        int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
        int r_v0  = g_R_V_tab[ pbtV[0] ];

        rgb_y = g_RGB_Y_tab[ pbtY[(0)*nStrideY + 0] ];
        YUV_TO_RGB565( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

        rgb_y = g_RGB_Y_tab[ pbtY[(0)*nStrideY + 1] ];
        YUV_TO_RGB565( pbtDst, RGB565_SIZE, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

        rgb_y = g_RGB_Y_tab[ pbtY[(1)*nStrideY + 0] ];
        YUV_TO_RGB565( pbtDst, 0, 1, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

        rgb_y = g_RGB_Y_tab[ pbtY[(1)*nStrideY + 1] ];
        YUV_TO_RGB565( pbtDst, RGB565_SIZE, 1, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

        pbtDst += (2) * RGB565_SIZE;
        pbtY += (2);
        pbtU += (2)/2;
        pbtV += (2)/2;
      }

      if ( rDstSize.cx - nEvenW )
      {
        int rgb_y;
        int b_u0 = g_B_U_tab[ pbtU[0] ];
        int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
        int r_v0 = g_R_V_tab[ pbtV[0] ];

        rgb_y = g_RGB_Y_tab[ pbtY[(0)*nStrideY + 0] ];
        YUV_TO_RGB565( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

        rgb_y = g_RGB_Y_tab[ pbtY[(1)*nStrideY + 0] ];
        YUV_TO_RGB565( pbtDst, 0, 1, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );
      }
      pbtDst += iDiffW + (2-1)*nStrideW;
      pbtY += nDiffY + (2-1)*nStrideY;
      pbtU += nDiffUV + ((2/2)-1)*nStrideUV;
      pbtV += nDiffUV + ((2/2)-1)*nStrideUV;
    }
    if ( rDstSize.cy - nEvenH )
    {
      for (i = 0; i < nEvenW; i+=(2))
      {
        int rgb_y;
        int b_u0 = g_B_U_tab[ pbtU[0] ];
        int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
        int r_v0 = g_R_V_tab[ pbtV[0] ];

        rgb_y = g_RGB_Y_tab[ pbtY[0] ];
        YUV_TO_RGB565( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

        rgb_y = g_RGB_Y_tab[ pbtY[1] ];
        YUV_TO_RGB565( pbtDst, RGB565_SIZE, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );

        pbtDst += (2)*(2);
        pbtY += (2);
        pbtU += (2)/2;
        pbtV += (2)/2;
      }
      if ( rDstSize.cx - nEvenW )
      {
        int rgb_y;
        int b_u0 = g_B_U_tab[ pbtU[0] ];
        int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
        int r_v0 = g_R_V_tab[ pbtV[0] ];

        rgb_y = g_RGB_Y_tab[ pbtY[0] ];
        YUV_TO_RGB565( pbtDst, 0, 0, nStrideW, rgb_y, b_u0, g_uv0, r_v0 );
      }
    }
  }
  else
  {
    uint16_t * image = (uint16_t *)pbtDst;
    const int   nStrideW_p16 = nStrideW / sizeof(uint16_t); 
    for (j = 0; j < nEvenH; j+=(2))
    {
      for (i = 0; i < nEvenW; i+=(2))
      {
        int rgb_y;
        int b_u0  = g_B_U_tab[ pbtU[0] ];
        int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
        int r_v0  = g_R_V_tab[ pbtV[0] ];

        rgb_y = g_RGB_Y_tab[ pbtY[(0)*nStrideY + 0] ];
        YUV_TO_RGB565_16( image, 0, 0, nStrideW_p16, rgb_y, b_u0, g_uv0, r_v0 );

        rgb_y = g_RGB_Y_tab[ pbtY[(0)*nStrideY + 1] ];
        YUV_TO_RGB565_16( image, RGB565_SIZE/sizeof(uint16_t), 0, nStrideW_p16, rgb_y, b_u0, g_uv0, r_v0 );

        rgb_y = g_RGB_Y_tab[ pbtY[(1)*nStrideY + 0] ];
        YUV_TO_RGB565_16( image, 0, 1, nStrideW_p16, rgb_y, b_u0, g_uv0, r_v0 );

        rgb_y = g_RGB_Y_tab[ pbtY[(1)*nStrideY + 1] ];
        YUV_TO_RGB565_16( image, RGB565_SIZE/sizeof(uint16_t), 1, nStrideW_p16, rgb_y, b_u0, g_uv0, r_v0 );

        image+= (2) * RGB565_SIZE/sizeof(uint16_t);
        pbtY += (2);
        pbtU += (2)/2;
        pbtV += (2)/2;
      }

      if ( rDstSize.cx - nEvenW )
      {
        int rgb_y;
        int b_u0 = g_B_U_tab[ pbtU[0] ];
        int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
        int r_v0 = g_R_V_tab[ pbtV[0] ];

        rgb_y = g_RGB_Y_tab[ pbtY[(0)*nStrideY + 0] ];
        YUV_TO_RGB565_16( image, 0, 0, nStrideW_p16, rgb_y, b_u0, g_uv0, r_v0 );

        rgb_y = g_RGB_Y_tab[ pbtY[(1)*nStrideY + 0] ];
        YUV_TO_RGB565_16( image, 0, 1, nStrideW_p16, rgb_y, b_u0, g_uv0, r_v0 );
      }

      image+= iDiffW/sizeof(uint16_t) + nStrideW_p16;
      pbtY += nDiffY + (2-1)*nStrideY;
      pbtU += nDiffUV + ((2/2)-1)*nStrideUV;
      pbtV += nDiffUV + ((2/2)-1)*nStrideUV;
    }
    if ( rDstSize.cy - nEvenH )
    {
      for (i = 0; i < nEvenW; i+=(2))
      {
        int rgb_y;
        int b_u0 = g_B_U_tab[ pbtU[0] ];
        int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
        int r_v0 = g_R_V_tab[ pbtV[0] ];

        rgb_y = g_RGB_Y_tab[ pbtY[0] ];
        YUV_TO_RGB565_16( image, 0, 0, nStrideW_p16, rgb_y, b_u0, g_uv0, r_v0 );

        rgb_y = g_RGB_Y_tab[ pbtY[1] ];
        YUV_TO_RGB565_16( image, RGB565_SIZE/sizeof(uint16_t), 0, nStrideW_p16, rgb_y, b_u0, g_uv0, r_v0 );

        image += (2);
        pbtY += (2);
        pbtU += (2)/2;
        pbtV += (2)/2;
      }
      if ( rDstSize.cx - nEvenW )
      {
        int rgb_y;
        int b_u0 = g_B_U_tab[ pbtU[0] ];
        int g_uv0 = g_G_U_tab[ pbtU[0] ] + g_G_V_tab[ pbtV[0] ];
        int r_v0 = g_R_V_tab[ pbtV[0] ];

        rgb_y = g_RGB_Y_tab[ pbtY[0] ];
        YUV_TO_RGB565_16( image, 0, 0, nStrideW_p16, rgb_y, b_u0, g_uv0, r_v0 );
      }
    }
  }
}


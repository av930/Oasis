/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: ColorSpaceIn.c

ABSTRACT:    This file contains procedures for color space RGB to YUV transformation

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
#include "wci_color_space_in.h"

//------------------------------------------------------------------------------

#define CSP_FLOAT_TRANS

#define GET_BGR( pbtImage, nX, nY, nStride, R, G, B, C1, C2, C3 )              \
	(R) = pbtImage[(nY)*nStride+(nX)+(C1)];			                                 \
	(G) = pbtImage[(nY)*nStride+(nX)+(C2)];						                           \
	(B) = pbtImage[(nY)*nStride+(nX)+(C3)]

#define GET_BGR565( piImg,  R, G, B )                                          \
    {                                                                          \
    int32_t iData =  *piImg;                                                   \
    (R) =   (iData & 0xF800) >> 8;                                             \
    (G) =   (iData & 0x07E0) >> 3;                                             \
    (B) =   (iData & 0x001F) << 3;                                             \
    }

#define GET_BGR_UV( pbtImage, nX, nY, nStride, R, G, B, R_UV, G_UV, B_UV, C1, C2, C3 ) \
	R_UV +=( R) = pbtImage[(nY)*nStride+(nX)+(C1)]; 	                           \
	G_UV +=( G) = pbtImage[(nY)*nStride+(nX)+(C2)];	                             \
	B_UV +=( B) = pbtImage[(nY)*nStride+(nX)+(C3)]


#define GET_BGR_UV565( piImg,  R, G, B, R_UV, G_UV, B_UV)                       \
    {                                                                           \
       int32_t iData =  *piImg;                                                 \
       R_UV +=(R) =   (iData & 0xF800) >> 8;                                    \
       G_UV +=(G) =   (iData & 0x07E0) >> 3;                                    \
       B_UV +=(B) =   (iData & 0x001F) << 3;                                    \
     }



  
#ifdef CSP_FLOAT_TRANS
#if  0
#define RGB_TO_Y( R, G, B )    (BYTE)( (Y_R_IN*R + Y_G_IN*G + Y_B_IN*B + 0.5) + Y_ADD_IN )
#else
#define RGB_TO_Y  wci_rgb_to_y
__inline BYTE wci_rgb_to_y(int r, int g, int b)
{
  double var;
  var = Y_R_IN*r;
  var +=Y_G_IN*g;
  var +=Y_B_IN*b;
  var += 0.5 + Y_ADD_IN;
  return (BYTE)var;
}

#endif

  #define RGB_TO_U( R, G, B, N ) (BYTE)( ((int)(-U_R_IN*R - U_G_IN*G + U_B_IN*B + 0.5) >> N) + U_ADD_IN )

  #define RGB_TO_V( R, G, B, N ) (BYTE)( ((int)( V_R_IN*R - V_G_IN*G - V_B_IN*B + 0.5) >> N) + V_ADD_IN )

#else

  #define RGB_TO_Y( R, G, B ) (BYTE) (	                                        \
                                ((FIX_IN(Y_R_IN)*R + FIX_IN(Y_G_IN)*G           \
                                + FIX_IN(Y_B_IN)*B) >> SCALEBITS_IN) + Y_ADD_IN \
                              )

  #define RGB_TO_U( R, G, B, N ) (BYTE) (	                                      \
                                ((-FIX_IN(U_R_IN)*R - FIX_IN(U_G_IN)*G          \
                                + FIX_IN(U_B_IN)*B) >> (SCALEBITS_IN + N)       \
                                ) + U_ADD_IN )

  #define RGB_TO_V( R, G, B, N ) (BYTE) (	                                      \
                                ((FIX_IN(V_R_IN)*R - FIX_IN(V_G_IN)*G           \
                                - FIX_IN(V_B_IN)*B) >> (SCALEBITS_IN + N)       \
                                ) + V_ADD_IN )

#endif

//------------------------------------------------------------------------------

void wci_yv12_in_from_bgra(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nEvenW  = rSrcSize.cx & ~1;
  const int nEvenH  = rSrcSize.cy & ~1;

  const int nStrideW = BGRA_SIZE * rSrcSize.cx;
  const int iDiffW = nStrideW - BGRA_SIZE * nEvenW;

  BYTE * pbtY = prImage->arComponent[0].pbtData;
  BYTE * pbtU = prImage->arComponent[1].pbtData; 
  BYTE * pbtV = prImage->arComponent[2].pbtData;

  const int nStrideY  = prImage->arComponent[0].rSize.cx;
  const int nStrideUV = prImage->arComponent[1].rSize.cx;
  
  const int nDiffY = nStrideY - nEvenW;
  const int nDiffUV = nStrideUV - nEvenW/2;
  
  int i, j;

  assert( prImage->arComponent[0].rSize.cx == 2*prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  for (j = 0; j < nEvenH; j+=(2))
  {
    for (i = 0; i < nEvenW; i+=(2))
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;
      GET_BGR_UV( pbtSrc, 0, 0, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );

      GET_BGR_UV( pbtSrc, BGRA_SIZE, 0, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 1] = RGB_TO_Y( r, g, b );

      GET_BGR_UV( pbtSrc, 0, 1, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[1*nStrideY + 0] = RGB_TO_Y( r, g, b );
      
      GET_BGR_UV( pbtSrc, BGRA_SIZE, 1, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[1*nStrideY + 1] = RGB_TO_Y( r, g, b );

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 2 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 2 );

      pbtSrc += (2) * BGRA_SIZE;

      pbtY += (2);
      pbtU += (2)/2;
      pbtV += (2)/2;
    }
    if ( rSrcSize.cx - nEvenW )
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;

      GET_BGR_UV( pbtSrc, 0, 0, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );
      
      GET_BGR_UV( pbtSrc, 0, 1, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[1*nStrideY + 0] = RGB_TO_Y( r, g, b );

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 1 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 1 );
    }
    pbtSrc += iDiffW + (2-1)*nStrideW;
    pbtY += nDiffY + (2-1)*nStrideY;
    pbtU += nDiffUV + ((2/2)-1)*nStrideUV;
    pbtV += nDiffUV + ((2/2)-1)*nStrideUV;
  }
  if ( rSrcSize.cy - nEvenH )
  {
    for (i = 0; i < nEvenW; i+=(2))
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;

      GET_BGR_UV( pbtSrc, 0, 0, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );
      
      GET_BGR_UV( pbtSrc, BGRA_SIZE, 0, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 1] = RGB_TO_Y( r, g, b );

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 1 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 1 );

      pbtSrc += (2) * BGRA_SIZE;
      pbtY += (2);
      pbtU += (2)/2;
      pbtV += (2)/2;
    }

    if ( rSrcSize.cx - nEvenW )
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;

      GET_BGR_UV( pbtSrc, 0, 0, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );
      
      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 0 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 0 );
    }
  }
}

//------------------------------------------------------------------------------

void wci_yv12_in_from_yv12(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nEvenW  = rSrcSize.cx & ~1;
  const int nEvenH  = rSrcSize.cy & ~1;

  BYTE * pbtY = prImage->arComponent[0].pbtData;
  BYTE * pbtU = prImage->arComponent[1].pbtData; 
  BYTE * pbtV = prImage->arComponent[2].pbtData;

  int iSizeY = wci_get_square(prImage->arComponent[0].rSize);
  int iSizeUV;
  assert( prImage->arComponent[0].rSize.cx == 2*prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  iSizeUV = (nEvenW * nEvenH)/4;
  if ( rSrcSize.cx - nEvenW )
    iSizeUV += nEvenH/2;
  if ( rSrcSize.cy - nEvenH )
  {
    iSizeUV += nEvenW/2;
    if ( rSrcSize.cx - nEvenW )
      iSizeUV += 1;
  }
  memcpy(pbtY, pbtSrc, iSizeY );
  memcpy(pbtU, &pbtSrc[iSizeY], iSizeUV);
  memcpy(pbtV, &pbtSrc[iSizeY + iSizeUV], iSizeUV);
}

//------------------------------------------------------------------------------


void wci_yv12_in_from_bgr(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nEvenW  = rSrcSize.cx & ~1;
  const int nEvenH  = rSrcSize.cy & ~1;

  const int nStrideW = BGR_SIZE * rSrcSize.cx;
  const int iDiffW = nStrideW - BGR_SIZE * nEvenW;

  BYTE * pbtY = prImage->arComponent[0].pbtData;
  BYTE * pbtU = prImage->arComponent[1].pbtData; 
  BYTE * pbtV = prImage->arComponent[2].pbtData;

  const int nStrideY  = prImage->arComponent[0].rSize.cx;
  const int nStrideUV = prImage->arComponent[1].rSize.cx;

  const int nDiffY = nStrideY - nEvenW;
  const int nDiffUV = nStrideUV - nEvenW/2;
  const int iCorrection = (4 - nStrideW%4)&3;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx <= 2*prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  for (j = 0; j < nEvenH; j+=(2))
  {
    for (i = 0; i < nEvenW; i+=(2))
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;
      GET_BGR_UV( pbtSrc, 0, 0, (nStrideW + iCorrection) , r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );

      GET_BGR_UV( pbtSrc, BGR_SIZE, 0, (nStrideW + iCorrection), r, g, b, r_uv, g_uv, b_uv, 2, 1, 0);
      pbtY[0*nStrideY + 1] = RGB_TO_Y( r, g, b );

      GET_BGR_UV( pbtSrc, 0, 1, (nStrideW + iCorrection) , r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[1*nStrideY + 0] = RGB_TO_Y( r, g, b );

      GET_BGR_UV( pbtSrc, BGR_SIZE, 1, (nStrideW + iCorrection), r, g, b, r_uv, g_uv, b_uv, 2, 1, 0);
      pbtY[1*nStrideY + 1] = RGB_TO_Y( r, g, b );

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 2 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 2 );

      pbtSrc += (2) * BGR_SIZE ;

      pbtY += (2);
      pbtU += (2)/2;
      pbtV += (2)/2;
    }
    if ( rSrcSize.cx - nEvenW )
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;

     GET_BGR_UV( pbtSrc, 0, 0, (nStrideW + iCorrection), r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
     pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );

      GET_BGR_UV( pbtSrc, 0, 1, (nStrideW + iCorrection), r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[1*nStrideY + 0] = RGB_TO_Y( r, g, b );

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 1 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 1 );
    }
    pbtSrc += iDiffW + (2-1)*nStrideW +  iCorrection * 2;
    pbtY += nDiffY + (2-1)*nStrideY;
    pbtU += nDiffUV + ((2/2)-1)*nStrideUV;
    pbtV += nDiffUV + ((2/2)-1)*nStrideUV;
  }
  if ( rSrcSize.cy - nEvenH )
  {
    for (i = 0; i < nEvenW; i+=(2))
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;

      GET_BGR_UV( pbtSrc, 0, 0, (nStrideW + iCorrection), r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );

      GET_BGR_UV( pbtSrc, BGR_SIZE, 0,(nStrideW + iCorrection), r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 1] = RGB_TO_Y( r, g, b );

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 1 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 1 );

      pbtSrc += (2) * BGR_SIZE;
      pbtY += (2);
      pbtU += (2)/2;
      pbtV += (2)/2;
    }

    if ( rSrcSize.cx - nEvenW )
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;

      GET_BGR_UV( pbtSrc, 0, 0, (nStrideW + iCorrection), r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 0 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 0 );
    }
  }
}

//------------------------------------------------------------------------------


void wci_yv12a_in_from_bgra(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nEvenW  = rSrcSize.cx & ~1;
  const int nEvenH  = rSrcSize.cy & ~1;

  const int nStrideW = BGRA_SIZE * rSrcSize.cx;
  const int iDiffW = nStrideW - BGRA_SIZE * nEvenW;

  BYTE * pbtY = prImage->arComponent[0].pbtData;
  BYTE * pbtU = prImage->arComponent[1].pbtData; 
  BYTE * pbtV = prImage->arComponent[2].pbtData;
  BYTE * pbtA = prImage->arComponent[3].pbtData;

  const int nStrideY  = prImage->arComponent[0].rSize.cx;
  const int nStrideUV = prImage->arComponent[1].rSize.cx;

  const int nDiffY = nStrideY - nEvenW;
  const int nDiffUV = nStrideUV - nEvenW/2;

  int i, j;
  assert( prImage->arComponent[0].rSize.cx == 2*prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );
  for (j = 0; j < nEvenH; j+=(2))
  {
    for (i = 0; i < nEvenW; i+=(2))
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;


      GET_BGR_UV( pbtSrc, 0, 0, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );
      pbtA[0*nStrideY + 0] = pbtSrc[0*nStrideW + 0 + 3];

      GET_BGR_UV( pbtSrc, BGRA_SIZE, 0, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 1] = RGB_TO_Y( r, g, b );
      pbtA[0*nStrideY + 1] = pbtSrc[0*nStrideW + 4 + 3];

      GET_BGR_UV( pbtSrc, 0, 1, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[1*nStrideY + 0] = RGB_TO_Y( r, g, b );
      pbtA[1*nStrideY + 0] = pbtSrc[1*nStrideW + 0 + 3];

      GET_BGR_UV( pbtSrc, BGRA_SIZE, 1, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[1*nStrideY + 1] = RGB_TO_Y( r, g, b );
      pbtA[1*nStrideY + 1] = pbtSrc[1*nStrideW + 4 + 3];

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 2 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 2 );

      pbtSrc += (2) * BGRA_SIZE;

      pbtY += (2);
      pbtU += (2)/2;
      pbtV += (2)/2;
      pbtA += (2);
    }
    if ( rSrcSize.cx - nEvenW )
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;

      GET_BGR_UV( pbtSrc, 0, 0, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );
      pbtA[0*nStrideY + 0] = pbtSrc[0*nStrideW + 0 + 3];
      
      GET_BGR_UV( pbtSrc, 0, 1, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[1*nStrideY + 0] = RGB_TO_Y( r, g, b );
      pbtA[1*nStrideY + 0] = pbtSrc[1*nStrideW + 0 + 3];

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 1 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 1 );
    }
    pbtSrc += iDiffW + (2-1)*nStrideW;
    pbtY += nDiffY + (2-1)*nStrideY;
    pbtU += nDiffUV + ((2/2)-1)*nStrideUV;
    pbtV += nDiffUV + ((2/2)-1)*nStrideUV;
    pbtA += nDiffY + (2-1)*nStrideY; //????
  }
  if ( rSrcSize.cy - nEvenH )
  {
    for (i = 0; i < nEvenW; i+=(2))
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;

      GET_BGR_UV( pbtSrc, 0, 0, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );
      pbtA[0*nStrideY + 0] = pbtSrc[0*nStrideW + 0 + 3];

      GET_BGR_UV( pbtSrc, BGRA_SIZE, 0, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 1] = RGB_TO_Y( r, g, b );
      pbtA[0*nStrideY + 1] = pbtSrc[0*nStrideW + 4 + 3];

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 1 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 1 );

      pbtSrc += (2) * BGRA_SIZE;
      pbtY += (2);
      pbtU += (2)/2;
      pbtV += (2)/2;
      pbtA += (2);
    }

    if ( rSrcSize.cx - nEvenW )
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;

      GET_BGR_UV( pbtSrc, 0, 0, nStrideW, r, g, b, r_uv, g_uv, b_uv, 2, 1, 0 );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );
      pbtA[0*nStrideY + 0] = pbtSrc[0*nStrideW + 0 + 3];

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 0 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 0 );
    }
  }
}

//------------------------------------------------------------------------------

void wci_yv12_in_from_rgb565(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nEvenW  = rSrcSize.cx & ~1;
  const int nEvenH  = rSrcSize.cy & ~1;

  const int nStrideW = BGRA_SIZE/2 * rSrcSize.cx;
  const int iDiffW = nStrideW - BGRA_SIZE/2 * nEvenW;

  BYTE * pbtY = prImage->arComponent[0].pbtData;
  BYTE * pbtU = prImage->arComponent[1].pbtData; 
  BYTE * pbtV = prImage->arComponent[2].pbtData;

  const int nStrideY  = prImage->arComponent[0].rSize.cx;
  const int nStrideUV = prImage->arComponent[1].rSize.cx;

  const int nDiffY = nStrideY - nEvenW;
  const int nDiffUV = nStrideUV - nEvenW/2;
  const int iCorrection = (4 - nStrideW%4)&3;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx == 2*prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  for (j = 0; j < nEvenH; j+=(2))
  {
    for (i = 0; i < nEvenW; i+=(2))
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;
      GET_BGR_UV565((uint16_t*)&pbtSrc[0], r, g, b, r_uv, g_uv, b_uv );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );

      GET_BGR_UV565( (uint16_t*)&pbtSrc[2], r, g, b, r_uv, g_uv, b_uv );
      pbtY[0*nStrideY + 1] = RGB_TO_Y( r, g, b );

      GET_BGR_UV565( (uint16_t*)&pbtSrc[nStrideW+iCorrection], r, g, b, r_uv, g_uv, b_uv );
      pbtY[1*nStrideY + 0] = RGB_TO_Y( r, g, b );

      GET_BGR_UV565( (uint16_t*)&pbtSrc[nStrideW+2+iCorrection], r, g, b, r_uv, g_uv, b_uv );
      pbtY[1*nStrideY + 1] = RGB_TO_Y( r, g, b );

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 2 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 2 );

      pbtSrc += (2) * 2;
      pbtY += (2);
      pbtU += (2)/2;
      pbtV += (2)/2;
    }
    if ( rSrcSize.cx - nEvenW )
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;

      GET_BGR_UV565( (uint16_t*)&pbtSrc[0], r, g, b, r_uv, g_uv, b_uv );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );

      GET_BGR_UV565( (uint16_t*)&pbtSrc[nStrideW+iCorrection], r, g, b, r_uv, g_uv, b_uv );
      pbtY[1*nStrideY + 0] = RGB_TO_Y( r, g, b );

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 1 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 1 );
    }
    pbtSrc += iDiffW +  nStrideW + iCorrection * 2;
    pbtY += nDiffY + (2-1)*nStrideY;
    pbtU += nDiffUV + ((2/2)-1)*nStrideUV;
    pbtV += nDiffUV + ((2/2)-1)*nStrideUV;


  
  }
  if ( rSrcSize.cy - nEvenH )
  {
    for (i = 0; i < nEvenW; i+=(2))
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;

      GET_BGR_UV565( (uint16_t*)&pbtSrc[0], r, g, b, r_uv, g_uv, b_uv );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );

      GET_BGR_UV565( (uint16_t*)&pbtSrc[2], r, g, b, r_uv, g_uv, b_uv );
      pbtY[0*nStrideY + 1] = RGB_TO_Y( r, g, b );

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 1 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 1 );

      pbtSrc += (2) * 2;
      pbtY += (2);
      pbtU += (2)/2;
      pbtV += (2)/2;
    }

    if ( rSrcSize.cx - nEvenW )
    {
      int r, g, b;
      int r_uv = 0, g_uv = 0, b_uv = 0;

      GET_BGR_UV565( (uint16_t*)&pbtSrc[0], r, g, b, r_uv, g_uv, b_uv );
      pbtY[0*nStrideY + 0] = RGB_TO_Y( r, g, b );

      pbtU[0] = RGB_TO_U( r_uv, g_uv, b_uv, 0 );
      pbtV[0] = RGB_TO_V( r_uv, g_uv, b_uv, 0 );
    }
  }
}

//------------------------------------------------------------------------------

void wci_yv24_in_from_bgra(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nW       = rSrcSize.cx;
  const int nStrideW = BGRA_SIZE * nW;
  const int iDiffW   = nStrideW - BGRA_SIZE*nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY   = nStrideY - rSrcSize.cx;

  BYTE * pbtY = prImage->arComponent[0].pbtData;
  BYTE * pbtU = prImage->arComponent[1].pbtData; 
  BYTE * pbtV = prImage->arComponent[2].pbtData;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  for (j = 0; j < rSrcSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
      int r, g, b;

      GET_BGR( pbtSrc, 0, 0, nStrideW, r, g, b, 2, 1, 0 );
      pbtSrc += (BGRA_SIZE);

      *pbtY++ = RGB_TO_Y( r, g, b );
      *pbtU++ = RGB_TO_U( r, g, b, 0 );
      *pbtV++ = RGB_TO_V( r, g, b, 0 );
    }

    pbtSrc += iDiffW + (1-1)*nStrideW;

    pbtY += nDiffY;
    pbtU += nDiffY;
    pbtV += nDiffY;
  }
}

//------------------------------------------------------------------------------
void wci_yv24_in_from_bgr(
const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nW       = rSrcSize.cx;
  const int nStrideW = BGR_SIZE * nW;
  const int iDiffW   = nStrideW - BGR_SIZE*nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY   = nStrideY - rSrcSize.cx;

  BYTE * pbtY = prImage->arComponent[0].pbtData;
  BYTE * pbtU = prImage->arComponent[1].pbtData; 
  BYTE * pbtV = prImage->arComponent[2].pbtData;
  const int iCorrection = (4 - nStrideW%4)&3;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  for (j = 0; j < rSrcSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
      int r, g, b;

      GET_BGR( pbtSrc, 0, 0, nStrideW, r, g, b, 2, 1, 0 );
      pbtSrc += (BGR_SIZE);

      *pbtY++ = RGB_TO_Y( r, g, b );
      *pbtU++ = RGB_TO_U( r, g, b, 0 );
      *pbtV++ = RGB_TO_V( r, g, b, 0 );
    }

    pbtSrc += iDiffW + (1-1)*nStrideW + iCorrection;

    pbtY += nDiffY;
    pbtU += nDiffY;
    pbtV += nDiffY;
  }
}

//------------------------------------------------------------------------------

void wci_yv24_in_from_rgb565(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nW       = rSrcSize.cx;
  const int nStrideW = RGB565_SIZE * nW;
  const int iDiffW   = nStrideW - RGB565_SIZE*nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY   = nStrideY - rSrcSize.cx;

  BYTE * pbtY = prImage->arComponent[0].pbtData;
  BYTE * pbtU = prImage->arComponent[1].pbtData; 
  BYTE * pbtV = prImage->arComponent[2].pbtData;
  const int iCorrection = (4 - nStrideW%4)&3;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  for (j = 0; j < rSrcSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
      int r, g, b;

      GET_BGR565( (uint16_t*)&pbtSrc[0], r, g, b);
      pbtSrc += (RGB565_SIZE);

      *pbtY++ = RGB_TO_Y( r, g, b );
      *pbtU++ = RGB_TO_U( r, g, b, 0 );
      *pbtV++ = RGB_TO_V( r, g, b, 0 );
    }

    pbtSrc += iDiffW + (1-1)*nStrideW + iCorrection;
 
    pbtY += nDiffY;
    pbtU += nDiffY;
    pbtV += nDiffY;
  }
}

//------------------------------------------------------------------------------


void wci_yv24a_in_from_bgra(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nW       = rSrcSize.cx;
  const int nStrideW = BGRA_SIZE * nW;
  const int iDiffW   = nStrideW - BGRA_SIZE*nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY   = nStrideY - rSrcSize.cx;

  BYTE * pbtY = prImage->arComponent[0].pbtData;
  BYTE * pbtU = prImage->arComponent[1].pbtData; 
  BYTE * pbtV = prImage->arComponent[2].pbtData;
  BYTE * pbtA = prImage->arComponent[3].pbtData;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  for (j = 0; j < rSrcSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
      int r, g, b;

      GET_BGR( pbtSrc, 0, 0, nStrideW, r, g, b, 2, 1, 0 );
      *pbtA++ = pbtSrc[3];
      pbtSrc += (BGRA_SIZE);

      *pbtY++ = RGB_TO_Y( r, g, b );
      *pbtU++ = RGB_TO_U( r, g, b, 0 );
      *pbtV++ = RGB_TO_V( r, g, b, 0 );
    }

    pbtSrc += iDiffW + (1-1)*nStrideW;

    pbtY += nDiffY;
    pbtA += nDiffY;
    pbtU += nDiffY;
    pbtV += nDiffY;
  }
}

//------------------------------------------------------------------------------

void wci_grayscale_in_from_grayscale(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nW       = rSrcSize.cx;
  const int nStrideW = 4 * ((rSrcSize.cx+3)/4);

  const int nStrideY = prImage->arComponent[0].rSize.cx;

  BYTE * pbtY = prImage->arComponent[0].pbtData;

  int   j;

  for (j = 0; j < rSrcSize.cy; j+=(1))
  {
    memcpy( pbtY, pbtSrc, nW );
    pbtSrc += nStrideW;
    pbtY += nStrideY;
  }
}

//------------------------------------------------------------------------------

#ifdef ENCODER_SUPPORT

void wci_bgr_in_from_bgra(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nW       = rSrcSize.cx;
  const int nStrideW = BGRA_SIZE * nW;
  const int iDiffW   = nStrideW - BGRA_SIZE*nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY   = nStrideY - rSrcSize.cx;

  BYTE * pbtB = prImage->arComponent[0].pbtData;
  BYTE * pbtG = prImage->arComponent[1].pbtData; 
  BYTE * pbtR = prImage->arComponent[2].pbtData;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  for (j = 0; j < rSrcSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
      GET_BGR( pbtSrc, 0, 0, nStrideW, *pbtR++, *pbtG++, *pbtB++, 2, 1, 0 );
      pbtSrc += (BGRA_SIZE);
    }

    pbtSrc += iDiffW + (1-1)*nStrideW;

    pbtB += nDiffY;
    pbtG += nDiffY;
    pbtR += nDiffY;
  }
}

//------------------------------------------------------------------------------


void wci_bgra_in_from_bgra(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nW       = rSrcSize.cx;
  const int nStrideW = BGRA_SIZE * nW;
  const int iDiffW   = nStrideW - BGRA_SIZE*nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY   = nStrideY - rSrcSize.cx;

  BYTE * pbtB = prImage->arComponent[0].pbtData;
  BYTE * pbtG = prImage->arComponent[1].pbtData; 
  BYTE * pbtR = prImage->arComponent[2].pbtData;
  BYTE * pbtA = prImage->arComponent[3].pbtData;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  for (j = 0; j < rSrcSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
      GET_BGR( pbtSrc, 0, 0, nStrideW, *pbtR++, *pbtG++, *pbtB++, 2, 1, 0 );
      *pbtA++ = pbtSrc[3];	
      pbtSrc += (BGRA_SIZE);
    }

    pbtSrc += iDiffW + (1-1)*nStrideW;

    pbtB += nDiffY;
    pbtG += nDiffY;
    pbtR += nDiffY;
    pbtA += nDiffY;

  }
}


//------------------------------------------------------------------------------

void wci_bgr_in_from_bgr(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nW       = rSrcSize.cx;
  const int nStrideW = BGR_SIZE * nW;
  const int iDiffW   = nStrideW - BGR_SIZE*nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY   = nStrideY - rSrcSize.cx;

  BYTE * pbtB = prImage->arComponent[0].pbtData;
  BYTE * pbtG = prImage->arComponent[1].pbtData; 
  BYTE * pbtR = prImage->arComponent[2].pbtData;
  const int iCorrection = (4 - nStrideW%4)&3;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  for (j = 0; j < rSrcSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
      GET_BGR( pbtSrc, 0, 0, nStrideW, *pbtR++, *pbtG++, *pbtB++, 2, 1, 0 );
      pbtSrc += (BGR_SIZE);
    }

    pbtSrc += iDiffW + (1-1)*nStrideW + iCorrection;

    pbtB += nDiffY;
    pbtG += nDiffY;
    pbtR += nDiffY;
  }
}

//------------------------------------------------------------------------------

void wci_bgr_in_from_rgb565(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nW       = rSrcSize.cx;
  const int nStrideW = RGB565_SIZE * nW;
  const int iDiffW   = nStrideW - RGB565_SIZE*nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY   = nStrideY - rSrcSize.cx;

  BYTE * pbtB = prImage->arComponent[0].pbtData;
  BYTE * pbtG = prImage->arComponent[1].pbtData; 
  BYTE * pbtR = prImage->arComponent[2].pbtData;
  const int iCorrection = (4 - nStrideW%4)&3;

  int i, j;

  assert( prImage->arComponent[0].rSize.cx == prImage->arComponent[1].rSize.cx );
  assert( prImage->arComponent[1].rSize.cx == prImage->arComponent[2].rSize.cx );

  for (j = 0; j < rSrcSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
      *pbtB++ = (pbtSrc[0]&0x1F)<<3;			                                
      *pbtG++ = ((pbtSrc[0]>>3)&0x1C)|((pbtSrc[1]<<5)&0xE0);						                        
      *pbtR++ = (pbtSrc[1])&0xF8;
       pbtSrc += (RGB565_SIZE);
    }

    pbtSrc += iDiffW + (1-1)*nStrideW + iCorrection;

    pbtB += nDiffY;
    pbtG += nDiffY;
    pbtR += nDiffY;
  }
}

#endif  //ENCODER_SUPPORT

//------------------------------------------------------------------------------

void wci_grayscale_in_from_bgra(
  const BYTE * IN     pbtSrc
, SIZE         IN     rSrcSize
, IMAGE_EX *   IN OUT prImage
)
{
  const int nW       = rSrcSize.cx;
  const int nStrideW = BGRA_SIZE * nW;
  const int iDiffW   = nStrideW - BGRA_SIZE*nW;

  const int nStrideY = prImage->arComponent[0].rSize.cx;
  const int nDiffY   = nStrideY - rSrcSize.cx;

  BYTE * pbtY = prImage->arComponent[0].pbtData;
  int i, j;

  for (j = 0; j < rSrcSize.cy; j+=(1))
  {
    for (i = 0; i < nW; i+=(1))
    {
      int r, g, b;

      GET_BGR( pbtSrc, 0, 0, nStrideW, r, g, b, 2, 1, 0 );
      pbtSrc += (BGRA_SIZE);

      *pbtY++ = RGB_TO_Y( r, g, b );
    }

    pbtSrc += iDiffW + (1-1)*nStrideW;
    pbtY += nDiffY;
  }
}



/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_wavelet.h

ABSTRACT:    This file contains definitions and data structures for wavelet transform

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef WAVELET_H
#define WAVELET_H

//------------------------------------------------------------------------------

#define L_CORRECTION

#define WP_MEM_RESERVE    4/3 

#define GROUP_SIZE_X      8
#define GROUP_SIZE_Y      2

#define GROUP_HALF_SIZE_X 4 //  (GROUP_SIZE_X/2)
#define GROUP_HALF_SIZE_Y 1 //  (GROUP_SIZE_Y/2)

//------------------------------------------------------------------------------

typedef void LIFT_WAV_BLOCK(
  short *Orig,
  int posx, int posy,
  int sizex, int sizey, int ImSizex,
  short *Tmp_Buffer);

typedef LIFT_WAV_BLOCK * LIFT_WAV_BLOCK_FUNC_PTR;

typedef void INV_LIFT_WAV_BLOCK(
  short *Split,
  int posx, int posy,
  int sizex, int sizey, int ImSizeX,
  short *Tmp_Buffer);

typedef INV_LIFT_WAV_BLOCK * INV_LIFT_WAV_BLOCK_FUNC_PTR;

typedef void INV_WAVLET_INSERT_LL_FUNC(
  short *pshImage, int iPosX, int iPosY,
  int iSizeX, int iSizeY, int iImageSizeX, short *pshLL);

typedef INV_WAVLET_INSERT_LL_FUNC * INV_WAVLET_INSERT_LL_FUNC_PTR;

extern LIFT_WAV_BLOCK_FUNC_PTR     wci_lift_wav_block   ;
extern INV_LIFT_WAV_BLOCK_FUNC_PTR wci_inv_lift_wav_block;

LIFT_WAV_BLOCK     wci_lift_wav_block_c;
INV_LIFT_WAV_BLOCK wci_inv_lift_wav_block_c;

LIFT_WAV_BLOCK     LiftWavBlock_mmx;
INV_LIFT_WAV_BLOCK InvLiftWavBlock_mmx;

INV_WAVLET_INSERT_LL_FUNC wci_inv_wavlet_insert_ll_c;

//------------------------------------------------------------------------------

void wci_inv_insert_8x8_ll(short *ptr_small, int ImSizeX, short *ptr_big);
void wci_inv_insert_4x4_ll(short *ptr_small, int ImSizeX, short *ptr_big);
void wci_inv_insert_2x2_ll(short *ptr_small, int ImSizeX, short *ptr_big);

#endif //ifndef WAVELET_H

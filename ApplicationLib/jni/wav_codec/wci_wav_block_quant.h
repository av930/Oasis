/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_wav_block_quant.h

ABSTRACT:    This file contains definitions for quantization of wavelet coefficients

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef WAV_BLOCK_QUANT_H
#define WAV_BLOCK_QUANT_H

#include "../wci_portab.h"
#include "../wci_global.h"
#include "../compress/wci_qsp_compress.h"
#include "wci_wav_quant.h"

//------------------------------------------------------------------------------

void wci_wav_blockquant(
  short *psImage                                 // [in/out]
, int iPels, VECTOR rBlockPos, SIZE rBlockSize   // [in]
, int iQuantizer, BOOL bReconstruct              // [in]
, BYTE * pbtPartition, QSP_BUFFER *prQSP );      // [in/out]

//------------------------------------------------------------------------------

void wci_wav_hl_lh_hh_quant(						
  short *ptr_big                                          // [in/out]
, SIZE rBlockSize								                          // [in]
, int iQuantizer_H, int iQuantizer_HH, BOOL bReconstruct  // [in]	   
, BYTE * pbtPartition, QSP_BUFFER *prQSP );               // [in/out]

//------------------------------------------------------------------------------

#define TAB_ABS

#define SQR_APPROXIMATION
#ifdef SQR_APPROXIMATION
  #define SQR_APPR_FOR_I		1 // no brackets !!!
  #define SQR_APPR_FOR_P		1	// no brackets !!!
#endif

#endif // #ifndef WAV_BLOCK_QUANT_H

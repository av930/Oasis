/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_lpc.h

ABSTRACT:    This file contains definitions and data structures for LPC codec

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef LPC_H
#define LPC_H

#include "../wci_global.h"

//------------------------------------------------------------------------------

#define LPC_MAX_FILTR  8192
#define LPC_HALF_FILTR (LPC_MAX_FILTR/2)

#define LPC_NUM_BT     32
#define LPC_NUM_AL     32

#define LPC_NUM_BIT_AL 5
#define LPC_NUM_BIT_BT 5

//------------------------------------------------------------------------------

typedef struct tagLPC_COEFF
{
  short shAL;
  short shBT;

} LPC_COEFF;

typedef struct tagLPC_QUANTIZER
{
  short shNum_AL;
  short shQuant_AL;
  short shDeadZ_AL;

  short shNum_BT;
  short shQuant_BT;
  short shDeadZ_BT;

} LPC_QUANTIZER;

//------------------------------------------------------------------------------

void wci_lpc_init_quantizer(
  int             IN  iNum_al
, int             IN  iNum_bt
, LPC_QUANTIZER * OUT prQuant
);

void wci_lpc_calc_coeff(
  const short * IN  pshImage
, int           IN  iStride
, SIZE          IN  rBlockSize
, int           IN  nBlocks
, BOOL          IN  bLastBand
, LPC_COEFF *   OUT prLPC
);

int wci_lpc_put_quant_value(
  LPC_COEFF *           IN OUT prLPC
, int                   IN     nBlocks
, const LPC_QUANTIZER * IN     prQuant
, BYTE *                IN     pbtStream
);

int wci_lpc_get_quant_value(
  const BYTE *          IN     pbtStream
, int                   IN     nBlocks
, const LPC_QUANTIZER * IN     prQuant
, LPC_COEFF *              OUT prLPC
);

#endif // #ifndef LPC_H

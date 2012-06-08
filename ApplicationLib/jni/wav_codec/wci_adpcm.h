/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_adpcm.h

ABSTRACT:    This file contains definitions and data structures for ADPCM codec

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef ADPCM_H
#define ADPCM_H

#include "../wci_portab.h"
#include "../wci_codec.h"
#include "../compress/wci_qsp.h"
#include "wci_wav_codec.h"
#include "wci_lpc.h"

//------------------------------------------------------------------------------

#define ADPCM_ENCODE_TAB
//#define ADPCM_DECODE_TAB

typedef enum
{
  eDPCM     = 1,
  eLPC_DPCM = 2

} enumDPCM_Mode;

//------------------------------------------------------------------------------

typedef struct tagADPCM_STATE
{
  enumDPCM_Mode   eMode;

  SIZE            rBlockSize;

  SIZE            rBlocks;

  int             nShift;

  int             iQuantizer;

  int             nCoeffLines;

  int             nCurrLine;

  int             nCurrBlockLine;

  short *         pshPrevLine;

  short *         pshMean;

  LPC_COEFF *     prLPC;

  LPC_QUANTIZER   rQuant;

} ADPCM_STATE;

//------------------------------------------------------------------------------

#ifdef ADPCM_ENCODE_TAB

  void wci_adpcm_encode
  (
    enumDPCM_Mode IN     eMode
  , short *       IN OUT pshLL
  , SIZE          IN     rBlockSize
  , QUANT_VALUE * IN     prQV
  , QSP_BUFFER *     OUT prQSP
  );
  
#else

  void wci_adpcm_encode
  (
    enumDPCM_Mode IN     eMode
  , short *       IN OUT pshLL
  , SIZE          IN     rBlockSize
  , int           IN     iDeadZone
  , int           IN     iQuantizer
  , QSP_BUFFER *     OUT prQSP
  );

#endif

//------------------------------------------------------------------------------

#ifdef ADPCM_DECODE_TAB

  void wci_adpcm_decode
  (
    enumDPCM_Mode IN     eMode
  , QSP_BUFFER *  IN     prQSP
  , SIZE          IN     rSize
  , const short   IN     shDeQuant[256]
  , short *          OUT pshLL
  );
  
#else

  void wci_adpcm_decode
  (
    enumDPCM_Mode IN     eMode
  , QSP_BUFFER *  IN     prQSP
  , SIZE          IN     rSize
  , int           IN     iQuantizer
  , short *          OUT pshLL
  );

#endif

//------------------------------------------------------------------------------

ERROR_CODE ADPCM_GetLastPos
(
  enumDPCM_Mode IN     eMode
, SIZE          IN     rSize
, QSP_BUFFER *  IN OUT prQSP
);

//------------------------------------------------------------------------------

SIZE wci_adpcm_get_partition_block(
  SIZE         IN     rSize
);

//------------------------------------------------------------------------------

int wci_adpcm_get_length_q(
  SIZE                IN rSize
, const QSP_BUFFER *  IN prQSP
, const ADPCM_STATE * IN prState
);

//------------------------------------------------------------------------------

int wci_adpcm_get_length_s(
  SIZE               IN rSize
, const QSP_BUFFER * IN prQSP
);

//------------------------------------------------------------------------------

int wci_adpcm_get_buffer_size(
  int IN iImageWidth
);

//------------------------------------------------------------------------------

ERROR_CODE wci_adpcm_init_stripe_state
(
  enumDPCM_Mode        IN     eMode
, SIZE                 IN     rBlockSize
, int                  IN     nShift
, int                  IN     iQuantizer
, ADPCM_STATE *        IN OUT prState
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
);

//------------------------------------------------------------------------------

void wci_adpcm_done_stripe_state
(
  ADPCM_STATE *        IN OUT prState
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
);

//------------------------------------------------------------------------------

void wci_adpcm_decodestripe
(
  QSP_BUFFER * IN     prQSP
, int          IN     iDecodeLines
, ADPCM_STATE* IN OUT prState
, int          IN     iStride
, short *         OUT pshLL
);

//------------------------------------------------------------------------------

void wci_adpcm_decode_stripe
(
  QSP_BUFFER * IN     prQSP
, int          IN     iDecodeLines
, ADPCM_STATE* IN OUT prState
, int          IN     iStride
, short *         OUT pshLL
);

#endif // #ifndef ADPCM_H

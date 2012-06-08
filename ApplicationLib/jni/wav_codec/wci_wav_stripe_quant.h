/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_wav_stripe_quant.h

ABSTRACT:    This file contains definitions for dequant stripe procedures

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef WAV_STRIPE_QUANT_H
#define WAV_STRIPE_QUANT_H

#include "../wci_portab.h"
#include "../wav_codec/wci_wav_codec.h"
#include "wci_wav_stripe.h"

//------------------------------------------------------------------------------

void wci_qsp_get_max_block_length(
  SIZE         IN     rSize
, enumWT_Mode  IN     eMode
, QSP_BUFFER * IN OUT prQSP
);

//------------------------------------------------------------------------------

ERROR_CODE wci_qsp_get_last_pos(
  int          IN     iPartitionLength
, SIZE         IN     rSize
, QSP_BUFFER * IN OUT prQSP
);

//------------------------------------------------------------------------------

int wci_qsp_get_length_p(
  SIZE IN rSize
);

//------------------------------------------------------------------------------

int wci_qsp_get_length_q(
  int                IN iP_Length
, SIZE               IN rSize
, const QSP_BUFFER * IN prQSP
);

//------------------------------------------------------------------------------

int wci_qsp_get_length_s(
  int                IN iQ_Length
, const QSP_BUFFER * IN prQSP
);

//------------------------------------------------------------------------------

void wci_dequant_step_stripe(
  QSP_BUFFER *            IN OUT prQSP
, int                     IN     iDequantLines
, const WAV_QUANT_SCALE * IN     prQS
, int                     IN     Step
, WAV_STRIPE_NO *         IN OUT Wav_Stripe
);

//------------------------------------------------------------------------------

void wci_stripe_dequant(
  QSP_BUFFER *            IN OUT qsp
, const WAV_QUANT_SCALE * IN     scale
, int                     IN     lines_to_dequant
, int                     IN     step
, WAV_STRIPE *            IN OUT stripe
);

//------------------------------------------------------------------------------

void wci_stripe_insertll_block(
  const short *   IN     pshLL
, int             IN     iLines
, WAV_STRIPE_NO * IN OUT prStripe
);

//------------------------------------------------------------------------------

short * wci_stripe_getll_block(
  const WAV_STRIPE_NO * IN prStripe
);

//------------------------------------------------------------------------------

static int __inline wci_stripe_getll_stride(
  const WAV_STRIPE_NO * IN prStripe
)
{
  return prStripe->Wav_State_LL.Size.cx;
}

#endif // #ifndef WAV_STRIPE_QUANT_H

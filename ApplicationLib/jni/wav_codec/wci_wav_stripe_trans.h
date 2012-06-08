/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: WavStripeTrans.h

ABSTRACT:    This file contains definitions for inverse Wavelet transformation procedure

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.27   v1.00

*******************************************************************************/

#ifndef WAV_STRIPE_TRANS_H
#define WAV_STRIPE_TRANS_H

#include "wci_wav_stripe.h"

//------------------------------------------------------------------------------


void wci_stripe_insert_ll_lines(
  int          IN     step
, WAV_STRIPE * IN OUT stripe
);

void wci_stripe_inv_transformation(
  int          IN     step
, WAV_STRIPE * IN OUT stripe
);


#endif // #ifndef WAV_STRIPE_TRANS_H

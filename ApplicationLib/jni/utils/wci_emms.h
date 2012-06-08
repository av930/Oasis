/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: emms.ð

ABSTRACT:    This file contains definitions of emms C wrapper

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef _EMMS_H_
#define _EMMS_H_

//------------------------------------------------------------------------------

typedef void (emmsFunc) (void);
typedef emmsFunc *emmsFuncPtr;
extern emmsFuncPtr emms;

//------------------------------------------------------------------------------

emmsFunc wci_emms_c;
emmsFunc wci_emms_mmx;

#if defined(ARCH_IS_IA32)
  extern int check_cpu_features(void);
  extern void sse_os_trigger(void);
  extern void sse2_os_trigger(void);
  extern int check_cpu_features(void);
#endif

#endif //ifndef _EMMS_H_

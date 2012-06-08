/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: emms.c

ABSTRACT:    This file contains procedures of emms C wrapper

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include "wci_emms.h"
#include "../wci_portab.h"

//------------------------------------------------------------------------------

emmsFuncPtr emms;

//------------------------------------------------------------------------------

void
wci_emms_c()
{
  // There is no op wrapper for non MMX platforms
}

//------------------------------------------------------------------------------

// The real mmx emms wrapper
void
//emms_mmx()
wci_emms_mmx()
{
	// the EMMS macro is defined according to the compiler in wci_portab.h
	EMMS();

}

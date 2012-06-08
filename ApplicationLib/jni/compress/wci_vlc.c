/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: VLC.c

ABSTRACT:    This file contains procedures for initialization of VLC table

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include <string.h>

#include "wci_vlc.h"

//------------------------------------------------------------------------------

#ifdef VLC_FORWARD
  VLC        g_arrForwardVLC[VLC_SIZE];
  const VLC *g_prForwardVLC = g_arrForwardVLC;
#endif

#ifdef VLC_INVERSE
  VLC        g_arrInverseVLC[1 << VLC_MAX_CODE_LENGTH];
  const VLC *g_prInverseVLC = g_arrInverseVLC;
#endif

const uint8_t g_vlc_code_length_table[VLC_PREFIX_LENGTH_TABLE_SIZE] = {
17,15,13,13,11,11,11,11,9,9,9,9,9,9,9,9,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
  };  // Length arr

  //------------------------------------------------------------------------------


void wci_vlc_init_tables()
{
// Fills array g_arrForwardVLC

#ifdef VLC_FORWARD
  {
    int i, j, iLog2[VLC_SIZE+1];

    //! Form table of binary log
    iLog2[0] = 0;
    for ( i = 1; i < VLC_SIZE+1; i++ )
    {
      int n = 0;

      for( j = i >> 1; j > 0; j >>= 1 )
      {
        n++;
      }
      iLog2[i] = n;
    }

    // Form VLC table of prefix code for forward transformation
    for ( i = 0; i < VLC_SIZE; i++ )
    {
      int m = iLog2[ i + 1 ];
      g_arrForwardVLC[i].iLength = 2*m + 1;
      g_arrForwardVLC[i].iCode = i + 1;
    }
  }
#endif

#ifdef VLC_INVERSE
  // Form VLC table for inverse transformation
  {
    int i, j;

    for ( i = 0; i < VLC_SIZE; i++ )
    {
      int k = VLC_MAX_CODE_LENGTH - g_arrForwardVLC[i].iLength;

      int iStart = g_arrForwardVLC[i].iCode << k;
      int iEnd   = (g_arrForwardVLC[i].iCode + 1) << k;

      for( j = iStart; j < iEnd; j++ )
      {
        g_arrInverseVLC[j].iLength = g_arrForwardVLC[i].iLength;
        g_arrInverseVLC[j].iCode   = g_arrForwardVLC[i].iCode - 1;
      }
    }
  }
#endif
}

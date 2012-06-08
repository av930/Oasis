/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: LPC.c

ABSTRACT:    This file contains procedures for LPC codec

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include <string.h>
#include <math.h>

#include "wci_lpc.h"

//------------------------------------------------------------------------------

#define ROUND(a)	( ( (a) < 0 ) ? (int) ( (a) - 0.5 ) : (int) ( (a) + 0.5 ) )

//------------------------------------------------------------------------------

void wci_lpc_init_quantizer(
  int             IN  iNum_AL
, int             IN  iNum_BT
, LPC_QUANTIZER * OUT prQuant
)
{
  memset( prQuant, 0, sizeof(LPC_QUANTIZER) );

  prQuant->shNum_AL =(short)iNum_AL;
  prQuant->shNum_BT =(short)iNum_BT;

  prQuant->shQuant_AL = (short)( 2*LPC_MAX_FILTR/(iNum_AL-1) );
  prQuant->shQuant_BT = (short)( 2*LPC_MAX_FILTR/(iNum_BT-1) );
}

//------------------------------------------------------------------------------

static void wci_calc_corr_block(
  const short * IN  pshImage
, int           IN  iStride
, BOOL          IN  bLastBand
, VECTOR        IN  vPos
, SIZE          IN  rBlockSize
, double *      OUT pdal
, double *      OUT pdbt
)
{
  int    x, y;
  double a11,a22,a12,X1,X2,DetSyst;
  int leny,startx;

	a11=0.0;
  a22=0.0;
  a12=0.0;

  X1=0.0;
  X2=0.0;

	pshImage += vPos.x + iStride*vPos.y;

	leny = rBlockSize.cy;

  startx=0;

	if(bLastBand)
		 leny = rBlockSize.cy-1;

	if(vPos.x==0)
		startx=1;

	for(y=0;y<leny;y++)
	{
		for(x=startx;x<rBlockSize.cx;x++)
		{
      double tmp1,tmp2,tmp3;

			tmp1 = pshImage[x-1] - pshImage[x-1 + iStride];
			tmp2 = pshImage[x-1] - pshImage[x            ];
			tmp3 = pshImage[x-1] - pshImage[x   + iStride];

			a11 += tmp1*tmp1;
			a22 += tmp2*tmp2;
			a12 += tmp1*tmp2;

			X1 += tmp3*tmp1;
			X2 += tmp3*tmp2;
		}
		pshImage += iStride;
	}

	DetSyst=a11*a22-a12*a12;

	if(fabs(DetSyst>1.0E-8))
	{
		*pdal=(X1*a22-X2*a12)/DetSyst;
		*pdbt=(X2*a11-a12*X1)/DetSyst;
	}
	else
	{
		*pdal=1.0;
		*pdbt=0.0;
	}
}	

//------------------------------------------------------------------------------

void wci_lpc_calc_coeff(
  const short * IN  pshImage
, int           IN  iStride
, SIZE          IN  rBlockSize
, int           IN  nBlocks
, BOOL          IN  bLastBand
, LPC_COEFF *   OUT prLPC
)
{
  int    i;
  VECTOR vPos;

  vPos.x=0;
  vPos.y=0;

  for(i=0;i<nBlocks;i++)
  {
    double al, bt;

    wci_calc_corr_block( pshImage, iStride, bLastBand, vPos, rBlockSize, &al, &bt );

    prLPC[i].shAL = (short)BOUND((ROUND(al*LPC_MAX_FILTR)),-LPC_MAX_FILTR,LPC_MAX_FILTR);
    prLPC[i].shBT = (short)BOUND(ROUND((bt*LPC_MAX_FILTR)),-LPC_MAX_FILTR,LPC_MAX_FILTR);

    vPos.x += rBlockSize.cx;
  }
}

//------------------------------------------------------------------------------

int wci_lpc_put_quant_value(
  LPC_COEFF *           IN OUT prLPC
, int                   IN     nBlocks
, const LPC_QUANTIZER * IN     prQuant
, BYTE *                IN     pbtStream
)
{
  int i;

  for(i=0;i<nBlocks;i++)
  {
    BYTE btQ;

    btQ = (BYTE)BOUND( (prLPC[i].shAL+LPC_MAX_FILTR)/prQuant->shQuant_AL,0,(LPC_NUM_AL-1));
    *pbtStream++ = btQ;
    prLPC[i].shAL=BOUND(prQuant->shQuant_AL*btQ-LPC_MAX_FILTR,-LPC_MAX_FILTR,LPC_MAX_FILTR);

    btQ = (BYTE)BOUND((prLPC[i].shBT+LPC_MAX_FILTR)/prQuant->shQuant_BT,0,(LPC_NUM_BT-1));
    *pbtStream++ = btQ;
    prLPC[i].shBT=BOUND(prQuant->shQuant_BT*btQ-LPC_MAX_FILTR,-LPC_MAX_FILTR,LPC_MAX_FILTR);

    //DPRINTF( DPRINTF_DEBUG, "AL= %3d, BT=%3d\n", prLPC[i].shAL, prLPC[i].shBT );
  }

  return 2*nBlocks;
}

//------------------------------------------------------------------------------

int wci_lpc_get_quant_value(
  const BYTE *          IN     pbtStream
, int                   IN     nBlocks
, const LPC_QUANTIZER * IN     prQuant
, LPC_COEFF *              OUT prLPC
)
{
  int i;

  for(i=0;i<nBlocks;i++)
  {
    BYTE btQ;

    btQ = *pbtStream++;
    prLPC[i].shAL=BOUND(prQuant->shQuant_AL*btQ-LPC_MAX_FILTR,-LPC_MAX_FILTR,LPC_MAX_FILTR);

    btQ = *pbtStream++;
    prLPC[i].shBT=BOUND(prQuant->shQuant_BT*btQ-LPC_MAX_FILTR,-LPC_MAX_FILTR,LPC_MAX_FILTR);

    //DPRINTF( DPRINTF_DEBUG, "AL= %3d, BT=%3d\n", prLPC[i].shAL, prLPC[i].shBT );
  }

  return 2*nBlocks;
}


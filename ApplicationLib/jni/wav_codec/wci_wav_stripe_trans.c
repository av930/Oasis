/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: WavStripeTrans.c

ABSTRACT:    This file contains procedures for inverse Wavelet transformation

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.27   v1.00

*******************************************************************************/

#include "wci_wav_stripe_trans.h"
#include "../wci_profile.h"
//#define WAV_VAR_2
#define L_Correction


/***********************************************************************************/


#ifdef L_Correction
/*#define  TransH4(pshBl) pshBl[1]-=((*pshBl+pshBl[2]+2)>>2);\
  pshBl[0]+=((pshBl[1]+pshBl[-1]+1)>>1);\
  pshBl[3]-=((pshBl[2]+pshBl[4]+2)>>2);\
  pshBl[2]+=((pshBl[1]+pshBl[3]+1)>>1)
*/
#define  TransH4(pshBl)\
{\
  register int32_t t1 = *(int32_t*)&pshBl[1];\
  register int32_t t3 = *(int32_t*)&pshBl[-1];\
  register short   t4;\
  t4 = t1 -  (((t3>>16)+(t1>>16)+2)>>2);\
  pshBl[1] = t4;\
  pshBl[0] = (t3>>16) + (((t4+t3+1)<<16)>>17);\
{ register int32_t t2 = *(int32_t*)&pshBl[3];\
  t2 = (t2&0xFFFF) - (((t1>>16)+(t2>>16)+2)>>2);\
  pshBl[3] = t2;\
  pshBl[2] = ((t1>>16) + (((t4+t2+1)<<16)>>17));\
}}

#else
#define  TransH4(pshBl)   pshBl[0]+=((pshBl[1]+pshBl[-1]+1)>>1);\
                          pshBl[2]+=((pshBl[1]+pshBl[3]+1)>>1)
#endif
#define  LLCorrInd(pshStr2,pshStr1,pshStr3)  (pshStr2[0])-=((pshStr1[0] +pshStr3[0] +2)>>2);\
  (pshStr2[1])-=((pshStr1[1] +pshStr3[1] +2)>>2);\
  (pshStr2[2])-=((pshStr1[2] +pshStr3[2] +2)>>2);\
  (pshStr2[3])-=((pshStr1[3] +pshStr3[3] +2)>>2);

#define HCalcInd(pshStr1,pshStr0,pshStr2)     (pshStr1[0])+=((pshStr0[0] + pshStr2[0]+1 )>>1 );\
       (pshStr1[1])+=((pshStr0[1] + pshStr2[1]+1)>>1 );\
      (pshStr1[2])+=((pshStr0[2] + pshStr2[2]+1 )>>1 );\
       (pshStr1[3])+=((pshStr0[3] + pshStr2[3]+1 )>>1 );

#define TransH1Def(pshStr0) (*pshStr0)-=((pshStr0[1]+1)>>1)

//#define _PSNRWITHOUTLOSS_
#ifdef _PSNRWITHOUTLOSS_

#define  LLCorr(pshStr2,pshStr1,pshStr3)\
{\
  *((uint32_t*)pshStr2) = ((pshStr2[0])-((*pshStr1 + *pshStr3 + 2 )>>2)&0xFFFF)|(((pshStr2[1])-((pshStr1[1] + pshStr3[1] +2 )>>2))<<16);\
  ((uint32_t*)pshStr2)[1] = ((pshStr2[2])-((pshStr1[2] + pshStr3[2] +2)>>2)&0xFFFF)|(((pshStr2[3])-((pshStr1[3] + pshStr3[3] +2)>>2))<<16);\
}

#define HCalc(s1,s0,s2)\
      { \
      *((uint32_t*)s1) = (((uint32_t)(s1[0])+((*s0 + *s2 +1)>>1 ))&0xFFFF)|\
      ((uint32_t)(((s1[1])+((s0[1] + s2[1] +1)>>1 ))<<16));\
      ((uint32_t*)s1)[1] = (((uint32_t)(s1[2])+((s0[2] + s2[2] +1)>>1 ))&0xFFFF)|\
      ((uint32_t)(((s1[3])+((s0[3] + s2[3] +1)>>1 ))<<16));\
      }

#else

#define  LLCorr1(pshStr2,pshStr1,pshStr3)  (*pshStr2++)-=((*pshStr1++ +*pshStr3++ +2)>>2);\
(*pshStr2++)-=((*pshStr1++ +*pshStr3++ +2)>>2);\
(*pshStr2++)-=((*pshStr1++ +*pshStr3++ +2)>>2);\
(*pshStr2)-=((*pshStr1 +*pshStr3 +2)>>2);\
pshStr2-=3;  pshStr3-=3;pshStr1-=3

#define HCalc1(pshStr1,pshStr0,pshStr2)     (*pshStr1++)+=((*pshStr0++ + *pshStr2++ +1)>>1 );\
(*pshStr1++)+=((*pshStr0++ + *pshStr2++ +1)>>1 );\
( *pshStr1++)+=((*pshStr0++ + *pshStr2++ +1)>>1 );\
(*pshStr1)+=((*pshStr0+     *pshStr2+1)>>1 );\
pshStr1-=3;  pshStr2-=3;pshStr0-=3;

/************************************************************************/
/*     Following LLCorr & HCalc impart a little distortion (fastest)    */
/************************************************************************/
#define  LLCorr(pshStr2,pshStr1,pshStr3)\
{ register int32_t t;\
  t=(*((int32_t*)pshStr1)+*((int32_t*)pshStr3));\
  t = ((t>>2)&0xFFFF0000)|(((t<<16)>>18)&0xFFFF);\
  *((int32_t*)pshStr2) -= t;\
  t=(((int32_t*)pshStr1)[1]+((int32_t*)pshStr3)[1]);\
  t = ((t>>2)&0xFFFF0000)|(((t<<16)>>18)&0xFFFF);\
  ((int32_t*)pshStr2)[1] -= t;\
}
/*
#define _HCALC_CORR_  0x00010001
#define HCalc(pshStr2,pshStr1,pshStr3)\
{\
  register int32_t t;\
  t=(*((int32_t*)pshStr1)+*((int32_t*)pshStr3)+_HCALC_CORR_);\
  t = ((t>>1)&0xFFFF0000)|(((t<<16)>>17)&0xFFFF);\
  *((int32_t*)pshStr2) += t;\
  t=(((int32_t*)pshStr1)[1]+((int32_t*)pshStr3)[1]+_HCALC_CORR_);\
  t = ((t>>1)&0xFFFF0000)|(((t<<16)>>17)&0xFFFF);\
  ((int32_t*)pshStr2)[1] += t;\
}
*/
#define HCalc(pshStr2,pshStr1,pshStr3)\
{\
  register int32_t t;\
  t=(*((int32_t*)pshStr1)+*((int32_t*)pshStr3));\
  t = ((t>>1)&0xFFFF0000)|(((t<<16)>>17)&0xFFFF);\
  *((int32_t*)pshStr2) += t;\
  t=(((int32_t*)pshStr1)[1]+((int32_t*)pshStr3)[1]);\
  t = ((t>>1)&0xFFFF0000)|(((t<<16)>>17)&0xFFFF);\
  ((int32_t*)pshStr2)[1] += t;\
}


#endif


#ifdef L_Correction
#define  TransH3(ptrBl)     ptrBl[1]-=((ptrBl[0]+ptrBl[2]+2)>>2);\
                            ptrBl[0]+=((ptrBl[-1]+ptrBl[1]+1)>>1);\
                            ptrBl[2]+=(ptrBl[1])
#else
#define  TransH3(ptrBl)     ptrBl[0]+=((ptrBl[-1]+ptrBl[1]+1)>>1);\
                            ptrBl[2]+=(ptrBl[1])
#endif

void wci_first_trans_v4(short *pshBl1,short *pshStr1,short *pshStr2,short *pshStr3);
void wci_first_trans_v2(short *pshBl1,short *pshStr1,short *pshStr2,short *pshStr3);
void __inline wci_middle_v2(short *pshStr0,short *pshStr1,short *pshStr2,short *pshStr3,short *pshStr4,short *pshStr5);
void __inline wci_last_v2(short *pshStr0,short *pshStr1,short *pshStr2,short *pshStr3,short *pshStr4,short *pshStr5);
void  MiddleInvWav2(short *buffer, int SizeX);

void  wci_first_trans_h1(short *pshStr0,short *pshStr1)
{
  (*pshStr0)-=((pshStr0[1]+1)>>1);
  (*pshStr1)-=((pshStr1[1]+1)>>1);

  return;
}

void __inline wci_trans_h1(short *pshStr0,short *pshStr1,short *pshStr2,short *pshStr3)
{
  (*pshStr0)-=((pshStr0[1]+1)>>1);
  (*pshStr1)-=((pshStr1[1]+1)>>1);
  (*pshStr2)-=((pshStr2[1]+1)>>1);
  (*pshStr3)-=((pshStr3[1]+1)>>1);

  return;
}

void __inline wci_last_trans_h1(short *pshStr0,short *pshStr1,short *pshStr2,short *pshStr3,short *pshStr4,short *pshStr5)
{
  (*pshStr0)-=((pshStr0[1]+1)>>1);
  (*pshStr1)-=((pshStr1[1]+1)>>1);
  (*pshStr2)-=((pshStr2[1]+1)>>1);
  (*pshStr3)-=((pshStr3[1]+1)>>1);
  (*pshStr4)-=((pshStr4[1]+1)>>1);
  (*pshStr5)-=((pshStr5[1]+1)>>1);

  return;
}

void __inline wci_trans_h5(short *ptrBl)
{
#ifdef L_Correction
  ptrBl[1]-=((ptrBl[0]+ptrBl[2]+2)>>2);
#endif
  ptrBl[0]+=((ptrBl[-1]+ptrBl[1]+1)>>1);
#ifdef L_Correction
  ptrBl[3]-=((ptrBl[2]+ptrBl[4]+2)>>2);
#endif
  ptrBl[2]+=((ptrBl[1]+ptrBl[3]+1)>>1);
  ptrBl[4]+=(ptrBl[3]);

  return;
}

void wci_first_trans_v4(short *pshBl1,short *pshStr1,short *pshStr2,short *pshStr3)
{
#ifdef L_Correction
  (*pshBl1++)-=((*pshStr1+1)>>1);pshStr1++;
  (*pshBl1++)-=((*pshStr1+1)>>1);pshStr1++;
  (*pshBl1++)-=((*pshStr1+1)>>1);pshStr1++;
  (*pshBl1)-=((*pshStr1+1)>>1);
  pshBl1-=3;  pshStr1-=3;


  (*pshStr2++)-=((*pshStr1+*pshStr3+2)>>2);pshStr1++;pshStr3++;
  (*pshStr2++)-=((*pshStr1+*pshStr3+2)>>2);pshStr1++;pshStr3++;
  (*pshStr2++)-=((*pshStr1+*pshStr3+2)>>2);pshStr1++;pshStr3++;
  (*pshStr2)-=((*pshStr1+*pshStr3+2)>>2);
  pshStr1-=3;pshStr2-=3;
#endif

  (*pshStr1++)+=((*pshBl1+ *pshStr2+1)>>1 );pshBl1++;pshStr2++;
  (*pshStr1++)+=((*pshBl1+ *pshStr2+1)>>1 );pshBl1++;pshStr2++;
  (*pshStr1++)+=((*pshBl1+ *pshStr2+1)>>1 );pshBl1++;pshStr2++;
  (*pshStr1)+=((*pshBl1+ *pshStr2+1)>>1 );
}

void wci_first_trans_v2(short *pshBl1,short *pshStr1,short *pshStr2,short *pshStr3)
{
#ifdef L_Correction
  (*pshBl1++)-=((*pshStr1+1)>>1);pshStr1++;
  (*pshBl1)-=((*pshStr1+1)>>1);
  pshBl1-=1;  pshStr1-=1;

  (*pshStr2++)-=((*pshStr1+*pshStr3+2)>>2);pshStr1++;pshStr3++;
  (*pshStr2)-=((*pshStr1+*pshStr3+2)>>2);
  pshStr1-=1;pshStr2-=1;
#endif
  (*pshStr1++)+=((*pshBl1+ *pshStr2+1)>>1 );pshBl1++;pshStr2++;
  (*pshStr1)+=((*pshBl1+ *pshStr2+1)>>1 );
}

// L Correction of 2 and 4 string
// H calculation of 1 and 3 string

void __inline wci_middle_v4(short *pshStr0,short *pshStr1,short *pshStr2,short *pshStr3,short *pshStr4,short *pshStr5)
{
#ifdef L_Correction
  LLCorr(pshStr2,pshStr1,pshStr3);
#endif
  HCalc(pshStr1,pshStr0,pshStr2);
#ifdef L_Correction
  LLCorr(pshStr4,pshStr3,pshStr5);
#endif
  HCalc(pshStr3,pshStr2,pshStr4);
  return;
}

void __inline wci_middle_v2(short *pshStr0,short *pshStr1,short *pshStr2,short *pshStr3,short *pshStr4,short *pshStr5)
{
  // LL correction of 2 string
#ifdef L_Correction
  (*pshStr2++)-=((*pshStr1++ +*pshStr3++ +2)>>2);
  (*pshStr2)-=((*pshStr1 +*pshStr3 +2)>>2);
  pshStr2--;  pshStr3--;pshStr1--;
#endif
  // H Calculation 1 string
  (*pshStr1++)+=((*pshStr0++ + *pshStr2++ +1)>>1 );
  (*pshStr1)+=((*pshStr0 + *pshStr2 +1)>>1 );
  pshStr1--;  pshStr2--;pshStr0--;

  // LL correction of 4 string
#ifdef L_Correction
  (*pshStr4++)-=((*pshStr3++ +*pshStr5++ +2)>>2);
  (*pshStr4)-=((*pshStr3 +*pshStr5 +2)>>2);
  pshStr4--;pshStr3--;pshStr5--;
#endif
  // H Calculation 3 string
  (*pshStr3++)+=((*pshStr2++ + *pshStr4++ +1)>>1 );
  (*pshStr3)+=((*pshStr2 + *pshStr4 +1)>>1 );
}


// L Correction of 2 and 4 string
// H calculation of 1,3 and 5 string

void __inline wci_last_v4(short *pshStr0,short *pshStr1,short *pshStr2,short *pshStr3,short *pshStr4,short *pshStr5)
{
#ifdef L_Correction
  // LL correction of 2 string
  (*pshStr2++)-=((*pshStr1++ +*pshStr3++ +2)>>2);
  (*pshStr2++)-=((*pshStr1++ +*pshStr3++ +2)>>2);
  (*pshStr2++)-=((*pshStr1++ +*pshStr3++ +2)>>2);
  (*pshStr2)-=((*pshStr1 +*pshStr3 +2)>>2);
  pshStr2-=3;  pshStr3-=3;pshStr1-=3;
  #endif
  // H Calculation 1 string
  (*pshStr1++)+=((*pshStr0++ + *pshStr2++ +1)>>1 );
  (*pshStr1++)+=((*pshStr0++ + *pshStr2++ +1)>>1 );
  (*pshStr1++)+=((*pshStr0++ + *pshStr2++ +1)>>1 );
  (*pshStr1)+=((*pshStr0+     *pshStr2+1)>>1 );
  pshStr1-=3;  pshStr2-=3;pshStr0-=3;

  // LL correction of 4 string
#ifdef L_Correction
  (*pshStr4++)-=((*pshStr3++ +*pshStr5++ +2)>>2);
  (*pshStr4++)-=((*pshStr3++ +*pshStr5++ +2)>>2);
  (*pshStr4++)-=((*pshStr3++ +*pshStr5++ +2)>>2);
  (*pshStr4)-=((*pshStr3 +*pshStr5 +2)>>2);
  pshStr4-=3;pshStr3-=3;pshStr5-=3;
#endif
  // H Calculation 3 string
  (*pshStr3++)+=((*pshStr2++ + *pshStr4++ +1)>>1 );
  (*pshStr3++)+=((*pshStr2++ + *pshStr4++ +1)>>1 );
  (*pshStr3++)+=((*pshStr2++ + *pshStr4++ +1)>>1 );
  (*pshStr3)+=((*pshStr2 + *pshStr4 +1)>>1 );
  pshStr4-=3;
  // H Calculation 5 string  
  (*pshStr5++)+=(*pshStr4++);
  (*pshStr5++)+=(*pshStr4++);
  (*pshStr5++)+=(*pshStr4++);
  (*pshStr5)+=(*pshStr4);

}

void __inline wci_last_v2(short *pshStr0,short *pshStr1,short *pshStr2,short *pshStr3,short *pshStr4,short *pshStr5)
{
  // LL correction of 2 string
#ifdef L_Correction
  (*pshStr2++)-=((*pshStr1++ +*pshStr3++ +2)>>2);
  (*pshStr2)-=((*pshStr1 +*pshStr3 +2)>>2);
  pshStr2--;  pshStr3--;pshStr1--;
#endif
  // H Calculation 1 string
  (*pshStr1++)+=((*pshStr0++ + *pshStr2++ +1)>>1 );
  (*pshStr1)+=((*pshStr0 + *pshStr2 +1)>>1 );
  pshStr1--;  pshStr2--;pshStr0--;

  // LL correction of 4 string
#ifdef L_Correction
  (*pshStr4++)-=((*pshStr3++ +*pshStr5++ +2)>>2);
  (*pshStr4)-=((*pshStr3 +*pshStr5 +2)>>2);
  pshStr4--;pshStr3--;pshStr5--;
#endif
  // H Calculation 3 string
  (*pshStr3++)+=((*pshStr2++ + *pshStr4++ +1)>>1 );
  (*pshStr3)+=((*pshStr2 + *pshStr4 +1)>>1 );
  pshStr4--;
  // H Calculation 5 string  
  (*pshStr5++)+=(*pshStr4++);
  (*pshStr5)+=(*pshStr4);

}

// The first two strings is ready. The third is vertically transformed.
void  wci_invwavstripe_first_hv(short *buffer, int SizeX)
{
  short *pshStr0=buffer;
  short *pshStr1=pshStr0+SizeX;
  short *pshStr2=pshStr1+SizeX;
  short *pshStr3=pshStr2+SizeX;
  int   i,Nblock=SizeX/4;
  int Rem=SizeX%4;

  wci_first_trans_v4(pshStr0,pshStr1,pshStr2,pshStr3);
#ifdef L_Correction
  wci_first_trans_h1(pshStr0,pshStr1);
#endif

  for(i=0;i<Nblock-1;i++)
  {
    pshStr0+=4;pshStr1+=4;pshStr2+=4;pshStr3+=4;
    wci_first_trans_v4(pshStr0,pshStr1,pshStr2,pshStr3);
    TransH4((pshStr0-3));
    TransH4((pshStr1-3));
  }
  if(!Rem)
  {
    TransH3((pshStr0+1));
    TransH3((pshStr1+1));
  }
  else 
  {
    pshStr0+=4;pshStr1+=4;pshStr2+=4;pshStr3+=4;
    wci_first_trans_v2(pshStr0,pshStr1,pshStr2,pshStr3);
    wci_trans_h5((pshStr0-3));
    wci_trans_h5((pshStr1-3));
  }


  return;
}


// This function performs:
// L correction of 2 and 4 string
// H calculation of 1,3,5 string
// Horizontal transform of 0-5 string

void  wci_invwavstripe_last_hv(short *buffer, int SizeX)
{
  short *pshStr0=buffer;
  short *pshStr1=pshStr0+SizeX;
  short *pshStr2=pshStr1+SizeX;
  short *pshStr3=pshStr2+SizeX;
  short *pshStr4=pshStr3+SizeX;
  short *pshStr5=pshStr4+SizeX;
  int   i,Nblock=SizeX/4;
  int Rem=SizeX%4;

  wci_last_v4(pshStr0,pshStr1,pshStr2,pshStr3,pshStr4,pshStr5);
  //  The first step for horizontal transform
#ifdef L_Correction
  wci_last_trans_h1(pshStr0,pshStr1,pshStr2,pshStr3,pshStr4,pshStr5);
#endif
  for(i=0;i<Nblock-1;i++)
  {
    pshStr0+=4;pshStr1+=4;pshStr2+=4;pshStr3+=4;pshStr4+=4;pshStr5+=4;
    wci_last_v4(pshStr0,pshStr1,pshStr2,pshStr3,pshStr4,pshStr5);
    TransH4((pshStr0-3));
    TransH4((pshStr1-3));
    TransH4((pshStr2-3));
    TransH4((pshStr3-3));
    TransH4((pshStr4-3));
    TransH4((pshStr5-3));
  }
  if(!Rem)
  {

    TransH3((pshStr0+1));
    TransH3((pshStr1+1));
    TransH3((pshStr2+1));
    TransH3((pshStr3+1));
    TransH3((pshStr4+1));
    TransH3((pshStr5+1));
  }
  else
  {
    pshStr0+=4;pshStr1+=4;pshStr2+=4;pshStr3+=4;pshStr4+=4;pshStr5+=4;
    wci_last_v2(pshStr0,pshStr1,pshStr2,pshStr3,pshStr4,pshStr5);
    wci_trans_h5((pshStr0-3));
    wci_trans_h5((pshStr1-3));
    wci_trans_h5((pshStr2-3));
    wci_trans_h5((pshStr3-3));
    wci_trans_h5((pshStr4-3));
    wci_trans_h5((pshStr5-3));
  }
  return;
}

//------------------------------------------------------------------------------

static int wci_stripe_inv_trans_full ( int16_t * image, int width, int height )
{
  assert( FALSE ); // tbd
  return height;
} 

//------------------------------------------------------------------------------

static int wci_stripe_inv_trans_first( int16_t * image, int width )
{
  wci_invwavstripe_first_hv( image, width );
  return STRIPE_SIZE_STEP-STRIPE_RESPONSE;
} 

//------------------------------------------------------------------------------
// This function performs:
// L correction of 2 and 4 string
// H calculation of 1 and 3 string
// Horizontal transform of 0-3 string

#ifndef WAV_VAR_2
static int wci_stripe_inv_trans_middle( int16_t * image, int width, BOOL blLLCorr )
{
  assert( (width & 0x1) == 0 );
  assert( (((BYTE*)image - (BYTE*)NULL) & 0x3) == 0 );
  blLLCorr = blLLCorr;

  if ( width & 0x3 )
  {
    short *s0=image;
    short *s1=s0+width;
    short *s2=s1+width;
    short *s3=s2+width;
    short *s4=s3+width;
    short *s5=s4+width;
    int   i,Nblock=width/4;

    wci_middle_v4(s0,s1,s2,s3,s4,s5);

    //  The first step for horizontal transform
#ifdef L_Correction
    wci_trans_h1(s0,s1,s2,s3);
#endif

    for(i=0;i<Nblock-1;i++)
    {
      s0+=4;s1+=4;s2+=4;s3+=4;s4+=4;s5+=4;
#ifdef L_Correction
      LLCorr(s2,s1,s3);
#endif
      HCalc (s1,s0,s2);
#ifdef L_Correction
      LLCorr(s4,s3,s5);
#endif
      HCalc (s3,s2,s4);


      TransH4((s0-3));
      TransH4((s1-3));
      TransH4((s2-3));
      TransH4((s3-3));
    }

    s0+=4;s1+=4;s2+=4;s3+=4;s4+=4;s5+=4;
    wci_middle_v2(s0,s1,s2,s3,s4,s5);
    wci_trans_h5((s0-3));
    wci_trans_h5((s1-3));
    wci_trans_h5((s2-3));
    wci_trans_h5((s3-3));
  }
  else
  {
    short *s0=image;
    short *s1=s0+width;
    short *s2=s1+width;
    short *s3=s2+width;
    short *s4=s3+width;
    short *s5=s4+width;
    int   i,Nblock=width/4;

    wci_middle_v4(s0,s1,s2,s3,s4,s5);

    //  The first step for horizontal transform
#ifdef L_Correction
    wci_trans_h1(s0,s1,s2,s3);
#endif

    for(i=0;i<Nblock-1;i++)
    {
      s4+=4;s5+=4;s0+=4;
      s1+=4;s2+=4;s3+=4;
#ifdef L_Correction
      LLCorr(s2,s1,s3);

#endif
      HCalc (s1,s0,s2);
#ifdef L_Correction
      LLCorr(s4,s3,s5);
#endif
      HCalc (s3,s2,s4);


      TransH4((s0-3));
      TransH4((s1-3));
      TransH4((s2-3));
      TransH4((s3-3));


    }

    TransH3((s0+1));
    TransH3((s1+1));
    TransH3((s2+1));
    TransH3((s3+1));
  }

  return STRIPE_SIZE_STEP;
} 
#else //WAV_VAR_2
//wci_stripe_inv_trans_middle; Second version
static int wci_stripe_inv_trans_middle( int16_t * image, int width )
{
  assert( (width & 0x1) == 0 );

  if ( width & 0x3 )
  {
    short *s0=image;
    short *s1=s0+width;
    short *s2=s1+width;
    short *s3=s2+width;
    short *s4=s3+width;
    short *s5=s4+width;
    int   i,Nblock=width/4;

    wci_middle_v4(s0,s1,s2,s3,s4,s5);

    //  The first step for horizontal transform
#ifdef L_Correction
    //    wci_trans_h1(s0,s1,s2,s3);
    TransH1Def(s0);
    TransH1Def(s1);
    TransH1Def(s2);
    TransH1Def(s3);

#endif

    for(i=0;i<Nblock-1;i++)
    {
      s1+=4;s2+=4;
      s3+=4;

#ifdef L_Correction
      LLCorrInd(s2,s1,s3);
#endif
      s0+=4;
      HCalcInd (s1,s0,s2);
      TransH4((s0-3));
      TransH4((s1-3));
      s4+=4;s5+=4;

#ifdef L_Correction
      LLCorrInd(s4,s3,s5);
#endif
      HCalcInd (s3,s2,s4);

      TransH4((s2-3));
      TransH4((s3-3));
    }

    s0+=4;s1+=4;s2+=4;s3+=4;s4+=4;s5+=4;
    wci_middle_v2(s0,s1,s2,s3,s4,s5);
    wci_trans_h5((s0-3));
    wci_trans_h5((s1-3));
    wci_trans_h5((s2-3));
    wci_trans_h5((s3-3));
  }
  else
  {
    short *s0=image;
    short *s1=s0+width;
    short *s2=s1+width;
    short *s3=s2+width;
    short *s4=s3+width;
    short *s5=s4+width;
    int   i,Nblock=width/4;

    wci_middle_v4(s0,s1,s2,s3,s4,s5);

    //  The first step for horizontal transform
#ifdef L_Correction
    //    wci_trans_h1(s0,s1,s2,s3);
    TransH1Def(s0);
    TransH1Def(s1);
    TransH1Def(s2);
    TransH1Def(s3);
#endif

    for(i=0;i<Nblock-1;i++)
    {
      s1+=4;s2+=4;
      s3+=4;
#ifdef L_Correction
      LLCorrInd(s2,s1,s3);
#endif
      s0+=4;
      HCalcInd (s1,s0,s2);
      TransH4((s0-3));
      TransH4((s1-3));
      s4+=4;s5+=4;

#ifdef L_Correction
      LLCorrInd(s4,s3,s5);
#endif
      HCalcInd (s3,s2,s4);

      TransH4((s2-3));
      TransH4((s3-3));
    }

    TransH3((s0+1));
    TransH3((s1+1));
    TransH3((s2+1));
    TransH3((s3+1));
  }

  return STRIPE_SIZE_STEP;
} 
#endif //WAV_VAR_2
//------------------------------------------------------------------------------

static int wci_stripe_inv_trans_last( int16_t * image, int width )
{
  wci_invwavstripe_last_hv( image, width );
  return STRIPE_SIZE_STEP+STRIPE_RESPONSE;
} 

//------------------------------------------------------------------------------

void wci_stripe_insert_ll_lines(
  int          IN     step
, WAV_STRIPE * IN OUT stripe
)
{
  WAV_STRIPE_LAYER * layer = &stripe->layer[step];
  if ( !layer->skip_transformation )
  {
    // Copy from previous layer
    if ( step+1 < stripe->wav_steps )
    {
      int16_t *          image = &layer->buffer[layer->free_line_pos * layer->buffer_size.cx];
      WAV_STRIPE_LAYER * next_layer = &stripe->layer[step+1];
      int16_t *          ll    = &next_layer->buffer[next_layer->ready_line_pos*next_layer->buffer_size.cx];
      int                n, m;


      if(((BYTE*)image-(BYTE*)0)&0x3)
      {
        for ( n=0; n<STRIPE_SIZE_STEP/2; ++n )
        {
          for ( m=0; m<next_layer->block_size.cx; ++m )
          {
            *image++ = *ll++;
            *image++ = 0;
            //          image += 2; // packed order
          }
          image += layer->buffer_size.cx;
        }
      }
      else
      {
        uint32_t * piImage;	
        for ( n=0; n<STRIPE_SIZE_STEP/2; ++n )
        {
          piImage   = (uint32_t*)image;     
          for ( m=0; m<next_layer->block_size.cx; ++m )
          {    
            *piImage++ =*ll++;
          }
          image = (int16_t*)piImage;	  
          image += layer->buffer_size.cx;
        }
      }


      next_layer->ready_lines    -= STRIPE_SIZE_STEP/2;
      next_layer->ready_line_pos += STRIPE_SIZE_STEP/2;
      next_layer->loaded_lines   -= STRIPE_SIZE_STEP/2;

    #ifdef PRINT_STRIPE_COUNTERS

      DPRINTF( DPRINTF_DEBUG, " ins(%d), to [%3d], ready(%3d), load %3d in %3d\n"
        , next_layer->block_size.cx
        , layer->free_line_pos
        , next_layer->ready_lines
        , next_layer->loaded_lines
        , next_layer->ready_line_pos
        );

    #endif
    }
  }
}

//------------------------------------------------------------------------------
// The inverse wavelet transform using stripe, buffer contans 6 lines,
//short *pshStL,short *pshStH - states for L and H components

void wci_stripe_inv_transformation(
  int          IN     step
, WAV_STRIPE * IN OUT stripe
)
{
  WAV_STRIPE_LAYER * layer = &stripe->layer[step];
  BOOL bLLCorr = TRUE;
  
  #ifdef PRINT_STRIPE_COUNTERS
    DPRINTF( DPRINTF_DEBUG, " it b(%d), ready(%s %3d %3d), in %3d ->"
            , step
            , layer->mode == STRIPE_MODE_NONE      ? "none"
              : (layer->mode == STRIPE_MODE_FIRST  ? "first"
              : (layer->mode == STRIPE_MODE_MIDDLE ? "middle"
              : (layer->mode == STRIPE_MODE_LAST   ? "last"
              : "full"
              )))
            , layer->total_ready_lines
            , layer->ready_lines
            , layer->prepared_line_pos
          );
  #endif
  if ( !layer->skip_transformation )
  {
    int16_t * image = &layer->buffer[layer->prepared_line_pos * layer->block_size.cx];
    int       new_ready_lines = 0;

    // Transformation
    switch( layer->mode )
    {
    case STRIPE_MODE_FULL:
      new_ready_lines = wci_stripe_inv_trans_full ( image, layer->buffer_size.cx, layer->buffer_size.cy );
      break;

    case STRIPE_MODE_FIRST:
      new_ready_lines = wci_stripe_inv_trans_first ( image, layer->buffer_size.cx );
      break;

    case STRIPE_MODE_MIDDLE:
      new_ready_lines = wci_stripe_inv_trans_middle( image, layer->buffer_size.cx, bLLCorr);      
      break;

    case STRIPE_MODE_LAST:
      new_ready_lines = wci_stripe_inv_trans_last  ( image, layer->buffer_size.cx );      
      break;
    }

    assert(new_ready_lines > 0);

    layer->prepared_line_pos += new_ready_lines;
    layer->ready_lines       += new_ready_lines;
    layer->total_ready_lines += new_ready_lines;
  }

  #ifdef PRINT_STRIPE_COUNTERS
    DPRINTF( DPRINTF_DEBUG, " it e, ready(%3d %3d), in %3d \n"
            , layer->total_ready_lines
            , layer->ready_lines

            , layer->prepared_line_pos
          );
  #endif
}

//#endif
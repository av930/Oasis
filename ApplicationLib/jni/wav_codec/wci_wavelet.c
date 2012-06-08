/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wavelet.c

ABSTRACT:    This file contains procedures for performing wavelet transform using
             lifting scheme and the special procedures for getting and putting LL-frame

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include "../wci_portab.h"
#include "wci_wavelet.h"

//------------------------------------------------------------------------------

#ifdef _MSC_VER
 #pragma warning (disable: 4244)
#endif

//------------------------------------------------------------------------------

LIFT_WAV_BLOCK_FUNC_PTR     wci_lift_wav_block    = wci_lift_wav_block_c   ;
INV_LIFT_WAV_BLOCK_FUNC_PTR wci_inv_lift_wav_block = wci_inv_lift_wav_block_c;

//------------------------------------------------------------------------------

static void wci_order_coef(short *Orig,int posx, int posy,int  sizex, int  sizey, int  ImSizex,
                short *Tmp_Buffer);
static void wci_inv_order_coef(short *Orig,int posx, int posy,int  sizex, int  sizey, int  ImSizex,
                short *Tmp_Buffer);
static void wci_wavlet_insert_ll_c(short *Orig,int posx, int posy,int  sizex, int  sizey, int  ImSizeX,
                        short *Tmp_Buffer);
static void wci_lift_wav_block_ord(short *Orig, int posx, int posy, int sizex, int sizey, int ImSizeX, short *Tmp_Buffer);
static void wci_inv_lift_wav_block_ord(short *Split, int posx, int posy,int sizex, int sizey, int ImSizeX, short *Tmp_Buffer);

//------------------------------------------------------------------------------

void wci_lift_wav_block_c(
  short *Orig,int posx, int posy,int  sizex, int  sizey, int  ImSizeX,
  short *Tmp_Buffer)
{
    wci_lift_wav_block_ord(Orig,posx,posy,sizex,sizey,ImSizeX,Tmp_Buffer);
    wci_order_coef(Orig,posx,posy,sizex,sizey,ImSizeX,Tmp_Buffer);
    wci_wavlet_insert_ll_c(Orig,posx,posy,sizex,sizey,ImSizeX,Tmp_Buffer);
    return;
}

//------------------------------------------------------------------------------

static void wci_wavlet_insert_ll_c(short *Orig,int posx, int posy,int  sizex, int  sizey, int  ImSizeX,
                        short *Tmp_Buffer)
{
  short *p_ll = Tmp_Buffer+posx/2+posy*ImSizeX/4;
  short *p_block = Orig + posx+posy*ImSizeX; 
  int Ngroups_in_row = sizex / GROUP_SIZE_X;
  int res_in_row_d2 = (sizex % GROUP_SIZE_X)/2;
  int i,j;
  
  for(i=0;i<sizey/2;i++)
  {
    for(j=0; j<Ngroups_in_row; j++)
    {
      memcpy(p_ll, p_block, GROUP_HALF_SIZE_X*sizeof(short));
      p_ll += GROUP_HALF_SIZE_X;p_block+=GROUP_SIZE_X;
    }
    if(res_in_row_d2)
    {
      memcpy(p_ll, p_block, res_in_row_d2*sizeof(short));
      p_ll += res_in_row_d2;p_block+=(res_in_row_d2*2);
    }

    p_block+=(2*ImSizeX-sizex);p_ll +=(ImSizeX/2-sizex/2);
  }
}

//------------------------------------------------------------------------------

static void wci_order_coef(short *Orig,int posx, int posy,int  sizex, int  sizey, int  ImSizex,
                short *Tmp_Buffer)
{
  int rest_x=sizex%GROUP_SIZE_X;
  int rest_x_half=rest_x/2;
  int num_xgroup=sizex/GROUP_SIZE_X;
  int num_ygroup=sizey/2;
  int i,j;
  short *ptr_ord,*ptr_h,*ptr_l,*ptr_st;
  int ShiftB=posx+posy*ImSizex;

  // Horizontal ordering  
  ptr_st=Orig+ShiftB;
  for(j=0;j<sizey;j++)
  {
     ptr_l=ptr_st;ptr_h=ptr_st+sizex/2;
     ptr_ord=Tmp_Buffer;

      for(i=0;i<num_xgroup;i++)
      {
        memcpy(ptr_ord,ptr_l,4*sizeof(short));
        ptr_ord+=4;
        memcpy(ptr_ord,ptr_h,4*sizeof(short));
        ptr_ord+=4;
        ptr_l+=4;ptr_h+=4;
      }
      if(rest_x)
      {
        memcpy(ptr_ord,ptr_l,rest_x_half*sizeof(short));
        ptr_ord+=rest_x_half;
        memcpy(ptr_ord,ptr_h,rest_x_half*sizeof(short));
      }
      memcpy(ptr_st,Tmp_Buffer,sizex*sizeof(short));
      ptr_st+=ImSizex;
  }

  // Vertical ordering
  
  for(i=0;i<sizex;i++)
  {
    ptr_st=Orig+ShiftB+i;
    ptr_l=ptr_st;ptr_h=ptr_st+ImSizex*sizey/2;
    ptr_ord=Tmp_Buffer;
    for(j=0;j<num_ygroup;j++)
    {
      *ptr_ord++=*ptr_l;
      *ptr_ord++=*ptr_h;
      ptr_l+=ImSizex;ptr_h+=ImSizex;
    }
    ptr_ord=Tmp_Buffer;
    for(j=0;j<sizey;j++)
    {
      *ptr_st=*ptr_ord++;
      ptr_st+=ImSizex;
    }
  
  }
}

//------------------------------------------------------------------------------

static void wci_inv_order_coef(short *Orig,int posx, int posy,int  sizex, int  sizey, int  ImSizex,
                short *Tmp_Buffer)
{
  int rest_x=sizex%GROUP_SIZE_X;
  int rest_x_half=rest_x/2;
  int num_xgroup=sizex/GROUP_SIZE_X;
  int num_ygroup=sizey/2;
  int i,j;
  short *ptr_ord,*ptr_h,*ptr_l,*ptr_st;
  int ShiftB=posx+posy*ImSizex;

// Horizontal ordering
  ptr_st=Orig+ShiftB;
  for(j=0;j<sizey;j++)
  {
    ptr_l=Tmp_Buffer;ptr_h=Tmp_Buffer+sizex/2;
    ptr_ord=ptr_st;
    for(i=0;i<num_xgroup;i++)
    {
      memcpy(ptr_l,ptr_ord,4*sizeof(short));
      ptr_ord+=4;ptr_l+=4;
      memcpy(ptr_h,ptr_ord,4*sizeof(short));
      ptr_ord+=4;ptr_h+=4;
    }
    if(rest_x)
    {
      memcpy(ptr_l,ptr_ord,rest_x_half*sizeof(short));
      ptr_ord+=rest_x_half;ptr_l+=rest_x_half;
      memcpy(ptr_h,ptr_ord,rest_x_half*sizeof(short));
    }
    memcpy(ptr_st,Tmp_Buffer,sizex*sizeof(short));
    ptr_st+=ImSizex;
  }

// Vertical ordering

  for(i=0;i<sizex;i++)
  {
    ptr_st=Orig+ShiftB+i;
    ptr_ord=ptr_st;
    ptr_l=Tmp_Buffer;ptr_h=Tmp_Buffer+sizey/2;
    for(j=0;j<num_ygroup;j++)
    {
      *ptr_l++=*ptr_ord;
      ptr_ord+=ImSizex;
      *ptr_h++=*ptr_ord;
      ptr_ord+=ImSizex;
    }
    ptr_ord=Tmp_Buffer;
    ptr_st=Orig+ShiftB+i;
    for(j=0;j<sizey;j++)
    {
      *ptr_st=*ptr_ord++;
      ptr_st+=ImSizex;
    }
  }

  return;
}

//------------------------------------------------------------------------------

void wci_inv_lift_wav_block_c(
  short *Split,int  posx, int  posy,int  sizex,int  sizey,int  ImSizeX,
  short *Tmp_Buffer)
{
    wci_inv_order_coef(Split,posx,posy,sizex,sizey,ImSizeX,Tmp_Buffer);
    wci_inv_lift_wav_block_ord(Split,posx,posy,sizex,sizey,ImSizeX,Tmp_Buffer);
}

//------------------------------------------------------------------------------

static void wci_lift_wav_block_ord(short *Orig, int posx, int posy, int sizex, int sizey, int ImSizeX, short *Tmp_Buffer)
{
  short *BufferV,*BufferH;
  short *ptr_orig,*ptr_spl,*ptr_sph;
  short i,j;
  short Hsizex=(short)(sizex>>1);
  short Hsizey=(short)(sizey>>1);
  short shiftH=(short)(ImSizeX-sizex);
  int shiftB=posx+posy*(int)ImSizeX;

  BufferV=Tmp_Buffer;BufferH=Tmp_Buffer+sizey;
	ptr_orig=Orig+shiftB;

// H-transform
	ptr_spl=BufferH;ptr_sph=ptr_spl+Hsizex;
  ptr_orig=Orig+shiftB;
	for(i=0;i<sizey;i++)
	{
		// Lazy Horizontal
		for(j=0;j<Hsizex;j++)
		{
		    ptr_spl[j]=*ptr_orig++;
			ptr_sph[j]=*ptr_orig++;
		}
		
    // H-calculation 
		for(j=0;j<Hsizex-1;j++)
		{
	    ptr_sph[j]-=( (ptr_spl[j]+ptr_spl[j+1]+1)>>1 );
    }
    ptr_sph[Hsizex-1]-=(ptr_spl[Hsizex-1]);     // Correction 

    // L-corr 
    #ifdef L_CORRECTION
		  for(j=1;j<Hsizex;j++)
		  {
        ptr_spl[j]+=( (ptr_sph[j]+ptr_sph[j-1]+2)>>2);
			}
      ptr_spl[0]+=((ptr_sph[0]+1)>>1); // Correction (Additional operator)
    #endif
    
  	memcpy(ptr_orig-sizex,BufferH,sizex*sizeof(short));
		
		ptr_orig+=shiftH;
	}

  BufferV=Tmp_Buffer;BufferH=Tmp_Buffer+sizey;
	ptr_orig=Orig+shiftB;

  // V-transform	
	for(j=0;j<sizex;j++)
	{
		ptr_orig=Orig+shiftB+j; 
		// Lazy Vertical
		ptr_spl=BufferV;ptr_sph=BufferV+Hsizey;
		for(i=0;i<Hsizey;i++)
		{
		    ptr_spl[i]=*ptr_orig;ptr_orig+=ImSizeX;
			ptr_sph[i]=*ptr_orig;ptr_orig+=ImSizeX;
		}

		//H calculation
		for(i=0;i<Hsizey-1;i++)
		{
      ptr_sph[i]-=( (ptr_spl[i]+ptr_spl[i+1]+1)>>1 );
    }
    ptr_sph[Hsizey-1]-=(ptr_spl[Hsizey-1]);     // Correction 

    #ifdef L_CORRECTION
		  // L- Correction
  		for(i=1;i<Hsizey;i++)
  		{
        ptr_spl[i]+=( (ptr_sph[i]+ptr_sph[i-1]+2)>>2);
  		}
      ptr_spl[0]+=((ptr_sph[0]+1)>>1); // Correction(Additional operator)
    #endif

		// Return string to Split
		ptr_orig=Orig+shiftB+j; 
		for(i=0;i<sizey;i++)
		{
		    *ptr_orig=ptr_spl[i];
			ptr_orig+=ImSizeX;
		}
	}

 return;
}

//------------------------------------------------------------------------------

static void wci_inv_lift_wav_block_ord(short *Split, int posx, int posy,int sizex, int sizey, int ImSizeX, short *Tmp_Buffer)
{
  short *BufferV;
  short *BufferH;
  short *ptr_orig,*ptr_spl,*ptr_sph;
  short *ptr_buf, *ptr_rec;
  short i,j;
  short Hsizex=(short)(sizex>>1);
  short Hsizey=(short)(sizey>>1);
  short shiftH=(short)(ImSizeX-sizex);
  int shiftB=posx+posy*(int)ImSizeX;

  BufferV=Tmp_Buffer;BufferH=Tmp_Buffer+sizey;

  // V-inverse transform	
  for(j=0;j<sizex;j++)
	{
		ptr_orig=Split+shiftB+j;
		ptr_rec=ptr_orig;
		ptr_buf=BufferV;
		// Copy to Buffer
		for(i=0;i<sizey;i++)
		{
		    ptr_buf[i]=*ptr_orig;
			ptr_orig+=ImSizeX;
		}
		ptr_spl=BufferV;ptr_sph=BufferV+Hsizey;

    #ifdef L_CORRECTION
		  // Inverse  L - Correction
		  for(i=1;i<Hsizey;i++)
		  {
        ptr_spl[i]-=( (ptr_sph[i]+ptr_sph[i-1]+2)>>2);
			}
      ptr_spl[0]-=((ptr_sph[0]+1)>>1); // Correction (Additional operator)
    #endif

		//  Reconstr

		for(i=0;i<Hsizey-1;i++)
		{
      ptr_sph[i]+=( (ptr_spl[i]+ptr_spl[i+1]+1)>>1 );
		}
    ptr_sph[Hsizey-1]+=(ptr_spl[Hsizey-1]);         // Correction


		for(i=0;i<Hsizey;i++)
		{
			*ptr_rec=ptr_spl[i];ptr_rec+=ImSizeX;
			*ptr_rec=ptr_sph[i];ptr_rec+=ImSizeX;
		}
		
	}

  BufferV=Tmp_Buffer;BufferH=Tmp_Buffer+sizey;
	
  // H-inverse transform	
	ptr_orig=Split+shiftB;
	for(i=0;i<sizey;i++)
	{
		memcpy(BufferH,ptr_orig,sizex*sizeof(short));
    ptr_spl=BufferH;ptr_sph=BufferH+Hsizex;
    
    #ifdef L_CORRECTION
      // L Calc
		  for(j=1;j<Hsizex;j++)
		  {
        ptr_spl[j]-=( (ptr_sph[j]+ptr_sph[j-1]+2)>>2); 
			}
      ptr_spl[0]-=((ptr_sph[0]+1)>>1); // Correction(Additional operator)
    #endif

    // Reconstr
		for(j=0;j<Hsizex-1;j++)
		{
      ptr_sph[j]+=( (ptr_spl[j]+ptr_spl[j+1]+1)>>1 );
    }
    ptr_sph[Hsizex-1]+=(ptr_spl[Hsizex-1]); //Correction


		for(j=0;j<Hsizex;j++)
		{
			*ptr_orig++=ptr_spl[j];
			*ptr_orig++=ptr_sph[j];
		}
		ptr_orig+=shiftH;
	}

  return;
}

//------------------------------------------------------------------------------

void wci_inv_wavlet_insert_ll_c(short *ptr_small, int pos_x, int pos_y, int size_x, int size_y, int ImSizeX, short *ptr_big)
{
  int jg;
  short *dest, *dest0;
  short *p_small = ptr_small;
  short *p_fin = ptr_big + size_x*size_y; 
  int Ngroups_in_row = size_x / GROUP_SIZE_X;
  int res_in_row_d2 = (size_x % GROUP_SIZE_X)/2;
  int sh2 = GROUP_SIZE_Y*size_x;
  dest0 = ptr_big;
  ImSizeX= ImSizeX; //shut up compiler warning for currently unused vars
  pos_x=pos_x;
  pos_y=pos_y;

  if(res_in_row_d2 != 0)
  {
    while(dest0<p_fin)
    {
      for(jg=0,dest = dest0; jg<Ngroups_in_row; jg++,dest += GROUP_SIZE_X)
      {
        memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
        p_small += GROUP_HALF_SIZE_X;
      }
      memcpy(dest, p_small, res_in_row_d2*sizeof(short));
      p_small += res_in_row_d2;

      dest0+=sh2;
    }
  }
  else
  {
    while(dest0<p_fin)
    {
      for(jg=0,dest = dest0; jg<Ngroups_in_row; jg++,dest += GROUP_SIZE_X)
      {
        memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
        p_small += GROUP_HALF_SIZE_X;
      }
      dest0+=sh2;
    }
  }
}

//------------------------------------------------------------------------------

void wci_inv_insert_8x8_ll(short *ptr_small, int ImSizeX, short *ptr_big)
{
  short *dest = ptr_big;
  short *p_small = ptr_small;
  int sh1 = ImSizeX/2 - GROUP_HALF_SIZE_X;
  int sh2 = GROUP_SIZE_Y*ImSizeX - GROUP_SIZE_X;

  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += GROUP_HALF_SIZE_X;
  dest += GROUP_SIZE_X;
  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += sh1;
  dest += sh2;

  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += GROUP_HALF_SIZE_X;
  dest += GROUP_SIZE_X;
  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += sh1;
  dest += sh2;

  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += GROUP_HALF_SIZE_X;
  dest += GROUP_SIZE_X;
  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += sh1;
  dest += sh2;

  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += GROUP_HALF_SIZE_X;
  dest += GROUP_SIZE_X;
  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += sh1;
  dest += sh2;

  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += GROUP_HALF_SIZE_X;
  dest += GROUP_SIZE_X;
  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += sh1;
  dest += sh2;

  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += GROUP_HALF_SIZE_X;
  dest += GROUP_SIZE_X;
  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += sh1;
  dest += sh2;

  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += GROUP_HALF_SIZE_X;
  dest += GROUP_SIZE_X;
  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += sh1;
  dest += sh2;

  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += GROUP_HALF_SIZE_X;
  dest += GROUP_SIZE_X;
  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));

}

//------------------------------------------------------------------------------

void wci_inv_insert_4x4_ll(short *ptr_small, int ImSizeX, short *ptr_big)
{
  short *dest = ptr_big;
  short *p_small = ptr_small;
  int sh1 = ImSizeX/2;
  int sh2 = GROUP_SIZE_Y*ImSizeX;

  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += sh1;
  dest+=sh2;

  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += sh1;
  dest+=sh2;

  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
  p_small += sh1;
  dest+=sh2;

  memcpy(dest, p_small, GROUP_HALF_SIZE_X*sizeof(short));
}

//------------------------------------------------------------------------------

void wci_inv_insert_2x2_ll(short *ptr_small, int ImSizeX, short *ptr_big)
{
  short *dest = ptr_big;
  short *p_small = ptr_small;

  memcpy(dest, p_small, 2*sizeof(short));
  p_small += (ImSizeX/2);
  dest+=(GROUP_SIZE_Y*ImSizeX);

  memcpy(dest, p_small, 2*sizeof(short));
}

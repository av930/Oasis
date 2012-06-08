/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: huff3_decompress.c

ABSTRACT:    This file contains procedures for Huffman decompression

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "wci_huff_decompress.h"

#ifdef _MSC_VER
 #pragma warning (disable : 4018 4127 4244 4389)
#endif


//------------------------------------------------------------------------------

#define CMP(p,q,r,s) (p<q ? 0 : (p>q ? 1 : (r<s ? 1 : 0)))

#define SWP(p,q)\
  {\
  if(p!=q){\
  symb[p]^=symb[q];\
  symb[q]^=symb[p];\
  symb[p]^=symb[q];\
  }\
  }

//------------------------------------------------------------------------------

static int wci_sh_sortlen(uchar *len, uchar *symb);
static int wci_sh_expand_tree(uchar *len, uint *rbuf);

#ifndef HUFF_KEYS 
  static void wci_sh_gendecode(uchar *len, uchar *symb, uchar *base, uchar *offs, uchar *cache, int n);
#else
  static void wci_sh_gendecode(uchar *len, uchar *symb, uchar *base, uchar *offs, uchar *cache, int n, int16_t *key, int32_t *maxlen);
#endif

//------------------------------------------------------------------------------

static int wci_sh_sortlen(uchar *len, uchar *symb)
{
  int ls[5], hs[5];
  int high, low, mid, cursymb, curlen, s, n, i, j;

  for(i=n=0; i<SH_N; i++)
  {
    if(len[i])
    {
      n++;
    }
    symb[i]=i;
  }

  ls[0]=0;
  hs[0]=SH_N-1;

  for(s=0; s>=0; s--)
  {
    low=ls[s];
    high=hs[s];

    while(low<high)
    {
      if(high-low<=8)
      {
        for(i=low+1; i<=high; i++)
        {
          curlen=len[symb[i]];
          cursymb=symb[i];
          for(j=i; j>low && CMP(curlen,len[symb[j-1]],cursymb,symb[j-1]); j--)
            symb[j]=symb[j-1];
          symb[j]=cursymb;
        }
        break;
      }

      mid=(high+low)>>1;
      curlen=len[symb[mid]];
      cursymb=symb[mid];
      SWP(low,mid);
      i=low+1;
      j=high;

      while(1)
      {
        while(i<=j && CMP(len[symb[i]],curlen,symb[i],cursymb))
          i++;
        while(CMP(curlen,len[symb[j]],cursymb,symb[j]))
          j--;
        if(i>=j)
          break;
        SWP(i,j);
      }
        
      SWP(low,j);

      if(j-low<=high-j)
      {
        ls[s]=j+1;
        hs[s++]=high;
        high=j-1;
      }
      else
      {
        ls[s]=low;
        hs[s++]=j-1;
        low=j+1;
      }
    }
  }
  
  return n;
}

//------------------------------------------------------------------------------

static int wci_sh_expand_tree(uchar *len, uint *rbuf)
{
  int zerobits, lenbits, flag, curlen, curzero, unused, i, j;
  uint buf, lenmask, zeromask;
  buf=GET_WORD32(&rbuf[0]);
  lenbits=buf>>28;
  zerobits=(buf>>24)&0xF;
  flag=(buf>>16)&0xFF;

  for(i=0; i<SH_N; i++) 
    len[i]=0;

  lenmask=(1<<lenbits)-1;
  zeromask=(1<<zerobits)-1;

  for(unused=16, i=0, j=1; i<SH_N; i++)
  {
    if(lenbits<=unused)
    {
      curlen=(buf>>(unused-lenbits))&lenmask;
      unused-=lenbits;
    }
    else
    {
      curlen=buf;
      buf=GET_WORD32(&rbuf[j]); j++;
      curlen<<=(lenbits-unused);
      curlen|=buf>>(32-lenbits+unused);
      curlen&=lenmask;
      unused+=32-lenbits;
    }

    if(curlen!=flag)
      len[i]=curlen;
    else
    {
      if(zerobits<=unused)
      {
        curzero=(buf>>(unused-zerobits))&zeromask;
        unused-=zerobits;
      } 
      else
      {
        curzero=buf;
        buf=GET_WORD32(&rbuf[j]);
        j++;
        curzero<<=(zerobits-unused);
        curzero|=buf>>(32-zerobits+unused);
        curzero&=zeromask;
        unused+=32-zerobits;
      }
      i+=curzero-1;
    }
  }
  return (j * sizeof(int));
}

//------------------------------------------------------------------------------

#ifndef HUFF_KEYS
 static void wci_sh_gendecode(uchar *len, uchar *symb, uchar *base, uchar *offs, uchar *cache, int n)
#else
 static void wci_sh_gendecode(uchar *len, uchar *symb, uchar *base, uchar *offs, uchar *cache, int n, int16_t *key, int32_t *maxlen)
#endif                  
{
  int intr, leaf, i, j;

  for(i=0; i<SH_MAXLENGTH; i++) 
    base[i]=offs[i]=0;

  for(intr=j=0,i=len[symb[0]]; i>=0; i--)
  {
    leaf=0;
    while((len[symb[j]]==i) && j<n)
    {
      leaf++;
      j++;
    }
    base[i]=intr;
    if(leaf)
      offs[i]=j-leaf-base[i];
    intr=(intr+leaf)>>1;
  }
  
  for(i=0; i<SH_CACHESIZE; i++)
  {
    for(j=1;(j<=SH_CACHEBITS)&&(i>>(SH_CACHEBITS-j)<base[j]);j++);
      cache[i]=j;
  }
  
  #ifdef HUFF_KEYS
    memset( key, 0xFFFFFFFF, sizeof(int16_t)*SH_CACHESIZE );

    // set key symbol
    {
      uint n,codelen;
      *maxlen=0;
      for ( n=0; n<256; ++n )
      {
        if(len[n]>*maxlen)
          *maxlen=len[n];
      
        codelen = cache[n];        
        if ( codelen <= SH_CACHEBITS )
          key[n] = codelen << 8 | symb[(n>>(8-codelen))+ offs[codelen]];
      }
    }
  #endif
}

//------------------------------------------------------------------------------

int wci_huff_decompress_table
(
  BYTE *       IN  pbtInBuffer //! Input buffer of data
, uint         IN  nInSize     //! Length of data [byte]
, HUFF_TABLE * OUT prTable     //! Table contains data for expand compressed data
, uint *       OUT pnOutSize   //! Size of expanded table
)
{
  int n;
  nInSize= nInSize;

  memset( prTable, 0, sizeof(HUFF_TABLE) );

  *pnOutSize= wci_sh_expand_tree(prTable->len, (uint *)(pbtInBuffer));
  n = wci_sh_sortlen(prTable->len, prTable->symb);

#ifndef HUFF_KEYS
  wci_sh_gendecode(prTable->len, prTable->symb, prTable->base, prTable->offs, prTable->cache, n);
#else
  wci_sh_gendecode(prTable->len, prTable->symb, prTable->base, prTable->offs, prTable->cache, n, prTable->key, &prTable->max_code_length);
#endif
  return 0;
}

//------------------------------------------------------------------------------

void wci_huff_init_context
(
  const BYTE *   IN  pbtInBuffer //! Input buffer of data
, HUFF_CONTEXT * OUT prContext   //! Context data
)
{
  prContext->nUnused  = 31;
  prContext->pbtBuf   = pbtInBuffer; 
  // put first word32 into local proc buffer
  prContext->nBuf     = GET_WORD32(prContext->pbtBuf);
  prContext->nBuf     = prContext->nBuf <<1;
  prContext->pbtBuf  += sizeof(uint);
}


//------------------------------------------------------------------------------


int wci_huff_decompress_data
(
 const HUFF_TABLE *    OUT  prTable     //! Table contains data for expand compressed data
 , HUFF_CONTEXT *     IN OUT  prContext   //! Context data contains current state of expander
 , uint               IN      nSize       //! Length of data to expand [byte]
 , BYTE         *        OUT  pbOutBuffer //! Output data buffer or NULL for skip
 )
{
  uint i, codelen;
  uint next_chunk = 0;
  uint unused  = prContext->nUnused ; 
  uint buffer  = prContext->nBuf    ; 
  uint unused2 = 0;
  uint max_code_length  = prTable->max_code_length;

#ifdef _DEBUG_HUFFMAN 
  FILE * pFile = fopen("huff_stat.dat","ab");
#endif

  assert(pbOutBuffer);

  {
    const int16_t *key = prTable->key;

    for( i=0; i<nSize; i++ )
    {
      int code, value;

      if (unused <= max_code_length)
      {
        // load
        if( unused + unused2 < 32 )
        {
          buffer = buffer | (next_chunk >> unused);
          unused += unused2;

          next_chunk = GET_WORD32(prContext->pbtBuf);
          prContext->pbtBuf += sizeof(uint);

          unused2 = 32;
        }

        buffer = buffer | (next_chunk >> unused);
        next_chunk <<= (32-unused);
        unused2 -= (32-unused);
        unused = 32;
      }

      code = buffer>>(32-SH_CACHEBITS);
      value = key[code];

      if( value >= 0 )                                        
      {
        *pbOutBuffer++ = value;      
        codelen = prTable->cache[code];
#ifdef _DEBUG_HUFFMAN        
        fwrite(&value,1,1,pFile);
        fflush(pFile);
#endif        
      }
      else
      {
        codelen = prTable->cache[code];
        while( (buffer>>(32-codelen))<prTable->base[codelen] )                    
        {                                                             
          codelen++;                                                  
        }                                                             
        *pbOutBuffer++ = prTable->symb[(buffer>>(32-codelen))+prTable->offs[codelen]]; 
#ifdef _DEBUG_HUFFMAN        
        fwrite(&(prTable->symb[(buffer>>(32-codelen))+prTable->offs[codelen]]),1,1,pFile);
        fflush(pFile);
#endif        
      }

      buffer <<= codelen;
      unused -= codelen;
    }                                                                 
  }

  if( unused + unused2 > 32)
  {
    prContext->pbtBuf -= sizeof(uint);
    unused-=(32-unused2);
  }
  else
  {
    buffer = buffer | (next_chunk >> unused);
    unused += unused2;
  }
  prContext->nBuf    = buffer; 
  prContext->nUnused = unused; 
#ifdef _DEBUG_HUFFMAN
  fclose(pFile);
#endif  
  return 0;
}

//------------------------------------------------------------------------------

int wci_huff_decompress_buffer
(  
  HUFF_TABLE   * OUT     prTable           //! Table for expand compressed data
, BYTE         * IN      pbtInBuffer       //! Input buffer containing compressed data
, uint           IN      nInSize           //! Length of Input buffer
, BYTE         * OUT     pbtOutBuffer      //! Output data buffer
, uint         * OUT     nOutSize          //! Length of Output data buffer [byte]
)
{
  uint tab_size;
  HUFF_CONTEXT rContext;

  if(wci_huff_decompress_table(pbtInBuffer, nInSize, prTable, &tab_size))
    return -1;
  
  wci_huff_init_context( (pbtInBuffer+tab_size), &rContext);

  if(wci_huff_decompress_data( prTable, &rContext, nInSize, pbtOutBuffer))
    return -1;
  
  *nOutSize = rContext.pbtBuf - pbtInBuffer + tab_size;
 
  return 0;
}

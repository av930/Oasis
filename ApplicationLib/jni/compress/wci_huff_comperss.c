/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: huff3_compress.c

ABSTRACT:    This file contains procedures for Huffman compression

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "wci_huff_compress.h"

//------------------------------------------------------------------------------

#ifdef _MSC_VER
 #pragma warning (disable : 4018 4127 4244 4389)
#endif

#define SH_N         (256)
#define SH_BUFSIZE   (65536)

#define SH_CACHEBITS (8)
#define SH_CACHESIZE (1<<SH_CACHEBITS)

#define SWP(p,q)     (tmp=symb[p], symb[p]=symb[q], symb[q]=tmp)
#define CMP(p,q,r,s) (p<q ? 0 : (p>q ? 1 : (r<s ? 1 : 0)))

//------------------------------------------------------------------------------

static int  wci_sh_sortlen(uchar *len, uchar *symb);
static int  wci_sh_sortfreq(uint *freq, uchar *symb);
static void wci_sh_calclen(uint *freq, uchar *symb, uchar *len, int n, int maxlen);
static void wci_sh_gencode(uchar *len, uchar *code, uchar *symb, int n, uchar *hz, uchar *hnz);
static int  wci_sh_packtree(uchar *len, uchar *symb, uchar *aux, uint *wbuf, int n);

//------------------------------------------------------------------------------

static int wci_sh_sortlen(uchar *len, uchar *symb)
{
  int ls[5], hs[5];
  int high, low, mid, cursymb, curlen, tmp, s, n, i, j;

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

static int wci_sh_sortfreq(uint *freq, uchar *symb)
{
  int ls[5], hs[5];
  int high, low, mid, tmp, pivot, n, i, j;
  int s;

  for(i=n=0; i<SH_N; i++)
  {
    if(freq[i])
      n++;
    symb[i]=i;
  }

  ls[0]=0; hs[0]=SH_N-1;

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
          pivot=freq[symb[i]];
          tmp=symb[i];
          for(j=i; j>low && pivot<freq[symb[j-1]]; j--)
            symb[j]=symb[j-1]; symb[j]=tmp;
        }
        break;
      }

      mid=(high+low)>>1;
      pivot=freq[symb[mid]];
      SWP(low,mid);
      i=low+1; j=high;

      while(1)
      {   while(i<=j && freq[symb[i]]<=pivot) i++;
          while(pivot<freq[symb[j]]) j--;
          if(i>=j) break; SWP(i,j);
      }
      SWP(low,j);

      if(j-low<=high-j)
      {   ls[s]=j+1;
          hs[s++]=high;
          high=j-1;
      }
      else
      {   ls[s]=low;
          hs[s++]=j-1;
          low=j+1;
      }
    }
  }
 
  for(i=0; i<n; i++)
    SWP(i,SH_N-n+i);
  return n;
}

//------------------------------------------------------------------------------

static void wci_sh_calclen(uint *freq, uchar *symb, uchar *len, int n, int maxlen)
{   
	int root, leaf, next, avbl, dpth;
  int removed, inserted, dpthins, first, last;

  if(n==1)
  {
    freq[symb[0]]=0;
    len[symb[0]]=1;
    return;
  }

  if(n==2)
  {
    freq[symb[0]]=freq[symb[1]]=0;
    len[symb[0]]=len[symb[1]]=1;
    return;
  }

  freq[symb[0]]+=freq[symb[1]];
  root=0;
  leaf=2;
    
  for(next=1; next<n-1; next++)
  {
    if (leaf>=n || freq[symb[root]]<freq[symb[leaf]])
    { 
      freq[symb[next]]=freq[symb[root]];
      freq[symb[root++]]=next;
    } 
    else
      freq[symb[next]]=freq[symb[leaf++]];

    if(leaf>=n || (root<next && freq[symb[root]]<freq[symb[leaf]]))
    { 
      freq[symb[next]]+=freq[symb[root]];
      freq[symb[root++]]=next+n;
    } 
    else 
      freq[symb[next]]+=freq[symb[leaf++]];
   }
   if(maxlen==-1)
   {
      removed=0;
      dpthins=n+2;
   }
   else
   {  first=last=n-3; dpth=1;
      while(dpth<maxlen-1)
      {  while((freq[symb[first]]%n)>last)
         {   root=first; first-=last-first+1;
             if(first<0) { first=-1; break; }
         }
         while(root-first>1)
         {   if((freq[symb[(first+root)>>1]]%n)>last)
             root=(first+root)>>1;
             else first=(first+root)>>1;
         }
         last=first; dpth++;
         if(last<0) 
           break;
      }
 
      removed=last+1;

      if((freq[symb[0]]%n)>=removed)
      {   removed=0;
          dpthins=n+2;
      }
      else
      {   
        for(dpthins=0; (1<<dpthins)<removed; dpthins++);
        dpthins=maxlen-dpthins;
      }
    }

    avbl=2; dpth=1; first=last=n-3;
    next=n-1; inserted=0;

    if(dpthins==1)
    {  avbl=4; dpth=2;
       inserted=2;
    }

    while(avbl>0)
    {  if(last<removed) first=last+1;
       else
       {  while((freq[symb[first]]%n)>last)
          {   root=first; first-=last-first+1;
              if(first<removed) { first=removed-1; break; }
          }

          while(root-first>1)
          {   if((freq[symb[(first+root)>>1]]%n)>last)
              root=(first+root)>>1;
              else first=(first+root)>>1;
          }
   
          first=root;
       }

       avbl-=last-first+1; root=0;

       if(dpth>=dpthins)
       {  root=1<<(dpth-dpthins);
          if((inserted+root)>removed)
          root=removed-inserted;
          inserted+=root; avbl-=root;
       }

       else if(dpth==(dpthins-1))
       {  if(freq[symb[first]]==(last+1)) first++;
          else avbl--; root=1; inserted++;
       }

       while (avbl>0)
       {  freq[symb[next--]]=dpth;
          avbl--;
       }

       avbl=(last-first+1+root)<<1;
       dpth++; last=(--first);
    }

    for(next=0; next<SH_N; next++) len[next]=freq[next];
}

//------------------------------------------------------------------------------

static void wci_sh_gencode(uchar *len, uchar *code, uchar *symb, int n, 
                uchar *hz, uchar *hnz)
{
  int i, codeval, next, prev;

  hz[0] = symb[0];
  for(i=0; i<SH_N; i++) code[i]=0;
  for(prev=len[symb[0]], i=codeval=0; i<n; i++, codeval++)
  {  next=len[symb[i]]; codeval>>=(prev-next);
     prev=next; code[symb[i]]=codeval;
     if ( codeval == (1 << (next -1)) ) 
       hnz[0] = symb[i];
  }
}

//------------------------------------------------------------------------------

static int wci_sh_packtree(uchar *len, uchar *symb, uchar *aux, 
				uint *wbuf, int n)
{
  int zerobits, maxzero, lenbits, maxlen, flag, i, j, t;
  int treesize, freebits, zerorun, curlen, cursymb, curbits, bufp;
  uint buf;

  t=maxlen=len[symb[0]];
  for(lenbits=0; t>0; t>>=1, lenbits++);
  zerobits=maxzero=flag=0;

  if(n<SH_N)
  { 
    t=0; i=n; cursymb=symb[n];
    while(i<SH_N)
    {  for(j=i; len[symb[i]]==0 && (symb[i]-cursymb<=1) && i<SH_N;
       cursymb=symb[i++]); aux[t++]=i-j;
       maxzero=((i-j) > maxzero ? (i-j) : maxzero);
       cursymb=symb[i];
    }

    for(zerobits=0, t=maxzero; t>0; t>>=1, zerobits++);
    flag=maxlen+1; if(((1<<lenbits)-1)==maxlen) lenbits++;
  }

  buf=(((lenbits<<4)|zerobits)<<8)|flag; freebits=16;

  for(treesize=bufp=t=i=0; i<SH_N; i++)
  { 
    curlen=len[i]; curbits=lenbits;
    if(!curlen)
    {  zerorun=aux[t++]; i+=zerorun-1;
       if(zerobits<(zerorun-1)*lenbits)
       {   curlen=(flag<<zerobits)|zerorun;
           curbits+=zerobits;
       } else curbits+=(zerorun-1)*lenbits;
    }

    if(curbits<=freebits)
    {  buf<<=curbits;
       buf|=curlen;
       freebits-=curbits;
    } else
    {  buf<<=freebits;
       buf|=(curlen>>(curbits-freebits));
       wbuf[bufp++]=buf; buf=curlen; 
       freebits+=(32-curbits);
       treesize+=4;
    }
  }

  wbuf[bufp]=(buf<<freebits); 
  treesize+=4;

  return treesize;
}

//------------------------------------------------------------------------------

static void wci_sh_encodefile(uint *ts,  
				uint *freq, uchar *symb, uchar *len, 
				uchar *code, uchar *aux, 
        uint insize, uchar *rbuf, 
        uint *outsize, uint *wbuf)
{   
  uint bx = 1;
  uint BX = 1;
  uchar hz[1], hnz[1];
  uint n, treesize, bufp, freebits, buf, i;
	
  memset( freq, 0, SH_N*sizeof(int) );

	for(i = 0; i < insize; i++) 
  {
    freq[rbuf[i]]++;
  }

  n = wci_sh_sortfreq(freq, symb);

  wci_sh_calclen(freq, symb, len, n, 31);

  wci_sh_sortlen(len, symb);

  wci_sh_gencode(len, code, symb, n, hz, hnz);
	    
  treesize = wci_sh_packtree(len, symb, aux, wbuf, n);
  *ts = treesize;

  bufp = treesize >> 2;

  freebits = 31; 

  buf = 0;

	for(i = 0; i < insize; i++)
	{    
    uint symblen, symbcode;
    symblen=len[rbuf[i]];
    symbcode=code[rbuf[i]];
    if(rbuf[i] == hz[0] ) bx = 1;
     else if (rbuf[i] != hnz[0]) bx = 0;
    if(symblen<=freebits)
    {  buf<<=symblen;
       buf|=symbcode;
       freebits-=symblen;
    } else
    {  buf<<=freebits;
       buf|=(symbcode>>(symblen-freebits));
       wbuf[bufp++]=buf;
       if( buf == 0 ) BX = 1;
       else if( buf != 0x80000000) BX = 0;
       buf=symbcode;
       freebits+=32-symblen;
    }
  } 
//  Check for endings 
  if( freebits == 0 ) 
	{
		memcpy(&wbuf[bufp], &buf, 4);
		bufp++;

  	if( buf == 0 ) 
	  	BX = 1;
		else 
			if( buf != 0x80000000) 
				BX = 0;
    
		if ( (bx + BX) == 1 ) 
		{
			buf = 0x80000000;
			memcpy(&wbuf[bufp], &buf, 4);
			bufp++;
		}
	} 
	else 
	{
		if ( bx == 1 ) 
		{
			buf <<= 1;
			freebits--;
			buf++;
			buf <<= freebits;
  		memcpy(&wbuf[bufp], &buf, 4);
	  	bufp++;
		} 
		else
		{
			if ( buf != 0 ) 
			{
				buf <<= freebits;
				memcpy(&wbuf[bufp], &buf, 4);
				bufp++;
			} 
		}
	}    

	*outsize = (bufp << 2);
}

//------------------------------------------------------------------------------

void wci_huff_compress_buffer(HUFF_PARAM *pHP,
						BYTE *pbInBuffer,  uint nInSize,
						BYTE *pbOutBuffer, uint *pnOutSize)
{
uint nTreeSize, nCompSize;
// Encoding with table generation
  wci_sh_encodefile(&nTreeSize,  
    pHP->freq, pHP->symb, 
    pHP->len,
    pHP->code,
    (uchar *)pHP->freq + 512, //aux
    nInSize, pbInBuffer,
    &nCompSize, &((uint*)pbOutBuffer)[3]);

  ((uint*)pbOutBuffer)[0] = nInSize;
  ((uint*)pbOutBuffer)[1] = nCompSize;
  ((uint*)pbOutBuffer)[2] = nTreeSize;

  *pnOutSize = nCompSize + 3 * sizeof(uint);
}

//------------------------------------------------------------------------------

int wci_huff_compress_table
(
  BYTE *       IN  pbtInBuffer //! Input buffer of data
, uint         IN  nInSize     //! Length of data [byte]
, HUFF_PARAM * OUT prTable     //! Huffman table contains data for compress data
, BYTE *       OUT pbtOutBuffer//! Out buffer where to put compressed table
, uint         OUT pnOutBufSize//! Size of Out buffer [byte]
, uint *       OUT pnTreeSize  //! Size of compressed table [byte]
)
{
  uint n, i;

	memset( prTable->freq, 0, SH_N*sizeof(int) );

	for(i = 0; i < nInSize; i++) 
  {
    prTable->freq[pbtInBuffer[i]]++;
  }

  n = wci_sh_sortfreq(prTable->freq, prTable->symb);

  wci_sh_calclen(prTable->freq, prTable->symb, prTable->len, n, 31);

  wci_sh_sortlen(prTable->len, prTable->symb);

  wci_sh_gencode(prTable->len, prTable->code, prTable->symb, n, prTable->hz, prTable->hnz);
	    
  *pnTreeSize = wci_sh_packtree(prTable->len, prTable->symb, (uchar *)prTable->freq + 512, (uint*)pbtOutBuffer, n);

  assert(*pnTreeSize <= pnOutBufSize);
  if(*pnTreeSize > pnOutBufSize)
    return -1;
  else
    return 0;
}

//------------------------------------------------------------------------------

int wci_huff_compress_data
(
  BYTE *       IN  pbtInBuffer //! Input buffer of data
, uint         IN  nInSize     //! Length of data [byte]
, HUFF_PARAM * OUT prTable     //! Huffman table contains data for compress data
, BYTE *       OUT pbtOutBuffer//! Out buffer where to put compressed table
, uint         OUT pnOutBufSize//! Size of Out buffer [byte]
, uint *       OUT pnOutSize   //! Size of compressed data [byte]
)
{
  uint bx = 1;
  uint BX = 1;
  uint  bufp, freebits, buf, i;
  uint * wbuf = (uint *)pbtOutBuffer;

  bufp = 0;
  freebits = 31;
  buf = 0;

	for(i = 0; i < nInSize; i++)
	{    
    uint symblen, symbcode;
    symblen=prTable->len[pbtInBuffer[i]];
    symbcode=prTable->code[pbtInBuffer[i]];
    if(pbtInBuffer[i] == prTable->hz[0] ) bx = 1;
     else if (pbtInBuffer[i] != prTable->hnz[0]) bx = 0;
    if(symblen<=freebits)
    {  buf<<=symblen;
       buf|=symbcode;
       freebits-=symblen;
    } else
    {  buf<<=freebits;
       buf|=(symbcode>>(symblen-freebits));
       wbuf[bufp++]=buf;
        if( buf == 0 ) BX = 1;
        else if( buf != 0x80000000) BX = 0;
       buf=symbcode;
       freebits+=32-symblen;
    }
  }
//  Check for endings 
  if( freebits == 0 ) 
	{
		memcpy(&wbuf[bufp], &buf, 4);
		bufp++;

  	if( buf == 0 ) 
	  	BX = 1;
		else 
			if( buf != 0x80000000) 
				BX = 0;
    
		if ( (bx + BX) == 1 ) 
		{
			buf = 0x80000000;
			memcpy(&wbuf[bufp], &buf, 4);
			bufp++;
		}
	} 
	else 
	{
		if ( bx == 1 ) 
		{
			buf <<= 1;
			freebits--;
			buf++;
			buf <<= freebits;
  		memcpy(&wbuf[bufp], &buf, 4);
	  	bufp++;
		} 
		else
		{
			if ( buf != 0 ) 
			{
				buf <<= freebits;
				memcpy(&wbuf[bufp], &buf, 4);
				bufp++;
			} 
		}
	}    

	*pnOutSize = (bufp << 2);

  assert(*pnOutSize <= pnOutBufSize);
  if( *pnOutSize > pnOutBufSize)
    return -1;
  else
    return 0;
}

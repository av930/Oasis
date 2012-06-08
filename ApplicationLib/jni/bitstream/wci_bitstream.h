/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_bitstream.h

ABSTRACT:    This file contains inlined functions and constants-headers 
             for procedures providing bitstream reading/writing.

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <string.h>

#include "../wci_portab.h"


#define      BSWAP1(tmp) \
{\
  register uint  t; \
  t = tmp ^ ((tmp << 16) | (tmp >> 16)); \
  t &= ~0xff0000; \
  tmp = (tmp << 24) | (tmp >> 8); \
  tmp = tmp ^ (t >> 8); \
}
     

//------------------------------------------------------------------------------

#ifdef _MSC_VER
 #if _MSC_VER < 1300 // _MSC_VER = 1300 for VC.Net
   #pragma warning(disable : 4710)
 #endif
#endif

//------------------------------------------------------------------------------

#define VIDOBJ_START_CODE     0x00000100  /* ..0x0000011f  */
#define VIDOBJLAY_START_CODE  0x00000120  /* ..0x0000012f */
#define VISOBJSEQ_START_CODE  0x000001b0
#define VISOBJSEQ_STOP_CODE   0x000001b1  
#define USERDATA_START_CODE   0x000001b2
#define GRPOFVOP_START_CODE   0x000001b3
#define VISOBJ_START_CODE     0x000001b5

#define VISOBJ_TYPE_VIDEO     1
//#define BITSTREAM_OPTIMIZE
//------------------------------------------------------------------------------

typedef struct
{
  uint bufa;            //! 32 bit buffer 
  uint bufb;            //! 32 bit buffer 
  uint buf;             //! 32 bit buffer 
  uint pos;             //! Position of bit pointer in "buf" or "bufa" and "bufb" fields 
  uint *tail;           //! Pointer of bitstream (uint) buffer tail. It is pointed to next unreaded uint or next free unfilled uint 
  uint *start;          //! Pointer of bitstream (uint) buffer start. It is presetted in initialization function 
  uint length;          //! Filled part length of bitstream (uint) buffer 
  uint *bsend;            //! Pointer to end of bitstream. It is presetted in initialization function
} BITSTREAM;            // Definition structure for bitstream operation 

//------------------------------------------------------------------------------

static void __inline wci_bitstream_init
(
  BITSTREAM * const prBS        //! the structure of bitstream
, void * const      pBitstream  //! the buffer
, uint32_t          length )    //! the length of the buffer
{
// This function initializes bitstream structure.
  uint32_t tmp;
  int i, left;
  BYTE * p= (BYTE *)pBitstream;

  assert( (pBitstream != NULL) && (length != 0) );

  prBS->tail = (uint32_t *) ((ptr_t)pBitstream & (~3));
  prBS->start = (uint32_t *) pBitstream;
  left= 4 - ((ptr_t)pBitstream & 3);
  prBS->pos = (4 - left) * 8;        

  for(i=0, tmp= 0; i<left; i++) // pBitstream can be not 32 bit aligned
    tmp |= (*p++)<<(i*8);
#ifndef ARCH_IS_BIG_ENDIAN
    BSWAP(tmp);
#endif
    prBS->bufa = tmp >> prBS->pos;
    tmp = *((uint32_t *) prBS->tail + 1);
#ifndef ARCH_IS_BIG_ENDIAN
    BSWAP(tmp);
#endif
    prBS->bufb = tmp;

  prBS->buf = 0;
  prBS->length = length;
  prBS->bsend = (uint*)((uint8_t*)prBS->start + prBS->length);

}

//------------------------------------------------------------------------------

static uint32_t __inline wci_bitstream_show_bits
(
  BITSTREAM * const prBS        //! the structure of bitstream
, const uint32_t    bits        //! the number of bits
)
{
// This function reads n bits from bitstream without changing the stream pos.
//   Return value is bitstream bits.
  int nbit = (bits + prBS->pos) - 32;

  assert(bits<=32);

  if (nbit > 0) {
    return ((prBS->bufa & (0xffffffff >> prBS->pos)) << nbit) | (prBS->bufb >> (32 - nbit));
  } else {
    return (prBS->bufa & (0xffffffff >> prBS->pos)) >> (32 - prBS->pos - bits);
  }
}

//------------------------------------------------------------------------------

static uint32_t __inline wci_bitstream_show_uint32
(
  BITSTREAM * const prBS        //! the structure of bitstream
 )
{
  if (prBS->pos > 0)
  {
    return (prBS->bufa << prBS->pos) | (prBS->bufb >> (32 - prBS->pos));
  }
  else
  {
    return prBS->bufa;
  }
}

//------------------------------------------------------------------------------

static void __inline wci_bitstream_skip
(
  BITSTREAM * const prBS        //! the structure of bitstream
, const uint32_t    bits        //! the number of bits
)
{
// This function skips n bits forward in bitstream.
// This function is internal, dont use it directly, use unified wci_bitstream_skip_bits()
  assert( ((ptr_t)prBS->tail < (ptr_t)prBS->start)
           ? ((ptr_t)prBS->start - (ptr_t)prBS->tail + (prBS->pos + bits + 7)/8) <= prBS->length
           : ((ptr_t)prBS->tail - (ptr_t)prBS->start + (prBS->pos + bits + 7)/8) <= prBS->length );
  
 
  prBS->pos += bits;
 
  if(prBS->pos >=32)
  {
    uint32_t tmp;
    prBS->tail +=prBS->pos>>5;

    if (prBS->tail > (prBS->bsend-2))
    {
      int iBytes = (int8_t*)prBS->bsend-(int8_t*)prBS->tail;
      assert( iBytes>=0 && iBytes < 8);   
      if( iBytes > 4)
      { 
        int i;
        uint32_t t = 0; 
        uint8_t *pt, *pTail ;
        pt = (uint8_t*)&t;
        pTail = (uint8_t*)&prBS->tail[1];
#ifdef BITSTREAM_OPTIMIZE
        {
          register uint* pTail = prBS->tail;
          prBS->bufa = ( (uint)((BYTE *)(pTail))[3] | (((uint)((BYTE *)(pTail))[2])<<8) | \
            ( ((uint)((BYTE *)(pTail))[1])<<16) | ( ((uint)((BYTE *)(pTail))[0])<<24) );
        }
#else
         tmp = GET_WORD32(prBS->tail);
#ifndef ARCH_IS_BIG_ENDIAN
        BSWAP1(tmp);
#endif
        prBS->bufa = tmp;
#endif
#ifndef ARCH_IS_BIG_ENDIAN
        for(i = 0; i < (iBytes - 4); i++)
        {
          pt[3-i] = pTail[i];
        }
#else
        for(i = 0; i < (iBytes - 4); i++)
        {
          pt[i] = pTail[i];
        }
#endif
        prBS->bufb = t;
      }
      else
      {
        int i;
        uint32_t t = 0; 
        uint8_t *pt, *pTail ;
        pt = (uint8_t*)&t;
        pTail = (uint8_t*)&prBS->tail[0];

#ifndef ARCH_IS_BIG_ENDIAN
        for(i = 0; i < iBytes ; i++)
        {
          pt[3-i] = pTail[i];
        }
#else
        for(i = 0; i < iBytes ; i++)
        {
          pt[3-i] = pTail[i];
        }
#endif
        prBS->bufa = t;
        prBS->bufb = 0;
      }

    }
    else
    {
#ifdef BITSTREAM_OPTIMIZE
      register uint* pTail = prBS->tail;
      prBS->bufa = ( (uint)((BYTE *)(pTail))[3] | (((uint)((BYTE *)(pTail))[2])<<8) | \
        ( ((uint)((BYTE *)(pTail))[1])<<16) | ( ((uint)((BYTE *)(pTail))[0])<<24) );
      pTail++;
      prBS->bufb = ( (uint)((BYTE *)(pTail))[3] | (((uint)((BYTE *)(pTail))[2])<<8) | \
        ( ((uint)((BYTE *)(pTail))[1])<<16) | ( ((uint)((BYTE *)(pTail))[0])<<24) );
#else
      tmp = GET_WORD32(prBS->tail);
#ifndef ARCH_IS_BIG_ENDIAN
      BSWAP1(tmp);
#endif
      prBS->bufa = tmp;
      tmp = GET_WORD32(prBS->tail + 1);
#ifndef ARCH_IS_BIG_ENDIAN
      BSWAP1(tmp);
#endif
      prBS->bufb = tmp;

#endif
    }
    prBS->pos = prBS->pos -  (prBS->pos>>5)*32;
  }
}


//------------------------------------------------------------------------------

static void __inline wci_bitstream_skip_backward
(
  BITSTREAM * const prBS        //! the structure of bitstream
, const uint32_t    bits        //! the number of bits
)
{
// This function skips n bits backward in bitstream.

	int left= prBS->pos - bits;
  
  assert((int)(prBS->tail - prBS->start - bits/8 - (prBS->pos - bits%32)/32) >= 0);
  
 	while(left < 0)
  {
    uint32_t tmp;
	  prBS->bufb = prBS->bufa;
	  prBS->tail--;
    tmp= *((uint32_t *) prBS->tail);
#ifndef ARCH_IS_BIG_ENDIAN
   	BSWAP(tmp);
#endif
		prBS->bufa= tmp;
    left+= 32;
	}

	prBS->pos= left;
}

//------------------------------------------------------------------------------

static void __inline wci_bitstream_byte_align
(
  BITSTREAM * const prBS
)
{
// This function moves forward to the next byte boundary.
  uint32_t remainder = prBS->pos % 8;

  if (remainder) {
    wci_bitstream_skip(prBS, 8 - remainder);
  }
}

//------------------------------------------------------------------------------

static uint32_t __inline wci_bitstream_get_unused_length(
  const BITSTREAM * const prBS
)
{
// This function returns bitstream length(unit bits).
  uint32_t nLastPos = sizeof(uint)*(prBS->tail - prBS->start) + (prBS->pos+7)/8;
  if ( prBS->length > nLastPos )
    return (uint32_t)(prBS->length - nLastPos);
  else
    return 0;
}

//------------------------------------------------------------------------------

static uint32_t __inline wci_bitstream_pos(
  const BITSTREAM * const prBS
)
{
// This function returns position (unit bytes)

  uint32_t len = ((ptr_t)prBS->tail < (ptr_t)prBS->start) ? 0 : (uint32_t)((ptr_t)prBS->tail - (ptr_t)prBS->start);

  if (prBS->pos) {
    len += (prBS->pos + 7) / 8;
  }
  return len;
}

//------------------------------------------------------------------------------

static void __inline wci_bitstream_forward
(
  BITSTREAM * const prBS
, const uint32_t    bits
)
{
// This function move bitstream position forward by n bits and write out buffer if needed.

  assert(bits<=32);
  assert( ((ptr_t)prBS->tail < (ptr_t)prBS->start)
    ? ((ptr_t)prBS->start - (ptr_t)prBS->tail + (prBS->pos + bits + 7)/8) <= prBS->length
    : ((ptr_t)prBS->tail - (ptr_t)prBS->start + (prBS->pos + bits + 7)/8) <= prBS->length );

  prBS->pos += bits;

  if (prBS->pos >= 32) {
    uint32_t b = prBS->buf;

#ifndef ARCH_IS_BIG_ENDIAN
    BSWAP(b);
#endif
    *prBS->tail++ = b;
    prBS->buf = 0;
    prBS->pos -= 32;
  }
}

//------------------------------------------------------------------------------

static void __inline wci_bitstream_pad(BITSTREAM * const prBS)
{
// This function pads bitstream to the next byte boundary.
  uint32_t remainder = prBS->pos % 8;

  if (remainder) {
    wci_bitstream_forward(prBS, 8 - remainder);
  }
}

//------------------------------------------------------------------------------

static void __inline wci_bitstream_put_bit
(
  BITSTREAM * const prBS
, const uint32_t    bit
)
{
// This function writes single bit to bitstream.

  if (bit)
    prBS->buf |= (0x80000000 >> prBS->pos);

  wci_bitstream_forward(prBS, 1);
}

//------------------------------------------------------------------------------

static void __inline wci_bitstream_put_bits
(
  BITSTREAM * const prBS,
         const uint32_t value,
         const uint32_t size)
{
// This function writes n bits to bitstream

  uint32_t shift = 32 - prBS->pos - size;
  assert(size <= 32);
  assert((size == 32) || ((value & (~((1<<size)-1)) ) == 0) );

  if (shift <= 32) {
    prBS->buf |= value << shift;
    wci_bitstream_forward(prBS, size);
  } else {
    uint32_t remainder;

    shift = size - (32 - prBS->pos);
    prBS->buf |= value >> shift;
    wci_bitstream_forward(prBS, size - shift);
    remainder = shift;

    shift = 32 - shift;

    prBS->buf |= value << shift;
    wci_bitstream_forward(prBS, remainder);
  }
}

//------------------------------------------------------------------------------

static void __inline wci_bitstream_put_uint32
(
  uint8_t * const pos,
  const uint32_t value
)
{
  uint32_t b = value;

#ifndef ARCH_IS_BIG_ENDIAN
  BSWAP(b);
#endif

  * (uint32_t*)pos = b;
}

//------------------------------------------------------------------------------

static void __inline wci_bitstream_put_uint16(
  uint8_t * const pos,
  const uint16_t value)
{
  uint32_t b = value << 16;

  #ifndef ARCH_IS_BIG_ENDIAN
    BSWAP(b);
  #endif

  * (uint16_t*)pos = (uint16_t)b;
}

//------------------------------------------------------------------------------

static __inline uint8_t * wci_bitstream_lock_buffer( BITSTREAM * const prBS )
{
  return (uint8_t*)prBS->start + wci_bitstream_pos(prBS);
}

//------------------------------------------------------------------------------

static void __inline wci_bitstream_unlock_buffer
(
  BITSTREAM * const prBS
, const uint8_t *   pbtBuffer,
  const uint32_t    uBufferLength
)
{
// This function unlocks bitstream
  uint32_t rest = uBufferLength % sizeof(uint32_t);

  assert(((ptr_t)prBS->tail - (ptr_t)prBS->start) < prBS->length );

  prBS->tail += uBufferLength / sizeof(uint32_t);

  if ((prBS->pos | rest) == 0)
    prBS->buf = 0;
  else
  {
    uint32_t b = *(uint32_t*)&pbtBuffer[uBufferLength-4];

    #ifndef ARCH_IS_BIG_ENDIAN
      BSWAP(b);
    #endif

    prBS->pos += 8*rest;
    if (prBS->pos >= 32)
      prBS->pos -= 32, prBS->tail++;

    if (prBS->pos == 0)
      prBS->buf = 0;
    else
      prBS->buf = b << (32 - prBS->pos);
  }
}

//------------------------------------------------------------------------------

static void __inline wci_bitstream_skip_bits(
  BITSTREAM * const prBS
, const uint32_t    uBits)
{
// This function skips uBits forward in bitstream.

  if (uBits <= 32)
    wci_bitstream_skip(prBS, uBits);
  else
  {
    int pos = prBS->pos;
    int bits = uBits;

    if (pos > 0)
    {
      wci_bitstream_skip(prBS, 32 - pos);
      bits -= 32 - pos;
    }

    if (bits > 32)
    {
      uint32_t tmp;

      prBS->tail += bits/32;

  
      
      tmp = *prBS->tail;

      #ifndef ARCH_IS_BIG_ENDIAN
        BSWAP(tmp);
      #endif

      prBS->bufa = tmp;

      tmp = *(prBS->tail+1);

      #ifndef ARCH_IS_BIG_ENDIAN
        BSWAP(tmp);
      #endif

      prBS->bufb = tmp;

      if(bits%32)
        wci_bitstream_skip(prBS, bits%32);
    }
    else
      wci_bitstream_skip(prBS, bits);
  }
}

//------------------------------------------------------------------------------

static uint32_t __inline wci_bitstream_get_bits
(
  BITSTREAM * const prBS
, const uint32_t    nBits)
{
// This function pads bitstream to the next byte boundary.

  uint32_t ret = wci_bitstream_show_bits(prBS, nBits);
  wci_bitstream_skip_bits(prBS, nBits);
  return ret;
}

//------------------------------------------------------------------------------

static uint32_t __inline wci_bitstream_get_bit(BITSTREAM * const prBS)
{
// This function pads bitstream to the next byte boundary.
#if 1 
  int res;
  int nbit = (1 + prBS->pos) - 32;
  if (nbit > 0) 
  {
    if(prBS->bufb & (0x80000000 >> nbit))
      res = 1;
    else 
      res = 0;
  }
  else
  {
    if(prBS->bufa & (0x80000000 >> prBS->pos))
      res = 1;
    else 
      res = 0;
  }
  prBS->pos += 1;
#ifdef BITSTREAM_OPTIMIZE
  if(prBS->pos >=32)
  {
    prBS->tail +=1;
    if (prBS->tail >= (prBS->bsend-1))
    {
      register uint* pTail = prBS->tail;
      prBS->bufa = ( (uint)((BYTE *)(pTail))[3] | (((uint)((BYTE *)(pTail))[2])<<8) | \
        ( ((uint)((BYTE *)(pTail))[1])<<16) | ( ((uint)((BYTE *)(pTail))[0])<<24) );
      prBS->bufb = 0;
    }
    else
    {
      
      register uint* pTail = prBS->tail;
      prBS->bufa = ( (uint)((BYTE *)(pTail))[3] | (((uint)((BYTE *)(pTail))[2])<<8) | \
        ( ((uint)((BYTE *)(pTail))[1])<<16) | ( ((uint)((BYTE *)(pTail))[0])<<24) );
      pTail++;
      prBS->bufb = ( (uint)((BYTE *)(pTail))[3] | (((uint)((BYTE *)(pTail))[2])<<8) | \
        ( ((uint)((BYTE *)(pTail))[1])<<16) | ( ((uint)((BYTE *)(pTail))[0])<<24) );
    }
    prBS->pos = prBS->pos - 32;
  }
#else
  if(prBS->pos >=32)
  {
    uint32_t tmp;
    prBS->tail +=1;
    if (prBS->tail >= (prBS->bsend-1))
    {
      tmp = GET_WORD32(prBS->tail);
      BSWAP1(tmp);
      prBS->bufa = tmp;
      prBS->bufb = 0;
    }
    else
    {
      tmp = GET_WORD32(prBS->tail);
      BSWAP1(tmp);
      prBS->bufa = tmp;
      tmp = GET_WORD32(prBS->tail + 1);
      BSWAP1(tmp);
      prBS->bufb = tmp;
    }
    prBS->pos = prBS->pos - 32;
  }
#endif
  return  res; 
#else
  return wci_bitstream_get_bits(prBS, 1);
#endif
}


//------------------------------------------------------------------------------

static __inline uint32_t wci_getbits32(BITSTREAM* prBS)
{
  register uint32_t res;
  if (prBS->pos > 0) 
  {
    res = (prBS->bufa << prBS->pos) | (prBS->bufb >> (32 - prBS->pos));
  }
  else 
  {
    res =  prBS->bufa;
  }
  {
#ifdef BITSTREAM_OPTIMIZE
    prBS->tail +=1;
    if (prBS->tail >= (prBS->bsend-1))
    {
      prBS->bufa = ( (uint)((BYTE *)(prBS->tail))[3] | (((uint)((BYTE *)(prBS->tail))[2])<<8) | \
        ( ((uint)((BYTE *)(prBS->tail))[1])<<16) | ( ((uint)((BYTE *)(prBS->tail))[0])<<24) );
      prBS->bufb = 0;
    }
    else
    {
      register uint* pTail = prBS->tail;
      prBS->bufa = ( (uint)((BYTE *)(pTail))[3] | (((uint)((BYTE *)(pTail))[2])<<8) | \
        ( ((uint)((BYTE *)(pTail))[1])<<16) | ( ((uint)((BYTE *)(pTail))[0])<<24) );
      pTail++;
      prBS->bufb = ( (uint)((BYTE *)(pTail))[3] | (((uint)((BYTE *)(pTail))[2])<<8) | \
        ( ((uint)((BYTE *)(pTail))[1])<<16) | ( ((uint)((BYTE *)(pTail))[0])<<24) );
    }
#else
    uint32_t tmp;
    prBS->tail +=1;
    if (prBS->tail >= (prBS->bsend-1))
    {
      tmp = GET_WORD32(prBS->tail);
      BSWAP1(tmp);
      prBS->bufa = tmp;
      prBS->bufb = 0;
    }
    else
    {
      tmp = GET_WORD32(prBS->tail);
      BSWAP1(tmp);
      prBS->bufa = tmp;
      tmp = GET_WORD32(prBS->tail + 1);
      BSWAP1(tmp);
      prBS->bufb = tmp;
    }

#endif

  }
  return res;
}
//------------------------------------------------------------------------------

static __inline void wci_bitstream_add_buffer
(
  BITSTREAM *    prBS
, const uint8_t* pbSource
, const int      iLength )    // [byte]
{
  uint8_t* pbtBuffer = wci_bitstream_lock_buffer(prBS);
  memcpy( pbtBuffer, pbSource, iLength );
  wci_bitstream_unlock_buffer(prBS, pbtBuffer, iLength);
}

//------------------------------------------------------------------------------

static __inline void wci_bitstream_get_buffer
(
  BITSTREAM * prBS,
  uint8_t *   pbtDest,
  const int   iLength )    // [byte]
{
  uint8_t* pbtBuffer = (BYTE*)prBS->tail + prBS->pos/8;
  memcpy( pbtDest, pbtBuffer, iLength );
  wci_bitstream_skip_bits( prBS, iLength*8 );
}

//------------------------------------------------------------------------------

static __inline uint8_t * wci_bitstream_get_pos
(
  BITSTREAM * const prBS
)
{
  return (uint8_t*)prBS->start + wci_bitstream_pos(prBS);
}

//------------------------------------------------------------------------------

static __inline void wci_bitstream_flush
(
  BITSTREAM * prBS
)
{
  wci_bitstream_pad( prBS );
  if (prBS->pos) {
    uint32_t i, b = prBS->buf;
    BYTE * p= (BYTE *)prBS->tail;

#ifndef ARCH_IS_BIG_ENDIAN
    BSWAP(b);
#endif
    for(i=0; i<prBS->pos/8;  i++)
      *p++= (BYTE)( (b >> (i*8)) & 0xFF );
    if(prBS->pos == 32)
    {
       prBS->pos = 0;
       prBS->buf = 0;
       prBS->tail++;
    }
  }
 }

#endif // #ifndef BITSTREAM_H

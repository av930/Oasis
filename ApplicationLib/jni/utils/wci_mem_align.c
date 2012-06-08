/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: mem_align.c

ABSTRACT:    This file contains procedures of aligned memory manager

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "wci_mem_align.h"

#define SIZE_OF_LEN 4
//#define NO_MEM_ALIGNMENT

//------------------------------------------------------------------------------

//! Internal heap memory manager
static void * wci_heap_aligned_malloc(
const size_t        IN     nSize        //! Size of allocated memory
, const BYTE          IN     btAlignment  //! Alignment of address [byte]
, ALIGNED_MEMMANAGER* IN OUT prHeap       //! Structure of heap manager
)
{
  BYTE * ptr;
  ptr_t overhead;
  uint32_t len;

  if( btAlignment > 1)
  {
    overhead= (ptr_t) (prHeap->btFree + SIZE_OF_LEN) & ((ptr_t)(btAlignment - 1));

    if(!overhead)
    { // we already got aligned address 
      ptr= prHeap->btFree + SIZE_OF_LEN ;
      len= (uint32_t)(SIZE_OF_LEN  + nSize);   // calculate actual allocated len 
    }
    else
    {
      ptr =(uint8_t *) ((ptr_t) (prHeap->btFree + SIZE_OF_LEN + btAlignment) & (~(ptr_t) (btAlignment - 1)));
      len= (uint32_t) ((ptr_t)(ptr - prHeap->btFree + nSize));   // calculate actual allocated len
    }
  }
  else
  {
      ptr= prHeap->btFree + SIZE_OF_LEN ;
      len= (uint32_t)(SIZE_OF_LEN  + nSize);   // calculate actual allocated len 
  }
  assert((prHeap->nUsedMem + len) <= prHeap->nHeapSize);
  if( (prHeap->nUsedMem + len) > prHeap->nHeapSize)
       return NULL; // there is no enough space in heap 

  *(ptr-1)= (BYTE)(len&0xFF); // store len in alloc header 
  *(ptr-2)= (BYTE)(len>>8);
  *(ptr-3)= (BYTE)(len>>16);
  *(ptr-4)= (BYTE)(len>>24);

  prHeap->nUsedMem+= len;
  if(prHeap->nUsedMem > prHeap->nMaxUsedMem)
    prHeap->nMaxUsedMem = prHeap->nUsedMem; 
  prHeap->btFree+=   len;
  prHeap->uNumAlloc++;
  return (void *)ptr;
}

//------------------------------------------------------------------------------

static void wci_heap_aligned_free(
  const void *        IN     pMem         //! Address of aligned allocated memory
, ALIGNED_MEMMANAGER* IN OUT prHeap       //! Structure of heap manager
)
{
  BYTE * ptr= (BYTE *)pMem;
  uint32_t len;
  len = *(ptr-1);
  len|= *(ptr-2) << 8;
  len|= *(ptr-3) << 16;
  len|= *(ptr-4) << 24;

  assert(prHeap->nUsedMem>=len);
  assert(prHeap->uNumAlloc>0);

  prHeap->nUsedMem-= len;
  prHeap->uNumAlloc--;

  if( !prHeap->nUsedMem && !(prHeap->uNumAlloc)) // all allocations are freed 
    prHeap->btFree= prHeap->btHeapBuf;           // move free pointer at the beginning 
}

//------------------------------------------------------------------------------

//! Initializes prHeap struct with heap buffer parameters before using wci_aligned_malloc()/wci_aligned_free()
//! NOT needed if allocation in system memory is used
int wci_aligned_memmanager_init(
  const BYTE *        IN     btBuf        //! Heap buffer pointer
, const size_t        IN     nSize        //! Size heap buffer [bytes]
, ALIGNED_MEMMANAGER* IN OUT prHeap       //! Structure of heap manager
)
{
  if( (btBuf==NULL) || (!nSize) || (prHeap==NULL))
    return (-1);

  prHeap->btHeapBuf= (BYTE *)btBuf;
  prHeap->btFree= prHeap->btHeapBuf;
  prHeap->nHeapSize= nSize;
  prHeap->nUsedMem= 0;
  prHeap->nMaxUsedMem= 0;
  prHeap->uNumAlloc= 0;
  return 0;
}

//! Returns size of free memory (in bytes)
size_t wci_aligned_get_freemem(
  ALIGNED_MEMMANAGER* IN prHeap  //! Structure of heap manager
)
{
  if(prHeap)
    return (prHeap->nHeapSize - prHeap->nUsedMem);
  else
    return 0;
}

//------------------------------------------------------------------------------

//! Allocate memory and returns aligned address of allocated memory if successfull or NULL otherwise.
//! If need allocation in system memory than set prHeap to NULL
void * wci_aligned_malloc(
  const size_t        IN     nSize        //! Size of allocated memory
, const BYTE          IN     btAlignment  //! Alignment of address [byte]
, ALIGNED_MEMMANAGER* IN OUT prHeap       //! Structure of heap manager
)
{
#ifdef NO_MEM_ALIGNMENT
  return malloc(nSize);
#else
  if (prHeap != NULL)
    return wci_heap_aligned_malloc(nSize, btAlignment, prHeap); /* allocate memory heap memory */
  else
  { // allocate memory in system memory 
    BYTE *mem_ptr;
  #ifdef ENCODER_SUPPORT
    if ( btAlignment <= 1)
    {  // We have not to satisfy any alignment 
      if ((mem_ptr = (uint8_t *) malloc(nSize + 1)) != NULL)
      {
         *mem_ptr = 1; // Store (mem_ptr - "real allocated memory") in *(mem_ptr-1) 
         return (void *)(mem_ptr+1); // Return the mem_ptr pointer 
      }
    }
    else
    {
      BYTE *tmp;
     // Allocate the required size memory + alignment so we
     // can realign the data if necessary
      if ((tmp = (uint8_t *) malloc(nSize + btAlignment)) != NULL)
      {
        // Align the tmp pointer 
        mem_ptr =(uint8_t *) ((ptr_t) (tmp + btAlignment - 1) & (~(ptr_t) (btAlignment - 1)));
        // Special case where malloc have already satisfied the alignment
        // We must add alignment to mem_ptr because we must store
        // (mem_ptr - tmp) in *(mem_ptr-1)
        // If we do not add alignment to mem_ptr then *(mem_ptr-1) points
        // to a forbidden memory space
        if (mem_ptr == tmp)
         mem_ptr += btAlignment;

        // (mem_ptr - tmp) is stored in *(mem_ptr-1) so we are able to retrieve
        // the real malloc block allocated and free it in xvid_free
        *(mem_ptr - 1) = (uint8_t) (mem_ptr - tmp);
        return (void *) mem_ptr; /* Return the aligned pointer */
      }
    }
  #else
    assert(prHeap != NULL);
  #endif
    return NULL;
  }
#endif //ifdef NO_MEM_ALIGNMENT
}

//------------------------------------------------------------------------------

// free aligned allocated memory
//! If memory was allocated in system memory than set prHeap to NULL
void wci_aligned_free(
  const void *        IN     pMem         //! Address of aligned allocated memory
, ALIGNED_MEMMANAGER* IN OUT prHeap       //! Structure of heap manager
)
{
#ifdef NO_MEM_ALIGNMENT
  free(pMem);
#else
  if(pMem == NULL)
    return;
  if( prHeap != NULL)
    wci_heap_aligned_free(pMem, prHeap); // free memory in heap memory
  else
  {
  #ifdef ENCODER_SUPPORT
     // free memory in system memory 
     //   free((BYTE *) pMem - *((BYTE *) pMem - 1));
	   void* pM = ((BYTE *) pMem - *((BYTE *) pMem - 1));
  	 free(pM);
  #else
    assert(prHeap != NULL);
  #endif
  }
#endif //ifdef NO_MEM_ALIGNMENT
}

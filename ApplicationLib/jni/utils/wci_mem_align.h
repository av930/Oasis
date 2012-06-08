/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_mem_align.h

ABSTRACT:    This file contains definitions and data structures of aligned memory manager

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef _MEM_ALIGN_H_
#define _MEM_ALIGN_H_

#include <string.h>

#include "../wci_portab.h"

//------------------------------------------------------------------------------

typedef struct tagALIGNED_MEMMANAGER
{
  BYTE *        IN     btHeapBuf;    //! Heap buffer pointer
  size_t        IN     nHeapSize;    //! Size heap buffer [bytes]
  size_t           OUT nUsedMem;     //! Used (allocated) mem [bytes]
  size_t           OUT nMaxUsedMem;  //! Latch of Max Used (allocated) mem [bytes]
  BYTE *           OUT btFree;       //! Pointer to next free byte in heap
  uint32_t         OUT uNumAlloc;    //! Number of current allocations
} ALIGNED_MEMMANAGER;

//------------------------------------------------------------------------------

//! Initializes prHeap struct with heap buffer parameters before using wci_aligned_malloc()/wci_aligned_free()
//! NOT needed if allocation in system memory is used
int wci_aligned_memmanager_init(
  const BYTE *        IN     btBuf   //! Heap buffer pointer
, const size_t        IN     nSize   //! Size heap buffer [bytes]
, ALIGNED_MEMMANAGER* IN OUT prHeap  //! Structure of heap manager
);

//------------------------------------------------------------------------------

//! Allocate memory and returns aligned address of allocated memory if successfull or NULL otherwise.
//! If need allocation in system memory than set prHeap to NULL
//! Alignment should be multiple of power of two (1,2,4,8,..., 128)
void * wci_aligned_malloc(
  const size_t        IN     nSize        //! Size of allocated memory
, const BYTE          IN     btAlignment  //! Alignment of address [byte]
, ALIGNED_MEMMANAGER* IN OUT prHeap       //! Structure of heap manager
);

//------------------------------------------------------------------------------

//! free aligned allocated memory
//! If memory was allocated in system memory than set prHeap to NULL
void wci_aligned_free(
  const void *        IN     pMem   //! Address of aligned allocated memory
, ALIGNED_MEMMANAGER* IN OUT prHeap //! Structure of heap manager
);

//------------------------------------------------------------------------------

//! Returns max value of used memory during the session (since wci_aligned_memmanager_init())
size_t wci_aligned_get_maxusedmem(
  ALIGNED_MEMMANAGER* IN prHeap  //! Structure of heap manager
);

//------------------------------------------------------------------------------

//! Returns size of free memory (in bytes)
size_t wci_aligned_get_freemem(
  ALIGNED_MEMMANAGER* IN prHeap  //! Structure of heap manager
);

#endif // _MEM_ALIGN_H_ 

/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_frame.h

ABSTRACT:    This file contains global definitions and data structures

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

#include "wci_portab.h"
#include "wci_codec.h"

//------------------------------------------------------------------------------

#define MB_SIZE    16            //! This should not be changed

#define SWAP(A,B)                { void * tmp = A; A = B; B = tmp; }

#define CELLS(x, cell_size)      ( ( (x) + (cell_size) - 1 ) / (cell_size) )

#define MIN(X, Y)                ((X)<(Y)?(X):(Y))
#define MAX(X, Y)                ((X)>(Y)?(X):(Y))
#define SIGN(X)                  (((X)>0)?1:-1)

#define BOUND(x, min, max)       ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define CHECK_BOUND(x, min, max) (((x) >= min) && ((x) <= max))

#define ALLOC_CHECK(p)           if((p) == NULL) return ERR_MEMORY;

#define PERCENT(value, percent)  ((percent*value)/100)

#define CSP_IS_THERE_ALPHA(csp)  ((csp & (CSP_YV24A | CSP_YV12A | CSP_BGRA)) != 0)

//------------------------------------------------------------------------------

typedef BYTE PEL;

typedef struct
{
  int x;
  int y;

} VECTOR;

//------------------------------------------------------------------------------

static int __inline wci_get_square( SIZE rSize )
{
  return rSize.cx*rSize.cy;
}

//------------------------------------------------------------------------------

static SIZE __inline wci_rect_get_size( RECT rRect )
{
  SIZE rSize;

  rSize.cx = rRect.right  - rRect.left;
  rSize.cy = rRect.bottom - rRect.top;

  return rSize;
}

//------------------------------------------------------------------------------

static int __inline wci_rect_is_crossed( RECT rRect1, RECT rRect2 )
{
  if( rRect1.left > rRect2.right )
    return 0;
  if( rRect1.top > rRect2.bottom )
    return 0;
  if( rRect1.right < rRect2.left )
    return 0;
  if( rRect1.bottom < rRect2.top )
    return 0;
  return 1;
}

//------------------------------------------------------------------------------

static RECT __inline wci_rect_get_intersection( RECT rRect1, RECT rRect2 )
{
  RECT rIntersection;

  rIntersection.left   = max( rRect1.left  , rRect2.left   );
  rIntersection.right  = min( rRect1.right , rRect2.right  );
  rIntersection.top    = max( rRect1.top   , rRect2.top    );
  rIntersection.bottom = min( rRect1.bottom, rRect2.bottom );

  return rIntersection;
}

//------------------------------------------------------------------------------

static __inline int32_t wci_get_component_number(COLOR_SPACE eColorSpace)
{
  int32_t iComponents = 0;
  switch(eColorSpace) 
  {
  case CSP_GRAYSCALE:
    iComponents = 1;
    break;
  case CSP_YV12A:
  case CSP_YV24A:
  case CSP_BGRA:
    iComponents = 4;
    break;
  default:
    iComponents = 3;
  }
  return iComponents;
}

#endif // #ifndef GLOBAL_H

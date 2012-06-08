/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_qsp.h

ABSTRACT:    This file contains definitions and data structures for compression of QSP buffers

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef QSP_H
#define QSP_H

#include "../wci_portab.h"
#include "../utils/wci_mem_align.h"

//------------------------------------------------------------------------------

typedef struct 
{
  BYTE *pbtQ;              //! Buffer of positive quanted values
  BYTE *pbtS;              //! Sign buffer of quanted values
  BYTE *pbtP;              //! Partion decision buffer

  int  iQ_Pos;             //! position in Q-buffer
  int  iS_Pos;             //! position in S-buffer
  int  iP_Pos;             //! position in P-buffer

  int  iQ_Length;          //! Q-buffer length
  int  iS_Length;          //! S-buffer length
  int  iP_Length;          //! P-buffer length

} QSP_BUFFER;

//------------------------------------------------------------------------------

int wci_qsp_init_buffer
(
  int                  IN     iComponentSize
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
, QSP_BUFFER *            OUT prQSP
);

void wci_qsp_done_buffer(
  QSP_BUFFER *         IN OUT prQSP
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
);

//------------------------------------------------------------------------------

static void __inline
wci_qsp_duplicate(
  const QSP_BUFFER * IN  prSrc //! Source
, QSP_BUFFER *       OUT prDst //! Destination
)
{
  prDst->pbtQ = prSrc->pbtQ;
  prDst->pbtS = prSrc->pbtS;
  prDst->pbtP = prSrc->pbtP;
  prDst->iQ_Pos = prDst->iS_Pos = prDst->iP_Pos = 0;
  prDst->iQ_Length = prDst->iS_Length = prDst->iP_Length = 0;
}

//------------------------------------------------------------------------------

static void __inline
wci_qsp_copy_pointers(
  const QSP_BUFFER * IN  prSrc //! Source
, QSP_BUFFER *       OUT prDst //! Destination
)
{
  prDst->pbtQ = prSrc->pbtQ;
  prDst->pbtS = prSrc->pbtS;
  prDst->pbtP = prSrc->pbtP;
}

//------------------------------------------------------------------------------

static void __inline
wci_qsp_reset_lengths( QSP_BUFFER *prQSP )
{
  prQSP->iQ_Length = prQSP->iS_Length = prQSP->iP_Length = 0;
}

//------------------------------------------------------------------------------

static void __inline
wci_qsp_resetpos( QSP_BUFFER *prQSP )
{
  prQSP->iQ_Pos = prQSP->iS_Pos = prQSP->iP_Pos = 0;
}

//------------------------------------------------------------------------------

static void __inline
wci_qsp_offset_pointers(
  QSP_BUFFER *prQSP
, const int   iOffset
)
{
  prQSP->pbtQ += iOffset;
  prQSP->pbtS += iOffset;
  prQSP->pbtP += iOffset;
}

#endif // #ifndef QSP_H


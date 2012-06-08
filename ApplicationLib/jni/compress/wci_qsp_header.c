/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: QSP_header.c

ABSTRACT:    This file contains procedures for processing of QSP header

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../wci_portab.h"
#include "../wci_codec.h"
#include "wci_qsp_header.h"
#include "wci_qsp_compress.h"

//------------------------------------------------------------------------------

// Block packing info header defines
#define QSP_P_PACK_16       0
#define QSP_P_PACK_24       1
#define QSP_P_NUM_PACK_1    0
#define QSP_P_NUM_PACK_2    1<<1
#define QSP_Q_PACK_16       0
#define QSP_Q_PACK_24       1<<2
#define QSP_Q_NUM_PACK_1    0
#define QSP_Q_NUM_PACK_2    1<<3
#define QSP_S_PACK_16       0
#define QSP_S_PACK_24       1<<4
#define QSP_S_NUM_PACK_1    0
#define QSP_S_NUM_PACK_2    1<<5

//------------------------------------------------------------------------------

//#define _WRITE_QSP_COMPR_INFO_  

#ifdef _WRITE_QSP_COMPR_INFO_
void wci_qsp_print_statistic(QSP_COMPRESS_HEADER * IN prHeader)
{
  FILE * pFile = fopen("purestat.txt","w");
  FILE * pFileStat = fopen("complstat.txt","w");
  int i,  iTotalCompr = 0;
  int  miCompFreq[] = {0,0,0}, miQSPFreq[] = {0,0,0};
  int *pmiCompFreq = miCompFreq;
  

  if(!pFile||!pFileStat)return;

  fprintf(pFile, "\n\n N   P-LH   P-HL   P-HH   C1P1   C2P1   C1P2   C2P2   C1P3   C2P3   Q      C1Q    C2Q    S       C1S    C2S\n");

  for ( i=0; i<prHeader->iBlocks; ++i )
  {
    int k, iTotal = 0 ;
      fprintf(pFile, "\n%2d  ",i + 1) ;

  //  Print P statistics
  
    if(prHeader->rBlockSize[i].aiP[1]) //LH, HL, HH
    {
      fprintf(pFile, "%5d  %5d  %5d  ",\
      prHeader->rBlockSize[i].aiP[0],prHeader->rBlockSize[i].aiP[1],prHeader->rBlockSize[i].aiP[2]);
      for(k=0; k< MAX_PARTITION_COMPONENTS; k++)
      {
        fprintf(pFile, "%5d  %5d  ",\
        prHeader->rCompressedBlockSize[i].aiP[k][COMPRESSION_STEP1], prHeader->rCompressedBlockSize[i].aiP[k][COMPRESSION_STEP2]);
        {
          int iCmprBlS;
          iCmprBlS = (!prHeader->rCompressedBlockSize[i].aiP[k][COMPRESSION_STEP2])?prHeader->rCompressedBlockSize[i].aiP[k][COMPRESSION_STEP1]:\
            prHeader->rCompressedBlockSize[i].aiP[k][COMPRESSION_STEP2];
          iTotalCompr += iCmprBlS;
          *pmiCompFreq+=iCmprBlS;
          miQSPFreq[0]+=iCmprBlS;
  
        }
      }
    }
    else   //LL
    {
      fprintf(pFile, "%5d  ",  prHeader->rBlockSize[i].aiP[0]);
      fprintf(pFile, "%5d  %5d  ",\
        prHeader->rCompressedBlockSize[i].aiP[0][COMPRESSION_STEP1], prHeader->rCompressedBlockSize[i].aiP[0][COMPRESSION_STEP2]);
      {
        int iCmprBlS;
        iCmprBlS = (!prHeader->rCompressedBlockSize[i].aiP[0][COMPRESSION_STEP2])?prHeader->rCompressedBlockSize[i].aiP[0][COMPRESSION_STEP1]:\
          prHeader->rCompressedBlockSize[i].aiP[0][COMPRESSION_STEP2];
        iTotalCompr += iCmprBlS;
        *pmiCompFreq+=iCmprBlS;
        miQSPFreq[0]+=iCmprBlS;

      }
    }

  //  Print Q statistics

    fprintf(pFile, "%5d  ", prHeader->rBlockSize[i].iQ);

    fprintf(pFile, "%5d  %5d  ",\
      prHeader->rCompressedBlockSize[i].iQ[COMPRESSION_STEP1], prHeader->rCompressedBlockSize[i].iQ[COMPRESSION_STEP2]);

    {
      int iCmprBlS;
      iCmprBlS = (!prHeader->rCompressedBlockSize[i].iQ[COMPRESSION_STEP2])?prHeader->rCompressedBlockSize[i].iQ[COMPRESSION_STEP1]:\
      prHeader->rCompressedBlockSize[i].iQ[COMPRESSION_STEP2];
      iTotalCompr += iCmprBlS;
      *pmiCompFreq+=iCmprBlS;
      miQSPFreq[1]+=iCmprBlS;
    }


  //  Print S statistics

    fprintf(pFile, "%5d  ",  prHeader->rBlockSize[i].iS);
    fprintf(pFile, "%5d  %5d\n",\
      prHeader->rCompressedBlockSize[i].iS[COMPRESSION_STEP1], prHeader->rCompressedBlockSize[i].iS[COMPRESSION_STEP2]);
      
    {
      int iCmprBlS;
      iCmprBlS = (!prHeader->rCompressedBlockSize[i].iS[COMPRESSION_STEP2])?prHeader->rCompressedBlockSize[i].iS[COMPRESSION_STEP1]:\
      prHeader->rCompressedBlockSize[i].iS[COMPRESSION_STEP2];
      iTotalCompr += iCmprBlS;
      *pmiCompFreq+=iCmprBlS;
      miQSPFreq[2]+=iCmprBlS;
    }
    
    if(!prHeader->rBlockSize[i].aiP[1]&&i<(prHeader->iBlocks - 1))
      pmiCompFreq++;

  }

  if(iTotalCompr)
  {
    fprintf(pFileStat, "\nY=%6d U=%6d V=%6d Total=%6d \n",miCompFreq[0],miCompFreq[1],miCompFreq[2],iTotalCompr);
    fprintf(pFileStat, "\Y=%.2f U=%.2f V=%.2f\n",100* (double)miCompFreq[0]/iTotalCompr,100* (double)miCompFreq[1]/iTotalCompr,100* (double)miCompFreq[2]/iTotalCompr);

    fprintf(pFileStat, "\nP=%6d Q=%6d S=%6d  Total=%6d\n",miQSPFreq[0],miQSPFreq[1],miQSPFreq[2], iTotalCompr);
    fprintf(pFileStat, "\P=%.2f Q=%.2f S=%.2f\n",100*(double) miQSPFreq[0]/iTotalCompr,100* (double)miQSPFreq[1]/iTotalCompr,100* (double)miQSPFreq[2]/iTotalCompr);
  }
 
  fclose(pFile);
  fclose(pFileStat);

}
#endif

//------------------------------------------------------------------------------

void wci_qsp_put_header(
  QSP_COMPRESS_HEADER * IN OUT prHeader //! The pointer to header structure
, BITSTREAM *           IN OUT prBS     //! The output bitstream
)
{
// This function puts QSP Header to stream.

  wci_bitstream_put_bits(prBS, prHeader->iCompressedLength, 32);

  prHeader->iQSP_Length = prHeader->rTotalSize.iQ
                        + prHeader->rTotalSize.iS
                        + prHeader->rTotalSize.aiP[0];

  wci_bitstream_put_bits(prBS, prHeader->iQSP_Length  , 32);

  if ( (prHeader->iCompressMode & QSP_BLOCK_PACK_FLAG) && prHeader->iBlocks > 0 )
  { // Block packing mode
    int i; 
    int iNumLens_P=1, iLenSize_P=16;
    int iNumLens_Q=1, iLenSize_Q=16;
    int iNumLens_S=1, iLenSize_S=16;
    int  nFlags;

    wci_bitstream_put_bits(prBS, prHeader->iCompressMode, 16);

    wci_bitstream_put_bits(prBS, prHeader->iBlocks, 8);
    // Prepare Blocks Packing info hdr (Determine max size of len's and num of lens)
    for ( i=0; i<prHeader->iBlocks; ++i )
    {
      int k;
      // P component
      nFlags= QSP_P_FLAG( prHeader->iCompressMode);
      iNumLens_P= (nFlags == QSP_CMP_RLE_VLC) || (nFlags == QSP_CMP_RLE_HUFFMAN) ? 2 : 1;
      for(k=0; k< MAX_PARTITION_COMPONENTS; k++)
      {
        if( prHeader->rBlockSize[i].aiP[k] > USHRT_MAX)
           iLenSize_P = 24; 
      }
      // Q component
//     if( prHeader->rBlockSize[i].iQ > USHRT_MAX)
           iLenSize_Q = 24; 

      nFlags= QSP_Q_FLAG( prHeader->iCompressMode);
      iNumLens_Q= (nFlags == QSP_CMP_RLE_VLC) || (nFlags == QSP_CMP_RLE_HUFFMAN) ? 2 : 1;
      // S component
      if( prHeader->rBlockSize[i].iS > USHRT_MAX)
          iLenSize_S = 24; 
      nFlags= QSP_S_FLAG( prHeader->iCompressMode);
      iNumLens_S= (nFlags == QSP_CMP_RLE_VLC) || (nFlags == QSP_CMP_RLE_HUFFMAN) ? 2 : 1;
    }
    // Pack Blocks Packing info hdr
    {
      BYTE hdr;
      hdr= (iLenSize_P == 16)  ? QSP_P_PACK_16 : QSP_P_PACK_24;
      hdr |= (iNumLens_P == 1) ? QSP_P_NUM_PACK_1 : QSP_P_NUM_PACK_2;
      hdr |= (iLenSize_Q == 16)? QSP_Q_PACK_16 : QSP_Q_PACK_24;
      hdr |= (iNumLens_Q == 1) ? QSP_Q_NUM_PACK_1 : QSP_Q_NUM_PACK_2;
      hdr |= (iLenSize_S == 16)? QSP_S_PACK_16 : QSP_S_PACK_24;
      hdr |= (iNumLens_S == 1) ? QSP_S_NUM_PACK_1 : QSP_S_NUM_PACK_2;
      wci_bitstream_put_bits(prBS, hdr, 8);
    }

    // Pack block lengths
    for ( i=0; i<prHeader->iBlocks; ++i )
    {
      int k;
      for(k=0; k< MAX_PARTITION_COMPONENTS; k++)
      {
        assert( prHeader->rBlockSize[i].aiP[k] < (1 << 23) );
        wci_bitstream_put_bits(prBS, prHeader->rBlockSize[i].aiP[k], iLenSize_P);
        if( prHeader->rBlockSize[i].aiP[k])
        {
          assert( prHeader->rCompressedBlockSize[i].aiP[k][COMPRESSION_STEP1] < (1 << 23) );   
          wci_bitstream_put_bits(prBS, prHeader->rCompressedBlockSize[i].aiP[k][COMPRESSION_STEP1], iLenSize_P);
          if( iNumLens_P == 2 )
          {
            assert( prHeader->rCompressedBlockSize[i].aiP[k][COMPRESSION_STEP2] < (1 << 23) );   
            wci_bitstream_put_bits(prBS, prHeader->rCompressedBlockSize[i].aiP[k][COMPRESSION_STEP2], iLenSize_P);
          }
        }
      }

      assert( prHeader->rBlockSize[i].iQ < (1 << 23) );
      wci_bitstream_put_bits(prBS, prHeader->rBlockSize[i].iQ, iLenSize_Q);
      if ( prHeader->rBlockSize[i].iQ )
      {
        assert( prHeader->rCompressedBlockSize[i].iQ[COMPRESSION_STEP1] < (1 << 23) );
        wci_bitstream_put_bits(prBS, prHeader->rCompressedBlockSize[i].iQ[COMPRESSION_STEP1], iLenSize_Q);
        if( iNumLens_Q == 2 )
        {
          assert( prHeader->rCompressedBlockSize[i].iQ[COMPRESSION_STEP2] < (1 << 23) );
          wci_bitstream_put_bits(prBS, prHeader->rCompressedBlockSize[i].iQ[COMPRESSION_STEP2], iLenSize_Q);
        }
      }

      assert( prHeader->rBlockSize[i].iS < (1 << 23) );
      wci_bitstream_put_bits(prBS, prHeader->rBlockSize[i].iS, iLenSize_S);
      if ( prHeader->rBlockSize[i].iS )
      {
        assert( prHeader->rCompressedBlockSize[i].iS[COMPRESSION_STEP1] < (1 << 23) );
        wci_bitstream_put_bits(prBS, prHeader->rCompressedBlockSize[i].iS[COMPRESSION_STEP1], iLenSize_S);
        if( iNumLens_S == 2 )
        {
          assert( prHeader->rCompressedBlockSize[i].iS[COMPRESSION_STEP2] < (1 << 23) );
          wci_bitstream_put_bits(prBS, prHeader->rCompressedBlockSize[i].iS[COMPRESSION_STEP2], iLenSize_S);
        }
      }
    }
  }
  else
  {
    wci_bitstream_put_bits(prBS, prHeader->iCompressMode, 16);

    wci_bitstream_put_bits(prBS, prHeader->rTotalSize.aiP[0], 32);
    wci_bitstream_put_bits(prBS, prHeader->rTotalSize.iQ, 32);
    wci_bitstream_put_bits(prBS, prHeader->rTotalSize.iS, 32);
  }

  if (prHeader->iCompressMode & QSP_CRC)
    wci_bitstream_put_bits(prBS, prHeader->iCRC, 32);

#ifdef _WRITE_QSP_COMPR_INFO_
wci_qsp_print_statistic(prHeader);
#endif
}

//------------------------------------------------------------------------------

int wci_qsp_get_header
(
  BITSTREAM          *  IN OUT prBS     //! The data bitstream
, QSP_COMPRESS_HEADER * IN OUT prHeader //! The pointer to header structure
)
{
// This function reads QSP Header from the stream

  QSP_SIZE rSize;
  prHeader->iCompressedLength = wci_bitstream_get_bits(prBS, 32);

  prHeader->iQSP_Length = wci_bitstream_get_bits(prBS, 32);
  prHeader->iCompressMode = wci_bitstream_get_bits(prBS, 16);

  prHeader->iBlocks = 0;
  if ( prHeader->iCompressMode & QSP_BLOCK_PACK_FLAG )
  {
    int      i;
    QSP_SIZE rSizeSum;
    uint nBlockPackInfo;

    memset( &rSizeSum, 0, sizeof(QSP_SIZE) );
    prHeader->iBlocks = wci_bitstream_get_bits(prBS, 8);
    
    nBlockPackInfo= wci_bitstream_get_bits(prBS, 8);

    for ( i=0; i<prHeader->iBlocks; ++i )
    {
      int k, iBlockPackedLen;

      iBlockPackedLen= (nBlockPackInfo & QSP_P_PACK_24) ? 24 : 16;
      for(k=0; k< MAX_PARTITION_COMPONENTS; k++)
      {
        prHeader->rBlockSize[i].aiP[k] = wci_bitstream_get_bits(prBS, iBlockPackedLen);
        if( prHeader->rBlockSize[i].aiP[k] )
        {
          prHeader->rCompressedBlockSize[i].aiP[k][COMPRESSION_STEP1] = wci_bitstream_get_bits(prBS, iBlockPackedLen);
          if ( nBlockPackInfo & QSP_P_NUM_PACK_2)
              prHeader->rCompressedBlockSize[i].aiP[k][COMPRESSION_STEP2] = wci_bitstream_get_bits(prBS, iBlockPackedLen);
          rSizeSum.aiP[0] += prHeader->rBlockSize[i].aiP[k];
        }
        else
          prHeader->rCompressedBlockSize[i].aiP[k][COMPRESSION_STEP2]= prHeader->rCompressedBlockSize[i].aiP[k][COMPRESSION_STEP1] = 0;
      }

      iBlockPackedLen= (nBlockPackInfo & QSP_Q_PACK_24) ? 24 : 16;
      prHeader->rBlockSize[i].iQ = wci_bitstream_get_bits(prBS, iBlockPackedLen);
      if( prHeader->rBlockSize[i].iQ )
      {
        prHeader->rCompressedBlockSize[i].iQ[COMPRESSION_STEP1] = wci_bitstream_get_bits(prBS, iBlockPackedLen);
        if ( nBlockPackInfo & QSP_Q_NUM_PACK_2)
          prHeader->rCompressedBlockSize[i].iQ[COMPRESSION_STEP2] = wci_bitstream_get_bits(prBS, iBlockPackedLen);
        rSizeSum.iQ += prHeader->rBlockSize[i].iQ;
      }
      else
        prHeader->rCompressedBlockSize[i].iQ[COMPRESSION_STEP2]= prHeader->rCompressedBlockSize[i].iQ[COMPRESSION_STEP1] = 0;

      iBlockPackedLen= (nBlockPackInfo & QSP_S_PACK_24) ? 24 : 16;
      prHeader->rBlockSize[i].iS = wci_bitstream_get_bits(prBS, iBlockPackedLen);
      if ( prHeader->rBlockSize[i].iS )
      {
        prHeader->rCompressedBlockSize[i].iS[COMPRESSION_STEP1] = wci_bitstream_get_bits(prBS, iBlockPackedLen);
        if ( nBlockPackInfo & QSP_S_NUM_PACK_2)
          prHeader->rCompressedBlockSize[i].iS[COMPRESSION_STEP2] = wci_bitstream_get_bits(prBS, iBlockPackedLen);
        rSizeSum.iS += prHeader->rBlockSize[i].iS;
      }
      else
        prHeader->rCompressedBlockSize[i].iS[COMPRESSION_STEP2]= prHeader->rCompressedBlockSize[i].iS[COMPRESSION_STEP1] = 0;
    }
    rSize = rSizeSum;
  }
  else
  {
    rSize.aiP[0] = wci_bitstream_get_bits(prBS, 32);
    rSize.iQ = wci_bitstream_get_bits(prBS, 32);
    rSize.iS = wci_bitstream_get_bits(prBS, 32);
  }

  if (prHeader->iCompressMode & QSP_CRC)
    prHeader->iCRC = wci_bitstream_get_bits(prBS, 32);

  if ( prHeader->iQSP_Length != ( rSize.aiP[0] + rSize.iQ + rSize.iS ) )
  {
    return ERR_FORMAT;
  }

  prHeader->rTotalSize.aiP[0] = rSize.aiP[0];
  prHeader->rTotalSize.iQ = rSize.iQ;
  prHeader->rTotalSize.iS = rSize.iS;

  return ERR_NONE;
}

//------------------------------------------------------------------------------

int wci_qsp_check_header
(
  const QSP_COMPRESS_HEADER * IN prHeader //! The pointer to header structure
, QSP_BUFFER *                IN prQSP    //! output QSP buffer
)
{
  const QSP_SIZE *prSize = &prHeader->rTotalSize;

  if ( prSize->iQ > prQSP->iQ_Length )
  {
    return ERR_MEMORY; // insufficiently allocated memory
  }
  if ( prSize->iS  > prQSP->iS_Length )
  {
    return ERR_MEMORY;
  }
  if ( prSize->aiP[0] > prQSP->iP_Length )
  {
    return ERR_MEMORY;
  }

  return ERR_NONE;
}


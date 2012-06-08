/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	제목

	Copyright(c) 2004 ~ LG Electronics Corp.

	Date		Name		Version		Description
	----------	------		----------	-----------------------------------------------------------
	2005.2.18	김기훈		v1.0		Created
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include "Wca3.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <getopt.h>
#include <sched.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <pthread.h>
#include <locale.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/sendfile.h>

#include "debug_utils.h"

CWca3::CWca3(void)
{
	m_pImage		= NULL;
	m_pEncodedData	= NULL;
	m_pCodecData	= NULL;

	m_nWidth		= 0;
	m_nHeight		= 0;
	m_nFrameNum		= 0;
	m_nCurFrame		= 0;	

	m_bLoad			= FALSE;					//Load되었는지 구분해주는 변수 
}

CWca3::~CWca3(void)
{
	if(m_pImage)
		m_env->ReleaseIntArrayElements(m_pImageIntArray, m_pImage, 0);
	if(m_pCodecData)
		delete [] m_pCodecData;
	if(m_pEncodedData)
		delete [] m_pEncodedData;
}

BOOL CWca3::Load(JNIEnv *env, char *pFileName)
{
    size_t len =0;
	int nEncodeDataSize;
	struct stat sb;

	CAPP_TRACE("CWca3 Load");
	if(m_bLoad)	{
		return FALSE;
	}

	if(pFileName == NULL ) 
		return FALSE;

	if (stat(pFileName, &sb) < 0) {
		CAPP_TRACE("File stat error");
		return FALSE;        
    }

	FILE* fd = fopen(pFileName, "r");

	if( fd == (FILE*) NULL ){
		CAPP_TRACE("File open error");
		return FALSE;
	} 
	
    nEncodeDataSize = sb.st_size;
	m_pEncodedData	= new BYTE[nEncodeDataSize + 1];
    
    if( (len=fread(m_pEncodedData,sizeof(char),nEncodeDataSize,fd)) < nEncodeDataSize){
		CAPP_TRACE("read error");
		return FALSE;
	}
	fclose(fd);	

	m_Stream.pbtData = m_pEncodedData;
	m_Stream.iLength = nEncodeDataSize;
	m_Stream.iOffset = 0;
	if( WCI3_GetStreamInfo(&m_Stream, &m_StreamInfo) != WCI3_ERR_NONE)
		return FALSE;

	// Check Stream Format
	if( !((m_StreamInfo.eFormat == WCI3_SF_STILL_IMAGE_V1) || (m_StreamInfo.eFormat == WCI3_SF_IMAGE_SEQUENCE_V1)))
		return FALSE;

//	m_pImage = new BYTE[m_StreamInfo.iMaxHeight * m_StreamInfo.iMaxWidth * 4];
    m_env = env;
	m_pImageIntArray = env->NewIntArray(m_StreamInfo.iMaxHeight * m_StreamInfo.iMaxWidth);
	m_pImage = env->GetIntArrayElements(m_pImageIntArray, 0);
	
	m_nWidth = m_StreamInfo.iMaxWidth;
	m_nHeight = m_StreamInfo.iMaxHeight;
	m_nFrameNum = m_StreamInfo.iFrames;

	m_bLoad = TRUE;

	return TRUE;
}

void CWca3::Decode(int nFrame)
{
	CAPP_TRACE("CWca3 Decode");	

	if(!m_bLoad)
		return ;
		
	// Find needed frame in the stream
	m_Stream.iOffset = 0;
	m_nFrameNum = m_StreamInfo.iFrames;  
	for (int i=0; i<m_nFrameNum; i++) 
	{
		m_Stream.iOffset = WCI3_FindFrame( &m_Stream, &m_FrmInfo );
		if(m_Stream.iOffset < 0)
			break;	// found no frames
		
		if( i == nFrame)
		{	// frame found
			if( nFrame > 1) // there is sequence of frames
			{	// set display duration of the current frame
				//pGrImg->pDelay[i] = m_FrmInfo.iDuration; // ms
			}
			break;	
		}
		m_Stream.iOffset += m_FrmInfo.iLength;	// jump to the next frame
	}	

	// Decode frame header
	if( WCI3_GetFrameInfo( &m_Stream, &m_ImageInfo ) != WCI3_ERR_NONE )
		return ;
	
	// Fill decode parameters
	m_DecodeParam.pbtImage							= (BYTE*)m_pImage;
	m_DecodeParam.eColorSpace						= WCI3_CSP_BGRA;//WCI3_CSP_RGB565;				// 32-bit BGRA packed
	m_DecodeParam.rScalability.eMode				= WCI3_SSM_ZOOM;				// scalability mode for big images
	m_DecodeParam.rScalability.iZoomOut				= 0;
	m_DecodeParam.rScalability.rClipRect.iBottom	= m_nHeight;						// Clipping rectangle for big images
	m_DecodeParam.rScalability.rClipRect.iTop		= 0;
	m_DecodeParam.rScalability.rClipRect.iLeft		= 0;
	m_DecodeParam.rScalability.rClipRect.iRight		= m_nWidth;
	
	int nWorkBufSize;
	
	nWorkBufSize= WCI3_GetDecoderBufferSize(&m_ImageInfo, m_DecodeParam.rScalability.iZoomOut);
	if( nWorkBufSize <= 0)
		return ;

	if(m_pCodecData)
		delete [] m_pCodecData;

	m_pCodecData					= new BYTE[nWorkBufSize + 1];
	m_DecodeParam.iProcBufSize	= nWorkBufSize + 1;
	m_DecodeParam.pbtProcBuf	= m_pCodecData;

	if(WCI3_DecodeFrame(&m_DecodeParam, &m_Stream, &m_ImageInfo) != WCI3_ERR_NONE)
		return ;
}

int CWca3::GetFrameNum(void)
{
    return m_nFrameNum;
}

void CWca3::SetAlpha(int nAlpha)
{

}

jintArray	CWca3::GetImage()
{
	return m_pImageIntArray;
}

int	CWca3::GetWidth(void)
{
	return m_nWidth;
}

int	CWca3::GetHeight(void)
{	
	return m_nHeight;
}





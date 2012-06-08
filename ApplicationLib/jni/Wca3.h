/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	제목을 써 넣으세요

	Copyright(c) 2004 ~ LG Electronics Corp.

	그룹 : APPLICATION
	버전 : 1.00
	구분 : Guide
	일자 : 2004.10.27
	성명 : 김기훈
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifndef __WCA3_H__
#define __WCA3_H__
#include "wci_portab.h"
#include	"WICA3.h"
//#include <JNIHelp.h>
#include "jni.h"
#include "nativehelper/JNIHelp.h"

//#include <JNIUtility.h>

//-------------------------------------------------------------------------------------------------
// WICA
	//-------------------------------------------------------------------------------------------------
class CWca3
{
//-------------------------------------------------------------------------------------------------
// Variable
private :
	WCI3_STREAM			m_Stream;
	WCI3_STREAM_INFO	m_StreamInfo;
	WCI3_FRAME_INFO		m_FrmInfo;
	WCI3_DECODE_PARAM	m_DecodeParam;
	WCI3_IMAGE_INFO		m_ImageInfo;	
	BYTE*				m_pEncodedData;
	JNIEnv*				m_env;
	jintArray			m_pImageIntArray;	
	jint*				m_pImage;
	BYTE*				m_pCodecData;
	BOOL				m_bLoad;
	int					m_nWidth;
	int					m_nHeight;
	int					m_nFrameNum;
	int					m_nCurFrame;	

//-------------------------------------------------------------------------------------------------
// Function
public :
			    CWca3				(void);
			    ~CWca3				(void);
	BOOL	    Load				(JNIEnv *env, char* pFileName);
	void	    Decode              (int nFrame);
    int         GetFrameNum         (void);
	int			GetWidth			(void);
	int			GetHeight			(void);
    void        SetAlpha            (int nAlpha);
	jintArray	GetImage            (void);
	
};
#endif//__WCA3_H__

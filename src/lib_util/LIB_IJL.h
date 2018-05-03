
#ifndef __LIB_IJL_H
#define __LIB_IJL_H
//-------------------------------------------------------------------------------------------------

//
//  Intel JPEG Library를 이용한 함수들...
//  IJL15.DLL이 실행파일과 같은 폴더에 있어야 한다.
//
/*
struct tagJPEGBUFFER
{
	WORD	m_wWidth;
	WORD	m_wHeight;	
	WORD	m_wNumberOfChannels;

	union {
		BYTE   *m_pByteBuffer;
		WORD   *m_pWordBuffer;
	} ;
} ;

struct tagJPEGBUFFER *IJL_DecodeFromJPEGBuffer (char *szFileName, int iPixelFormat, long lFileNo=-1);
void                  IJL_DeleteJPEGBuffer (struct tagJPEGBUFFER *pJpegBuffer);
*/
bool IJL_EncodeToJPEGFile (char *lpszPathName, unsigned long dwWidth, unsigned long dwHeight, unsigned char *pRGB);

//-------------------------------------------------------------------------------------------------
#endif
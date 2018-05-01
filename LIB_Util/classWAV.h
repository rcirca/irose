#ifndef	__CLASSWAV_H
#define __CLASSWAV_H
#include <stdio.h>
#include <mmstream.h>		// multimedia stream interfaces
//-------------------------------------------------------------------------------------------------

#ifdef	DLL_ICARUS_EXPORTS
	#define DLL_WAVCLASS	__declspec(dllexport)
#else
	#define DLL_WAVCLASS	//__declspec(dllimport)
#endif

//-------------------------------------------------------------------------------------------------

class DLL_WAVCLASS classWAV {
private :
	FILE					*m_fp;
	long					 m_lDataFPTR;
public  :
	WAVEFORMATEX			 m_sWFX;
    DWORD					 m_dwDataSize;
	DWORD					 m_dwReadData;

	classWAV();
	~classWAV()		{	_Free();	}

	bool	_Init (void);
	void	_Free (void);

	void	Reset (void)	{	m_dwReadData = 0;	fseek(m_fp, m_lDataFPTR, SEEK_SET);	}

	bool	OpenFile (char *szWAVFile, long lFilePtr);
	bool	ReadData (UINT uiSizeToRead, void *pBuffer, UINT *puiActualBytesWritten);
} ;


//-------------------------------------------------------------------------------------------------
#endif


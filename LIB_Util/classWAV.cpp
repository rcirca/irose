
#include <Windows.h>
#include <MMSystem.h>

// #include "iCARUS_DLL.h"
#include "classWAV.h"


//-------------------------------------------------------------------------------------------------

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

//-------------------------------------------------------------------------------------------------
classWAV::classWAV ()
{
	m_fp = NULL;

    m_dwDataSize	= 0;
	m_dwReadData	= 0;
}


//-------------------------------------------------------------------------------------------------
bool classWAV::_Init (void)
{
	_Free ();

    m_dwDataSize	= 0;
	m_dwReadData	= 0;

    return true;
}


//-------------------------------------------------------------------------------------------------
void classWAV::_Free (void)
{
	if ( m_fp )
		fclose (m_fp);
}


//-------------------------------------------------------------------------------------------------
bool classWAV::OpenFile (char *szWAVFile, long lFilePtr)
{
    DWORD       dwType;
    DWORD       dwLength;
    ULONG        lFPos;

	if ( lFilePtr >= 0 )
		lFPos = lFilePtr + sizeof(long);
	else
		lFPos = 0;

	_Free ();

    if ( (m_fp = fopen (szWAVFile, "rb")) == NULL )
		return false;
    fseek (m_fp, lFPos, SEEK_SET);

    fread (&dwType,     sizeof (DWORD), 1, m_fp);  // rID
    if ( dwType != mmioFOURCC('R', 'I', 'F', 'F') )
		return false;

    fread (&dwLength,   sizeof (DWORD), 1, m_fp);  // rLen

    fread (&dwType,     sizeof (DWORD), 1, m_fp);  // wID
    if ( dwType != mmioFOURCC('W', 'A', 'V', 'E') )
		return false;

    fread (&dwType,     sizeof (DWORD), 1, m_fp);  // fID
    if ( dwType != mmioFOURCC('f', 'm', 't', ' ') )
		return false;

    fread (&dwLength,   sizeof (DWORD), 1, m_fp);  // fLen
    fread (&m_sWFX,		sizeof (WAVEFORMATEX), 1, m_fp);

    fseek (m_fp, dwLength-18, SEEK_CUR);
    fread (&dwType,     sizeof (DWORD), 1, m_fp);  // fID
    if ( dwType != mmioFOURCC('d', 'a', 't', 'a') )
		return false;

    fread (&m_dwDataSize, sizeof (DWORD), 1, m_fp);
	m_lDataFPTR = ftell (m_fp);

	return true;
}


//-------------------------------------------------------------------------------------------------
bool classWAV::ReadData (UINT uiSizeToRead, void *pBuffer, UINT *puiActualBytesWritten)
{
	int	iReadLen;

	if ( m_dwReadData + uiSizeToRead >= m_dwDataSize )
		uiSizeToRead = m_dwDataSize - m_dwReadData;

	iReadLen = fread ( (BYTE*)pBuffer, sizeof(BYTE), uiSizeToRead, m_fp);
	if ( iReadLen <= 0 ) {
		*puiActualBytesWritten = 0;
		return false;
	}

	m_dwReadData += iReadLen;
	*puiActualBytesWritten = iReadLen;


	return true;
}


//-------------------------------------------------------------------------------------------------

#ifndef _CLAN_MARK_TRANSFER_
#define _CLAN_MARK_TRANSFER_

#include <../Util/JSingleton.h>

//----------------------------------------------------------------------------------
/// clan mark manager
/// User can register new clan mark, using this class.
/// Functionality of this class are that send mark image to server and receive mark
/// image from server.
///
//----------------------------------------------------------------------------------

class CClanMarkTransfer : public CJSingleton< CClanMarkTransfer >
{
public:
	CClanMarkTransfer(void);
	~CClanMarkTransfer(void);

	//----------------------------------------------------------------------------------
	/// Register clan mark to server.
	//----------------------------------------------------------------------------------
	bool		RegisterMarkToServer( int iClanID, const char* pStrFileName );

	//----------------------------------------------------------------------------------
	/// Receive clan mark from server.
	//----------------------------------------------------------------------------------
	void		ReceiveMarkFromServer( int iClanID, WORD wCRC16, BYTE* pData, int iSize );

	//----------------------------------------------------------------------------------
	/// Request clan mark to server.
	//----------------------------------------------------------------------------------
	void		RequestMarkFromServer( int iClanID );

	bool		Check_BmpFile( HANDLE hFile, SIZE size );

private:	
	
	bool		CheckValidImage( HANDLE hBmp );
	bool		GetImageFileCRC( HANDLE hBmpFile, WORD* pBmpCRC16, DWORD* pdwNumberOfBytesRead, DWORD* pdwFileSizeHigh, DWORD* pdwFileSizeLow );
	bool		WriteToZipFile( const char* TempZipFileName, HANDLE hBmpFile, DWORD* pNumberOfBytesRead );
	bool		SendToServerTempZipFile( const char* TempZipFileName, WORD* pBumCRC16, DWORD* pdwFileSizeHigh, DWORD* pdwFileSizeLow );

	bool		SaveZipFileFromServer( const char* TempZipFileName, DWORD* pdwNumberOfBytesWritten, BYTE* pData, int iSize );
	void		CreateImageFileFromZipFile( const char* TempZipFileName, int iClanID, DWORD* pdwNumberOfBytesWritten );
};

#endif //_CLAN_MARK_TRANSFER_
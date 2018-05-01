#ifndef _CAPTURE_CHAT_LIST_
#define _CAPTURE_CHAT_LIST_


///
/// 쳇 내용을 갈무리 합니다.
///

class CCaptureChatList
{
 
public:
	CCaptureChatList(void);
	~CCaptureChatList(void);

	bool	DoCaptureChatList(); 

private:

	char*	EncodingString( const char* strInput );
	bool	OpenCapturedFile( const char* strFileName );
	void	CloseCapturedFile();
	void	WriteStringToFile( char* strInput );

private:

	FILE*	m_Fp;

};

#endif //_CAPTURE_CHAT_LIST_
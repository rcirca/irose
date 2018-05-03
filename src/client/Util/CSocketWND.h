#ifndef	__CSOCKETWND_H
#define	__CSOCKETWND_H
//-------------------------------------------------------------------------------------------------

//#define	__USE_MESSAGE_PUMP
#define	WM_SOCKETWND_MSG	( WM_USER+3571 )

typedef	bool (*fpSockMsgPROC) (WPARAM wParam, LPARAM lParam);

class CSocketEVENT;
class CSocketWND 
{
private:

#ifdef	__USE_MESSAGE_PUMP
	CSocketEVENT   *m_pSocketEVENT;
#endif
	CClientSOCKET **m_ppSocketBUFF;
	HWND			m_hWND;
	UINT			m_uiMaxSocket;

	static CSocketWND *m_pInstance;
	CSocketWND (HINSTANCE hInstance, UINT uiMaxSocket);//, HINSTANCE hPrevInstance);
	~CSocketWND ();

public :
	static CSocketWND* InitInstance (HINSTANCE hInstance, UINT uiMaxSocket)//, HINSTANCE hPrevInstance)
	{
		if ( NULL == m_pInstance )
			m_pInstance = new CSocketWND(hInstance, uiMaxSocket);//, hPrevInstance);
		return m_pInstance;
	}
	static	CSocketWND* GetInstance ()		{	return m_pInstance;				}
	void	Destroy ()						{	SAFE_DELETE( m_pInstance );		}

	HWND	GetWindowHandle()				{	return m_hWND;					}
	UINT	GetMaxSocket()					{	return m_uiMaxSocket;			}

	bool	AddSocket( CClientSOCKET *pSocket, UINT uiSocketWndMSG );
	void	DelSocket( UINT uiSocketWndMSG );

	friend LRESULT CALLBACK CSockWNDProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
} ;

//-------------------------------------------------------------------------------------------------
#endif
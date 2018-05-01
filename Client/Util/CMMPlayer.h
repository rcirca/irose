/*
	$Header: /7HeartsOnline/Client/Util/CMMPlayer.h 2     04-05-27 8:32p Icarus $
*/
#ifndef	__CMMPLAYER_H
#define	__CMMPLAYER_H
#include <dshow.h>
//-------------------------------------------------------------------------------------------------

class classMMPLAYER
{
public:
	IGraphBuilder *m_pGraphBuilder;
	IMediaEventEx *m_pMediaEvent;
	IMediaControl *m_pMediaControl;
	IMediaSeeking *m_pMediaSeeking;
	IBaseFilter   *m_pSourceCurrent;
	IBaseFilter   *m_pSourceNext;

public :
	HRESULT HandleEvent();

	HRESULT Init( HWND argHwnd, UINT uiEventMsg );
	void	Free (void);

	classMMPLAYER();
	virtual ~classMMPLAYER();

	HRESULT Play( char *szFileName);

	HRESULT Run ();
	HRESULT Pause ();
	HRESULT	Stop ();

	virtual	void ON_Completed ()	{	;	}
	virtual void ON_Abort ()		{	;	}
};


//-------------------------------------------------------------------------------------------------
#endif

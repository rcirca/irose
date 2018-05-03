#ifndef	__CURLTHREAD_H
#define	__CURLTHREAD_H
#include "classHTTP.h"
#include "classTHREAD.h"
#include "classSYNCOBJ.h"
//-------------------------------------------------------------------------------------------------

class CurlTHREAD : public classTHREAD
{
protected : 
	classEVENT	*m_pEVENT;
    classHTTP    m_HTTP;
	char		 m_szURL[ 2048 ];

public :
	CurlTHREAD ();
	virtual ~CurlTHREAD ();

	virtual void Execute();
	void Free ();

	void SendURL (const char *szURL);
} ;

//-------------------------------------------------------------------------------------------------
#endif

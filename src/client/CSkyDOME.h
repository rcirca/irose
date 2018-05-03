/*
	$Header: /Client/CSkyDOME.h 8     04-02-18 10:36a Zho $
*/
#ifndef	__CSKYDOME_H
#define	__CSKYDOME_H


/// 
/// Sky dome class
///

class CSkyDOME {
private:
	short	m_nSkyIDX;
	HNODE	m_hMESH;
	HNODE	m_hMAT;

	HNODE	m_hSKY;

	static CSkyDOME *m_pInstance;

	CSkyDOME (HNODE hShader, HNODE hLight, short nSkyIDX);
	~CSkyDOME ();

public :
	short	GetSkyIDX ()	{	return m_nSkyIDX;	}

	static CSkyDOME* GetInstance ()	{	return m_pInstance;			}

	static bool		Init (HNODE hShader, HNODE hLight, short nSkyIDX);
	static void		Free ()		{	SAFE_DELETE( m_pInstance );	}
	static HNODE GetHNODE () { return (m_pInstance) ? m_pInstance->m_hSKY : 0; }
} ;

//-------------------------------------------------------------------------------------------------
#endif

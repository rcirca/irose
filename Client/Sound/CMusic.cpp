/*
	$Header: /Client/Sound/CMusic.cpp 4     03-12-30 6:43p Jeddli $
*/
#include "stdAFX.h"
#include "CMusic.h"

CMusic* CMusic::m_pInstance = NULL;

//-------------------------------------------------------------------------------------------------
CMusic::CMusic ()
{

}
CMusic::~CMusic ()
{	
}
CMusic* CMusic::Instance ()
{
	if ( !m_pInstance )
		m_pInstance = new CMusic;

	return m_pInstance;
}
void CMusic::Destroy ()
{
	SAFE_DELETE( m_pInstance );
}

void CMusic::ON_Completed ()
{
	// next music ...
}

HRESULT CMusic::SetVolume( long lVolume )
{
	HRESULT hr = S_OK;
	// 할때마다 하는건 무식하지만.. 적당히 넣을곳이 없다.. 현재로는..
	if (FAILED(hr = m_pGraphBuilder->QueryInterface(IID_IBasicAudio,   (void **)&m_pBA)))
		return hr;	

	m_pBA->put_Volume( lVolume );

	SAFE_RELEASE(m_pBA);

	return hr;
}

//-------------------------------------------------------------------------------------------------

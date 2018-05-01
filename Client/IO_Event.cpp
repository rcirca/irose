/*
	$Header: /Client/IO_Event.cpp 16    05-01-07 11:32a Jeddli $
*/
#include "stdAFX.h"
#include "Common\IO_STB.h"
#include "IO_Event.h"


//-------------------------------------------------------------------------------------------------
CEventLIST::CEventLIST () : CFileLIST< CEvent* >("EVT")
{
	;
}

bool CEventLIST::Load_FILE (tagFileDATA< CEvent* > *pDATA)
{
	if ( pDATA->m_DATA ) {
		int kkkk = 0;
		return false;
	}

	pDATA->m_DATA = new CEvent;
	if ( pDATA->m_DATA->Load( pDATA->m_FileName.Get() ) ) {
		return true;
	}
	SAFE_DELETE( pDATA->m_DATA );

	return false;
}
void CEventLIST::Free_FILE (tagFileDATA< CEvent* > *pDATA)
{
	SAFE_DELETE( pDATA->m_DATA );
}

//-------------------------------------------------------------------------------------------------
t_HASHKEY CEventLIST::Add_EVENT (char *szFileName)
{
	if ( (CVFSManager::GetSingleton()).IsExistFile( szFileName ) == false )		
	{
		return 0;
	}

	LogString ( LOG_DEBUG, "Add_EVENT:: %s \n", szFileName);

	return this->Add_FILE(szFileName);
}
void CEventLIST::Del_EVENT (t_HASHKEY HashKEY)
{
	this->Del_FILE (HashKEY);
}

//-------------------------------------------------------------------------------------------------
void CEventLIST::Clear ()
{
	this->Free ();
}

//-------------------------------------------------------------------------------------------------
bool CEventLIST::Run_EVENT( int iOwnerObjIDX, char *szQuestFILE, short nEventIDX, int iEventDlgType )
{
	t_HASHKEY HashKEY = this->Add_EVENT( szQuestFILE );

	if ( HashKEY ) {
		CEvent *pEvent;

		pEvent = this->Get_DATAUseKEY( HashKEY );
		if ( pEvent == NULL ) {
			return false;
		}
		
		pEvent->SetEventDlgType( iEventDlgType );

		pEvent->m_iOwnerObjIDX = iOwnerObjIDX;			 
		bool bReturn = pEvent->Start( nEventIDX );

		// if ( !bReturn )	this->Del_EVENT( HashKEY );

		return bReturn;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
/// @param bool bItemEvent : 아이템에 의해 발생한 이벤트 인가?
//-------------------------------------------------------------------------------------------------
bool CEventLIST::Run_EVENT( int iOwnerObjIDX, short nQuestIDX, short nEventIDX, int iEventDlgType )
{
	return this->Run_EVENT( iOwnerObjIDX, EVENT_FILENAME(nQuestIDX), nEventIDX, iEventDlgType );
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

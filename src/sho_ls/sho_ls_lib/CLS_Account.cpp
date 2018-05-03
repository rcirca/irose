
#include "stdAFX.h"
#include "classTIME.h"
#include "CLS_Account.h"

//-------------------------------------------------------------------------------------------------

CDataPOOL< CLS_Account >* CLS_AccountLIST::m_pAccountPOOL = NULL;
classTIME CLS_AccountLIST::m_Timer;

//-------------------------------------------------------------------------------------------------
CLS_Account::CLS_Account ()
{
//    m_pListITEM = NULL;
    m_pListNODE = new classDLLNODE< CLS_Account* >;
    m_pListNODE->DATA = this;
	this->Init ();
}
CLS_Account::~CLS_Account ()
{
    SAFE_DELETE( m_pListNODE );
}

void CLS_Account::Init ()
{
    m_pWorldServer = NULL;
    m_HashACCOUNT = 0;				// CLS_Account() 생성자
    m_dwRIGHT = 0;
	m_dwDisconnectTryTIME = 0;
	m_dwPayFLAG = 0;
}

//-------------------------------------------------------------------------------------------------
CLS_AccountLIST::CLS_AccountLIST (char *szListName, int iHashTableSIZE) : m_csListACCOUNT( 4000 )
{
	if ( NULL == m_pAccountPOOL ) {
		m_pAccountPOOL = new CDataPOOL< CLS_Account > ( "CLS_AccountPOOL", 0x1ffff, 0xfff );
	    m_Timer.InitStartTime ();
	}

	m_Name.Set( szListName );
    m_pHTAcnt = new classHASH< CLS_Account* > ( iHashTableSIZE );
    m_pHTLSID = new classHASH< CLS_Account* > ( iHashTableSIZE );

	m_dwLastLSID = 0;
}
CLS_AccountLIST::~CLS_AccountLIST ()
{
    SAFE_DELETE( m_pHTLSID );
    SAFE_DELETE( m_pHTAcnt );

	SAFE_DELETE( m_pAccountPOOL );
}


//-------------------------------------------------------------------------------------------------
bool CLS_AccountLIST::Insert_ACCOUNT (CLS_Account *pCAccount, CLS_Server *pWorldServer)
{
    bool bRet;

    pCAccount->m_pWorldServer = pWorldServer;
    m_csListACCOUNT.Lock ();
    {
        bRet = m_pHTLSID->Insert( pCAccount->m_dwLSID, pCAccount );
		if ( bRet ) {
			if ( pCAccount->m_HashACCOUNT ) {
				bRet = m_pHTAcnt->Insert( pCAccount->m_HashACCOUNT, pCAccount );
				if ( !bRet ) {
					// false면 메모리가 없다는것...
					m_pHTLSID->Delete( pCAccount->m_dwLSID, pCAccount );
				}
			}
			// LogString (LOG_NORMAL, "Insert[%s]: %d / %d :: %s\n", m_Name.Get(), m_pHTLSID->GetCount(), m_pHTAcnt->GetCount(), pCAccount->m_Account.Get() );
        }
    }
    m_csListACCOUNT.Unlock ();

    return bRet;
}

//-------------------------------------------------------------------------------------------------
CLS_Account* CLS_AccountLIST::Search_ACCOUNT (DWORD dwLSID)
{
	tagHASH< CLS_Account* > *pHashNode;
    CLS_Account *pCAccount;

    m_csListACCOUNT.Lock ();
    {
		pHashNode = m_pHTLSID->Search( dwLSID );
		pCAccount = pHashNode ? pHashNode->m_DATA : NULL;;
    }
    m_csListACCOUNT.Unlock ();

    return pCAccount;
}


//-------------------------------------------------------------------------------------------------
CLS_Account* CLS_AccountLIST::Search_ACCOUNT (char *szAccount)
{
	tagHASH< CLS_Account* > *pHashNode;
    t_HASHKEY HashKEY = CStr::GetHASH( szAccount );

    CLS_Account *pCAccount=NULL;

    m_csListACCOUNT.Lock ();
    {
		pHashNode = m_pHTAcnt->Search( HashKEY );
        while( pHashNode ) {
			if ( 0 == strcmpi( szAccount, pHashNode->m_DATA->m_Account.Get() ) ) {
				pCAccount = pHashNode->m_DATA;
                break;
			}

			pHashNode = m_pHTAcnt->SearchContinue( pHashNode, HashKEY );
        }
    }
    m_csListACCOUNT.Unlock ();

    return pCAccount;
}


// 월드 서버에서 컨펌요청시 Wait list ㄴ에서 삭제될때 호출...
CLS_Account* CLS_AccountLIST::Remove_FromLIST(DWORD dwLSID)
{
    CLS_Account *pCAccount;

    m_csListACCOUNT.Lock ();
    {
        pCAccount = m_pHTLSID->Delete( dwLSID );
		if ( pCAccount ) {
			// 2004. 9. 21 Account를 Pool_Alloc() 호출후 초기화가 없었다.. 모른 메모리풀이 순환후
			// ASSERT가 걸렸던것으로 판단...
			_ASSERT( 0 == pCAccount->m_HashACCOUNT ); 
            // m_pHTAcnt->Delete( pCAccount->m_HashACCOUNT, pCAccount );
			// LogString (LOG_NORMAL, "Remove[%s]: %d / %d :: %s\n", m_Name.Get(), m_pHTLSID->GetCount(), m_pHTAcnt->GetCount(), pCAccount->m_Account.Get() );
		}
    }

    m_csListACCOUNT.Unlock ();

    return pCAccount;
}


// login list에서 삭제될때 호출..
void CLS_AccountLIST::Delete_ACCOUNT (CLS_Account *pCAccount)
{
	if ( pCAccount->m_HashACCOUNT ) {
		m_csListACCOUNT.Lock ();
		{

			if ( m_pHTLSID->Delete( pCAccount->m_dwLSID, pCAccount ) ) {
				/*
				if ( pCAccount->m_pListITEM ) {
					BCBUtil::DelListITEM (FormMAIN->ListViewLogin, pCAccount->m_pListITEM);
					pCAccount->m_pListITEM = NULL;
				}
				*/
				m_pHTAcnt->Delete( pCAccount->m_HashACCOUNT, pCAccount );

				// LogString (LOG_NORMAL, "Delete[%s]: %d / %d :: %s\n", m_Name.Get(), m_pHTLSID->GetCount(), m_pHTAcnt->GetCount(), pCAccount->m_Account.Get() );

				this->Mem_DEL( pCAccount );
				pCAccount = NULL;
			}
		}
		m_csListACCOUNT.Unlock ();
	}

	/*
    FormMAIN->Update_AccountCNT( m_pHTLSID->GetCount() );
	*/
}


//-------------------------------------------------------------------------------------------------
// waitlist에서만 호출됨,,, 정상적으로 월드 서버로 이동되지 않은 계정들...
void CLS_AccountLIST::Delete_IdleACCOUNT (int iTimeOutSecond)
{
    DWORD dwCurTIME = classTIME::GetCurrentAbsSecond ();

    CLS_Account *pCAccount;
    tagHASH< CLS_Account* > *pAccNODE;

    m_csListACCOUNT.Lock ();

	for (int iL=0; iL<m_pHTLSID->GetTableCount(); iL++) {
		pAccNODE = m_pHTLSID->GetEntryNode( iL );
		while( pAccNODE ) {
            if ( dwCurTIME - pAccNODE->m_DATA->m_dwLoginTIME >= iTimeOutSecond ) {
                pCAccount = pAccNODE->m_DATA;
    			pAccNODE  = pAccNODE->m_NEXT;

                m_pHTLSID->Delete( pCAccount->m_dwLSID, pCAccount );
	
				g_LOG.CS_ODS (LOG_NORMAL, "TimeOut[%s] %d / %d :: %s\n", m_Name.Get(), m_pHTLSID->GetCount(), m_pHTAcnt->GetCount(), pCAccount->m_Account.Get() );

				// m_pHTAcnt->???
				this->Mem_DEL( pCAccount );

                continue;
            }
			pAccNODE = pAccNODE->m_NEXT;
		}
	}

    m_csListACCOUNT.Unlock ();
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

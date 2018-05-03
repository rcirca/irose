
#include "LIB_gsMAIN.h"

#include "OBJECT.h"


//-------------------------------------------------------------------------------------------------
int AI_SysRANDOM(int iMod)
{
	return ( rand()%iMod );
}

//-------------------------------------------------------------------------------------------------
CObjMNG::CObjMNG (int iMaxGameObject) : CSyncIdxARRAY< CGameOBJ* > ( "GS_ObjIDX", iMaxGameObject )
{
}
CObjMNG::~CObjMNG ()
{
}

CGameOBJ *CObjMNG::Get_OBJECT ( WORD wIndex )
{
    return this->GetData( wIndex );
}


CGameOBJ *CObjMNG::Get_GameOBJ( WORD wIndex, t_ObjTAG ObjTYPE )
{
    CGameOBJ *pObject;

    pObject = this->GetData( wIndex );
    if ( pObject && pObject->IsA( ObjTYPE ) ) {
        return pObject;
    }

    return NULL;
}


classUSER*CObjMNG::Get_UserOBJ( DWORD dwIndex ) 
{
    CGameOBJ *pObject = this->GetData( dwIndex );
    if ( pObject && pObject->IsUSER() ) {
        return (classUSER*)pObject;
    }
    return NULL;
}

CObjCHAR*CObjMNG::Get_CharOBJ( WORD wIndex, bool bCheckHP )
{
    CGameOBJ *pObject;

    pObject = this->GetData( wIndex );
    if ( pObject && pObject->IsCHAR() ) {
		if ( bCheckHP ) {
			if ( ((CObjCHAR*)pObject)->Get_HP() > 0 )
				return (CObjCHAR*)pObject;
		} else
			return (CObjCHAR*)pObject;
    }
    return NULL;
}

CObjITEM*CObjMNG::Get_ItemOBJ( WORD wIndex )
{
    CGameOBJ *pObject;

    pObject = this->GetData( wIndex );
    if ( pObject && pObject->IsITEM() ) {
		if ( ( (CObjITEM*)pObject )->m_iRemainTIME > 0 )
			return (CObjITEM*)pObject;
    }
    return NULL;
}

/*
//-------------------------------------------------------------------------------------------------
int CObjMNG::Add_Object( CGameOBJ *pObject )
{
	static WORD wO;

    ::EnterCriticalSection( &m_csOBJ );

	for (wO=m_wEmptySlotNo; wO<MAX_SERVER_OBJECTS; wO++) {
		if ( NULL == m_pOBJECTS[ wO ] ) {
			m_wEmptySlotNo = wO;
            m_pOBJECTS[ wO ] = pObject;
        	::LeaveCriticalSection( &m_csOBJ );

			return wO;
		}
	}

	for (wO=MIN_OBJECTS; wO<m_wEmptySlotNo; wO++) {
		if ( NULL == m_pOBJECTS[ wO ] ) {
			m_wEmptySlotNo = wO;
            m_pOBJECTS[ wO ] = pObject;
        	::LeaveCriticalSection( &m_csOBJ );

			return wO;
		}
	}

	::LeaveCriticalSection( &m_csOBJ );

    return 0;
}

//-------------------------------------------------------------------------------------------------
void CObjMNG::Del_Object( WORD wIndex )
{
    if ( m_pOBJECTS[ wIndex ] ) {
        m_pOBJECTS[ wIndex ] = NULL;
    }
}
*/
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
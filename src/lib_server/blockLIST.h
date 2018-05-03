#ifndef ipLISTH
#define ipLISTH
#include "classSTR.h"
#include "classHASH.h"
#include "classTIME.h"
//-------------------------------------------------------------------------------------------------

#define	DEF_BLACK_HASHTABLE_SIZE	128


struct tagBlockDATA
{
    CStrVAR m_Tag;

    WORD    m_wAttackTYPE;
    DWORD   m_dwAttackCOUNT;

	DWORD	m_dwBlockSECOND;
    DWORD   m_dwFirstTryTIME;
    DWORD   m_dwLastTryTIME;
} ;


template <class dType>
class classListBLOCK {
public :
    classHASH <dType*> *m_pHashTable;

    classListBLOCK (int iHashTableSIZE=DEF_BLACK_HASHTABLE_SIZE);
    ~classListBLOCK ();

	void ClearALL ();

    virtual dType*	Insert (t_HASHKEY HashKEY, char *szTag, WORD wAttackTYPE, DWORD dwBlockSecond);
    virtual dType*	Search (t_HASHKEY HashKEY, char *szTag, bool bUpdateList);
	virtual	void	Update (dType *pInfo, WORD wAttackTYPE, DWORD dwAddBlockSecond);
} ;

//-------------------------------------------------------------------------------------------------
template <class dType>
classListBLOCK<dType>::classListBLOCK (int iHashTableSIZE)
{
    m_pHashTable = new classHASH< dType* >( iHashTableSIZE );
}

template <class dType>
classListBLOCK<dType>::~classListBLOCK ()
{
	this->ClearALL ();
	delete m_pHashTable;
}

template <class dType>
void classListBLOCK<dType>::ClearALL ()
{
    tagHASH< dType* > *pFindNODE;

	for (int iL=0; iL<m_pHashTable->GetTableCount(); iL++) {
		pFindNODE = m_pHashTable->GetEntryNode( iL );
		while( pFindNODE ) {
            delete pFindNODE->m_DATA;

			pFindNODE = pFindNODE->m_NEXT;
		}
	}
}


template <class dType>
dType *classListBLOCK<dType>::Insert (t_HASHKEY HashKEY, char *szTag, WORD wAttackTYPE, DWORD dwBlockSecond)
{
	DWORD dwCurTIME = classTIME::GetCurrentAbsSecond();
    dType *pBlockDATA;

    pBlockDATA = new dType;
    
	pBlockDATA->m_dwBlockSECOND = dwBlockSecond;	// 0 이면 영구 차단 !!!

    pBlockDATA->m_Tag.Set( szTag );
    pBlockDATA->m_dwAttackCOUNT  = 1;
    pBlockDATA->m_wAttackTYPE    = wAttackTYPE;
	pBlockDATA->m_dwFirstTryTIME = dwCurTIME;
	pBlockDATA->m_dwLastTryTIME  = dwCurTIME;

    m_pHashTable->Insert( HashKEY, pBlockDATA );

	return pBlockDATA;
}


template <class dType>
dType *classListBLOCK<dType>::Search (t_HASHKEY HashKEY, char *szTag, bool bUpdateList)
{
    tagHASH< dType* > *pBlockDATA;

    pBlockDATA = m_pHashTable->Search( HashKEY );
    while( pBlockDATA ) {
		if ( 0 == strcmpi( pBlockDATA->m_DATA->m_Tag.Get(), szTag ) ) {
			DWORD dwCurTIME = classTIME::GetCurrentAbsSecond();

			if ( bUpdateList && pBlockDATA->m_DATA->m_dwBlockSECOND ) {
                // 차단됐어도 목록에서 삭제될수 있는것이면 ???
				if ( dwCurTIME-pBlockDATA->m_DATA->m_dwLastTryTIME > pBlockDATA->m_DATA->m_dwBlockSECOND ) {
					LogString (LOG_NORMAL, "del block data [ %s ] Expired: %d / %d \n", szTag, pBlockDATA->m_DATA->m_dwBlockSECOND, dwCurTIME );
                    
					dType *pDelData = pBlockDATA->m_DATA;
					m_pHashTable->Delete( HashKEY, pDelData );
                    delete pDelData;

					return NULL;
				}
            }

			pBlockDATA->m_DATA->m_dwLastTryTIME = dwCurTIME;
			pBlockDATA->m_DATA->m_dwAttackCOUNT ++;
			return pBlockDATA->m_DATA;
        }

        pBlockDATA = m_pHashTable->SearchContinue( pBlockDATA, HashKEY );
    }

    return NULL;
}

template <class dType>
void classListBLOCK<dType>::Update (dType *pBlockDATA, WORD wAttackTYPE, DWORD dwAddBlockSecond)
{
	pBlockDATA->m_wAttackTYPE |= wAttackTYPE;

	if ( pBlockDATA->m_dwBlockSECOND ) {
	    pBlockDATA->m_dwLastTryTIME = classTIME::GetCurrentAbsSecond();
		pBlockDATA->m_dwBlockSECOND += dwAddBlockSecond;
	}
}


//-------------------------------------------------------------------------------------------------
#define IP_BLOCK_TYPE_ACCEPT   0x0001
#define IP_BLOCK_TYPE_PACKET   0x0002

#define DEF_BLACK_IP_HASHTABLE_SIZE     128


#define	DEF_BLACK_NAME_HASHTABLE_SIZE	128

#define NAME_BLOCK_TYPE_ACCEPT		IP_BLOCK_TYPE_ACCEPT
#define NAME_BLOCK_TYPE_PACKET		IP_BLOCK_TYPE_PACKET
#define	NAME_BLOCK_TYPE_ABUSE		0x04					// 욕설
#define	NAME_BLOCK_TYPE_HACK		0x08					// 핵킹툴 사용.

//-------------------------------------------------------------------------------------------------
#endif

/*
	$Header: /Client/Util/CFileLIST.h 51    04-10-25 4:33p Jeddli $
*/
#ifndef	__CFILELIST_H
#define	__CFILELIST_H
#include "classSTB.h"
#include "classSTR.h"
#include "classHASH.h"
#include "classLOG.h"
#ifndef	__SERVER
	#include "CGameSTB.h"
	#include "Util\\VFSManager.h"
#else
	typedef		classSTB	CGameSTB;	
#endif

//-------------------------------------------------------------------------------------------------

/// file data
template <class dType>
struct tagFileDATA {
	dType	m_DATA;

	CStrVAR m_FileName;
	short	m_nIndex;
	bool	m_bLoad;
	short	m_nTag;
	int		m_iRefCnt;
	tagFileDATA ()			{	m_DATA=NULL,m_bLoad=false,m_nIndex=0,m_nTag=0,m_iRefCnt=0;	}
} ;


//-------------------------------------------------------------------------------------------------
///
/// ** 주의 !!!
/// 상속 받은 클래스에의 파괴자에서 아래의 함수
/// CFileLIST< dType >::Free () 를 호출해야 메모리가 풀린다.
/// ~CFileLIST() 에서 this->Free()일 경우 상속 받은 클래스의 Sub_DATA( ... ) 함수가
/// 이미 소멸되어 가상 함수 호출시 오류 발생함.
///
#ifdef	_DEBUG
//	#define	__VIEW_GETDATA
//	#define	__VIEW_LOADFILE
#endif
template <class dType>
class CFileLIST {
protected:
	CStrVAR								m_NameTAG;
	short								m_nTotalFileCNT;

	classHASH< tagFileDATA<dType>* >	m_HASH;

	short								m_nMaxLoadCNT;
	classDLLIST< tagFileDATA<dType>* >	m_DelLIST;

	void Delete_DATA (tagFileDATA<dType> *pData);
	void Delete_LIST (void);

protected:
	short				m_nLoadCNT;
	short				m_nDataCNT;

	tagFileDATA<dType> *m_pDATAs;		// Index data buffer

	dType				Get_DATA(tagFileDATA<dType> *pData);
	bool				Sub_DATA(tagFileDATA<dType> *pData, bool bImmediateFree=false);

	virtual	bool  Load_FILE (tagFileDATA<dType> *pData) = 0{ *(int*)0 = 10; };
	virtual void  Free_FILE (tagFileDATA<dType> *pData) = 0{ *(int*)0 = 10; };

public :
	CFileLIST (char *szNameTAG, short nMaxLoadCnt=128, short nHashEnetryCount=128);
	virtual ~CFileLIST ();

			bool  Alloc_FILES (short nFilesCNT);
	virtual bool  Load (char *szSTBFile, short nFileNameColNO=0);
	virtual void  Free (void);

	// 리스트에 등록시 메모리를 할당한것(CMatLIST등..)에서 메모리 해제할 부분..
	virtual void		Mem_FREE (tagFileDATA<dType> *pDATA)			{	/* nop */	}

	// list에 등록되지 않은 파일이름 등록시에는 nIndex=-1로... 등록후 로드는 하지 않음.
			t_HASHKEY	Add_FILE (char *szFileName, short nIndex, dType DATA, bool bCheckFileExist=false, bool bImmediateLoad = false );
	virtual t_HASHKEY	Add_FILE (char *szFileName, short nIndex=-1, bool bImmediateLoad = false)	{	return this->Add_FILE(szFileName, nIndex, NULL, false, bImmediateLoad );	}
	virtual bool		Del_FILE (char *szFileName);
	virtual bool		Del_FILE(unsigned int uiHashKey);

	tagFileDATA<dType> *KEY_Find_DATA(unsigned int uiKEY)	
	{	
		tagHASH< tagFileDATA<dType>* > *pNode;
		pNode = m_HASH.Search(uiKEY);
		return ( pNode ) ? pNode->m_DATA : NULL;
	}
	tagFileDATA<dType> *IDX_Find_DATA(short nIDX)			{	return ( (nIDX>=0 && nIDX<m_nDataCNT) ? &m_pDATAs[ nIDX ] : NULL );	}

	short Get_ListCOUNT ()							{	return	m_nDataCNT;								}
	short Get_LoadCOUNT ()							{	return  m_nLoadCNT;								}

#ifndef	__VIEW_GETDATA
	dType Get_DATAUseKEY(unsigned int uiKEY)		{	return	Get_DATA( KEY_Find_DATA(uiKEY ));		}
	dType Get_DATAUseIDX(short nIndex)				{	return  Get_DATA( IDX_Find_DATA(nIndex));		}
#else
	dType Get_DATAUseKEY(unsigned int uiKEY);
	dType Get_DATAUseIDX(short nIndex);
#endif
	bool  Sub_DATAUseKEY(unsigned int uiKEY)		{	return	Sub_DATA( KEY_Find_DATA(uiKEY ));		}
	bool  Sub_DATAUseIDX(short nIndex)				{	return  Sub_DATA( IDX_Find_DATA(nIndex));		}

	char *Get_FileNAME(t_HASHKEY HashKEY) {
		tagFileDATA<dType> *pDATA = KEY_Find_DATA(HashKEY);
		if ( pDATA )
			return pDATA->m_FileName.Get();

		return NULL;
	}
} ;

#ifdef	__VIEW_GETDATA
template <class dType>
dType CFileLIST<dType>::Get_DATAUseKEY(unsigned int uiKEY)
{	
	tagFileDATA<dType> *pData = KEY_Get_DATA(uiKEY );
	if ( pData )
		LogString (LOG_DEBUG, "key Get %s: [ %s ]\n", m_NameTAG.Get(), pData->m_FileName.Get());
	else
		LogString (LOG_DEBUG, "key Get %s == NULL\n", m_NameTAG.Get());
	return	Get_DATA( pData );	
}
template <class dType>
dType CFileLIST<dType>::Get_DATAUseIDX(short nIndex)
{	
	tagFileDATA<dType> *pData = IDX_Get_DATA(nIndex);
	if ( pData )
		LogString (LOG_DEBUG, "idx Get %s: [ %s ]\n", m_NameTAG.Get(), pData->m_FileName.Get());
	else
		LogString (LOG_DEBUG, "idx Get %s == NULL\n", m_NameTAG.Get());
	return  Get_DATA( pData );	
}
#endif

template <class dType>
CFileLIST<dType>::CFileLIST(char *szNameTAG, short nMaxLoadCnt, short nHashEnetryCount) : m_HASH(nHashEnetryCount)
{
	m_pDATAs=NULL;
	m_nDataCNT=0;
	m_nLoadCNT=0;
	m_nTotalFileCNT=0;
	m_nMaxLoadCNT=nMaxLoadCnt;
	m_NameTAG.Set( szNameTAG );
}


template <class dType>
CFileLIST<dType>::~CFileLIST ()
{
	// this->Free (); 호출시 오류 발생.. 
	// Free함수 내의 가상 함수 Sub_DATA가 이미 파괴되어 
	// 호출되어 진다.
	
//	_ASSERT( m_nLoadCNT == 0 );
}


//-------------------------------------------------------------------------------------------------
template <class dType>
bool CFileLIST<dType>::Alloc_FILES (short nFilesCNT)
{
	m_nDataCNT = nFilesCNT;
	m_pDATAs   = new tagFileDATA<dType> [ m_nDataCNT ];
	return ( m_pDATAs != NULL );
}

//-------------------------------------------------------------------------------------------------
template <class dType>
t_HASHKEY CFileLIST<dType>::Add_FILE( char *szFileName, short nIndex, dType DATA, bool bCheckFileExist, bool bImmediateLoad )
{
#ifndef	__SERVER
	if ( bCheckFileExist && ( (CVFSManager::GetSingleton()).IsExistFile( szFileName ) == false ) )	
	{
		char *szMSG = CStr::Printf ("ERROR File [%s] not found in %s ... \n", szFileName, m_NameTAG.Get());
		// ::MessageBox (NULL, szMSG, "ERROR", MB_OK);
		LogString ( LOG_DEBUG, szMSG );
		return 0;
	}
#else
	if ( bCheckFileExist && !CUtil::Is_FileExist( szFileName ) ) {
		CStrVAR str( 256 );
		str.Printf ("ERROR File [%s] not found in %s ... \n", szFileName, m_NameTAG.Get());
		// ::MessageBox (NULL, szMSG, "ERROR", MB_OK);
		LogString ( LOG_DEBUG, str.Get() );
		return 0;
	}
#endif

	t_HASHKEY uiHashKey;

	uiHashKey = StrToHashKey(szFileName);

	if( uiHashKey == 0 )
		return 0;
	
	tagFileDATA<dType> *pData;

	tagHASH< tagFileDATA<dType>* > *pHashNode;
	pHashNode = m_HASH.Search (uiHashKey);
	pData = ( pHashNode ) ? pHashNode->m_DATA : NULL;

	/// 이미들어간거라면..
	if ( pData ) {
		if ( !strcmpi(szFileName, pData->m_FileName.Get() ) )
			return uiHashKey;

		t_HASHKEY uiHashKey2;
		uiHashKey2 = StrToHashKey( pData->m_FileName.Get() );

		LogString ( LOG_DEBUG, "HashKey Collision (%d:%s,  %d:%s)\n", uiHashKey, szFileName, uiHashKey2, pData->m_FileName.Get() );

		_ASSERT( !pData );
		return 0;
	}

	/// 파일 리스트 내부에서 관리되는 데이터 배열의 인덱스..( 리스트를 인덱스로 유지 관리하는 데이터 들의 경우??)
	if ( nIndex >= 0 && nIndex < m_nDataCNT ) {
		m_pDATAs[ nIndex ].m_FileName.Set( szFileName );
		m_pDATAs[ nIndex ].m_nIndex= nIndex;
		m_pDATAs[ nIndex ].m_DATA = DATA;

		m_pDATAs[ nIndex ].m_nTag = m_nTotalFileCNT;
		m_HASH.Insert (uiHashKey, &m_pDATAs[ nIndex ]);

		
		/// test 파일을 더하면서 다 읽어 놔보자
		if( bImmediateLoad )
		{
			if ( this->Load_FILE (&m_pDATAs[ nIndex ]) ) 
			{
				m_pDATAs[ nIndex ].m_bLoad = true;
				m_nLoadCNT ++;
				LogString ( LOG_DEBUG, "Add and Load FILE : %s, LoadCNT: %d\n", m_pDATAs[ nIndex ].m_FileName.Get(), Get_LoadCOUNT() );				
			}
		}		

	} else {
		tagFileDATA<dType> *pNewDATA;
		pNewDATA = new tagFileDATA<dType>;

		pNewDATA->m_FileName.Set( szFileName );
		pNewDATA->m_nIndex = -1;
		pNewDATA->m_DATA   = DATA;

		pNewDATA->m_nTag = m_nTotalFileCNT;
		m_HASH.Insert (uiHashKey, pNewDATA);		
	}	

	m_nTotalFileCNT ++;

	return uiHashKey;
}

//-------------------------------------------------------------------------------------------------
template <class dType>
bool CFileLIST<dType>::Del_FILE(unsigned int uiHashKey)
{
	tagHASH< tagFileDATA<dType>* > *pHashNode;

	pHashNode = m_HASH.Search (uiHashKey);
	tagFileDATA<dType> *pData;

	pData = ( pHashNode ) ? pHashNode->m_DATA : NULL;

	if ( pData ) {
		m_HASH.Delete (uiHashKey);
		if ( pData->m_nIndex == -1 ) {
			this->Sub_DATA( pData, true );
			delete pData;
		} else {
			//pData->m_FileName.Del ();
			//pData->m_nIndex = ??
			//pData->m_bLoad  = false;
			//pData->m_DATA   = 
		}
		return true;
	}
	return false;
}

template <class dType>
bool CFileLIST<dType>::Del_FILE(char *szFileName)
{
	unsigned int uiHashKey = StrToHashKey(szFileName);
	return Del_FILE (uiHashKey);
}

//-------------------------------------------------------------------------------------------------
template <class dType>
void CFileLIST<dType>::Delete_LIST (void)
{
	if ( m_DelLIST.GetNodeCount() >= m_nMaxLoadCNT ) {
		classDLLNODE< tagFileDATA<dType>* > *pNODE, *pDelNODE;
		pNODE = m_DelLIST.GetHeadNode ();
		while( pNODE ) {
			if ( pNODE->DATA->m_iRefCnt > 0 ) {
				// 다시 사용되고 있다.
				pDelNODE = pNODE;
				pNODE = m_DelLIST.GetNextNode( pNODE );
				m_DelLIST.DeleteNFree( pDelNODE );
				continue;
			}

			this->Delete_DATA( pNODE->DATA );
			m_DelLIST.DeleteNFree( pNODE );

			return;
			// pNODE = m_DelLIST.GetNextNode( pNODE );
		}
	}
}

template <class dType>
void CFileLIST<dType>::Delete_DATA (tagFileDATA<dType> *pDATA)
{
	if ( pDATA->m_bLoad ) {
		this->Free_FILE( pDATA );
		m_nLoadCNT --;
	#ifndef	__SERVER
		LogString (LOG_DEBUG, "Free FILE : %s, LoadCNT: %d\n", pDATA->m_FileName.Get(), Get_LoadCOUNT() );
	#endif
		pDATA->m_bLoad = false;
	}
}

//-------------------------------------------------------------------------------------------------
template <class dType>
dType CFileLIST<dType>::Get_DATA(tagFileDATA<dType> *pDATA)
{
	if ( pDATA == NULL ) {
		// not exist file in m_pDATAs[] ..
		return NULL;
	}

	if ( !pDATA->m_bLoad ) {
		if ( NULL == pDATA->m_FileName.Get() )
			return NULL;

		if( strcmp( pDATA->m_FileName.Get(), "NULL" ) == 0 )
		{
			return NULL;
		}

		if ( this->Load_FILE (pDATA) ) {
			pDATA->m_bLoad = true;
			m_nLoadCNT ++;
		#ifndef	__SERVER
			LogString (LOG_DEBUG, "Load FILE : %s, LoadCNT: %d\n", pDATA->m_FileName.Get(), Get_LoadCOUNT() );
		#endif
		} else
			return NULL;
	}
	pDATA->m_iRefCnt ++;
	this->Delete_LIST ();

	return pDATA->m_DATA;
}

template <class dType>
bool CFileLIST<dType>::Sub_DATA(tagFileDATA<dType> *pDATA, bool bImmediateFree)
{
	if ( pDATA == NULL ) {
		// not exist file in m_pDATAs[] ..
		return false;
	}

	if ( pDATA->m_bLoad ) {
		pDATA->m_iRefCnt --;

		if ( pDATA->m_iRefCnt <= 0 ) {
			if ( bImmediateFree ) {
				this->Delete_DATA( pDATA );
			} else {
				this->Delete_LIST ();
				// 삭제 대기 버퍼에 등록.
				m_DelLIST.AllocNAppend( pDATA );
			}	
		}
	}
	
	return true;
}


//-------------------------------------------------------------------------------------------------
template <class dType>
bool CFileLIST<dType>::Load (char *szSTBFile, short nFileNameColNO)
{
	CGameSTB fSTB;

	if ( fSTB.Open (szSTBFile) ) {
		char *szFileName;

		Alloc_FILES( fSTB.GetRowCount() );

		for (short nI=0; nI<m_nDataCNT; nI++) {
			szFileName = fSTB.GetString(nFileNameColNO, nI);
			if ( szFileName ) {
				this->Add_FILE(szFileName, nI);
			}
		}
		fSTB.Close ();
		return true;
	}

	return false;
}

template <class dType>
void CFileLIST<dType>::Free (void)
{
	unsigned long ulHashKey;
	tagFileDATA<dType> *pDATA;

	while( m_HASH.GetFirst( &ulHashKey, &pDATA) ) {
		m_HASH.Delete (ulHashKey);

		pDATA->m_iRefCnt = 0;
		this->Sub_DATA( pDATA, true );
		this->Mem_FREE( pDATA );	// delete pDATA->m_DATA !!!!

		pDATA->m_DATA = NULL;

		if ( pDATA->m_nIndex == -1 ) {
			delete pDATA;
		}
	}

	if ( m_pDATAs ) { 
		for (short nI=0; nI<m_nDataCNT; nI++) {
			m_pDATAs[ nI ].m_DATA  = NULL;
			m_pDATAs[ nI ].m_bLoad = false;
		}

		delete[] m_pDATAs;
		m_pDATAs = NULL;
		m_nDataCNT = 0;
	}
}

//-------------------------------------------------------------------------------------------------
#endif

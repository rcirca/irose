/*
	$Header: /Client/OBJECT.h 32    05-08-30 5:17p Gioend $
*/
#ifndef	__OBJECT_H
#define	__OBJECT_H
#include "CModelCHAR.h"
#include "CObjGROUND.h"
#include "CObjUSER.h"
#include "Game.h"
#include <vector>
#include <list>


#define SPECIAL_WARP_OBJECT				1
#define SPECIAL_COLLISION_OBJECT		2
#define WARP_OBJECT_INDEX_OFFSET		1000000

class CMAP;
class CObjFixedEvent;


///CMap::Load에서 읽은 NPC정보

struct	NpcInfoInMap{
	CMAP*		m_pMap;///npc가 로드된 CMAP의 메모리 포인터
	int			m_iNpcID;///stb No
	D3DVECTOR	m_Position;
};

/// 
/// Object manager class
///
class CObjectMANAGER {
private:
	classDLLNODE< int > *m_pFindNODE;

	int		m_iEmptySlotNo;								/// < 현재 비어있는 슬롯 시작 번호.			
	short	m_nServer2ClientOBJ[ MAX_SERVER_OBJECTS ];	/// < 서버에서 이 인덱스의 오브젝트가 클라이언트의 어떤 슬롯인가?
	WORD	m_wClient2ServerOBJ[ MAX_CLIENT_OBJECTS ];	/// < 서버로 부터 받은 인덱스 번호 저장.
	
	int		m_iObjectCNT[ OBJ_MAX_TYPE ];				/// < 오브젝트 타입당 오브젝트 개수.
	int		m_iTotalObjCNT;								/// < 전체 오브젝트 개수

	DWORD	m_dwCurTIME;
	DWORD	m_dwPassTIME;


	//----------------------------------------------------------------------------------------------------
	/// 미니맵의 표시를 위한 리스트..
	std::vector< int >				m_ViewObjectList;
	///MAP에서 로드된 NPC를 미니맵에 표시하기 위하여 임시 저장하는 List
	std::list< NpcInfoInMap >		m_listNpcInfo;
	//----------------------------------------------------------------------------------------------------


	static	CObjectMANAGER* m_pInstance;
	CObjectMANAGER ();
	~CObjectMANAGER ();

public :
	CGameOBJ	*m_pOBJECTS[ MAX_CLIENT_OBJECTS ];		///< 오브젝트 배열.
	classDLLIST< int >	m_CharLIST;						///< 캐릭터 오브텍트 번호( 오브젝트 배열내의 인덱스) 리스트
	classDLLIST< int >	m_CnstLIST;						///< 건물, 나무, 기타 오브젝트 번호 리스트
	classDLLIST< int >	m_ItemLIST;						///< 아이템 오브젝트 번호 리스트

	std::map< int, CObjFixedEvent* >  m_EventObjectList;			///< 이벤트 오브젝트 리스트.

	static	CObjectMANAGER* Instance ();
	static  void	Destroy ();


	void ResetTime(){ m_dwCurTIME = g_GameDATA.GetGameTime(); m_dwPassTIME = 0; }


	/// 미니맵의 표시를 위한 리스트..
	void	ReserverViewObjectList( int iReserveCnt = 100 ){ m_ViewObjectList.reserve( 100 ); }
	void	ClearViewObjectList(){ /*m_ViewObjectList.clear()*/ m_ViewObjectList.erase( m_ViewObjectList.begin(), m_ViewObjectList.end() ); }
	void	AddViewObject( int Index ){ m_ViewObjectList.push_back( Index ); }
	int		GetViewObjectCnt(){ return m_ViewObjectList.size(); }
	int		GetViewObjectIndex( int Index ){ return m_ViewObjectList[ Index ]; }


	/// 오브젝트를 신그래프에 넣는다.
	void	InsertToScene ();							
	/// 신그래프로부터 오브젝트 제거.
	void	RemoveFromScene ();							


	/// 빈 슬롯 할당( 마지막 할당 슬롯 뒤쪽부터 검색 )
	int		Get_EmptySlot ();							
	/// 해당 슬롯에 오브젝트 할당
	/// @param wServerObjectIndex 서버에서의 오브젝트 인덱스..
	bool	Set_EmptySlot (short nSlotNO, WORD wServerObjectIndex, CGameOBJ *pObject); 


	/// 모든 맴버 리스트 클리어.
	void	Clear (short nExceptObjIndex);
	/// 해당 인덱스의 오브젝트 제거.
	void	Del_Object (short nIndex);
	/// 해당 오브젝트 제거.
	void	Del_Object (CGameOBJ *pObject);



	/// 나무 추가.
	int		Add_GndTREE( short nObjID, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale );
	/// 건물 추가.
	int		Add_GndCNST( short nObjID, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale );
	/// 아이템 추가
	int		Add_GndITEM( WORD wServerObjectIndex, tagITEM &sITEM, tPOINTF &Position, float fRefZPos = 0.0f, bool bUseRefZPos = false );
	/// 모퍼 추가
	int		Add_MORPHER( short nObjID, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale );
	/// Warp object 추가
	int		Add_WARP( short nObjID, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale );
	/// Collision object 추가
	int	    Add_CollisionBox( int iReserved, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale );

	/// 몹 추가
	int		Add_MobCHAR( WORD wServerObjectIndex, short nMobIdx, const D3DVECTOR& Position, short nQuestIDX, bool bRunMODE);
	/// NPC 캐릭터 추가
	int		Add_NpcCHAR( WORD wServerObjectIndex, short nNpcIdx, const D3DVECTOR& Position, short nQuestIDX, float fModelDIR );
	/// 아바타 캐릭터 추가.
	bool	Add_AvtCHAR(CObjAVT *pAvtCHAR, D3DVECTOR &Position, BYTE btCharRACE);

	/// Cart 추가
	CObjCART*	Add_CartCHAR( int iPetType, CObjAVT* pOwner, WORD wServerObjectIndex );


	/// Event object 추가
	int		Add_FixedEvent( int iEventID, short nObjID, 
							const char* pTriggerFileName,
							const char* pConFileName,
							D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale );


	/// 새로운 유져 캐릭터 추가.
	CObjUSER  *New_UserCHAR (WORD wServerObjectIndex, char *szName);
	/// 새로운 아바타 추가.
	CObjAVT   *New_AvtCHAR  (WORD wServerObjectIndex, char *szName);


	/// 서버인덱스와 클라이언트 인덱스 등록
	void	Set_ServerObjectIndex (short nClientObjectIndex, WORD wServerObjectIndex);
	/// 서버 인덱스로 클라이언트 인덱스를 구함.
	short	Get_ClientObjectIndex (WORD  wServerObjectIndex)	{	return m_nServer2ClientOBJ[ wServerObjectIndex ];	}
	/// 클라이언트 인덱스로 서버 인덱스를 구함.
	WORD	Get_ServerObjectIndex (short nClientObjectIndex)	{	return m_wClient2ServerOBJ[ nClientObjectIndex ];	}

	/// 서버 인덱스로 클라이언트 오브젝트를 얻는다.
	inline CGameOBJ *Get_ClientOBJECT  (WORD wServerObjectIndex, t_ObjTAG ObjTYPE=OBJ_NULL)
	{
		return this->Get_OBJECT( this->Get_ClientObjectIndex(wServerObjectIndex), ObjTYPE);
	}
	/// 서버 인덱스로 캐릭터 오브젝트를 얻는다.
	inline CObjCHAR*Get_ClientCharOBJ (WORD wServerObjectIndex, bool bCheckHP)
	{
		return this->Get_CharOBJ( this->Get_ClientObjectIndex(wServerObjectIndex), bCheckHP );
	}

	///캐릭터이름으로 캐릭터 오브젝트를 얻는다.
	CObjCHAR*	Get_ClientCharOBJByName( const char* szName );

	/// 서버 인덱스로 아바자 오브젝트를 얻는다.
	inline CObjAVT*Get_ClientCharAVT (WORD wServerObjectIndex, bool bCheckHP)
	{
		return this->Get_CharAVT( this->Get_ClientObjectIndex(wServerObjectIndex), bCheckHP );
	}
	/// 서버 인덱스로 아이템 오브젝트를 얻는다.
	inline CObjITEM*Get_ClientItemOBJ (WORD wServerObjectIndex)
	{
		return this->Get_ItemOBJ( this->Get_ClientObjectIndex(wServerObjectIndex) );
	}


	CGameOBJ*	Get_OBJECT  (int iObjectIndex, t_ObjTAG ObjTYPE=OBJ_NULL);
	CObjCHAR*	Get_CharOBJ (int iObjectIndex, bool bCheckHP);
	CObjAVT*	Get_CharAVT (int iObjectIndex, bool bCheckHP);
	CObjITEM*	Get_ItemOBJ (int iObjectIndex);
	
	CObjCHAR*Get_FirstCHAR ();	// ai test function ...
	CObjCHAR*Get_NextCHAR ();	// ai test function ...


	int		 Get_ObjectCount(t_ObjTAG Type)	{	return m_iObjectCNT[ Type ];	}
	int		 Get_ObjectCount ()				{	return m_iTotalObjCNT;			}

	char	*Get_NAME (int iTargetObjectIndex);
	DWORD	 Get_PassTIME ()				{	return m_dwPassTIME;			}
	DWORD	 Get_CurTIME ()					{	return m_dwCurTIME;				}

	void	 ProcOBJECT ();


	void		AddNpcInfo( CMAP* pMap, int iNpcNo, D3DVECTOR Position );
	void		RemoveNpcInfo( CMAP* pMap );
	void		ClearNpcInfoList();
	const std::list< NpcInfoInMap >& GetNpcInfoList(); 


	//-------------------------------------------------------------------------------------------------
	/// 이벤트 오브젝트 관련
	CObjFixedEvent*	GetEventObject( int iEventID );

	void		ResetClanMarkInfo( DWORD dwClanID, WORD crc16 );

} ;
extern CObjectMANAGER *g_pObjMGR;
extern CObjUSER *g_pAVATAR;

//-------------------------------------------------------------------------------------------------
#endif

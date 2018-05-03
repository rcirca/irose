/*
	$Header: /Client/Event/CEvent.h 22    05-01-07 2:20p Jeddli $
*/
#ifndef	__CEVENT_H
#define	__CEVENT_H
#include "..\util\classLUA.h"
//-------------------------------------------------------------------------------------------------

struct tagSCRIPTMSG {
	CStrVAR	m_CheckFunc;
	CStrVAR	m_ClickFunc;
} ;

//-------------------------------------------------------------------------------------------------
/// 하나의 대사 노드는 타입, 자식세트 인덱스, 체크 펑션, 클릭 펑션, 메세지로 구성
/// 	m_iType					: 현재 대사 노드의 타입
/// 	m_lChildDataIDX			: 자식 세트의 인덱스
/// 	CStrVAR	m_CheckFunc		: 트리가 구성될때 수행되는 체크 펑션
/// 	CStrVAR	m_ClickFunc		: 해당 대사가 클릭되었을때 수행되어야할 펑션
/// 	CStrVAR m_Message		: 실제로 보여지는 메세지
//-------------------------------------------------------------------------------------------------
struct tagSCRIPTITEM {
	int		m_iType;
	long	m_lChildDataIDX;

	CStrVAR	m_CheckFunc;
	CStrVAR	m_ClickFunc;

	CStrVAR m_Message;
} ;


//-------------------------------------------------------------------------------------------------
/// 하나의 트리 부모 노드를 가지는 실버링들의 리스트
/// !!! 특정 트리노드의 자식들이 바로 참조되는것이 아니라 모든 트리의 노드들은 자신의 형재들과 함께 세트로 저장
/// 부모 노드는 단순히 차일드노드세트의 인덱스만 저장하고 있다.
///
///   ||A||    ||B||        ||C||    
/// 
///  위의 그림에서 A, B, C 는 노드들의 리스트로 구성된 세트
///  A 리스트중 어떤 한 노드가 B, C 세트의 인덱스를 가지면 그놈들이 해당노드의 자식이 되는것이다.
///
//-------------------------------------------------------------------------------------------------
struct tagSCRIPTDATA {
	short			m_iScrItemCNT;
	tagSCRIPTITEM  *m_pScrITEM;

	tagSCRIPTDATA ()	{	m_pScrITEM = NULL;					}
	~tagSCRIPTDATA ()	{	SAFE_DELETE_ARRAY( m_pScrITEM );	}
} ;

//-------------------------------------------------------------------------------------------------
class CEvent;
struct tagEventITEM {
	CEvent			*m_pEVENT;
	tagSCRIPTITEM	*m_pScrITEM;
} ;

#define	MAX_CONV_STR_LEN	2048

class CEvent {
private:
	static char	    m_TempBuffer[ MAX_CONV_STR_LEN ];
	static char		m_MBCSBuffer[ MAX_CONV_STR_LEN ];

	int				m_iScrMsgCNT;
	tagSCRIPTMSG   *m_pScrMSG;

	int				m_iScrDataCNT;
	tagSCRIPTDATA  *m_pScrDATA;

	int				m_iLuaDataLEN;
	char		   *m_pLuaDATA;

	t_HASHKEY		m_HashKEY;
	classLUA	   *m_pLUA;
	classDLLIST< tagEventITEM >	m_EventITEM;	

	CStrVAR			m_EventFUNC[ 16 ];



	//-------------------------------------------------------------------------------------------------
	/// 아이템에 의해 생성된 이벤트 인가?
	/// 2005/1/6 생성할 Dlg 타입으로 변경
	//-------------------------------------------------------------------------------------------------
	///bool			m_bItemEvent;
	int				m_iEventDlgType;


	
	char *ParseMESSAGE (char *szMessage);
	classDLLNODE< tagEventITEM >* Add_ClickITEM (tagSCRIPTITEM *pScrITEM);
	void Del_ClickITEMS (void);
	bool End ();
	int  GetQueryType();

public :
	CEvent ( int iEventDlgType = 0 );
	~CEvent ();

	void			SetEventDlgType( int iEventDlgType ){ m_iEventDlgType = iEventDlgType; }

	int				m_iOwnerObjIDX;

	bool			Load (char *szFileName);
	bool			Start (short nEventIDX);
	short			Conversation (int iMenuIDX);

	static void		Click_ITEM( int iHandle );
	static void		Click_CLOSE( int iHandle );

	void			Decode( char *pData, int iSize, int iValue1, int iValue2 );
} ;

//-------------------------------------------------------------------------------------------------
#endif

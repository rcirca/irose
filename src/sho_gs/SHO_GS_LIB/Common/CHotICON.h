/**
 * \ingroup SHO_GS
 * \file	CHotICON.h
 * \brief	단축 아이콘 보관
 */
#ifndef	__CHOTICON_H
#define	__CHOTICON_H
#pragma warning (disable:4201)
//-------------------------------------------------------------------------------------------------

/// 최대 단축 아이콘 등록 가능 갯수
#define	MAX_HOT_ICONS		48
#ifdef _PA
	#define	MAX_ICONS_PAGES		6
#else
	#define	MAX_ICONS_PAGES		4
#endif
#define	HOT_ICONS_PER_PAGE	8

enum t_HotIconTYPE 
{
	INV_ICON = 1,
	COMMAND_ICON,
	SKILL_ICON,
	EMOTION_ICON,
	DIALOG_ICON,
} ;


#pragma pack (push, 1)
union tagHotICON {
	struct {
		unsigned short	m_cType			: 5;	// 0~31
		unsigned short	m_nSlotNo		: 11;	// 0~2047
	} ;
	WORD	m_wHotICON;
} ;

/**
 * \ingroup SHO_GS_LIB
 * \class	CHotICONS
 * \author	wookSang.Jo
 * \brief	사용자가 지정한 클라이언트의 단축 아이콘 관리 클래스
 */
class CHotICONS {
public :
	union {
		tagHotICON	m_IconLIST[ MAX_HOT_ICONS ];
		tagHotICON	m_IconPAGE[ MAX_ICONS_PAGES ][ HOT_ICONS_PER_PAGE ];
	} ;
	void Init ();
	bool RegHotICON (BYTE btListIDX, tagHotICON sHotICON);
	void DelHotICON (BYTE btListIDX);

#ifndef	__SERVER
	void UpdateHotICON();
#endif
} ;
#pragma pack (pop)

//-------------------------------------------------------------------------------------------------
#pragma warning (default:4201)
#endif

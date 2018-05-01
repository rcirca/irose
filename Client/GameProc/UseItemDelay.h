#ifndef _USE_ITEM_DELAY_
#define _USE_ITEM_DELAY_


#define  MAX_DELAYTICK_TYPE			4



enum enumUSE_ITEM_TYPE
{
	USE_ITEM_HP = 0,
	USE_ITEM_MP,
	USE_ITEM_OTHERS,
	USE_ITEM_SCROLL,
	USE_ITEM_MAX,
};

const int DEFAULT_HP_ITEM_DELAY = 500;
const int DEFAULT_MP_ITEM_DELAY = 500;
const int DEFAULT_OTHER_ITEM_DELAY = 500;
const int DEFAULT_USE_SCROLL_DELAY = 3000;

//----------------------------------------------------------------------------------------------------
/// class CUseItemDelay
/// 각 Use Item 타입별로 정해진 딜레이 적용.
//----------------------------------------------------------------------------------------------------

//class CUseItemDelay
//{
//private:
//	int					m_UseItemDelay[ USE_ITEM_MAX ];
//
//public:
//	CUseItemDelay(void);
//	~CUseItemDelay(void);
//
//	void				SetUseItemDelay( int iUseItemType, int iDelayTime ) { m_UseItemDelay[ iUseItemType ] = iDelayTime; }
//	int					GetUseItemDelay( int iUseItemType )					{ return m_UseItemDelay[ iUseItemType ]; }
//
//	void				Clear();
//	void				Proc();
//};

/*===================================================================
	2005. 05. 26	박	지호 
=====================================================================*/
#include <map>
using namespace std;


typedef map<int,float>::iterator USERITEMDELAYITR;
class CUseItemDelay
{

	int				m_nCount;	
	map<int,float>	m_UseItemDelay;

public:
	CUseItemDelay();
	~CUseItemDelay();
	

	void	Release(void);
	void	SetUseItemDelay(int iUseItemType,float iDelayTime);
	float	GetUseItemDelay(int iUseItemType);				
	void	Proc(void);

};



//UseItemp에 사용되는 클래스 
extern CUseItemDelay	g_UseItemDelay;
extern CUseItemDelay	g_CurUseItemDelayTick;
extern CUseItemDelay	g_SoloUseItemDelayTick;

//스킬에 사용되는 클래스 
extern CUseItemDelay   g_UseSkillDelay;
extern CUseItemDelay   g_CurSkillDelayTick;
extern CUseItemDelay   g_SoloSkillDelayTick;

#endif //_USE_ITEM_DELAY_
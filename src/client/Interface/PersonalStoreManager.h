#ifndef _PERSONAL_STORE_MANAGER_
#define _PERSONAL_STORE_MANAGER_

#include <list>


//----------------------------------------------------------------------------------------------------
/// class CPersonalStoreManager
/// 개인상점 타이틀을 관리&Draw 하기 위한 클래스
//----------------------------------------------------------------------------------------------------

class CPersonalStoreManager
{
private:
	std::list< int >		m_PersonalStoreList;
	HNODE					m_ShopTitleBox;
	int						m_iShopTitleBoxWidth;
	int						m_iShopTitleBoxHeight;

public:
	CPersonalStoreManager(void);
	~CPersonalStoreManager(void);

	bool					Init();
	void					FreeResource();
	void					ClearList();


	void					AddStoreList( int iClientObjIDX ){ m_PersonalStoreList.push_back( iClientObjIDX ); }
	void					SubStoreList( int iClientObjIDX ){ m_PersonalStoreList.remove( iClientObjIDX ); }

	void					Draw();
};


#endif /// _PERSONAL_STORE_MANAGER_
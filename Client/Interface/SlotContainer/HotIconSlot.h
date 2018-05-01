#ifndef _CHOTICONSLOT_
#define _CHOTICONSLOT_

#include "../../Common/CHotICON.h"

class CIcon;
class CHotICONS;


/**
* CItemSlot과 같은 이유로 만들어진 단축창아이템 컨테이너 클래스
*
* @Author		이동현, 최종진
* @Date			2005/9/15
*/
class CHotIconSlot
{
private:


public:
	CHotIconSlot(void);
	~CHotIconSlot(void);

	void			SetHotIcons( CHotICONS* pHotIcon );
	void			AddQuickItem( int iSlot, int iType, int iIndex );

	CIcon*			GetHotIcon( int iSlot );

	tagHotICON		GetHotItem( int iSlot );
};


#endif //_CHOTICONSLOT_

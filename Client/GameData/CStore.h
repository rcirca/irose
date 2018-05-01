#ifndef _CStore_
#define _CStore_
#include <map>
#include "../gamecommon/CObservable.h"
#include "../common/CItem.h"

const int c_iSlotCountPerTab = 48;

class CItem;
class CTCmdNumberInput;
class CTCmdOpenNumberInputDlg;



/**
* NPC상점이용시의 DataClass
*
* @Author		최종진
* @Date			2005/9/15
*/
class CStore : public CObservable
{
	CStore(void);
public:
	~CStore(void);

	static	CStore& GetInstance();
	bool	ChangeStore( int iNpcObjIndex, bool bSpecialTab );
	
	std::string&	GetTabName( int i );
	int				GetNpcObjIndex();
	CItem*	GetItem( int iIndex );
	bool	IsUnionStore();
	int		GetUnionNo();

private:
	//CItem*	CreateItem( tagITEM& Item );

private:
	int						m_iUnionNo;///if Union Store Set UnionNo( 1 ~ 20 );
	int						m_iNpcObjIndex;
	std::string				m_strTabName[4];
	std::map<int, CItem*>	m_pItems;

	CTCmdNumberInput*			m_pCmdAddItem2DealFromStore;
	CTCmdOpenNumberInputDlg*	m_pCmdOpenNumberInputDlg;
};
#endif

#ifndef _CHARACTER_DLG_H
#define _CHARACTER_DLG_H

#include "../../../TGameCtrl/TDialog.h"
#include "../../Common/CItem.h"
#include "../../Common/DataType.h"

/**
* 캐릭터 정보를 보여주기 위한 다이얼로그 
*
* @Warning		현재(2005/9/12) 대만(New)버젼에는 조합 정보를 보여주지 않는다( XML에서 탭버튼을 없애서 탭을 이동할수 없도록 되어 있다 )
* @Author		최종진
* @Date			2005/9/12
*/
class CCharacterDLG : public CTDialog
{

public:
	CCharacterDLG( int iType );
	virtual ~CCharacterDLG();
	virtual void			Draw();
	virtual unsigned int	Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	virtual void			Update( POINT ptMouse );
	virtual bool			IsVision();
	virtual bool			Create( const char* IDD );


	enum{
		IID_BTN_CLOSE = 10,
		IID_BTN_DIALOG2ICON,
		IID_TABBEDPANE			= 20,
		IID_TAB_BASICINFO		= 21,
		IID_TAB_BASICINFO_BG,
		IID_TAB_BASICINFO_BTN,
		IID_GUAGE_STAMINA		= 24,
		IID_TAB_ABILITY			= 31,
		IID_TAB_ABILITY_BG,
		IID_TAB_ABILITY_BTN,
		IID_BTN_UP_STR,
		IID_BTN_UP_DEX,
		IID_BTN_UP_INT,
		IID_BTN_UP_CON,
		IID_BTN_UP_CHARM,
		IID_BTN_UP_SENSE,
		IID_TAB_UNION			= 41,
		IID_TAB_UNION_BG,
		IID_TAB_UNION_BTN
	};
protected:

	enum{
		IID_TABABLITY = 0,
		IID_TABUNION  = 1,
		IID_TABBASIC = 2
	};
	enum{
		BASIC_INFO,
		ABILITY_INFO,
		UNION_INFO
	};
	bool On_LButtonUP( unsigned iProcID, WPARAM wParam, LPARAM lParam );
	bool On_LButtonDN( unsigned iProcID, WPARAM wParam, LPARAM lParam );


	void DrawBasicInfo();						/// 기본정보 Draw
	void DrawAbilityInfo();						/// 능력치 Draw
	void DrawUnionInfo();						/// 조합정보 Draw


private:
	int			m_iTab;							/// 탭구분
	int			m_iGuageBlueGID;				/// 마나 게이지의 그래픽 ID
	int			m_iGuageYellowGID;				/// 스테미나 게이지의 그래픽 ID
	int			m_iGuageRedGID;					/// 체력 게이지의 그래픽 ID
};




#endif

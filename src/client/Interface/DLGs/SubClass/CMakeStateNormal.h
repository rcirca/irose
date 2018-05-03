#ifndef _CMAKESTATENORMAL_
#define _CMAKESTATENORMAL_

#include "cmakestate.h"
#include "../../../Common/CItem.h"
#include <vector>
class CTComboBox;

/**
* 제조인터페이스 창의 State Class : 제조할 아이템의 선택및 재료를 준비하는 상태
*
* @Author		최종진
* @Date			2005/9/15
*/
class CMakeStateNormal : public CMakeState
{
public:
	CMakeStateNormal( CMakeDLG* pParent);
	virtual ~CMakeStateNormal(){}

	virtual void Draw();
	virtual void Update( POINT ptMouse );
	virtual void Show();
	virtual void Hide();
	virtual unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual void Init();
	virtual void MoveWindow(POINT ptPos ){}
	virtual bool IsModal();
 
private:
	void	OnLButtonUp( unsigned iProcID, WPARAM wParam, LPARAM lParam );
	void	OnLButtonDown( unsigned iProcID, WPARAM wParam, LPARAM lParam );

	
//private:
//	int		m_iSelectedItemIdx;

};
#endif

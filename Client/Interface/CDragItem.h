#ifndef _CDragItem_
#define _CDragItem_

#include "interfacetype.h"
#include "../gamecommon/IObserver.h"
#include <map>
class CIcon;
class CTCommand;

/**
* 드래그N드랍 시스템에서 드래그 시작시 등록되는 Class
*	- 현재 CIcon만 드래그N드랍이 된다
*	- Target 및 해당 타겟에 드랍시 실행할 CTCommand를 가지고 있는다.
*		-- TagetType
*			0						: Ground, 
*			1 ~ DLG_TYPE_MAX - 1	: TDialog
*			DLG_TYPE_MAX			: TARGETALL - 항상 처리
*		-- CTCommand가 Null일경우  CIcon::ExecuteCommand이 실행된다
*	
* @Author		최종진
* @Date			2005/9/5
*/
class CDragItem
{
public:
	CDragItem(void);
	~CDragItem(void);

	void AddTarget( int iTargetType , CTCommand* pCmd );		/// 드랍할수 있는 Target과 Target에 드랍시 실행할 CTCommand 
	void SetIcon( CIcon* pIcon );								/// 드래그할 CIcon의 Clone
	
	CIcon*	GetIcon();
	const std::map< int , CTCommand* >& GetTargets();

	enum{
		TARGET_GROUND,
		TARGET_ALL			= DLG_TYPE_MAX
	};

protected:
	std::map< int , CTCommand* > m_Targets;
	CIcon*	m_pIcon;

};
#endif
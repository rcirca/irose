#ifndef _TCOMMAND_
#define _TCOMMAND_

#include "TGameCtrl.h"
class CTObject;


/**
* 인터페이스 전반적으로 사용되는 Command Pattern에서 사용되는 Command의 Base Class
* - 연관Class				CTMacroCommand, CTCommandQ
*
* @Author	최종진
*
* @Date		2005/8/30
*/
class TGAMECTRL_API CTCommand
{
public:
	CTCommand(void);
	virtual ~CTCommand(void);
	
	virtual	bool Exec(CTObject*) = 0;
	bool	IsWaitExecSucceed(){ return m_bWaitExec; }

protected:
	/// Execute실패시에 보통 TCommand는 큐의 맨뒤로 이동한다.
	/// 그러나 m_bWaitExec가 true일경우에는 삭제하지 않고 큐의 맨 앞에 그래도 대기하여 실행이 성공할때 까지 반복하게 한다.
	/// Default는 false;
	/// => 최종에는 Command마다 우선순위를 두고 우선순위 큐를 구현해야 한다.
	bool	m_bWaitExec;
};


#include <list>
//----------------------------------------------------------------------------------------------------------
/// @Brief					CTCommand를 복수로 실행하기 위한 Multi Command Class
///
/// @Warning				MacroCommand는 Exec인자로 항상 NULL이 가니까 주의할껏
///
/// @Author					2005/8/30 최종진
//----------------------------------------------------------------------------------------------------------
class TGAMECTRL_API CTMacroCommand : public CTCommand
{
public:
	CTMacroCommand( void ){};
	~CTMacroCommand( void );
	virtual bool Exec( CTObject* );
	void	AddSubCommand( CTCommand* pCmd );
protected:
	std::list<CTCommand*>	m_listCommand;
};
#endif
#ifndef _CTCmdSystem_
#define _CTCmdSystem_
#include "tcommand.h"

/// 사용안함
class CTCmdChangeGameState : public CTCommand
{
public:
	CTCmdChangeGameState( int iState );
	virtual ~CTCmdChangeGameState(void){};
	virtual bool Exec( CTObject* );
protected:
	int		m_iState;
};
#endif
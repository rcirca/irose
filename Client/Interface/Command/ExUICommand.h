#ifndef _EXUICOMMAND_
#define _EXUICOMMAND_
#include "../../../tgamectrl/tcommand.h"
#include <string>

/// 게임선택창에서 캐릭터 삭제요청
class CTCmdDeleteCharacter : public CTCommand
{
public:
	CTCmdDeleteCharacter(int iSelectListID, const char* pszName );
	virtual ~CTCmdDeleteCharacter(void){}
	virtual bool Exec( CTObject* pObj );
protected:
	int			m_iSelectListID;
	std::string m_strName;


};

/// 게임선택창에서 삭제 대기중인 캐릭터 되살리기 요청
class CTCmdReviveCharacter : public CTCommand
{
public:
	CTCmdReviveCharacter(int iSelectListID, const char* pszName );
	virtual ~CTCmdReviveCharacter(void){}
	virtual bool Exec( CTObject* pObj );
protected:
	int			m_iSelectListID;
	std::string m_strName;
};
#endif
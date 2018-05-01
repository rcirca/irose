#ifndef _EXTERNALUI_
#define _EXTERNALUI_

#include "..\\..\\Util\\RTTI.h"

enum EXTERNALUI
{
	EUI_LOGIN = 0,
	EUI_SELECT_AVATA,
	EUI_SELECT_SERVER,
	EUI_CREATE_AVATA,
	EUI_WARNING,
	EUI_MAX,
};

class CExternalUIManager;
///
///	Abstraction class for external UI
///

class CExternalUI
{
public:
	///
	/// Create RTTI base
	///
	CREATE_RTTI_BASE( CExternalUI );

public:

	CExternalUI(){};
	virtual ~CExternalUI(){};

	///
	/// set observer class
	///
	virtual void SetEUIObserver( CExternalUIManager* pObserver ) = 0;

};


#endif //_EXTERNALUI_
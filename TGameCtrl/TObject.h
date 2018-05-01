#ifndef _CTOBJECT_
#define _CTOBJECT_
#include <string>

#include "itcontrol.h"

class TGAMECTRL_API CTObject
{
public:
	CTObject(void);
	virtual ~CTObject(void);
	virtual const char* toString() const { return m_strString.c_str(); }

protected:
	std::string	m_strString;

};
#endif
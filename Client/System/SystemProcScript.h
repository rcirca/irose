#ifndef _SYSTEMPROCSCRIPT_
#define _SYSTEMPROCSCRIPT_

#include "../Util/JSingleton.h"


class classLUA;

///
/// Managing script for system state classes
///

class CSystemProcScript : public CJSingleton< CSystemProcScript >
{
private:
	classLUA	   *m_pLUA;

	void			set_initial_globals();
	void			set_global (const char * variable_name, int value);
	void			set_global (const char * variable_name, float value);
	void			set_global (const char * variable_name, const char * value);

public:
	CSystemProcScript(void);
	~CSystemProcScript(void);

	///
	/// Init system script system
	///
	bool			InitSystem();

	void			ClearSystem();

	///
	/// do new file
	///
	int				DoScript( const char* strScriptName );

	///
	/// Prepare system function
	///
	void			PrepareSystemFunction( lua_State *L );

	///
	/// Call lua function in systemfunction.lua
	///
	int				CallLuaFunction( const char* strFuncName, ... );
	int				CallLuaFunction( int& iReturn, const char* strFuncName, ... );
	int				CallLuaFunction( char** pChar, const char* strFuncName, ... );
	int				CallLuaFunction( float& fReturn, const char* strFuncName, ... );
};


#endif //_SYSTEMPROCSCRIPT_
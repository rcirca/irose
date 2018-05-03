/*
	$Header: /Client/LUA_Func.cpp 12    04-04-07 1:50p Jeddli $
*/
#include "stdAFX.h"
#include "LUA_Func.h"

//-------------------------------------------------------------------------------------------------
//
// file scope variables
// for saved return type
//
int f_return_int = 0;
unsigned int f_return_uint = 0;
float f_return_float = 0.0f;
const float * f_return_float3 = NULL;
const char * f_return_string = NULL;

unsigned int get_param_uint (lua_State * L, int& index_from_end, const char * where)
{
	int index = lua_gettop(L) - index_from_end;
	_ASSERT(index >= 1);
	index_from_end++;
	if (!lua_isnumber(L, index)) {
		LogString(LOG_DEBUG, "script_lua: %s().parameter(uint) match failed\n", where);
		return 0;
	}
	return (unsigned int)lua_tonumber(L, index);
}


int get_param_int (lua_State * L, int& index_from_end, const char * where)
{
	int index = lua_gettop(L) - index_from_end;
	_ASSERT(index >= 1);
	index_from_end++;
	if (!lua_isnumber(L, index)) {
		LogString(LOG_DEBUG, "script_lua: %s().parameter(int) match failed\n", where);
		return 0;
	}
	return (int)lua_tonumber(L, index);
}

float get_param_float (lua_State * L, int& index_from_end, const char * where)
{
	int index = lua_gettop(L) - index_from_end;
	_ASSERT(index >= 1);
	index_from_end++;
	if (!lua_isnumber(L, index)) {
		LogString(LOG_DEBUG, "_script_lua: %s().parameter(float) match failed\n", where);
		return 0;
	}
	return (float)lua_tonumber(L, index);
}

float * get_param_float3 (lua_State * L, int& index_from_end, const char * where)
{
	int index = lua_gettop(L) - index_from_end;
	_ASSERT(index >= 1);
	index_from_end += 3;
	if (!lua_isnumber(L, index-2) || !lua_isnumber(L, index-1) || !lua_isnumber(L, index)) {
		LogString(LOG_DEBUG, "script_lua: %s().parameter(float3) match failed\n", where);
		return NULL;
	}
	static float ret[3];
	ret[0] = (float)lua_tonumber(L, index-2);
	ret[1] = (float)lua_tonumber(L, index-1);
	ret[2] = (float)lua_tonumber(L, index);
	return ret;
}

const char * get_param_string (lua_State * L, int& index_from_end, const char * where)
{
	int index = lua_gettop(L) - index_from_end;
	_ASSERT(index >= 1);
	index_from_end++;
	if (!lua_isstring(L, index)) {
		LogString(LOG_DEBUG, "script_lua: %s().parameter(string) match failed\n", where);
		return NULL;
	}
	return lua_tostring(L, index);
}


//-------------------------------------------------------------------------------------------------
//
//
void set_param_uint (lua_State * L, int& return_num, unsigned int val)
{
	f_return_uint = val;
	lua_pushnumber(L, val);
	return_num++;
}

void set_param_int (lua_State * L, int& return_num, int val)
{
	f_return_int = val;
	lua_pushnumber(L, val);
	return_num++;
}

void set_param_float (lua_State * L, int& return_num, float val)
{
	f_return_float = val;
	lua_pushnumber(L, val);
	return_num++;
}

void set_param_float3 (lua_State * L, int& return_num,  const float * val)
{
	f_return_float3 = val;
	lua_pushnumber(L, val[0]);
	lua_pushnumber(L, val[1]);
	lua_pushnumber(L, val[2]);
	return_num += 3;
}

void set_param_string (lua_State * L, int& return_num, const char * val)
{
	f_return_string = val;
	lua_pushstring(L, val);
	return_num++;
}


void set_global (lua_State * L, const char * variable_name, int value)
{
	lua_pushnumber(L, value);
	lua_setglobal(L, variable_name);
}

void set_global (lua_State * L, const char * variable_name, float value)
{
	lua_pushnumber(L, value);
	lua_setglobal(L, variable_name);
}

void set_global (lua_State * L, const char * variable_name, const char * value)
{
	lua_pushstring(L, value);
	lua_setglobal(L, variable_name);
}

//-------------------------------------------------------------------------------------------------
//
// 루아 함수 호출후 리턴된 결과값 갯수를 리턴한다.
int lua_CallFUNC (lua_State *L, const char* function_name, va_list va)
{
	int param_count = 0;
    int iBeforeStackIDX = lua_gettop (L);

	lua_getglobal(L, function_name);
	if (!lua_isfunction(L, -1)) {	// if no lua function exists
		lua_pop(L, param_count+1);	// restore function name and arguments in stack

		char *szMsg=CStr::Printf( "invalid script function( %s ) name ...", function_name);
		g_pCApp->ErrorBOX ("ERROR :: function not found ", szMsg);

		return -1;
	}

	zz_param_type type;
	while ((type = va_arg(va, zz_param_type)) != ZZ_PARAM_END) {
		switch (type) {
			case ZZ_PARAM_UINT:
			{
				set_param_uint		(L, param_count, va_arg(va, unsigned int)	);
				break;
			}
			case ZZ_PARAM_INT:
			{
				set_param_int		(L, param_count, va_arg(va, int)			);
				break;
			}
			case ZZ_PARAM_FLOAT:
			{
				set_param_float		(L, param_count, va_arg(va, float)			);
				break;
			}
			case ZZ_PARAM_FLOAT3:
			{
				set_param_float3	(L, param_count, va_arg(va, const float*)	);
				break;
			}
			case ZZ_PARAM_STRING:
			{
				set_param_string	(L, param_count, va_arg(va, const char*)	);
				break;
			}
		}
	}
	
#ifdef ZZ_LUA50
	int err = lua_pcall(L, param_count, LUA_MULTRET /* nresults */, 0 /* errfunc */);
#else // 40
	int err = lua_call(L, param_count, LUA_MULTRET /* nresults */);
#endif

	param_count = 0;
	if (err == 0) {
		return (lua_gettop (L)-iBeforeStackIDX);
/*
		while (lua_gettop( L ))
		{
			int iType = lua_type( L, lua_gettop( L ) );
			switch( iType )
			{
				case LUA_TNUMBER	: LogString (LOG_DEBUG, "script returned %d \n", lua_tonumber( L, lua_gettop( L ) ) );		break;
				case LUA_TTABLE	: LogString (LOG_DEBUG, "script returned a table \n" ); break;
				case LUA_TSTRING	: LogString (LOG_DEBUG, "script returned %s \n", lua_tostring( L, lua_gettop( L ) ) );		break;
//				case LUA_TBOOLEAN	: LogString (LOG_DEBUG, "script returned %d \n", lua_toboolean( L, lua_gettop( L ) ) );	break;
				default: LogString (LOG_DEBUG, "script returned unknown param  %d \n", iType );	break;
			}
			lua_pop( L, 1 );
	   }
*/
	}

	// error code print
	char *szErrMSG;

	switch ( err ) {
		case LUA_ERRRUN:	// 1
			szErrMSG = "script : error while running the chunk\n";
			break;
		case LUA_ERRFILE:	// 2
			szErrMSG = "script: error opening the file (only for lua_dofile). \n";
			break;
		case LUA_ERRSYNTAX:	// 3
			szErrMSG = "script: syntax error during pre-compilation";
			break;
		case LUA_ERRMEM:	// 4
			szErrMSG = "script: memory allocation error. For such errors, Scriptor does not call _ERRORMESSAGE."; // (see Section 4.7). ");
			break;
		case LUA_ERRERR:	// 5
			szErrMSG = "script: error while running _ERRORMESSAGE. For such errors, Scriptor does not call _ERRORMESSAGE again, to avoid loops.";
			break;
	}

	g_pCApp->ErrorBOX ( szErrMSG, "Script running error..");

	return -1;
}

//-------------------------------------------------------------------------------------------------
int lua_CallIntFUNC (lua_State *pLUA, const char *szFuncName, ...)
{
	if ( !szFuncName )
		return 0x80000000;

	va_list va;
	va_start(va, szFuncName);
    int iResultCnt = lua_CallFUNC(pLUA, szFuncName, va);
	va_end(va);

	if ( iResultCnt > 0 ) {
		// 리턴된 결과가 있으므로 결과 값을 얻어온다.
		lua_GetRETURN(pLUA, iResultCnt);

		LogString (LOG_DEBUG, "lua function( %s ) return %d ... \n", szFuncName, iResultCnt );

	    return iResultCnt;
	}

	LogString (LOG_DEBUG, "lua function( %s ) return %d result ... \n", szFuncName, iResultCnt );

	return 0x80000000;
}

//-------------------------------------------------------------------------------------------------
char*lua_CallCharFUNC (lua_State *pLUA, const char *szFuncName, ...)
{
	va_list va;
	va_start(va, szFuncName);
    int iResultCnt = lua_CallFUNC(pLUA, szFuncName, va);
	va_end(va);

    char *pStr = NULL;
    if ( iResultCnt > 0 )
        lua_GetRETURN(pLUA, &pStr);

    return pStr;
}

//-------------------------------------------------------------------------------------------------
bool lua_GetRETURN (lua_State *pLUA, int &iValue)
{
	int iResult;

	iResult = lua_gettop(pLUA);
    if ( iResult < 1 ) {
		return false;
	}

	if ( !lua_isnumber(pLUA, -1) ) {
        return false;
    }

    iValue = (int)lua_tonumber(pLUA, -1);
    lua_pop(pLUA, -1);

    return true;
}
bool lua_GetRETURN (lua_State *pLUA, char **ppChar)
{
    if ( lua_gettop(pLUA) < 1 ||
        !lua_isstring(pLUA, -1) ) {
        return false;
    }

    *ppChar = (char*)lua_tostring (pLUA, -1);
    lua_pop(pLUA, -1);

    return true;
}

bool lua_GetRETURN (lua_State *pLUA, float& fValue )
{
    int iResult;

	iResult = lua_gettop(pLUA);
    if ( iResult < 1 ) {
		return false;
	}

	if ( !lua_isnumber(pLUA, -1) ) {
        return false;
    }

    fValue = (float)lua_tonumber(pLUA, -1);
    lua_pop(pLUA, -1);

    return true;
}

//-------------------------------------------------------------------------------------------------

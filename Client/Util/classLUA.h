#ifndef	__CLASSLUA_H
#define __CLASSLUA_H
extern "C" {
	#include "lua.h"
    #include "lualib.h"
//	#include "luadebug.h"
//	#include "lauxlib.h"
}
//-------------------------------------------------------------------------------------------------

/*
LUA에서 호출되도록 등록할 함수의 형태.
int lua_Write (lua_State *L)
{
	int i, n = lua_gettop(L);	//

	printf ("-----> lua_Write ArgCnt: %d\n", n);

	char *pStr;
	for (i=1; i<=n; i++) {
		pStr = (char*)lua_tostring(L, i);
		printf ("%s", pStr);
	}

	return 0;
}
*/

/*
    static index는 1부터 !!! 0부터 아님.
*/
class classLUA
{
private :
    long           m_lBufferSize;
    unsigned char *m_pBuffer;

public  :
	lua_State *m_pState;

	classLUA (int iStackSize=0);
	~classLUA ();

	// 실행.
    int  LoadToBuffer (const char *szFileName);
	int  Do_File	(const char *szFileName)	{	return lua_dofile (m_pState, szFileName);	}
    int  Do_String	(const char *pString)		{	return lua_dostring (m_pState, pString);	}
    int  Do_Buffer	(const char *pBuffer, size_t iSize, const char *szNameOfTheChunk=NULL);
    int  Do_Buffer  (void)                      {   return ( ( m_pBuffer ) ? Do_Buffer ((const char*)m_pBuffer, m_lBufferSize) : -1 ); }

	// C 함수 등록.
	void Reg_Function (const char* szFunctionName, lua_CFunction Function);

	// 스텍 검사.  iIndex == 스텍 인덱스
	int	 Stack_QueryType (int iIndex)			{	return lua_type (m_pState, iIndex);		}	// 스텍의 iIndex의 element 자료 형태를 판단한다.
	const char *Stack_TypeName (int iType)		{	return lua_typename (m_pState, iType);	}	// 스텍의 Stack_QueryType에서 얻은 형태의 이름을 얻는다.
	int	 Stack_GetSpaceCount ()					{	return lua_stackspace (m_pState);		}	// 스텍의 남은 공간을 얻는다.
	int	 Stack_GetElementCount()				{	return lua_gettop (m_pState);			}	// 스텍의 element 수를 얻는다.
	void Stack_Clear ()							{	lua_settop (m_pState, 0);				}	// 스텍을 비운다.
	void Stack_SetTop (int iIndex)				{	lua_settop (m_pState, iIndex);			}	// 스텍의 top index를 iIndex로 바꾼다.
	void Stack_PushValue (int iIndex)			{	lua_pushvalue (m_pState, iIndex);		}	// 스텍의 iIndex의 element를 스텍에 push한다.
	void Stack_Remove (int iIndex)				{	lua_remove (m_pState, iIndex);			}	// 스텍의 iIndex를 삭제한다.
	void Stack_Move (int iIndex)				{	lua_insert (m_pState, iIndex);			}	// 스텍의 top element를 iIndex로 옮긴다.
	int  Stack_Tag (int iIndex)					{	return lua_tag (m_pState, iIndex);		}

	void Stack_PushNumber (double dbValue)						{	lua_pushnumber (m_pState, dbValue);			}
	void Stack_PushString (const char *pStr, size_t iLen=-1);
	void Stack_PushUserTag (void *pData, int iTag)				{	lua_pushusertag (m_pState, pData, iTag);	}
	void Stack_PushNil ()										{	lua_pushnil (m_pState);						}
	void Stack_PushCFunction (lua_CFunction Func)				{	lua_pushcfunction (m_pState, Func);			}

	bool Is_Nil		(int iIndex)					{	return lua_isnil		(m_pState, iIndex);			}
	bool Is_Null	(int iIndex)					{	return lua_isnull		(m_pState, iIndex);			}
	bool Is_Number  (int iIndex)					{	return (0!=lua_isnumber		(m_pState, iIndex));	}
	bool Is_String	(int iIndex)					{	return (0!=lua_isstring		(m_pState, iIndex));	}
	bool Is_CFunction(int iIndex)					{	return (0!=lua_iscfunction	(m_pState, iIndex));	}
	bool Is_Table	(int iIndex)					{	return lua_istable		(m_pState, iIndex);			}
	bool Is_Function(int iIndex)					{	return lua_isfunction	(m_pState, iIndex);		}
	bool Is_UserData(int iIndex)					{	return lua_isuserdata	(m_pState, iIndex);			}

	double			 To_Double (int iIndex)			{	return lua_tonumber		(m_pState, iIndex);			}
	int				 To_Number (int iIndex)			{	return (int)lua_tonumber(m_pState, iIndex);			}
	const char		*To_String (int iIndex)			{	return lua_tostring		(m_pState, iIndex);			}
	size_t			 StringLength (int iIndex)		{	return lua_strlen		(m_pState, iIndex);			}
	lua_CFunction	 To_CFunction (int iIndex)		{	return lua_tocfunction	(m_pState, iIndex);			}
	void			*To_UserData (int iIndex)		{	return lua_touserdata	(m_pState, iIndex);			}

	void	GetGlobal (const char *VarName)			{	lua_getglobal (m_pState, VarName);	}
	void	SetGlobal (const char *VarName)			{	lua_setglobal (m_pState, VarName);	}

	bool	GetGlobalValue (const char *VarName, double &dbNumber);
	bool	GetGlobalValue (const char *VarName, int  &iNumber);
	bool	GetGlobalValue (const char *VarName, char *pString);

    void    SetGolbalValue (const char *VarName, double dbValue);
    void    SetGolbalValue (const char *VarName, int    iValue);
    void    SetGolbalValue (const char *VarName, char *szValue);

	int		Call (int iArgCnt, int iResultCnt)		{	return lua_call (m_pState, iArgCnt, iResultCnt);    }
} ;

inline int  classLUA::Do_Buffer (const char *pBuffer, size_t iSize, const char *szNameOfTheChunk)
{
	return lua_dobuffer (m_pState, pBuffer, iSize, szNameOfTheChunk);
}

inline void classLUA::Reg_Function (const char* szFunctionName, lua_CFunction Function)
{
	lua_register(m_pState, szFunctionName, Function);
}
inline void classLUA::Stack_PushString (const char *pStr, size_t iLen)
{
	if ( iLen > 0 )
		lua_pushlstring (m_pState, pStr, iLen);
	else
		lua_pushstring (m_pState, pStr);
}

//-------------------------------------------------------------------------------------------------
#endif

/*
	$Header: /Client/LUA_Func.h 7     04-04-07 1:50p Jeddli $
*/
#ifndef	__LUA_FUNC_H
#define	__LUA_FUNC_H
//-------------------------------------------------------------------------------------------------

unsigned int get_param_uint (lua_State * L, int& index_from_end, const char * where);
int			 get_param_int (lua_State * L, int& index_from_end, const char * where);
float		 get_param_float (lua_State * L, int& index_from_end, const char * where);
float		*get_param_float3 (lua_State * L, int& index_from_end, const char * where);
const char	*get_param_string (lua_State * L, int& index_from_end, const char * where);
void		 set_param_uint (lua_State * L, int& return_num, unsigned int val);
void		 set_param_int (lua_State * L, int& return_num, int val);
void		 set_param_float (lua_State * L, int& return_num, float val);
void		 set_param_float3 (lua_State * L, int& return_num,  const float * val);
void		 set_param_string (lua_State * L, int& return_num, const char * val);

void		set_global (lua_State * L, const char * variable_name, int value);
void		set_global (lua_State * L, const char * variable_name, float value);
void		set_global (lua_State * L, const char * variable_name, const char * value);

//-------------------------------------------------------------------------------------------------
//
// assume that function call evaluate right to left
//
#define ZL_FUNC_BEGIN(FUNCNAME, RETURNTYPE)		\
int lf_##FUNCNAME (lua_State * L)			\
{												\
	int index_from_end = 0;						\
	const char where[] = #FUNCNAME;				\
	int return_num = 0;							\
	RETURNTYPE									\
		FUNCNAME (								\

#define ZL_FUNC_END								\
		)										\
	);											\
	return return_num;							\
}												\

#define ZL_INT						get_param_int(L, index_from_end, where)
#define ZL_UINT						get_param_uint(L, index_from_end, where)
#define ZL_HNODE					get_param_uint(L, index_from_end, where)
#define ZL_FLOAT					get_param_float(L, index_from_end, where)
// CAUTION: ZL_FLOAT3 should be alone in the parameter list. Do not put one more ZL_FLOAT3
#define ZL_FLOAT3					get_param_float3(L, index_from_end, where)
#define ZL_STRING					get_param_string(L, index_from_end, where)

#define RETURNS_NONE				(
#define RETURNS_UINT				set_param_uint(L, return_num, 
#define RETURNS_HNODE				set_param_uint(L, return_num, 
#define RETURNS_INT					set_param_int(L, return_num, 
#define RETURNS_FLOAT				set_param_float(L, return_num, 
//#define RETURNS_FLOAT3 set_param_float3(L, return_num, 
#define RETURNS_STRING				set_param_string(L, return_num, 

#define ZL_REGISTER(func_name)		lua_register(L, #func_name, lf_##func_name);
#define ZL_SETVAR(NAME)				set_global( L, #NAME, NAME ); 

//-------------------------------------------------------------------------------------------------

int		lua_CallFUNC (lua_State *L, const char* function_name, va_list va);


int		lua_CallIntFUNC (lua_State *pLUA, const char *szFuncName, ...);
char*	lua_CallCharFUNC (lua_State *pLUA, const char *szFuncName, ...);
bool	lua_GetRETURN (lua_State *pLUA, int &iValue);
bool	lua_GetRETURN (lua_State *pLUA, char **ppChar);
bool	lua_GetRETURN (lua_State *pLUA, float& fValue );


//-------------------------------------------------------------------------------------------------
#endif
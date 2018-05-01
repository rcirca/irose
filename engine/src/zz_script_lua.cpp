/** 
 * @file zz_script_lua.h
 * @brief script for lua class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    28-nov-2002
 *
 * $Header: /engine/src/zz_script_lua.cpp 10    04-07-16 7:53p Zho $
 * $History: zz_script_lua.cpp $
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-07-16   Time: 7:53p
 * Updated in $/engine/src
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-01-07   Time: 11:03p
 * Updated in $/engine/src
 * Added error log in call()
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-12-23   Time: 10:51a
 * Updated in $/engine/src
 * null filename bug fixed
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-15   Time: 10:11p
 * Updated in $/engine/src
 * print current lua file name in error/warning message
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-15   Time: 2:31p
 * Updated in $/engine/src
 * converted to lua500
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-02   Time: 9:20p
 * Updated in $/engine/src
 * do not use zz_vfs_pkg as defaulst filesystem 
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-02   Time: 6:28p
 * Updated in $/engine/src
 * In type.h, added "zz_vfs.h"
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 23  *****************
 * User: Zho          Date: 03-11-29   Time: 1:10a
 * Updated in $/znzin11/engine/source
 * modified lua script to write .inc file. For now, .inc file does not
 * have any MACROS. All codes are in cpp.
 * 
 * *****************  Version 22  *****************
 * User: Zho          Date: 03-11-28   Time: 10:44p
 * Updated in $/znzin11/engine/source
 * parameter evaluation ordering problem fixed. adjust compiler option not
 * to inline.
 * 
 * *****************  Version 21  *****************
 * User: Zho          Date: 03-11-28   Time: 6:33p
 * Updated in $/znzin11/engine/source
 * fixed zz_script_lua.cpp parameter passing error. Do not compile
 * zz_script_lua.cpp with /O2 option. Why? I have no idea...
 * 
 * *****************  Version 20  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include <stdarg.h>
#include "zz_algebra.h"
#include "zz_interface.h"
#include "zz_view.h"
#include "zz_channel.h"
#include "zz_mesh.h"
#include "zz_visible.h"
#include "zz_renderer.h"
#include "zz_log.h"
#include "zz_script_lua.h"
#include "zz_vfs.h"
#include "zz_shader.h"

#define ZZ_LUA500

// file scope variables
// for saved return type
int f_return_int = 0;
unsigned int f_return_uint = 0;
float f_return_float = 0.0f;
const float * f_return_float3 = NULL;
const char * f_return_string = NULL;

char current_lua_file[ZZ_MAX_STRING];

ZZ_IMPLEMENT_DYNCREATE(zz_script_lua, zz_script)


zz_script_lua::zz_script_lua (const char * filename) : param_count(0), zz_script()
{
#ifdef ZZ_LUA500	
	L = lua_open();
	luaopen_base(L);
	luaopen_string(L);
	luaopen_table(L);
	luaopen_math(L);
	luaopen_io(L);
	luaopen_debug(L);
#else // 401
	L = lua_open(0);
	lua_baselibopen(L);
	lua_mathlibopen(L);
	lua_iolibopen(L);
	lua_strlibopen(L);
#endif

	set_initial_globals();
}

zz_script_lua::~zz_script_lua ()
{
	if (L) lua_close(L);
}

// if filename is not null, buffer is ignored. 
// if filename is null, do buffer
bool zz_script_lua::do_script (const char * filename, const char * buffer)
{
	bool ret = true;
	zz_vfs script_file;

	const char * data = NULL;
	uint32 size = 0;
	if (filename) {
		strcpy(current_lua_file, filename); // save current lua file name for error/warning message printing
		data = (const char *)script_file.open_read_get_data(filename, &size);
		//ZZ_LOG("script_lua: do_script(%s, %d)\n", filename, size);
		if (!data) {
			ZZ_LOG("script_lua: do_scipt(%s) failed. file not found\n", filename);
			ret = false; // error
		}
	}
	else {
		data = buffer;
		size = (int)strlen(buffer);
		if (!data || size == 0) {
			ZZ_LOG("script_lua: do_script(NULL, %s) failed. size 0\n",	buffer);
			ret = false; // error
		}
	}
	int lua_error = 0;

	if (ret != false) {
		lua_error = lua_dobuffer(L, data, size, NULL);
		//script_file.dump();
	}

	if (lua_error != 0) {
		ZZ_LOG("script: do_script::lua_dobuffer(%s:%x) failed\n", filename, buffer);
		switch (lua_error) {
			case LUA_ERRRUN:
				ZZ_LOG("\t error while running the chunk\n");
				break;
			case LUA_ERRSYNTAX:
				ZZ_LOG("\t syntax error during pre-compilation\n");
				break;
			case LUA_ERRMEM:
				ZZ_LOG("\t memory allocation error.\n");
				break;
			case LUA_ERRERR:
				ZZ_LOG("\t error while running _ERRORMESSAGE\n");
				break;
			case LUA_ERRFILE:
				ZZ_LOG("\t error opening the file.\n");
				break;
		}
		ret = false;
	} // end of if (lua_error...
	return true;
}

bool zz_script_lua::get_global (const char * variable_name, int& variable_to_set)
{
	lua_getglobal(L, variable_name);
	if (lua_isnumber(L, -1)) {
		variable_to_set = static_cast<int>(lua_tonumber(L, -1));
		return true;
	}
	return false; // no such global variable
}

bool zz_script_lua::get_global (const char * variable_name, char * variable_to_set)
{
	lua_getglobal(L, variable_name);
	if (lua_isstring(L, -1)) {
		strcpy(variable_to_set, lua_tostring(L, -1));
		return true;
	}
	return false; // no such global variable
}

bool zz_script_lua::get_global (const char * variable_name, float& variable_to_set)
{
	lua_getglobal(L, variable_name);
	if (lua_isnumber(L, -1)) {
		variable_to_set = static_cast<float>(lua_tonumber(L, -1));
		return true;
	}
	return false; // no such global variable
}

void zz_script_lua::set_global (const char * variable_name, int value)
{
	lua_pushnumber(L, value);
	lua_setglobal(L, variable_name);
}

void zz_script_lua::set_global (const char * variable_name, float value)
{
	lua_pushnumber(L, value);
	lua_setglobal(L, variable_name);
}

void zz_script_lua::set_global (const char * variable_name, const char * value)
{
	lua_pushstring(L, value);
	lua_setglobal(L, variable_name);
}

void zz_script_lua::set_param_uint (unsigned int val)
{
	assert(L);
	lua_pushnumber(L, (double)val);
	param_count++;
}

void zz_script_lua::set_param_int (int val)
{
	assert(L);
	lua_pushnumber(L, (double)val);
	param_count++;
}

void zz_script_lua::set_param_float (float val)
{
	assert(L);
	lua_pushnumber(L, (double)val);
	param_count++;
}

void zz_script_lua::set_param_float3 (const float * val)
{
	assert(L);
	assert(val);
	lua_pushnumber(L, (double)val[0]);
	lua_pushnumber(L, (double)val[1]);
	lua_pushnumber(L, (double)val[2]);
	param_count += 3;
}

void zz_script_lua::set_param_string (const char * val)
{
	assert(L);
	lua_pushstring(L, val);
	param_count++;
}

void zz_script_lua::set_param_hnode (void * val)
{
	assert(L);
	lua_pushnumber(L, double(reinterpret_cast<int>(val)));
	param_count++;
}

bool zz_script_lua::call (const char * function_name, va_list va)
{
	param_count = 0;
	lua_getglobal(L, function_name);
	if (!lua_isfunction(L, -1)) { // if no lua function exists
		lua_pop(L, param_count+1); // restore function name and arguments in stack
		return false;
	}

	zz_param_type type;
	int int_type_value;
	float float_type_value;
	unsigned int uint_type_value;
	const float * pfloat_type_value;
	const char * pchar_type_value;
	void * hnode_type_value;
	while ((type = va_arg(va, zz_param_type)) != ZZ_PARAM_END) {
		switch (type) {
			case ZZ_PARAM_UINT:
				uint_type_value = va_arg(va, unsigned int);
				//? uint_type_value = (unsigned int)va_arg(va, int);
				set_param_uint(uint_type_value);
				break;
			case ZZ_PARAM_INT:
				int_type_value = va_arg(va, int);
				set_param_int(int_type_value);
				break;
			case ZZ_PARAM_FLOAT:
				// float types use double argument type
				// in [stdarg.h]
				/*
				+++++++++++++++++++++++++++++++++++++++++++
				Because of parameter passing conventions in C:
				use mode=int for char, and short types
				use mode=double for float types
				use a pointer for array types
				+++++++++++++++++++++++++++++++++++++++++++
				*/
				float_type_value = (float)va_arg(va, double);
				set_param_float(float_type_value);
				break;
			case ZZ_PARAM_FLOAT3:
				pfloat_type_value = va_arg(va, const float *);
				set_param_float3(pfloat_type_value);
				break;
			case ZZ_PARAM_STRING:
				pchar_type_value = va_arg(va, const char *);
				set_param_string(pchar_type_value);
				break;
			case ZZ_PARAM_HNODE:
				hnode_type_value = va_arg(va, void *);
				set_param_hnode(hnode_type_value);
				break;
		}
	}

	
#ifdef ZZ_LUA500
	int err = lua_pcall(L, param_count, 0 /* nresults */, 0 /* errfunc */);
#else // 401
	int err = lua_call(L, param_count, 0 /* nresults */);
#endif

	param_count = 0;

	if (err == 0) {
		return true;
	}
	// error code print
	ZZ_LOG("script_lua: call(%s) failed.\t", function_name);
	switch (err) {
		case LUA_ERRRUN:
			ZZ_LOG("error while running the chunk\n");
			break;
		case LUA_ERRSYNTAX:
			ZZ_LOG("syntax error during pre-compilation\n");
			break;
		case LUA_ERRMEM:
			ZZ_LOG("memory allocation error. For such errors, Lua does not call _ERRORMESSAGE (see Section 4.7). \n");
			break;
		case LUA_ERRERR:
			ZZ_LOG("error while running _ERRORMESSAGE. For such errors, Lua does not call _ERRORMESSAGE again, to avoid loops. \n");
			break;
		case LUA_ERRFILE:
			ZZ_LOG("error opening the file (only for lua_dofile). \n");
			break;
		default:
			ZZ_LOG("unknown error type.\n");
			break;
	}
	return false;
}

//
//bool zz_script_lua::get_table_size (const char * table_name, int& size)
//{
//	lua_getglobal(L, table_name);
//	if (lua_istable(L, -1)) {
//		size = lua_getn(L, -1);
//		return true;
//	}
//	return false;
//}
//
//bool zz_script_lua::get_table_item_string (const char * table_name, int index, char * string_to_set)
//{
//	lua_getglobal(L, table_name);
//	if (lua_istable(L, -1)) {
//		if (lua_getn(L, -1) < index) {
//			return false;
//		}
//		lua_rawgeti(L, -1 /* index of stack */, index /* n-th element (1-base) */);
//		if (lua_isstring(L, -1)) {
//			strcpy(string_to_set, lua_tostring(L, -1));
//			return true;
//		}
//	}
//	return false;
//}

// Lua stack index range :
// 1 <= param_index <= lua_gettop(L)
unsigned int get_param_uint (lua_State * L, int& param_index, const char * where)
{
	assert(param_index <= lua_gettop(L));
	assert(param_index >= 1);
	if (!lua_isnumber(L, param_index)) {
		ZZ_LOG("script_lua: %s().parameter(uint:%d) match failed\n", where, param_index);
		return 0;
	}
	return (unsigned int)lua_tonumber(L, param_index++);
}

int get_param_int (lua_State * L, int& param_index, const char * where)
{
	assert(param_index <= lua_gettop(L));
	assert(param_index >= 1);
	if (!lua_isnumber(L, param_index)) {
		ZZ_LOG("script_lua: %s().parameter(int;%d) match failed\n", where, param_index);
		return 0;
	}
	return (int)lua_tonumber(L, param_index++);
}

const char * get_param_string (lua_State * L, int& param_index, const char * where)
{
	assert(param_index <= lua_gettop(L));
	assert(param_index >= 1);
	if (!lua_isstring(L, param_index)) {
		ZZ_LOG("script_lua: %s().parameter(string:%d) match failed\n", where, param_index);
		return NULL;
	}
	return lua_tostring(L, param_index++);
}

float get_param_float (lua_State * L, int& param_index, const char * where)
{
	assert(param_index <= lua_gettop(L));
	assert(param_index >= 1);
	if (!lua_isnumber(L, param_index)) {
		ZZ_LOG("_script_lua: %s().parameter(float:%d) match failed\n", where, param_index);
		return 0;
	}
	return (float)lua_tonumber(L, param_index++);
}

float * get_param_float3 (lua_State * L, int& param_index, const char * where)
{
	assert(param_index <= lua_gettop(L));
	assert(param_index >= 1);
	if (!lua_isnumber(L, param_index-2) || !lua_isnumber(L, param_index-1) || !lua_isnumber(L, param_index)) {
		ZZ_LOG("script_lua: %s().parameter(float3:%d) match failed\n", where, param_index);
		return NULL;
	}
	static float ret[3];
	ret[0] = (float)lua_tonumber(L, param_index++);
	ret[1] = (float)lua_tonumber(L, param_index++);
	ret[2] = (float)lua_tonumber(L, param_index++);
	return ret;
}

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


// sample) zz_api_define.inc
/*
//--------------------------------------------------------------------------------
// setLight
//--------------------------------------------------------------------------------
int zz_lua_setLight (lua_State * L)
{
	int param_index = 1;
	const char where[] = "setLight";
	int return_num = 0;

	HNODE hLight = get_param_uint( L, param_index, where );
	ZSTRING pProperty = get_param_string( L, param_index, where );
	float fValue1 = get_param_float( L, param_index, where );
	float fValue2 = get_param_float( L, param_index, where );
	float fValue3 = get_param_float( L, param_index, where );

	set_param_int( L, return_num, setLight( hLight, pProperty, fValue1, fValue2, fValue3 ) );

	return return_num;
}
*/

// load lua interface definition and register function
#include "zz_script_lua.inc"

#define ZL_SETVAR(NAME) \
	set_global( #NAME, NAME ); \

// lua error message receiver
// redefinition of luaB__ALERT() in lbaselib.c
static int my_ERROR_MESSAGE (lua_State *L) {
	ZZ_LOG("script_lua:error: %s in [%s]\n", luaL_check_string(L, 1), current_lua_file);
	return 0;
}

static int my_ALERT_MESSAGE (lua_State * L)
{
	ZZ_LOG("script_lua:alert: %s in [%s]\n", luaL_check_string(L, 1), current_lua_file);
	return 0;
}

void zz_script_lua::set_initial_globals ()
{
	// lua alert message handler
	lua_register(L, "_ALERT", my_ALERT_MESSAGE);

	// lua error message handler
	lua_register(L, "_ERRORMESSAGE", my_ERROR_MESSAGE);

	// for initial setting
	// lua_register(L, "setScreen", zz_lua_setScreen);

	register_api_interfaces(L); // in zz_script_lua.inc

	// set global variables or enum
	ZL_SETVAR( ZZ_KEY_NONE )
	ZL_SETVAR( ZZ_KEY_0 )
	ZL_SETVAR( ZZ_KEY_1 )
	ZL_SETVAR( ZZ_KEY_2 )
	ZL_SETVAR( ZZ_KEY_3 )
	ZL_SETVAR( ZZ_KEY_4 )
	ZL_SETVAR( ZZ_KEY_5 )
	ZL_SETVAR( ZZ_KEY_6 )
	ZL_SETVAR( ZZ_KEY_7 )
	ZL_SETVAR( ZZ_KEY_8 )
	ZL_SETVAR( ZZ_KEY_9 )
	ZL_SETVAR( ZZ_KEY_NONE )
	ZL_SETVAR( ZZ_KEY_ESC )
	ZL_SETVAR( ZZ_KEY_LEFT )
	ZL_SETVAR( ZZ_KEY_RIGHT )
	ZL_SETVAR( ZZ_KEY_UP )
	ZL_SETVAR( ZZ_KEY_DOWN )
	ZL_SETVAR( ZZ_KEY_INSERT )
	ZL_SETVAR( ZZ_KEY_DELETE )
	ZL_SETVAR( ZZ_KEY_HOME )
	ZL_SETVAR( ZZ_KEY_END )
	ZL_SETVAR( ZZ_KEY_PAGEUP )
	ZL_SETVAR( ZZ_KEY_PAGEDOWN )
	ZL_SETVAR( ZZ_KEY_F1 )
	ZL_SETVAR( ZZ_KEY_F2 )
	ZL_SETVAR( ZZ_KEY_F3 )
	ZL_SETVAR( ZZ_KEY_F4 )
	ZL_SETVAR( ZZ_KEY_F5 )
	ZL_SETVAR( ZZ_KEY_F6 )
	ZL_SETVAR( ZZ_KEY_F7 )
	ZL_SETVAR( ZZ_KEY_F8 )
	ZL_SETVAR( ZZ_KEY_F9 )
	ZL_SETVAR( ZZ_KEY_F10 )
	ZL_SETVAR( ZZ_KEY_F11 )
	ZL_SETVAR( ZZ_KEY_F12 )
	ZL_SETVAR( ZZ_KEY_SHIFT )
	ZL_SETVAR( ZZ_KEY_CTRL )
	ZL_SETVAR( ZZ_KEY_ALT )
	ZL_SETVAR( ZZ_KEY_A )
	ZL_SETVAR( ZZ_KEY_B )
	ZL_SETVAR( ZZ_KEY_C )
	ZL_SETVAR( ZZ_KEY_D )
	ZL_SETVAR( ZZ_KEY_E )
	ZL_SETVAR( ZZ_KEY_F )
	ZL_SETVAR( ZZ_KEY_G )
	ZL_SETVAR( ZZ_KEY_H )
	ZL_SETVAR( ZZ_KEY_I )
	ZL_SETVAR( ZZ_KEY_J )
	ZL_SETVAR( ZZ_KEY_K )
	ZL_SETVAR( ZZ_KEY_L )
	ZL_SETVAR( ZZ_KEY_M )
	ZL_SETVAR( ZZ_KEY_N )
	ZL_SETVAR( ZZ_KEY_O )
	ZL_SETVAR( ZZ_KEY_P )
	ZL_SETVAR( ZZ_KEY_Q )
	ZL_SETVAR( ZZ_KEY_R )
	ZL_SETVAR( ZZ_KEY_S )
	ZL_SETVAR( ZZ_KEY_T )
	ZL_SETVAR( ZZ_KEY_U )
	ZL_SETVAR( ZZ_KEY_V )
	ZL_SETVAR( ZZ_KEY_W )
	ZL_SETVAR( ZZ_KEY_X )
	ZL_SETVAR( ZZ_KEY_Y )
	ZL_SETVAR( ZZ_KEY_Z )

	ZL_SETVAR( ZZ_INTERP_NONE )
	ZL_SETVAR( ZZ_INTERP_LINEAR )
	ZL_SETVAR( ZZ_INTERP_SLERP )
	ZL_SETVAR( ZZ_INTERP_SQUAD )
	ZL_SETVAR( ZZ_INTERP_CATMULLROM )
	ZL_SETVAR( ZZ_INTERP_TCB )

	ZL_SETVAR( ZZ_VF_NONE )
	ZL_SETVAR( ZZ_VF_POSITION )
	ZL_SETVAR( ZZ_VF_NORMAL )
	ZL_SETVAR( ZZ_VF_COLOR )
	ZL_SETVAR( ZZ_VF_BLEND_WEIGHT )
	ZL_SETVAR( ZZ_VF_BLEND_INDEX )
	ZL_SETVAR( ZZ_VF_TANGENT )
	ZL_SETVAR( ZZ_VF_UV0 )
	ZL_SETVAR( ZZ_VF_UV1 )
	ZL_SETVAR( ZZ_VF_UV2 )
	ZL_SETVAR( ZZ_VF_UV3 )

	ZL_SETVAR( ZZ_TYPE_NONE )
	ZL_SETVAR( ZZ_TYPE_VISIBLE )
	ZL_SETVAR( ZZ_TYPE_ANIMATABLE )
	ZL_SETVAR( ZZ_TYPE_MORPHER    )
	ZL_SETVAR( ZZ_TYPE_SKELETON   )
	ZL_SETVAR( ZZ_TYPE_MODEL      )
	ZL_SETVAR( ZZ_TYPE_PARTICLE   )
	ZL_SETVAR( ZZ_TYPE_TRAIL      )
	ZL_SETVAR( ZZ_TYPE_TERRAIN    )

	ZL_SETVAR( ZZ_CL_NONE )
	ZL_SETVAR( ZZ_CL_SPHERE )
	ZL_SETVAR( ZZ_CL_AABB )
	ZL_SETVAR( ZZ_CL_OBB )
	ZL_SETVAR( ZZ_CL_POLYGON )

	ZL_SETVAR( ZZ_DP_HIGHEST )
	ZL_SETVAR( ZZ_DP_HIGH    )
	ZL_SETVAR( ZZ_DP_NORMAL  )
	ZL_SETVAR( ZZ_DP_LOW     )
	ZL_SETVAR( ZZ_DP_LOWEST  )

	ZL_SETVAR( ZZ_BLEND_ZERO )
    ZL_SETVAR( ZZ_BLEND_ONE )
    ZL_SETVAR( ZZ_BLEND_SRCCOLOR )
    ZL_SETVAR( ZZ_BLEND_INVSRCCOLOR )
    ZL_SETVAR( ZZ_BLEND_SRCALPHA )
    ZL_SETVAR( ZZ_BLEND_INVSRCALPHA )
    ZL_SETVAR( ZZ_BLEND_DESTALPHA )
    ZL_SETVAR( ZZ_BLEND_INVDESTALPHA )
    ZL_SETVAR( ZZ_BLEND_DESTCOLOR )
    ZL_SETVAR( ZZ_BLEND_INVDESTCOLOR )
    ZL_SETVAR( ZZ_BLEND_SRCALPHASAT )
    ZL_SETVAR( ZZ_BLEND_BOTHSRCALPHA )
    ZL_SETVAR( ZZ_BLEND_BOTHINVSRCALPHA )
    ZL_SETVAR( ZZ_BLEND_BLENDFACTOR )
    ZL_SETVAR( ZZ_BLEND_INVBLENDFACTOR )
    
	ZL_SETVAR( ZZ_BLENDOP_ADD )
    ZL_SETVAR( ZZ_BLENDOP_SUBTRACT )
    ZL_SETVAR( ZZ_BLENDOP_REVSUBTRACT )
    ZL_SETVAR( ZZ_BLENDOP_MIN )
    ZL_SETVAR( ZZ_BLENDOP_MAX )

	ZL_SETVAR( SHADER_FORMAT_DEFAULT	)
	ZL_SETVAR( SHADER_FORMAT_FIRSTMAP	 )
	ZL_SETVAR( SHADER_FORMAT_SECONDMAP )
	ZL_SETVAR( SHADER_FORMAT_SPECULARMAP )
	ZL_SETVAR( SHADER_FORMAT_IRADIANCEMAP )
	ZL_SETVAR( SHADER_FORMAT_LIGHTMAP )
	ZL_SETVAR( SHADER_FORMAT_SHADOWMAP )
	ZL_SETVAR( SHADER_FORMAT_FOG )
}

int zz_script_lua::get_return_int (void)
{
	return f_return_int;
}

unsigned int zz_script_lua::get_return_uint (void)
{
	return f_return_uint;
}

float zz_script_lua::get_return_float (void)
{
	return f_return_float;
}

const float * zz_script_lua::get_return_float3 (void)
{
	return f_return_float3;
}

const char * zz_script_lua::get_return_string (void)
{
	return f_return_string;
}

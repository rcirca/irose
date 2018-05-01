#include "stdafx.h"
#include ".\systemprocscript.h"

#include "System_Func.h"
#include "System_FUNC_DEF.inc"

#include "../interface/interfacetype.h"
#include "../interface/dlgs/cmenudlg.h"





#define		TL_SETVAR(NAME)		set_global( #NAME, NAME );




CSystemProcScript		__SystemProcScript;


CSystemProcScript::CSystemProcScript(void)
{
	m_pLUA = NULL;
}

CSystemProcScript::~CSystemProcScript(void)
{
	ClearSystem();
}

///
/// Init system script system
///
bool CSystemProcScript::InitSystem()
{
	m_pLUA = new classLUA();

	if( m_pLUA == NULL )
		return false;


	PrepareSystemFunction( m_pLUA->m_pState );


	int iResult = 0;

	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( "Scripts\\SystemFunc.lua" ) == false )
	{
		g_pCApp->ErrorBOX( "File open error...", "Scripts\\SystemFunc.lua" );
		(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );	
		return false;
	}

	pFileSystem->ReadToMemory();

	iResult = m_pLUA->Do_Buffer( (const char*)( pFileSystem->GetData() ), pFileSystem->GetSize() );

	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );

	switch( iResult ) {
		// error codes for lua_do* 
		case LUA_ERRRUN		:
		case LUA_ERRFILE	:	// 2
		case LUA_ERRSYNTAX	:	// 3
		case LUA_ERRMEM		:
		case LUA_ERRERR		:
			LogString (LOG_NORMAL, "Script File ERROR[ %d ] %s\n", iResult, "Scripts\\SystemFunc.lua" );
			SAFE_DELETE( m_pLUA );
			return false;
	}	

	set_initial_globals();

	return true;
}

void CSystemProcScript::ClearSystem()
{
	if( m_pLUA != NULL )
	{
		delete m_pLUA;
		m_pLUA = NULL;
	}
}


///
/// do new file
///
int CSystemProcScript::DoScript( const char* strScriptName )
{
	int iResult = 0;

	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( strScriptName ) == false )
	{
		g_pCApp->ErrorBOX( "File open error...", (char*)strScriptName );
		(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );		

		return 0;
	}

	pFileSystem->ReadToMemory();

	iResult = m_pLUA->Do_Buffer( (const char*)( pFileSystem->GetData() ), pFileSystem->GetSize() );

	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );

	return iResult;
}


///
/// Prepare system function
///
void CSystemProcScript::PrepareSystemFunction( lua_State *L )
{	
	#include "System_FUNC_REG.inc"	
}

///
/// Call lua function in systemfunction.lua
///
int CSystemProcScript::CallLuaFunction( const char* strFuncName, ... )
{
	va_list va;
	va_start( va, strFuncName);
    int iResultCnt = lua_CallFUNC( m_pLUA->m_pState, strFuncName, va );
	va_end(va);

	if ( iResultCnt > 0 ) 
	{
	    return iResultCnt;
	}

	LogString (LOG_DEBUG, "lua function( %s ) return %d result ... \n", strFuncName, iResultCnt );
	return 0;
}

int CSystemProcScript::CallLuaFunction( int& iReturn, const char* strFuncName, ... )
{
	va_list va;
	va_start( va, strFuncName);
    int iResultCnt = lua_CallFUNC( m_pLUA->m_pState, strFuncName, va );
	va_end(va);

	if ( iResultCnt > 0 ) 
	{
		LogString (LOG_DEBUG, "lua function( %s ) return %d ... \n", strFuncName, iResultCnt );		
		if( lua_GetRETURN( m_pLUA->m_pState, iReturn ) == false )
			return 0;

	    return iResultCnt;
	}

	LogString (LOG_DEBUG, "lua function( %s ) return %d result ... \n", strFuncName, iResultCnt );
	return 0;
}

int CSystemProcScript::CallLuaFunction( char** pChar, const char* strFuncName, ... )
{
	va_list va;
	va_start( va, strFuncName);
    int iResultCnt = lua_CallFUNC( m_pLUA->m_pState, strFuncName, va );
	va_end(va);

	if ( iResultCnt > 0 ) 
	{
		LogString (LOG_DEBUG, "lua function( %s ) return %d ... \n", strFuncName, iResultCnt );

		if( lua_GetRETURN( m_pLUA->m_pState, pChar ) == false )
			return 0;

	    return iResultCnt;
	}

	LogString (LOG_DEBUG, "lua function( %s ) return %d result ... \n", strFuncName, iResultCnt );
	return 0;
}

int CSystemProcScript::CallLuaFunction( float& fResult, const char* strFuncName, ... )
{
	va_list va;
	va_start( va, strFuncName);
    int iResultCnt = lua_CallFUNC( m_pLUA->m_pState, strFuncName, va );
	va_end(va);

	if ( iResultCnt > 0 ) 
	{
		LogString (LOG_DEBUG, "lua function( %s ) return %d ... \n", strFuncName, iResultCnt );
		if( lua_GetRETURN( m_pLUA->m_pState, fResult ) == false )
			return 0;

	    return iResultCnt;
	}

	LogString (LOG_DEBUG, "lua function( %s ) return %d result ... \n", strFuncName, iResultCnt );
	return 0;
}
void CSystemProcScript::set_initial_globals()
{
	TL_SETVAR( DLG_TYPE_MENU );

	TL_SETVAR( DLG_TYPE_CHAR );
	TL_SETVAR( DLG_TYPE_ITEM );
	TL_SETVAR( DLG_TYPE_SKILL );
	TL_SETVAR( DLG_TYPE_QUEST );
	TL_SETVAR( DLG_TYPE_COMMUNITY );
	TL_SETVAR( DLG_TYPE_CLAN );
	TL_SETVAR( DLG_TYPE_HELP );
	TL_SETVAR( DLG_TYPE_OPTION );
	 
	TL_SETVAR( MENU_BTN_CHAR );
	TL_SETVAR( MENU_BTN_ITEM );
	TL_SETVAR( MENU_BTN_SKILL );
	TL_SETVAR( MENU_BTN_QUEST );
	TL_SETVAR( MENU_BTN_COMMUNITY );
	TL_SETVAR( MENU_BTN_CLAN );
	TL_SETVAR( MENU_BTN_HELP );
	TL_SETVAR( MENU_BTN_INFO );
	TL_SETVAR( MENU_BTN_OPTION );
	TL_SETVAR( MENU_BTN_EXIT );
}

void CSystemProcScript::set_global (const char * variable_name, int value)
{
	lua_pushnumber(m_pLUA->m_pState, value);
	lua_setglobal(m_pLUA->m_pState, variable_name);
}

void CSystemProcScript::set_global (const char * variable_name, float value)
{
	lua_pushnumber(m_pLUA->m_pState, value);
	lua_setglobal(m_pLUA->m_pState, variable_name);
}

void CSystemProcScript::set_global (const char * variable_name, const char * value)
{
	lua_pushstring(m_pLUA->m_pState, value);
	lua_setglobal(m_pLUA->m_pState, variable_name);
}

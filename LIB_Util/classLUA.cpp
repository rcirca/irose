
// #include "stdAFX.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <io.h>
#include "classLUA.h"
#include "classLOG.h"

//-------------------------------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma	comment (lib, "BCB5_lua.lib")
    #pragma	comment (lib, "BCB5_lualib.lib")
#else
    #pragma	comment (lib, "lua.lib")
    #pragma	comment (lib, "lualib.lib")
#endif

//-------------------------------------------------------------------------------------------------

classLUA::classLUA (int iStackSize)
{
	m_pState = lua_open (0);

    if ( m_pState ) {
        lua_baselibopen(m_pState);
        lua_mathlibopen(m_pState);
        lua_iolibopen(m_pState);
    }

    m_pBuffer = NULL;
	m_lBufferSize = 0;
}
classLUA::~classLUA ()
{
    if ( m_pState )
    	lua_close (m_pState);

    if ( m_pBuffer ) delete[] m_pBuffer;
}




//-------------------------------------------------------------------------------------------------
bool classLUA::GetGlobalValue (const char *VarName, double &dbNumber)
{
	this->GetGlobal( VarName );	// 	lua_getglobal (m_pState, VarName)
	if ( Is_Number( -1 ) ) {	//	lua_isstring  (m_pState, -1)
		dbNumber = To_Double( -1 );
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
bool classLUA::GetGlobalValue (const char *VarName, int  &iNumber)
{
	this->GetGlobal( VarName );	// 	lua_getglobal (m_pState, VarName)
	if ( Is_Number( -1 ) ) {	//	lua_isstring  (m_pState, -1)
		iNumber = To_Number( -1 );
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
bool classLUA::GetGlobalValue (const char *VarName, char *pString)
{
	this->GetGlobal( VarName );	// 	lua_getglobal (m_pState, VarName)
	if ( Is_String( -1 ) ) {	//	lua_isstring  (m_pState, -1)
		strcpy (pString, To_String( -1 ));
		return true;
	}

	return false;
}


//-------------------------------------------------------------------------------------------------
void classLUA::SetGolbalValue (const char *VarName, double dbValue)
{
    this->Stack_PushNumber (dbValue);
    this->SetGlobal (VarName);
}
//-------------------------------------------------------------------------------------------------
void classLUA::SetGolbalValue (const char *VarName, int    iValue)
{
    this->Stack_PushNumber (iValue);
    this->SetGlobal (VarName);
}
//-------------------------------------------------------------------------------------------------
void classLUA::SetGolbalValue (const char *VarName, char *szValue)
{
    this->Stack_PushString (szValue);
    this->SetGlobal (VarName);
}

//-------------------------------------------------------------------------------------------------
int classLUA::LoadToBuffer (const char *szFileName)
{
    // Check File exist
    if ( _access (szFileName, 0) == -1 ) {
        return false;
    }

    FILE *fp;
    long  lSize;

    fp = fopen(szFileName, "rb");
    if ( fp == NULL )
        return 0;

    fseek (fp, 0L, SEEK_END);
    lSize = ftell (fp);

    if ( lSize > 0 ) {
        m_pBuffer = new unsigned char[ lSize ];
        fseek (fp, 0L, SEEK_SET);
        fread (m_pBuffer, sizeof(unsigned char), lSize, fp);

        m_lBufferSize = lSize;
    }

    fclose (fp);
    return 1;
}


//-------------------------------------------------------------------------------------------------


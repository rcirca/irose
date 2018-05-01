// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef	__SERVER
#include <VCL.h>
#else
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define	_WIN32_WINNT			0x0501

// Windows Header Files:
#include <windows.h>
#include <winUser.h>
#include <mmsystem.h>
// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <crtdbg.h>

#include <d3d9.h>
#include <D3DX9.h>

// TODO: reference additional headers your program requires here
//-------------------------------------------------------------------------------------------------

typedef	LPDIRECT3DDEVICE9			LPD3DDEVICE;
typedef	IDirect3DTexture9			ID3DTexture;
typedef	LPDIRECT3DTEXTURE9			LPD3DTEXTURE;
typedef	D3DMATERIAL9				D3DMATERIAL;
typedef	LPDIRECT3DVERTEXBUFFER9		LPD3DVERTEXBUFFER;

//#define _D_RELEASE

#define	ZZ_MEM_OFF	// not use zz_mem
//#define _USE_BG		// Bug Guard
//#define FRAROSE
#ifdef FRAROSE
	#define _GBC
#endif

//#define _NEWUI
//#define __APPLY_EXTAND_QUEST_VAR
//#define __USE_IME

#include "zz_interface.h"

// #pragma warning(default : 4786)
#pragma warning( disable : 4651 ) 

#include "util\CVector.h"
#include "common\DataTYPE.h"

#include "util\CD3DUtil.h"
#include "util\classSTR.h"
#include "util\classLOG.h"
#include "util\classLUA.h"
#include "util\classHASH.h"
#include "util\classUTIL.h"
#include "util\SLLIST.h"
#include "util\DLLIST.h"
#include "util\CFileLIST.h"

#include "util\CGameSTR.h"
#include "util\CGameSTB.h"

#include "CApplication.h"

#endif
//-------------------------------------------------------------------------------------------------

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif


#ifndef	GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
#endif

//-------------------------------------------------------------------------------------------------


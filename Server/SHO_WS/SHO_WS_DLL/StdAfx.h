// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__2E87C312_C044_4A44_9313_866BD67485F7__INCLUDED_)
#define AFX_STDAFX_H__2E87C312_C044_4A44_9313_866BD67485F7__INCLUDED_

#define	_WIN32_WINNT 0x0500

#ifndef	__SERVER
	#define	__SERVER
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <crtdbg.h>

#include "LIB_Util.h"
#include "classHASH.h"
#include "classTIME.h"
#include "classLOG.h"
#include "classSTR.h"
#include "classSYNCOBJ.h"
#include "CDataPOOL.h"
#include "DLLIST.h"

#include "CVector.h"
//#include "DataTYPE.h"
#include "Net_Prototype.h"

#include "IOCPSocketSERVER.h"
#include "ioDataPOOL.h"
#include "CRandom.h"

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2E87C312_C044_4A44_9313_866BD67485F7__INCLUDED_)

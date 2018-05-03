// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BC125CC8_4AFD_4EA7_A465_E7BFBABBD26E__INCLUDED_)
#define AFX_STDAFX_H__BC125CC8_4AFD_4EA7_A465_E7BFBABBD26E__INCLUDED_



#define	_WIN32_WINNT 0x0500
#define	__SERVER



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// TODO: reference additional headers your program requires here
#include <windows.h>
#include <crtdbg.h>
#include <assert.h>

#define	__SHO_WS 
#define ENABLE_CHATROOM		// Enables the chatroom functions

#ifdef FRAROSE
	#define __KCHS_BATTLECART__
#endif


#include "LIB_Util.h"
#include "classHASH.h"
#include "classTIME.h"
#include "classLOG.h"
#include "classSTR.h"
#include "classSTB.h"
#include "classSYNCOBJ.h"
#include "CRandom.h"
#include "CDataPOOL.h"
#include "DLLIST.h"
#include "CDLList.h"
#include "CSLList.h"

#include "CVector.h"
#include "Net_Prototype.h"
#include "ioDataPOOL.h"			//	Net_Prototype.h 보다 뒤에 !!!

#include "IOCPSocketSERVER.h"
#include "CClientSOCKET.h"
#include "CSocketWND.h"

typedef		classSTB	CGameSTB;	

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BC125CC8_4AFD_4EA7_A465_E7BFBABBD26E__INCLUDED_)

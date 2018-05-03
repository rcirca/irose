#include "StdAfx.h"
#include "statistics.h"
#include "resource.h"
#include <assert.h>


Statistics::Statistics(void)
{
	m_BytesRead=0;
	m_BytesSent=0;
	m_BytesReadLast=0;
	m_BytesSentLast=0;
	m_StartTime=0;
	m_StartTimeLast=0;
	m_Connection=0;
	InitializeCriticalSection(&m_cs);

	m_hThread=CreateThread(NULL,0,ThreadFunc,this,0,NULL);
}

Statistics::~Statistics(void)
{
  ShowWindow(m_hWnd,SW_HIDE);
  DestroyWindow(m_hWnd);
 
  CloseHandle(m_hThread);
  DeleteCriticalSection(&m_cs);

}

void Statistics::SetHWND(HWND hWnd)
{
	m_hWnd=hWnd;
}

inline void Statistics::SetAvgSent(int data)
{
	SetDlgItemInt(m_hWnd,IDC_AVGSENT,data,FALSE);
}

inline void Statistics::SetAvgRead(int data)
{
	SetDlgItemInt(m_hWnd,IDC_AVGREAD,data,FALSE);
}

inline void Statistics::SetCurSent(int data)
{
	SetDlgItemInt(m_hWnd,IDC_CURSENT,data,FALSE);
}

inline void Statistics::SetCurRead(int data)
{
	SetDlgItemInt(m_hWnd,IDC_CURREAD,data,FALSE);
}

inline void Statistics::SetCurConn(int data)
{
    SetDlgItemInt(m_hWnd,IDC_CURCONN,data,FALSE);
}

void Statistics::DataInit(void)
{
   SetAvgSent(0);
   SetAvgRead(0);
   SetCurRead(0);
   SetCurSent(0);
   SetCurConn(0);
}

void Statistics::CalcStatistics(void)
{
	ULONG bps=0,tick=0,elapsed=0;

	tick=GetTickCount();

	elapsed=(tick-m_StartTime)/1000;

	if(elapsed==0)
		return ;

	bps=m_BytesSent/elapsed;
	SetAvgSent(bps);

	bps=m_BytesRead/elapsed;
	SetAvgRead(bps);

	elapsed=(tick-m_StartTimeLast)/1000;
	if(elapsed==0)
		return ;

	bps=m_BytesSentLast/elapsed;
	SetCurSent(bps);

	bps=m_BytesReadLast/elapsed;
	SetCurRead(bps);

	SetCurConn(m_Connection);

	InterlockedExchange(&m_BytesSentLast,0);
	InterlockedExchange(&m_BytesReadLast,0);

	m_StartTimeLast=tick;
	
}

void Statistics::IncreaseBytesSent(DWORD dwBytesTransferred)
{
	EnterCriticalSection(&m_cs);
	m_BytesSent+=dwBytesTransferred;
	m_BytesSentLast+=dwBytesTransferred;
	LeaveCriticalSection(&m_cs);
}

void Statistics::IncreaseBytesRead(DWORD dwBytesTransferred)
{
	EnterCriticalSection(&m_cs);
	m_BytesRead+=dwBytesTransferred;
	m_BytesReadLast+=dwBytesTransferred;
	LeaveCriticalSection(&m_cs);
}

DWORD __stdcall Statistics::ThreadFunc(PVOID param)
{
   Statistics* pS=(Statistics*)param;

   MSG msg;
   HWND hWnd=CreateDialogParam(NULL,MAKEINTRESOURCE(IDD_STATE),NULL,DlgProc,(LPARAM)pS);
   assert(hWnd);
   pS->SetHWND(hWnd);
   ShowWindow(hWnd,SW_SHOW);
   
   pS->DataInit();

   while ((GetMessage(&msg, NULL, 0, 0)) != 0 ) 
   {
    
    if (!IsWindow(hWnd) || !IsDialogMessage(hWnd, &msg)) 
    { 
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    } 
   }

   return 0;
}

BOOL CALLBACK Statistics::DlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    static Statistics* pS=NULL;

    switch (message) 
    { 
        case WM_INITDIALOG:  
			pS=(Statistics*)lParam;
			SetTimer(hwndDlg,0,1000,NULL);
            return TRUE; 

		case WM_TIMER:
            pS->CalcStatistics();
			return TRUE;       
    } 
    return FALSE; 
} 


void Statistics::SetTime(void)
{
	m_StartTime=m_StartTimeLast=GetTickCount();
}

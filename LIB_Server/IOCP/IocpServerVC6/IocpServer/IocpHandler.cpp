#include "StdAfx.h"
#include "iocphandler.h"
#include <assert.h>

typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define BEGINTHREADEX(psa, cbStack, pfnStartAddr, \
   pvParam, fdwCreate, pdwThreadId)                 \
      ((HANDLE)_beginthreadex(                      \
         (void *)        (psa),                     \
         (unsigned)      (cbStack),                 \
         (PTHREAD_START) (pfnStartAddr),            \
         (void *)        (pvParam),                 \
         (unsigned)      (fdwCreate),               \
         (unsigned *)    (pdwThreadId)))

IocpHandler::IocpHandler(void)
{
	m_hIOCP=NULL;
}

IocpHandler::~IocpHandler(void)
{
	if(m_hIOCP)
		CloseHandle(m_hIOCP);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// IOCP 생성 및 초기화
// nMaxNumberOfConcurrentThreads: Concurrent Thread 의 갯수
// pnOutErrCode: 에러 상황 시에 외부로 던져질 에러코드, NULL 이 들어오면 에러코드를 던지지 않는다.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL IocpHandler::Create(int nMaxNumberOfConcurrentThreads, int* pnOutErrCode)
{
	m_hIOCP=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,nMaxNumberOfConcurrentThreads);
    if(m_hIOCP==NULL && pnOutErrCode!=NULL)
	{        
       *pnOutErrCode=GetLastError(); 
	}

	return (m_hIOCP!=NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// 소켓과 IOCP 와 연결
// hSocket		 : IOCP 와 연결될 소켓
// CompletionKey : 연결되면서 쓰여질 Completion Key
// pnOutErrCode  : 위와 상동
//
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL IocpHandler::Associate(SOCKET hSocket, ULONG_PTR CompletionKey, int* pnOutErrCode)
{
	assert(hSocket!=INVALID_SOCKET);

	return Associate((HANDLE)hSocket,CompletionKey,pnOutErrCode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// 파일 핸들과 IOCP 와의 연결
// hDevice		 : IOCP 와 연결될 디바이스
// CompletionKey : 연결되면서 쓰여질 Completion Key
// pnOutErrCode  : 위와 상동
//
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL IocpHandler::Associate(HANDLE hDevice, ULONG_PTR CompletionKey, int* pnOutErrCode)
{
	assert(hDevice!=INVALID_HANDLE_VALUE);
	assert(CompletionKey!=0);

	HANDLE retHandle=CreateIoCompletionPort(hDevice,m_hIOCP,CompletionKey,0);
    if(retHandle!=m_hIOCP && pnOutErrCode!=NULL)
	{
		*pnOutErrCode=GetLastError();     
	}

	return (retHandle==m_hIOCP);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// IO Completion Queue에 한 완료 패킷를 사용자 정의로 추가
// CompletionKey      : 사용자 정의 완료 패킷에 들어갈 Completion Key
// dwBytesTransferred : 사용자 정의 완료 패킷에 들어갈 바이트수(0 디폴트)
// pOverlapped        : 사용자 정의 완료 패킷에 들어가는 오버랩드 구조체(NULL 디폴트)
// pnOutErrCode       : 위와 상동
//
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL IocpHandler::PostCompletionStatus(ULONG_PTR CompletionKey, DWORD dwBytesTransferred, OVERLAPPED* pOverlapped, int* pnOutErrCode)
{
	BOOL ret=PostQueuedCompletionStatus(m_hIOCP,dwBytesTransferred,CompletionKey,pOverlapped);
	if(!ret && pnOutErrCode!=NULL)
	{
       *pnOutErrCode=GetLastError();	   
	}

	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// IO Completion Queue에서 완료 패킷을 얻어온다.
// pOutCompletionKey      : IO Completon 큐에서 빼내온 레코드의 Completion Key
// pdwOutBytesTransferred : IO Completon 큐에서 빼내온 레코드의 바이트수(0 디폴트)
// pOutOverlapped         : IO Completon 큐에서 빼내온 레코드의 오버랩드 구조체(NULL 디폴트)
//  pnOutErrCode          : 위와 상동
//  dwWaitingTime         : 완료 패킷이 없을 시에 기다릴 시간 (디폴트: 무한대)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL IocpHandler::GetCompletionStatus(ULONG_PTR* pOutCompletionKey, LPDWORD pdwOutBytesTransferred, LPOVERLAPPED* pOutOverlapped, int* pnOutErrCode, DWORD dwWaitingTime)
{
	BOOL ret=GetQueuedCompletionStatus(m_hIOCP,pdwOutBytesTransferred,pOutCompletionKey,pOutOverlapped,dwWaitingTime);
	if(!ret && pnOutErrCode!=NULL)
	{
       *pnOutErrCode=GetLastError();	   
	}

	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// IOCP Worker Thread 풀을 만듬
// piProcessThread: IIocpProcessThread 인터페이스를 상속받은 클래스에 대한 포인터 즉 쓰레드 풀에 들어갈
//                  쓰레드 함수가 구현된 클래스의 포인터
// nNumOfPooledThredad: 풀링할 쓰레드 개수, 0이면 디폴트 값에 맞추어짐 
//
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL IocpHandler::CreateThreadPool(IIocpProcessThread* piProcessThread, int nNumOfPooledThread)
{
	assert(piProcessThread);
	assert(nNumOfPooledThread>=0);
	
	if(nNumOfPooledThread==0)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);

		// 디폴트 쓰레드 수로 
		// 2 * 프로세서수 + 2 의 공식을 따랐음
		m_CreatedThreadNumber=si.dwNumberOfProcessors*2+2;
	}
	else
	{
		m_CreatedThreadNumber=nNumOfPooledThread;
	}

	for(int i=0;i<m_CreatedThreadNumber;i++)
	{
		DWORD dwThreadId=0;		
		CloseHandle(BEGINTHREADEX(NULL,0,IocpWorkerThreadStartingPoint,piProcessThread,0,&dwThreadId));
	}

	return TRUE;
}

// IOCP 워커 쓰레드 시작 포인트로서만의 역활
DWORD __stdcall IocpHandler::IocpWorkerThreadStartingPoint(PVOID pvParam)
{
	IIocpProcessThread* piProcessThread=(IIocpProcessThread*)pvParam;
	piProcessThread->ProcessingThread();

	return 0;
}

// 현재 돌고 있는 쓰레드 모두 죽임
void IocpHandler::CloseAllThreads(void)
{
	for(int i=0;i<m_CreatedThreadNumber;i++)
	{
		PostCompletionStatus(THREAD_DIE);
	}
}

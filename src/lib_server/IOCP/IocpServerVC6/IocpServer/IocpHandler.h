#pragma once

#include "IocpProcessThreadDef.h"

class IocpHandler
{
public:
	IocpHandler(void);
	~IocpHandler(void);

	// IOCP 생성 및 초기화
	BOOL Create(int nMaxNumberOfConcurrentThreads=0, int* pnOutErrCode=NULL);
	// 소켓과 IOCP 와 연결
	BOOL Associate(SOCKET hSocket, ULONG_PTR CompletionKey, int* pnOutErrCode=NULL);
	// 파일 핸들과 IOCP 와의 연결
	BOOL Associate(HANDLE hDevice, ULONG_PTR CompletionKey, int* pnOutErrCode=NULL);
	// IO Completion Queue에 한 완료 레코드를 사용자 정의로 추가
	BOOL PostCompletionStatus(ULONG_PTR CompleitonKey, DWORD dwBytesTransferred=0, OVERLAPPED* pOverlapped=NULL, int* pnOutErrCode=NULL);
	// IO Completion Queue에서 완료 패킷을 얻어온다.
	BOOL GetCompletionStatus(ULONG_PTR* pOutCompletionKey, LPDWORD pdwOutBytesTransferred, LPOVERLAPPED* pOutOverlapped, int* pnOutErrCode=NULL, DWORD dwWaitingTime=INFINITE);
	
	// IOCP Worker Thread 풀을 만듬
	BOOL CreateThreadPool(IIocpProcessThread* piProcessThread, int nNumOfPooledThread=0);

private:
	// IOCP 커널 객체에 대한 핸들
	HANDLE m_hIOCP;
	// 만들어진 Worker Thread 수
	int m_CreatedThreadNumber;
	
private:
	// IOCP 워커 쓰레드 시작 포인트로서만의 역활
	static DWORD __stdcall IocpWorkerThreadStartingPoint(PVOID pvParam);
public:
	// 현재 돌고 있는 쓰레드 모두 죽임
	void CloseAllThreads(void);
};

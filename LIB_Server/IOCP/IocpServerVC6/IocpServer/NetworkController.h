#pragma once

#include "IocpHandler.h"
#include "def.h"
#include "MemPooler.h"
#include "Statistics.h"
#define  MAX_USER 3000

class NetworkController:public IIocpProcessThread
{
public:
	NetworkController(void);
	~NetworkController(void);

	// 초기화 처리
	BOOL Init(const int Port=15000);
    // RECV 요청
	BOOL RecvPost(PPerSocketContext pPerSocketCtx);
	// Send 요청
	BOOL SendPost(PPerSocketContext pPerSocketCtx);	
	// Accept 작업 처리
	void AcceptProcess(void);
    // 서버 중단
	void ServerClose(void);

private:	
    // Per Io Context 메모리 할당
	PPerIoContext AllocPerIoContextForRecv(void);
	PPerIoContext AllocPerIoContextForSend(void);
	// Per Socket Context 메모리 할당
	PPerSocketContext AllocPerSocketContext(SOCKET clientSocket);
	// 완료 패킷 처리 함수
	void ProcessingThread(void);
	// PerIoContext 메모리 제거
	void DeallocPerIoContextForSend(PPerIoContext pPerIoCtx);
	void DeallocPerIoContextForRecv(PPerIoContext pPerIoCtx);
	// PerSocketContext 메모리 제거
	void DeallocPerSocketContext(PPerSocketContext pPerSocketCtx);
	// 리시브 이벤트 처리 핸들러 함수
	BOOL RecvCompleteEvent(PPerSocketContext pPerSocketCtx, DWORD dwBytesTransferred);
	// Send 완료 패킷 처리 핸들러 함수
	BOOL SendCompleteEvent(PPerSocketContext pPerSocketCtx, DWORD dwBytesTransferred);
	// Recv, Send 완료 동작 외의 처리 핸들러 함수
	BOOL OtherCompleteEvent(PPerSocketContext pPerSocketCtx, DWORD dwBytesTransferred);
	// 클라이언트 소켓 컨텍스트 제거하고 소켓 닫음
	void CloseClient(PPerSocketContext pPerSocketCtx,bool bGraceful);
private:
	// 리슨 소켓
	SOCKET m_listenSocket;
	// IOCP 핸들러 
	IocpHandler m_IocpHandler;

#ifdef STATISTICS
	// 통계 표시를 위한 클래스 인스턴스
	Statistics m_state;
#endif

	// 동기화를 위한 변수( 이렇게 하는거 보단 다른 방법이 더 나을 것 같음)
	CRITICAL_SECTION m_cs;

    MemPooler<PerSocketContext> * m_pPerSocketCtxMemPool;
	MemPooler<PerIoContext> * m_pRecvMemPool;
	MemPooler<PerIoContext> * m_pSendMemPool;
	
};

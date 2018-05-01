#pragma once

#include <windows.h>
#include <assert.h>

/*
Title     : IOCP로 구현된 큐 템플릿 클래스
Class Name: IocpQueue
Date      : 2002.8.30                                              
Programmer: snaiper- 이기탁 (snaiper80@korea.com) 
*/                                           

template<typename Type, int QueueSize>
class IocpQueue
{
// Public Method
public:
   // Constructor and Destructor
   IocpQueue():m_hIocp(NULL),m_hSemaphore(NULL)
   {
	   Create();
   }
   ~IocpQueue()
   {
       Destroy();	   
   }

   // Push 동작 - 큐에 빈 자리가 없으면 자리 생길때까지 블러킹 
   void Blocking_Push(Type PushingData)
   {
	   Push(PushingData,INFINITE);
   }

   // Pop 동작 - 큐에 아이템이 없으면 블러킹
   Type Blocking_Pop()
   {
	   Type PopedData;
	   Pop(PopedData,INFINITE);
	   return PopedData;
   }

   // Push 동작 - 큐에 빈 자리가 없으면 타임아웃 시간까지 블러킹, 자리 생기면 넣고 리턴
   BOOL Blocking_Push_Timeout(Type PushingData,DWORD dwTimeout)
   {
	  return Push(PushingData,dwTimeout);
   }

   // Pop 동작 - 큐에 아이템이 없으면 타임아웃 시간까지 블러킹, 아이템 있으면 리턴
   BOOL Blocking_Pop_Timeout(Type& PopingData, DWORD dwTimeout)
   {
	  return Pop(PopingData,dwTimeout);
   }

   // Push 동작 - 큐에 자리가 있으면 넣고 자리 없으면 바로 FALSE라는 결과와 함께 리턴
   BOOL NonBlocking_Push(Type PushingData)
   {	   
	   return Push(PushingData,0);
   }
   
   // Pop 동작 - 큐에 아이템이 있으면 가지고 오고 TRUE 리턴, 아이템 없으면 바로 FALSE 와 함께 리턴 
   BOOL NonBlocking_Pop(Type& PopingData)
   {
	   return Pop(PopingData,0);
   }

// Private Method   
private:
	// 초기화, IOCP 생성, 세마포어 초기화
	void Create()   
	{
		m_hIocp=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,0);
		assert(m_hIocp!=NULL);

		m_hSemaphore=CreateSemaphore(NULL,QueueSize,QueueSize,NULL);
		assert(m_hSemaphore!=NULL);

	}
	// 자원정리, IOCP 객체 및 세마포어 정리
	void Destroy()  
	{
	   if(m_hIocp!=NULL)
	   {
		   CloseHandle(m_hIocp);
           m_hIocp=NULL;
	   }

	   if(m_hSemaphore!=NULL)
	   {
		   CloseHandle(m_hSemaphore);
		   m_hSemaphore=NULL;
	   }
	}

	// Push Operation
	BOOL Push(Type& data, DWORD dwTimeout)
	{
	   DWORD ret=WaitForSingleObject(m_hSemaphore,dwTimeout);
	   if(ret==WAIT_OBJECT_0)
	   {
		   BOOL ret=PostQueuedCompletionStatus(m_hIocp,0,(ULONG_PTR)data,NULL);
		   assert(ret);
		   return TRUE;
	   }
	   else 
	   { 
		   assert(ret==WAIT_TIMEOUT);
		   return FALSE;
	   }
	}

	// Pop Operation
	BOOL Pop(Type& retData,DWORD dwTimeout)
	{
	   Type PopingItem;
	   DWORD dwTemp=0;	   
       LPOVERLAPPED pTempOverlapped=NULL;
	   
	   BOOL ret=GetQueuedCompletionStatus(m_hIocp,&dwTemp,((PULONG_PTR)(&PopingItem)),&pTempOverlapped,dwTimeout);
	   if(ret)
	   {       
	      assert(ReleaseSemaphore(m_hSemaphore,1,NULL));
		  retData=PopingItem;
		  return TRUE;
	   }
	   else
	   {
		  assert(WAIT_TIMEOUT==GetLastError());
	      return FALSE;
	   }
	}

// Private Member
private:
	HANDLE m_hIocp;
	HANDLE m_hSemaphore;
};
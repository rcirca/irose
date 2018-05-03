#pragma once

#define MAX_BUFFER 4096

typedef struct tagPerIoContext
{

	WSAOVERLAPPED overlapped;
	WSABUF wsaBuf;
	char Buffer[MAX_BUFFER];

	//DWORD dwBytesTransferred; // 현 IO 전송된 바이트 수 
	//DWORD dwTotalBytes;       // 총 얼마만큼 전송되었느냐를 저장하는 바이트 수 

} PerIoContext, *PPerIoContext;

typedef struct tagPerSocketContext
{

  SOCKET socket;
  PPerIoContext recvContext;
  PPerIoContext sendContext;

} PerSocketContext, *PPerSocketContext;
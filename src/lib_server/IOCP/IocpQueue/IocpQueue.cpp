// IocpQueue.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
using namespace std;

#include "IocpQueue.h"

IocpQueue<int,999> g_queue;

DWORD __stdcall PushingThread(PVOID param)
{
	int i=(int)param;
    while(TRUE)
	{
		
		cout<<"Pushing "<<i<<endl;
        g_queue.Blocking_Push(i++);
	}

	return 0;
}

DWORD __stdcall PopingThread(PVOID param)
{
	int PopData;

	while(TRUE)
	{
        PopData=g_queue.Blocking_Pop();
		cout<<"Poping "<<PopData<<endl;
	}
    
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	/*
	// 각 종류별 push, pop 테스트 코드
	IocpQueue<int,5> queue;
	///////////////////////////////////////////////////////
	cout<<"Blocking Queueing Test"<<endl;
	for(int i=0;i<5;i++)
	{ 
		queue.Blocking_Push(i);
        cout<<"Pushing "<<i<<endl;		
	}

	int temp;
	for(int j=0;j<5;j++)
	{
        temp=queue.Blocking_Pop();
		cout<<"Poping "<<temp<<endl;
	}
    
	///////////////////////////////////////////////////////
	cout<<endl<<"NonBlocking Queueing Test"<<endl;
	for(i=0;i<10;i++)
	{ 
		if(queue.NonBlocking_Push(i))
			cout<<"Pushing "<<i<<endl;	
		else
			cout<<"Pushing Operation Failed."<<endl;
	}

	for(int j=0;j<7;j++)
	{
        if(queue.NonBlocking_Pop(temp))
			cout<<"Poping "<<temp<<endl;
		else
			cout<<"Poping Operation Error."<<endl;
	}

	//////////////////////////////////////////////////////
	cout<<endl<<"Timeout Blocking Queueing Test"<<endl;
    for(int i=0;i<7;i++)
	{ 
		if(queue.Blocking_Push_Timeout(i,1000))
			cout<<"Pushing "<<i<<endl;	
		else
			cout<<"Pushing Operation Failed."<<endl;	
	}

	for(int j=0;j<7;j++)
	{
        if(queue.Blocking_Pop_Timeout(temp,1000))
			cout<<"Poping "<<temp<<endl;
		else
			cout<<"Poping Operation Error."<<endl;
	}
	*/

	// 쓰레드 테스트 코드
	HANDLE hThreads[5]={NULL};
	DWORD dwThreadId=0;
	// 푸싱 쓰레드
    hThreads[0]=CreateThread(NULL,0,PushingThread,NULL,0,&dwThreadId);
	hThreads[2]=CreateThread(NULL,0,PushingThread,(LPVOID)2000,0,&dwThreadId);
	// 팝핑 쓰레드
	hThreads[1]=CreateThread(NULL,0,PopingThread,NULL,0,&dwThreadId);
	hThreads[3]=CreateThread(NULL,0,PopingThread,NULL,0,&dwThreadId);
	hThreads[4]=CreateThread(NULL,0,PopingThread,NULL,0,&dwThreadId);
	
	WaitForMultipleObjects(5,hThreads,TRUE,INFINITE);
    cout<<endl<<"All Test End"<<endl;
	cin.get();
	
	return 0;
}


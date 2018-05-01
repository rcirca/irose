// S-Server.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "NetworkController.h"
#include "Statistics.h"


DWORD __stdcall ServerStart(PVOID param)
{	
	NetworkController* pNc=(NetworkController*)param;

    pNc->Init();
	pNc->AcceptProcess();
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	
	char buffer[256];
	cout<<"Server Control Commmander"<<endl;
	cout<<"-------------------------"<<endl;
	cout<<" Command: start, shutdown"<<endl;
      
	NetworkController nc;

	while(TRUE)
	{
		cout<<">";
		cin>>buffer;
		if(!strcmp(buffer,"start"))
		{
			DWORD dwThreadId=0;
			HANDLE hThread=CreateThread(NULL,0,ServerStart,&nc,0,&dwThreadId);
			CloseHandle(hThread);
		}
		else if(!strcmp(buffer,"shutdown"))
		{
			nc.ServerClose();
			break;
		}
		else
		{
			cout<<"Invalid Operation"<<endl;
		}
	}	
	

	return 0;
}




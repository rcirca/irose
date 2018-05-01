#pragma once

#define  THREAD_DIE 1

struct IIocpProcessThread
{
	 virtual void ProcessingThread()=0;
};
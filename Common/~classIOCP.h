#ifndef classIOCPH
#define classIOCPH
#include <ScktComp.hpp>
#include "DLLIST.h"
//---------------------------------------------------------------------------

class classIOCP
{
private:
	HANDLE				 m_hIOCP;
	DWORD				 m_dwWorkerThreadCnt;

    static  classIOCP   *m_pInstance;

protected:
    classIOCP ();
    ~classIOCP ();

public :
	static  classIOCP *Instance ();
    static  void       Free ();

    static  DWORD  GetWorkerThreadCount ();
            HANDLE GetHANDLE ()             {   return m_hIOCP;    }

    static  bool   AddClientSocket   (SOCKET hSocket, DWORD dwCompKey);
	static  void   CloseClientSocket (SOCKET hSocket);
} ;


//---------------------------------------------------------------------------
#endif

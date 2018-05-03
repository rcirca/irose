//---------------------------------------------------------------------------

#ifndef ThreadACCEPTH
#define ThreadACCEPTH
//---------------------------------------------------------------------------
#include <ScktComp.hpp>
#include <Classes.hpp>
//---------------------------------------------------------------------------
class ThreadACCEPT : public TThread
{
private:
	SOCKET		    m_ListenSocket;

    void __fastcall LogSTART ();
    void __fastcall LogSTOP ();

protected:
    void __fastcall Execute();
public:
            __fastcall ThreadACCEPT(bool CreateSuspended);
    virtual __fastcall ~ThreadACCEPT();

    bool __fastcall Init (int iTCPPort);
    void __fastcall Free (void);

    virtual bool AcceptSOCKET (SOCKET hSocket, sockaddr_in &SockADDR)=0;
};
//---------------------------------------------------------------------------
#endif

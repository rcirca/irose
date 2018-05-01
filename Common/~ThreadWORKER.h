//---------------------------------------------------------------------------

#ifndef ThreadWORKERH
#define ThreadWORKERH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------

class ThreadWORKER : public TThread
{
private:
	HANDLE       m_hIOCP;

    void __fastcall LogSTART ();
    void __fastcall LogSTOP ();
    void __fastcall LogERROR ();

protected:
    int          m_iThreadNO;

    void __fastcall Execute();
public:
         __fastcall ThreadWORKER(bool CreateSuspended);

    bool __fastcall Init (HANDLE hIOCP, int iThreadNO);
    void __fastcall Free (void);

    virtual void STATUS_ReturnTRUE  (LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey, DWORD dwBytesIO)=0;
    virtual void STATUS_ReturnFALSE (LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey)=0;
} ;

//---------------------------------------------------------------------------
#endif

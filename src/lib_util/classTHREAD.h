#ifndef	__CLASSTHREAD_H
#define	__CLASSTHREAD_H
//-------------------------------------------------------------------------------------------------

#ifdef  __BORLANDC__
    #include <Classes.hpp>
    #include <SyncObjs.hpp>
    class classTHREAD : public TThread
    {
    public :
        classTHREAD (bool bCreateSuspended);

        bool	IsFinished ()				
		{	
			this->WaitFor ();
			return	this->Terminated;   
		}
        BOOL	SetPriority( int iPriority);
    } ;
#else
	#pragma warning( disable : 4166 )	// warning C4166: illegal calling convention for constructor/destructor
    class classTHREAD
    {
    private:
        bool	m_bSuspended;
        bool	m_bFinished;

        // friend DWORD WINAPI ThreadFunc (LPVOID lpParameter);
		friend unsigned __stdcall ThreadFunc( void* lpParameter );

    protected:
        HANDLE			m_hThread;
        unsigned int	ThreadID;
        bool			Terminated;

    public :
        classTHREAD (bool bCreateSuspended);
        virtual ~classTHREAD ();

        DWORD	Suspend ();
        DWORD	Resume ();

        void	Terminate ();
        int		WaitFor ();

        bool	IsFinished ()				{	return	m_bFinished;	}

        void	SetSuspended (bool bValue);
        BOOL	SetPriority( int iPriority)	{	return ::SetThreadPriority( m_hThread, iPriority );	}

        virtual void Execute ()=0;
    } ;
//	#pragma warning( default : 4166 )
#endif

//-------------------------------------------------------------------------------------------------
#endif

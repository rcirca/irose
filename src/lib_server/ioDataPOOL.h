/*
	$Header: /7HeartsOnline/LIB_Server/ioDataPOOL.h 1     04-03-25 11:20a Icarus $
*/
#ifndef	__IODATA_POOL_H
#define	__IODATA_POOL_H
#include "LIB_Util.h"
#include "DLLIST.h"
#include "CDataPOOL.h"
#include "classPACKET.h"
//-------------------------------------------------------------------------------------------------

#define	__USE_PACKET_POOL
#define	__USE_RECV_IODATA_POOL
#define	__USE_SEND_IODATA_POOL

typedef enum {
	ioREAD,
	ioWRITE
//  ClientIoWrite,
//  ClientQoS
} IO_MODE, IO_OPERATION;


struct tagIO_DATA {
	OVERLAPPED					m_Overlapped;
	IO_MODE						m_IOmode;
	DWORD						m_dwIOBytes;
	classPACKET				   *m_pCPacket;
	classDLLNODE<tagIO_DATA>   *m_pNODE;
} ;
typedef	classDLLNODE<tagIO_DATA>	IODATANODE;
typedef	classDLLNODE<tagIO_DATA>*	LPIODATANODE;


//-------------------------------------------------------------------------------------------------
class CPoolPACKET 
#ifdef	__USE_PACKET_POOL
				  : private CDataPOOL< classPACKET >
#endif
{
	DECLARE_INSTANCE2( CPoolPACKET )
public :
	static CPoolPACKET *Instance( UINT uiInitDataCNT, UINT uiIncDataCNT )
	{
		if ( !m_pCPoolPACKET ) {
			m_pCPoolPACKET = new CPoolPACKET( uiInitDataCNT, uiIncDataCNT );
		}
		return m_pCPoolPACKET;
	}

#ifndef	__USE_PACKET_POOL
	LPCPACKET	Pool_Alloc ()						
	{
		LPCPACKET pPacket;
		pPacket = new classPACKET;
		return pPacket;
	}
	void		Pool_Free( LPCPACKET pCPacket )		
	{	
		SAFE_DELETE( pCPacket );
	}
#endif


private:
	CPoolPACKET( UINT uiInitDataCNT, UINT uiIncDataCNT );

public :
	inline void InitData (LPCPACKET pCPacket)
	{	
		pCPacket->SetLength( 0 );
	}

	LPCPACKET AllocOnly ()
	{
		LPCPACKET pCPacket;
		pCPacket = this->Pool_Alloc ();

		this->InitData( pCPacket );
		pCPacket->SetRefCnt( 0 );

		return pCPacket;
	}

	void ReleaseOnly (LPCPACKET pCPacket)
	{
		// 무조건 해제...
		//if ( pCPacket->GetRefCnt() <= 0 ) 
		{
			this->Pool_Free( pCPacket );
		}
	}

	LPCPACKET AllocNLock ()
	{
		LPCPACKET pCPacket;

		pCPacket = this->Pool_Alloc ();

		this->InitData( pCPacket );
		pCPacket->SetRefCnt( 1 );

		return pCPacket;
	}

	void ReleaseNUnlock (LPCPACKET pCPacket)
	{
		if ( pCPacket->DecRefCnt() <= 0 ) {
			this->Pool_Free( pCPacket );
		}
	}

	void DecRefCount (LPCPACKET pCPacket)
	{
		if ( pCPacket->DecRefCnt() <= 0 ) {
			this->Pool_Free( pCPacket );
		}
	}
} ;



//-------------------------------------------------------------------------------------------------
class CPoolRECVIO 
#ifdef	__USE_RECV_IODATA_POOL
					: public CDataPOOL< IODATANODE > 
#endif
{
	DECLARE_INSTANCE2( CPoolRECVIO )
public :
	static CPoolRECVIO *Instance( UINT uiInitDataCNT, UINT uiIncDataCNT )
	{
		if ( !m_pCPoolRECVIO ) {
			m_pCPoolRECVIO = new CPoolRECVIO( uiInitDataCNT, uiIncDataCNT );
		}
		return m_pCPoolRECVIO;
	}
#ifndef	__USE_RECV_IODATA_POOL
	LPIODATANODE Pool_Alloc ()	
	{	
		return new classDLLNODE< tagIO_DATA >;
	}

	void  Pool_Free( LPIODATANODE pDelNODE )
	{
		_ASSERT( pDelNODE->DATA.m_pCPacket );
		_ASSERT( pDelNODE == pDelNODE->DATA.m_pNODE );
		SAFE_DELETE( pDelNODE );
	}
#endif

private:
	CPoolRECVIO( UINT uiInitDataCNT, UINT uiIncDataCNT );

public :
	inline void	 InitData (LPIODATANODE pData)
	{	
        ::ZeroMemory (&pData->DATA.m_Overlapped, sizeof(OVERLAPPED));
		// 2003. 11. 12 반드시 0으로 초기화 !!!, 빼먹어서 Recv_Start에서 기존의 쓰레기 패킷 뒤에 추가로 받아졌다.
		pData->DATA.m_dwIOBytes = 0;

		pData->DATA.m_pNODE		   = pData;
		pData->DATA.m_IOmode       = ioREAD;
	}
} ;




//-------------------------------------------------------------------------------------------------
class CPoolSENDIO
#ifdef	__USE_SEND_IODATA_POOL
					: public CDataPOOL< IODATANODE >
#endif
{
	DECLARE_INSTANCE2( CPoolSENDIO )
public :
	static CPoolSENDIO *Instance( UINT uiInitDataCNT, UINT uiIncDataCNT )
	{
		if ( !m_pCPoolSENDIO ) {
			m_pCPoolSENDIO = new CPoolSENDIO( uiInitDataCNT, uiIncDataCNT );
		}
		return m_pCPoolSENDIO;
	}

#ifndef	__USE_SEND_IODATA_POOL
	LPIODATANODE Pool_Alloc ()						
	{
		return new classDLLNODE< tagIO_DATA >;
	}

	void Pool_Free (LPIODATANODE pData)
	{	
		_ASSERT( pData == pData->DATA.m_pNODE );
		SAFE_DELETE( pData );
	}
#endif

private:
	CPoolSENDIO( UINT uiInitDataCNT, UINT uiIncDataCNT );

public :
	inline void	 InitData (LPIODATANODE pData)
	{	
        ::ZeroMemory (&pData->DATA.m_Overlapped, sizeof(OVERLAPPED));
		pData->DATA.m_dwIOBytes = 0;

		pData->DATA.m_pNODE	 = pData;
		pData->DATA.m_IOmode = ioWRITE;
	}
} ;


//-------------------------------------------------------------------------------------------------

inline classPACKET *Packet_AllocOnly ()						
{	
	return CPoolPACKET::GetInstance()->AllocOnly ();
}

inline void Packet_ReleaseOnly (classPACKET *pCPacket)		
{	
	CPoolPACKET::GetInstance()->ReleaseOnly( pCPacket );
}

inline classPACKET *Packet_AllocNLock ()					
{	
	return CPoolPACKET::GetInstance()->AllocNLock ();			
}

inline void Packet_ReleaseNUnlock (classPACKET *pCPacket)	
{	
	CPoolPACKET::GetInstance()->ReleaseNUnlock( pCPacket );
}

inline void Packet_DecRefCount (classPACKET *pCPacket)		
{	
	CPoolPACKET::GetInstance()->DecRefCount( pCPacket );
}

//-------------------------------------------------------------------------------------------------
#endif

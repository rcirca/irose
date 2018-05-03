#ifndef __CLASSPACKET_H
#define __CLASSPACKET_H
#include <crtdbg.h>
#include "PacketHEADER.h"
#include "CPacketCODEC.h"
//-------------------------------------------------------------------------------------------------

struct tagIO_DATA;
#ifdef	__T_PACKET
class classPACKET : public t_PACKET {
#else
class classPACKET {
public :
	union {
		t_PACKETHEADER	m_HEADER;
		BYTE			m_pDATA[ MAX_PACKET_SIZE ];
	} ;
#endif

private :
	long						m_lRefCnt;
	WORD						m_wPacketLEN;

public :
	classPACKET ()				
	{   
		m_lRefCnt = 0;
	}
	~classPACKET ()
	{   
		// _ASSERT( 0 == m_lRefCnt );
	}

	void SetRefCnt (long lRefCnt)
	{
		this->m_lRefCnt = lRefCnt;
	}

	long IncRefCnt () 
	{
		//_ASSERT( 0 != m_wPacketLEN );
		return ::InterlockedIncrement( &this->m_lRefCnt );
	}
	long DecRefCnt ()
	{
		return ::InterlockedDecrement( &this->m_lRefCnt );
	}

	inline int	  GetRefCnt ()				{	return m_lRefCnt;		}
	inline WORD	  GetLength ()				{	return m_wPacketLEN;	}
	inline void   SetLength (WORD wLen)		{	m_wPacketLEN = wLen;	}

	char *GetStringPtr	(short *pOffset);
	BYTE *GetDataPtr	(short *pOffset, short nSize);
	bool  AppendString	(char *pStr);
	bool  AppendData	(void *pData, short nLen);
} ;
typedef	classPACKET*				LPCPACKET;

//-------------------------------------------------------------------------------------------------
#endif

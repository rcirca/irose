#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include "LIB_Util.h"

#include "classLOG.h"
//#include "CRandom.h"

//-------------------------------------------------------------------------------------------------
#pragma pack (push, 1)
struct t_PACKETHEADER 
{
	union {
		// Decoded Header 
		struct {
			short	m_nSize;
			WORD	m_wType;
			BYTE	m_btReserved;
			BYTE	m_CRC;
		} ;

		//------------------------------
		// Client => Server
		struct tagCSH {
			// Packet size total 11 bits
			DWORD	m_T2	: 3;
			DWORD	m_R2	: 3;
			DWORD	m_S1	: 3;	// 9

			DWORD	m_R3	: 3;	// 12

			DWORD	m_T1	: 3;
			DWORD	m_R7	: 3;
			DWORD	m_S2	: 3;	// 21

			DWORD	m_R6	: 2;	// 23

			DWORD	m_S3	: 3;
			DWORD	m_R4	: 2;
			DWORD	m_T4	: 2;	// 30

			DWORD	m_R5	: 2;	// 32

			BYTE	m_T3	: 3;
			BYTE	m_R1	: 3;
			BYTE	m_S4	: 2;	// 40
		} m_CS;


		//------------------------------
		// Server => Client
		struct tagSCH {
			// Packet size total 11 bits
			DWORD	m_S2	: 3;
			DWORD	m_R1	: 3;
			DWORD	m_T3	: 3;	// 9

			DWORD	m_R7	: 3;	// 12

			DWORD	m_S3	: 3;
			DWORD	m_R3	: 3;
			DWORD	m_T2	: 3;	// 21

			DWORD	m_R4	: 2;	// 23

			DWORD	m_T1	: 3;
			DWORD	m_R5	: 2;
			DWORD	m_S4	: 2;	// 30

			DWORD	m_R6	: 2;	// 32

			BYTE	m_S1	: 3;
			BYTE	m_R2	: 3;
			BYTE	m_T4	: 2;	// 40
		} m_SC;

		struct {
			DWORD	m_wPacketSIZE		: 11;
			DWORD	m_wPacketTYPE		: 11;	// 7ff		: 0 ~ 2047
		} ;

		struct {	// 40 bit
			// LSB
			BYTE	m_btNULL2			: 8;
			DWORD	m_dwNULL2			: 14;

			DWORD	m_wRandAryIDX		: 11;
			DWORD	m_wRandTblIDX		: 4;
			DWORD	m_wAddedDummyLEN	: 3;	// 추가되는 패킷 길이 또는 검증 비트...
			// MSB
		} ;

		BYTE		m_pDATA [ 1 ];
		WORD		m_wDATA [ 1 ];
		DWORD		m_dwDATA[ 1 ];
	} ;
} ;


union tagCODEC {
	struct {
		// Packet size total 11 bits
		DWORD	m_S1				: 3;
		DWORD	m_S2				: 3;
		DWORD	m_S3				: 3;
		DWORD	m_S4				: 2;

		// Packet type total 11 bits
		DWORD	m_T1				: 3;
		DWORD	m_T2				: 3;
		DWORD	m_T3				: 3;
		DWORD	m_T4				: 2;
	} ;
	struct {
		BYTE	m_btNULL1			: 8;
		DWORD	m_dwNULL1			: 14;

		// Random array index total 11 bits
		DWORD	m_R1				: 3;
		DWORD	m_R2				: 3;
		DWORD	m_R3				: 3;
		DWORD	m_R4				: 2;

		// Random table index total 4 bits
		DWORD	m_R5				: 2;
		DWORD	m_R6				: 2;

		// Added dummy bytes	3 bits
		DWORD	m_R7				: 3;
	} ;

	// 값 넣을 구조체...
	struct {
		DWORD	m_wPacketSIZE		: 11;
		DWORD	m_wPacketTYPE		: 11;	// 7ff		: 0 ~ 2047
	} ;
	struct {	// 40 bit
		// LSB
		BYTE	m_btNULL2			: 8;
		DWORD	m_dwNULL2			: 14;

		DWORD	m_wRandAryIDX		: 11;
		DWORD	m_wRandTblIDX		: 4;
		DWORD	m_wAddedDummyLEN	: 3;	// 추가되는 패킷 길이...
		// MSB
	} ;

	BYTE		m_pDATA[ 1 ];
} ;
#pragma pack (pop)


//-------------------------------------------------------------------------------------------------

struct tPOINTF {
	float	x;
	float	y;
} ;

#define	__POINTF
#define	__SERVER
//#define	__SKIP_SRV_PROTOTYPE
#define	__T_PACKETHEADER

#include "Net_Prototype.h"
#include "CPacketCODEC.h"

//-------------------------------------------------------------------------------------------------

#define	m_pCRANDOM		this->m_pCR
#define	s_iRandTABLE	this->m_ppRT
#define	s_wRandINDEX	this->m_pRI

#define	RANDOM_TABLE_COUNT			0x0f		// 0 ~ 15
#define	MAX_RANDOM_TABLE_SIZE		0x07ff		// 0~2047
#define	MAX_RANDOM_INDEX_SIZE		0x01ff		// 0~511

// static int	s_iRandTABLE[ RANDOM_TABLE_COUNT+1 ][ MAX_RANDOM_TABLE_SIZE+1 ];
// static WORD	s_wRandINDEX[ MAX_RANDOM_INDEX_SIZE+1 ];

//-------------------------------------------------------------------------------------------------
bool CpC::Find_DupTABLE (int iValue, int iTableIDX, int iRange)
{
	int iC, iT;

	for (iT=0; iT<iTableIDX; iT++) {
		for (iC=0; iC<MAX_RANDOM_TABLE_SIZE; iC++)
			if ( s_iRandTABLE[ iT ][ iC ] == iValue )
				return true;
	}

	for (iC=0; iC<iRange; iC++)
		if ( s_iRandTABLE[ iTableIDX ][ iC ] == iValue )
			return true;

	return false;
}
bool CpC::Find_DupINDEX (WORD wValue, int iRange)
{
	int iC;
	for (iC=0; iC<iRange; iC++)
		if ( s_wRandINDEX[ iC ] == wValue )
			return true;

	return false;
}

//-------------------------------------------------------------------------------------------------
CpC::CpC ()
{
	// random generator..
	m_pCR = new CRandom;
	
	// static WORD	s_wRandINDEX[ MAX_RANDOM_INDEX_SIZE+1 ];
	m_pRI = new WORD[ MAX_RANDOM_INDEX_SIZE+1 ];

	// static int	s_iRandTABLE[ RANDOM_TABLE_COUNT+1 ][ MAX_RANDOM_TABLE_SIZE+1 ];
	m_ppRT = new int* [ RANDOM_TABLE_COUNT+1 ];
	for (int iR=0; iR<=RANDOM_TABLE_COUNT; iR++)
		m_ppRT[ iR ] = new int [ MAX_RANDOM_TABLE_SIZE+1 ];
}
CpC::~CpC ()
{
	for (int iR=0; iR<=RANDOM_TABLE_COUNT; iR++)
		SAFE_DELETE_ARRAY( m_ppRT[ iR ] );
	SAFE_DELETE_ARRAY( m_ppRT );

	SAFE_DELETE_ARRAY( m_pRI );
	SAFE_DELETE( m_pCR );
}
void CpC::Default ()
{
	m_pCRANDOM->Init( 4379238 );

	short nT, nI;
	for (nT=0; nT<=RANDOM_TABLE_COUNT; nT++) {
		m_pCRANDOM->SetType( nT % MAX_RANDOM_FUNC );

		for (nI=0; nI<=MAX_RANDOM_TABLE_SIZE; nI++) {
			do {
				s_iRandTABLE[ nT ][ nI ] = m_pCRANDOM->Get();
			} while( Find_DupTABLE(s_iRandTABLE[ nT ][ nI ], 0, nI) );
		}
	}

	for (nI=0; nI<=MAX_RANDOM_INDEX_SIZE; nI++) {
		do {
			s_wRandINDEX[ nI ] = ( m_pCRANDOM->AcRANDOM() ) & MAX_RANDOM_TABLE_SIZE;
		} while( Find_DupINDEX(s_wRandINDEX[ nI ], nI) ); 

		_ASSERT( s_wRandINDEX[ nI ] >= 0 );
	}
}

//-------------------------------------------------------------------------------------------------
bool CpC::Init (DWORD dwInitCODE)
{
	COMPILE_TIME_ASSERT( sizeof(t_PACKETHEADER) == 6 );
	
	short nT, nI;

	m_pCRANDOM->Init( dwInitCODE );
	dwInitCODE = m_pCRANDOM->R_MY ();
	m_pCRANDOM->Init( dwInitCODE );

	BYTE btRandTYPE[ 17 ] ;
	for (nT=0; nT<17; nT++) {
		btRandTYPE[ nT ] = m_pCRANDOM->BcRANDOM() % MAX_RANDOM_FUNC;
	}

	for (nT=0; nT<=RANDOM_TABLE_COUNT; nT++) {
		m_pCRANDOM->SetType( btRandTYPE[ nT ] );

		for (nI=0; nI<=MAX_RANDOM_TABLE_SIZE; nI++) {
			do {
				s_iRandTABLE[ nT ][ nI ] = m_pCRANDOM->Get();
			} while( Find_DupTABLE(s_iRandTABLE[ nT ][ nI ], 0, nI) );
		}
	}

	m_pCRANDOM->SetType( btRandTYPE[ 16 ] );
	for (nI=0; nI<=MAX_RANDOM_INDEX_SIZE; nI++) {
		do {
			s_wRandINDEX[ nI ] = ( m_pCRANDOM->Get() ) & MAX_RANDOM_TABLE_SIZE;
		} while( Find_DupINDEX(s_wRandINDEX[ nI ], nI) ); 

		_ASSERT( s_wRandINDEX[ nI ] >= 0 );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
// 클라이언트에서 서버로 보낼 패킷 암호화...
// Encode_SendClientPACKET
WORD CpC::ESCP (t_PACKETHEADER *pHEADER, int &iSendSEQ)
{
	tagCODEC sEnCODE;

	short nReaminLen = MAX_PACKET_SIZE - pHEADER->m_nSize;
	if ( nReaminLen >= 7 )
		sEnCODE.m_wAddedDummyLEN = RANDOM( 8 );
	else
		sEnCODE.m_wAddedDummyLEN = 0;

	iSendSEQ &= MAX_RANDOM_INDEX_SIZE;
	int iRandIdx = s_wRandINDEX[ iSendSEQ++ ];

	int iRandTbl = ( iSendSEQ + sEnCODE.m_wAddedDummyLEN ) & 0x0f;
	sEnCODE.m_wRandAryIDX = iRandIdx;
	sEnCODE.m_wRandTblIDX = iRandTbl;

	WORD wI, wRealSize, wTotSize;
	wRealSize = pHEADER->m_nSize;
	wTotSize  = (WORD)( wRealSize + sEnCODE.m_wAddedDummyLEN);

	sEnCODE.m_wPacketTYPE = pHEADER->m_wType;
	sEnCODE.m_wPacketSIZE = wTotSize;

	pHEADER->m_CS.m_S1 = sEnCODE.m_S1;
	pHEADER->m_CS.m_S2 = sEnCODE.m_S2;
	pHEADER->m_CS.m_S3 = sEnCODE.m_S3;
	pHEADER->m_CS.m_S4 = sEnCODE.m_S4;

	pHEADER->m_CS.m_T1 = sEnCODE.m_T1;
	pHEADER->m_CS.m_T2 = sEnCODE.m_T2;
	pHEADER->m_CS.m_T3 = sEnCODE.m_T3;
	pHEADER->m_CS.m_T4 = sEnCODE.m_T4;

	pHEADER->m_CS.m_R5 = sEnCODE.m_R5;
	pHEADER->m_CS.m_R6 = sEnCODE.m_R6;
	pHEADER->m_CS.m_R7 = sEnCODE.m_R7;

	WORD wCRC = 0;

	// Encoding Packet Header ...
	for (wI=0; wI<sizeof(tagCODEC); wI++) {
		// 원본 헤더의 CRC계산...
		wCRC = CalTAG( wCRC, sEnCODE.m_pDATA[ wI ] );
		pHEADER->m_pDATA[ wI ] ^= s_iRandTABLE[ wI ][ iRandIdx ];
	}

	// Encoding Packet Body ...
	for (wI=sizeof(t_PACKETHEADER); wI<wRealSize; wI++) {
		wCRC = CalTAG( wCRC, pHEADER->m_pDATA[ wI ] );
		pHEADER->m_pDATA[ wI ] ^= s_iRandTABLE[ (iRandTbl+wI) & RANDOM_TABLE_COUNT ][ (iRandIdx+wI) & MAX_RANDOM_TABLE_SIZE ];
	}

	pHEADER->m_CRC = wCRC;
	for (	; wI<wTotSize; wI++) {
		pHEADER->m_pDATA[ wI ] ^= s_iRandTABLE[ (iRandTbl+wI) & RANDOM_TABLE_COUNT ][ (iRandIdx+wI) & MAX_RANDOM_TABLE_SIZE ];
	}

	pHEADER->m_CS.m_R1 = sEnCODE.m_R1;
	pHEADER->m_CS.m_R2 = sEnCODE.m_R2;
	pHEADER->m_CS.m_R3 = sEnCODE.m_R3;
	pHEADER->m_CS.m_R4 = sEnCODE.m_R4;
	
	return wTotSize;
}


//-------------------------------------------------------------------------------------------------
// 서버에서 클라이언트로 보낼 패킷 암호화...
// Encode_SendServerPACKET
WORD CpC::ESSP (t_PACKETHEADER *pHEADER)
{
	tagCODEC sEnCODE;

	int iRandIdx = RANDOM( MAX_RANDOM_INDEX_SIZE+1 );
	int iRandTbl = RANDOM( RANDOM_TABLE_COUNT+1 );

	// 서버에서 전송되는 패킷은 더미 사이즈 아니다...
	sEnCODE.m_wRandAryIDX    = iRandIdx;
	sEnCODE.m_wRandTblIDX	 = iRandTbl;
	sEnCODE.m_wAddedDummyLEN = ( iRandIdx + iRandTbl ) & 0x07;  // 0 ~ 7

	WORD wI, wRealSize;
	wRealSize = pHEADER->m_nSize;

	sEnCODE.m_wPacketTYPE = pHEADER->m_wType;
	sEnCODE.m_wPacketSIZE = wRealSize;

	pHEADER->m_SC.m_S1 = sEnCODE.m_S1;
	pHEADER->m_SC.m_S2 = sEnCODE.m_S2;
	pHEADER->m_SC.m_S3 = sEnCODE.m_S3;
	pHEADER->m_SC.m_S4 = sEnCODE.m_S4;

	pHEADER->m_SC.m_T1 = sEnCODE.m_T1;
	pHEADER->m_SC.m_T2 = sEnCODE.m_T2;
	pHEADER->m_SC.m_T3 = sEnCODE.m_T3;
	pHEADER->m_SC.m_T4 = sEnCODE.m_T4;

	pHEADER->m_SC.m_R5 = sEnCODE.m_R5;
	pHEADER->m_SC.m_R6 = sEnCODE.m_R6;
	pHEADER->m_SC.m_R7 = sEnCODE.m_R7;

	WORD wCRC = 0;
	// Encoding Packet Header ...
	for (wI=0; wI<sizeof(tagCODEC); wI++) {
		// 원본 헤더의 CRC계산...
		wCRC = CalTAG( wCRC, sEnCODE.m_pDATA[ wI ] );
		pHEADER->m_pDATA[ wI ] ^= s_iRandTABLE[ wI ][ iRandIdx ];
	}

	// Encoding Packet Body ...
	for (wI=sizeof(t_PACKETHEADER); wI<wRealSize; wI++) {
		wCRC = CalTAG( wCRC, pHEADER->m_pDATA[ wI ] );
		pHEADER->m_pDATA[ wI ] ^= s_iRandTABLE[ (iRandTbl+wI) & RANDOM_TABLE_COUNT ][ (iRandIdx+wI) & MAX_RANDOM_TABLE_SIZE ];
	}

	pHEADER->m_CRC = wCRC;
	/*
	더미 패킷 없다..
	for (	; wI<wTotSize; wI++) {
		pHEADER->m_pDATA[ wI ] ^= s_iRandTABLE[ (iRandTbl+wI) & RANDOM_TABLE_COUNT ][ (iRandIdx+wI) & MAX_RANDOM_TABLE_SIZE ];
	}
	*/

	pHEADER->m_SC.m_R1 = sEnCODE.m_R1;
	pHEADER->m_SC.m_R2 = sEnCODE.m_R2;
	pHEADER->m_SC.m_R3 = sEnCODE.m_R3;
	pHEADER->m_SC.m_R4 = sEnCODE.m_R4;
	
	return wRealSize;
}


//-------------------------------------------------------------------------------------------------
WORD CpC::GDPL(t_PACKETHEADER *pPacket)
{
	return pPacket->m_wPacketSIZE;
}

//-------------------------------------------------------------------------------------------------
// 서버에서 받은 패킷헤더 복호화...
// Decode_RecvServerHEADER
WORD CpC::DRSH(t_PACKETHEADER *pHEADER)
{
	tagCODEC sEnCODE;

	sEnCODE.m_R1 = pHEADER->m_SC.m_R1;
	sEnCODE.m_R2 = pHEADER->m_SC.m_R2;
	sEnCODE.m_R3 = pHEADER->m_SC.m_R3;
	sEnCODE.m_R4 = pHEADER->m_SC.m_R4;

	// 헤더 복호화...
	int iSeqRandIDX = sEnCODE.m_wRandAryIDX;
	for (short nI=0; nI<sizeof(tagCODEC); nI++) {
		pHEADER->m_pDATA[ nI ] ^= s_iRandTABLE[ nI ][ iSeqRandIDX ];
	}

	// 사이즈...
	sEnCODE.m_S1 = pHEADER->m_SC.m_S1;
	sEnCODE.m_S2 = pHEADER->m_SC.m_S2;
	sEnCODE.m_S3 = pHEADER->m_SC.m_S3;
	sEnCODE.m_S4 = pHEADER->m_SC.m_S4;
	WORD wPacketSize = sEnCODE.m_wPacketSIZE;

	sEnCODE.m_R5 = pHEADER->m_SC.m_R5;
	sEnCODE.m_R6 = pHEADER->m_SC.m_R6;

	// 검증 코드...
	sEnCODE.m_R7 = pHEADER->m_SC.m_R7;

	// 패킷 크기 검증..
	if ( wPacketSize > MAX_PACKET_SIZE || wPacketSize < sizeof(t_PACKETHEADER) ) {
		return 0;
	}
	if ( sEnCODE.m_wAddedDummyLEN != ( (sEnCODE.m_wRandAryIDX+sEnCODE.m_wRandTblIDX) & 0x07 ) )
		return 0;

	// 패킷 타입...
	sEnCODE.m_T1 = pHEADER->m_SC.m_T1;
	sEnCODE.m_T2 = pHEADER->m_SC.m_T2;
	sEnCODE.m_T3 = pHEADER->m_SC.m_T3;
	sEnCODE.m_T4 = pHEADER->m_SC.m_T4;

	::CopyMemory( pHEADER, &sEnCODE, sizeof(tagCODEC) );

	return wPacketSize;
}

//-------------------------------------------------------------------------------------------------
// 클라이언트에서 받은 패킷헤더 복호화...
// Decode_RecvClientHEADER
WORD CpC::DRCH(t_PACKETHEADER *pHEADER, int &iRecvSEQ)
{
	tagCODEC sEnCODE;

	sEnCODE.m_R1 = pHEADER->m_CS.m_R1;
	sEnCODE.m_R2 = pHEADER->m_CS.m_R2;
	sEnCODE.m_R3 = pHEADER->m_CS.m_R3;
	sEnCODE.m_R4 = pHEADER->m_CS.m_R4;

	// 서버에서만 클라이언트에서 받은 패킷 검증...
	iRecvSEQ &= MAX_RANDOM_INDEX_SIZE;
	int iSeqRandIDX = ( s_wRandINDEX[ iRecvSEQ++ ] & 0x07ff );
	if ( iSeqRandIDX != sEnCODE.m_wRandAryIDX ) {
		return 0;
	}

	// 헤더 복호화...
	for (short nI=0; nI<sizeof(tagCODEC); nI++) {
		pHEADER->m_pDATA[ nI ] ^= s_iRandTABLE[ nI ][ iSeqRandIDX ];
	}

	// 사이즈...
	sEnCODE.m_S1 = pHEADER->m_CS.m_S1;
	sEnCODE.m_S2 = pHEADER->m_CS.m_S2;
	sEnCODE.m_S3 = pHEADER->m_CS.m_S3;
	sEnCODE.m_S4 = pHEADER->m_CS.m_S4;
	WORD wPacketSize = sEnCODE.m_wPacketSIZE;

	sEnCODE.m_R5 = pHEADER->m_CS.m_R5;
	sEnCODE.m_R6 = pHEADER->m_CS.m_R6;

	// 더미 사이즈...
	sEnCODE.m_R7 = pHEADER->m_CS.m_R7;

	// 패킷 크기 검증..
	if ( wPacketSize > MAX_PACKET_SIZE ||
		 wPacketSize - sEnCODE.m_wAddedDummyLEN < sizeof(t_PACKETHEADER) ) {
		return 0;
	}

	if ( sEnCODE.m_wRandTblIDX != ( (iRecvSEQ+sEnCODE.m_wAddedDummyLEN)&0x0f ) ) {
		return 0;
	}

	// 패킷 타입...
	sEnCODE.m_T1 = pHEADER->m_CS.m_T1;
	sEnCODE.m_T2 = pHEADER->m_CS.m_T2;
	sEnCODE.m_T3 = pHEADER->m_CS.m_T3;
	sEnCODE.m_T4 = pHEADER->m_CS.m_T4;

	::CopyMemory( pHEADER, &sEnCODE, sizeof(tagCODEC) );

	return wPacketSize;
}


//-------------------------------------------------------------------------------------------------
// 받은 패킷의 몸통 복호화... 헤더는 복호화 되어 있어야 한다 !!!
// Decode_RecvServerBODY
short CpC::DRSB (t_PACKETHEADER *pHEADER)
{
	int iSeqRandTBL, iSeqRandIDX;

	iSeqRandTBL  = pHEADER->m_wRandTblIDX;
	iSeqRandIDX  = pHEADER->m_wRandAryIDX;
	WORD  wType  = pHEADER->m_wPacketTYPE;
	short nSize = pHEADER->m_wPacketSIZE;
//	short nSize  = (short)( nTotal - pHEADER->m_wAddedDummyLEN );

	short nI;
	WORD wCRC = 0;

	for (nI=0; nI<sizeof(tagCODEC); nI++)
		wCRC = CalTAG( wCRC, pHEADER->m_pDATA[ nI ] );

	for (nI=sizeof(t_PACKETHEADER); nI<nSize; nI++) {
		pHEADER->m_pDATA[ nI ] ^= s_iRandTABLE[ ( iSeqRandTBL+nI) & RANDOM_TABLE_COUNT ][ (iSeqRandIDX+nI) & MAX_RANDOM_TABLE_SIZE ];
		wCRC = CalTAG( wCRC, pHEADER->m_pDATA[ nI ] );
	}

	if ( wCRC != pHEADER->m_CRC )
		return 0;

	pHEADER->m_wType = wType;
	pHEADER->m_nSize = nSize;

	return nSize;
}

//-------------------------------------------------------------------------------------------------
// 받은 패킷의 몸통 복호화... 헤더는 복호화 되어 있어야 한다 !!!
// Decode_RecvClientBODY
short CpC::DRCB (t_PACKETHEADER *pHEADER)
{
	int iSeqRandTBL, iSeqRandIDX;

	iSeqRandTBL  = pHEADER->m_wRandTblIDX;
	iSeqRandIDX  = pHEADER->m_wRandAryIDX;
	WORD  wType  = pHEADER->m_wPacketTYPE;
	short nTotal = pHEADER->m_wPacketSIZE;
	short nSize  = (short)( nTotal - pHEADER->m_wAddedDummyLEN );

	short nI;
	WORD wCRC = 0;

	for (nI=0; nI<sizeof(tagCODEC); nI++)
		wCRC = CalTAG( wCRC, pHEADER->m_pDATA[ nI ] );

	for (nI=sizeof(t_PACKETHEADER); nI<nSize; nI++) {
		pHEADER->m_pDATA[ nI ] ^= s_iRandTABLE[ ( iSeqRandTBL+nI) & RANDOM_TABLE_COUNT ][ (iSeqRandIDX+nI) & MAX_RANDOM_TABLE_SIZE ];
		wCRC = CalTAG( wCRC, pHEADER->m_pDATA[ nI ] );
	}

	if ( wCRC != pHEADER->m_CRC )
		return 0;

	pHEADER->m_wType = wType;
	pHEADER->m_nSize = nSize;

	return nTotal;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

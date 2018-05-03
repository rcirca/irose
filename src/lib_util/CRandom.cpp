
#include <windows.h>
#include "LIB_Util.h"
#include "CRandom.h"

#define	_2EXP16				65536	
#define	_2EXP31				2147483648
#define	_2EXP32				4294967296

#define	_MOD_2EXP16(V)		( (V) % 65536		)
#define	_MOD_2EXP31(V)		( (V) % 2147483648	)	// ( X )% _MOD_2EXP31  ==  ( X & 0x7fff ffff )
#define	_MOD_2EXP32(V)		( (V) % 4294967296	)	// ( X )% _MOD_2EXP32  ==  ( X & 0xffff ffff )

#define	_AND_2EXP16(V)		( (V) & 0x0000ffff	)
#define	_AND_2EXP31(V)		( (V) & 0x7fffffff	)
#define	_AND_2EXP32(V)		( (V) &	0xffffffff	)

// IMPLEMENT_INSTANCE( CR001 );

//-------------------------------------------------------------------------------------------------
void CR001::Init (DWORD dwSeed)
{
/*
	m_iTotalCalledCnt = 0;
	ZeroMemory (m_pCalledCnt, sizeof(int) * m_nBufferCnt);
*/
	if ( !dwSeed ) {
		dwSeed = ::timeGetTime();
	}

	m_dwVcCallCnt = 0;
	m_dwBcCallCnt = 0;
	m_dwAcCallCnt = 0;
    m_dwMyCallCnt = 0;

	::srand (dwSeed);

	m_iVcSeed	= (int) dwSeed;
	m_iBcSeed	= (int) dwSeed;
	m_iAcSeed	= (int) dwSeed;
    m_iMySeed	= (int) dwSeed;
}

void CR001::SetType (BYTE btRandTYPE)
{
	m_btType = btRandTYPE % MAX_RANDOM_FUNC;
}

//-------------------------------------------------------------------------------------------------
int  CR001::Get ()
{
	switch( m_btType ) {
		case 0 :	return this->BcRANDOM ();
		case 1 :	return this->AcRANDOM ();
		case 2 :	return this->MyRANDOM ();
	}

	return this->VcRANDOM ();
}

//-------------------------------------------------------------------------------------------------
int	CR001::R_AC()
{
/*
C and ANSI C 
Definition: 

SEED = (1103515245*SEED + 12345) mod 2**31 
X = SEED 
Returns integer in range [0, 2**31) {including 0, excluding 2**31} 
*/
	m_dwAcCallCnt ++;
	m_iAcSeed = _AND_2EXP31( 1103515245*m_iAcSeed + 12345);
	return m_iAcSeed;
}

//-------------------------------------------------------------------------------------------------
int	CR001::R_BC()
{
/*
The random function in Turbo Pascal v6.0 
Definition: 

SEED = (134775813*SEED + 1) mod 2**32 
X = int( SEED/2**16 ) {i.e. the upper 16 bits of SEED} 
Returns integer in range [0, 2**16) {0 inclusive, 2**16 exclusive} 
*/
	m_dwBcCallCnt ++;
	m_iBcSeed = _AND_2EXP32( 134775813*m_iBcSeed + 1);
	return ( (int)(m_iBcSeed / 65536L) );
}

//-------------------------------------------------------------------------------------------------
int	CR001::R_VC()
{
/*
//비절스튜디오6.0에서 테스트결과 rand()와 똑같은 결과를 얻었습니다. 
The rand() function in the Microsoft C library v4.0. 
Definition: 

SEED = (214013*SEED + 2531011) mod 2**31 
X = int( SEED/2**16 ) 
Returns integer in range [0, 2**15) {0 inclusive, 2**15 exclusive} 
*/
	m_dwVcCallCnt ++;

	m_iVcSeed = _AND_2EXP31( 214013*m_iVcSeed + 2531011 ) ;
	return (int)( m_iVcSeed / _2EXP16 );
}

//-------------------------------------------------------------------------------------------------
int CR001::R_MY()
{
    m_dwMyCallCnt ++;
    m_iMySeed = m_iMySeed * 1103515245L + 12345;
    return ( (int)(m_iMySeed / 65536L) );
}


//-------------------------------------------------------------------------------------------------

/**
 * \ingroup SHO_GS
 * \file	CIngSTATUS.h
 * \brief	케릭터에 적용된 지속상태 처리및 적용
 */
#ifndef	__CINGSTATUS_H
#define	__CINGSTATUS_H
//-------------------------------------------------------------------------------------------------

#define	FLAG_CHEAT_INVINCIBLE			0x80000000		/// 서버에서만 사용

//-------------------------------------------------------------------------------------------------
#define	IDX_ING_STB_DEC_LIFE_TIME		43

class CObjCHAR;
#pragma pack (push, 1)

/**
 * \ingroup SHO_GS_LIB
 * \class	CIngSTATUS
 * \author	wookSang.Jo
 * \brief	케릭터에게 적용된 지속형 상태관리 클래스
 */
class CIngSTATUS {
private :
	/*
	const static BYTE m_btSwitchBitMask[8];// = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80	};
	enum {	BIT_SHIFT=3, WHICH_BIT=0x07 };
	union {
		BYTE	m_btSWITCHES[ sizeof(DWORD)*2 ];	// 32개 스위치
		DWORD	m_dwSWITCHES[ 2 ];
		struct {
			DWORD	m_dwIngStatusFLAG1;
			DWORD	m_dwIngStatusFLAG2;
		} ;
	} ;

	void ClearAllSwitch ()		{	::ZeroMemory (m_btSWITCHES, sizeof(m_btSWITCHES));	}
	BYTE GetBit (int iIndex)	{	return ( m_btSWITCHES[ iIndex >> BIT_SHIFT ] & m_btSwitchBitMask[ iIndex & WHICH_BIT ] ) ? 1 : 0;	}
	void SetBit (int iIndex)	{	m_btSWITCHES[ iIndex >> BIT_SHIFT ] |= m_btSwitchBitMask[ iIndex & WHICH_BIT ];						}
	void ClearBit (int iIndex)	{	m_btSWITCHES[ iIndex >> BIT_SHIFT ] &= ~(m_btSwitchBitMask[ iIndex & WHICH_BIT ]);					}
	BYTE FlipBit (int iIndex)	
	{
		if ( this->GetBit( iIndex ) )
			this->ClearBit( iIndex );
		else
			this->SetBit ( iIndex );

		return this->GetBit( iIndex );
	}
	*/

	DWORD	m_dwIngStatusFLAG;

	struct tagIngPOTION {
		DWORD	m_dwTotTIME;
		DWORD	m_dwAccTIME;
		short	m_nAccVALUE;
		short	m_nTotVALUE;
	} m_POTION[ FLAG_ING_MP+1 ] ;

	short	m_nTICKs    [ ING_MAX_STATUS ];			// 남은 시간
	short	m_nIngSKILL [ ING_MAX_STATUS ];			// 적용된 스킬번호
	short	m_nAdjVALUE [ ING_MAX_STATUS ];			// 적용된 수치
	short	m_nIngSTBIdx[ ING_MAX_STATUS ];			// 중복 적용시 비교위해서...
	int		m_iTargetOBJ[ ING_MAX_STATUS ];			// 대상 오브젝트 번호

	DWORD	m_dwAccTIME;							// 누적 시간

	short   Proc_IngPOTION(tagIngPOTION *pPOTION, DWORD dwPassTIME);

public :
	union {
		// 여신 버프시 보정되는 값
		short		m_nAruaVALUE[ 9 ];
		struct {
			short	m_nAruaAtkSPD;
			short	m_nAruaRunSPD;
			short	m_nAruaCRITICAL;
			short	m_nAruaMaxHP;
			short	m_nAruaMaxMP;
			short	m_nAruaRcvHP;
			short	m_nAruaRcvMP;
			short	m_nAruaRES;
			short	m_nAruaATK;
		} ;
	} ;

	short	Inc_MAX_HP()					{	return m_nAdjVALUE[ ING_INC_MAX_HP ] + m_nAruaMaxHP;			}
	short	Inc_MAX_MP()					{	return m_nAdjVALUE[ ING_INC_MAX_MP ] + m_nAruaMaxMP;			}

	short	Inc_RUN_SPEED()					{	return m_nAdjVALUE[ ING_INC_MOV_SPD	];			}
	short	Dec_RUN_SPEED()					{	return m_nAdjVALUE[ ING_DEC_MOV_SPD	];			}

	short	Inc_ATK_SPEED()					{	return m_nAdjVALUE[ ING_INC_ATK_SPD	];			}
	short	Dec_ATK_SPEED()					{	return m_nAdjVALUE[ ING_DEC_ATK_SPD	];			}

	short	Adj_RUN_SPEED()					{	return m_nAdjVALUE[ ING_INC_MOV_SPD	] - m_nAdjVALUE[ ING_DEC_MOV_SPD	] + m_nAruaRunSPD;		}
	short	Adj_ATK_SPEED()					{	return m_nAdjVALUE[ ING_INC_ATK_SPD	] - m_nAdjVALUE[ ING_DEC_ATK_SPD	];						}
	short	Adj_APOWER()					{	return m_nAdjVALUE[ ING_INC_APOWER	] - m_nAdjVALUE[ ING_DEC_APOWER		] + m_nAruaATK;			}
	short	Adj_DPOWER()					{	return m_nAdjVALUE[ ING_INC_DPOWER	] - m_nAdjVALUE[ ING_DEC_DPOWER		] + m_nAruaRES;			}
	short	Adj_RES()						{	return m_nAdjVALUE[ ING_INC_RES		] - m_nAdjVALUE[ ING_DEC_RES		];						}
	short	Adj_HIT()						{	return m_nAdjVALUE[ ING_INC_HIT		] - m_nAdjVALUE[ ING_DEC_HIT		];						}
	short	Adj_CRITICAL()					{	return m_nAdjVALUE[ ING_INC_CRITICAL] - m_nAdjVALUE[ ING_DEC_CRITICAL	] + m_nAruaCRITICAL;	}
	short	Adj_AVOID()						{	return m_nAdjVALUE[ ING_INC_AVOID	] - m_nAdjVALUE[ ING_DEC_AVOID		];						}

	DWORD	m_dwSubStatusFLAG;
	void	SetSubFLAG( DWORD dwFLAG )		{	m_dwSubStatusFLAG |= dwFLAG;	}
	void	ClearSubFLAG( DWORD dwFLAG )	{	m_dwSubStatusFLAG &= ~dwFLAG;	}
	DWORD	IsSubSET( DWORD dwCheckFLAG )	{	return ( m_dwSubStatusFLAG & dwCheckFLAG );		}
	void	ToggleSubFLAG( DWORD dwFLAG )
	{
		if ( this->IsSubSET( dwFLAG ) )
			this->ClearSubFLAG( dwFLAG );
		else
			this->SetSubFLAG( dwFLAG );
	}
	void	DelArua()
	{
		::ZeroMemory( m_nAruaVALUE, sizeof(m_nAruaVALUE) );
		this->ClearSubFLAG( FLAG_SUB_ARUA_FAIRY );
	}

	void	Reset( bool bFirst )
	{
		if ( bFirst ) {
			m_dwSubStatusFLAG = 0;
			::ZeroMemory( m_nAruaVALUE, sizeof(m_nAruaVALUE) );
		} else
			m_dwSubStatusFLAG &= FLAG_SUB_ARUA_FAIRY;		// 아루아 요정 버프를 제외한 나머지 삭제~

		m_dwAccTIME = 0;
		m_dwIngStatusFLAG = 0;
		::ZeroMemory( m_nAdjVALUE, sizeof(short) * ING_MAX_STATUS );
	}

	void	ClearALL( DWORD dwAndMask=0 )
	{
		m_dwAccTIME = 0;
		m_dwIngStatusFLAG &= dwAndMask;	// FLAG_ING_CLEAR 소환몹 지속시간, 무적 상태 제외, 상점상태
		::ZeroMemory( m_nAdjVALUE, sizeof(short) * ING_MAX_STATUS );
	}
	void	ClearAllGOOD ()
	{
		for (short nI=ING_POISONED; nI<=ING_CHECK_END; nI++) {
			if ( ( FLAG_ING_GOOD & c_dwIngFLAG[ nI ] ) && this->IsSET( c_dwIngFLAG[ nI ] ) ) {
				this->m_nAdjVALUE[ nI ] = 0;
			}
		}
		// 비트 삭제~
		this->ClearStatusFLAG( FLAG_ING_GOOD );
	}
//	void	SetALL( DWORD dwIngStatus )			{	m_dwIngStatusFLAG = dwIngStatus;				}

	DWORD	IsSET( DWORD dwCheckFLAG )			{	return ( m_dwIngStatusFLAG & dwCheckFLAG );		}
	DWORD	GetFLAGs ()							{	return m_dwIngStatusFLAG;						}
	short	GetSkillIDX( eING_TYPE eTYPE )		{	return m_nIngSKILL[ eTYPE ];					}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void	SetFLAG( DWORD dwIngFLAG )			{	m_dwIngStatusFLAG |= dwIngFLAG;					}
	void	ClearStatusFLAG( DWORD dwIngFLAG )	{	m_dwIngStatusFLAG &= ~dwIngFLAG;				}
	void	ClearSTATUS( eING_TYPE IngType )
	{
		this->ClearStatusFLAG( c_dwIngFLAG[ IngType ] );
		m_nAdjVALUE[ IngType ] = 0;
	}
	void	ClearStatusIfENABLED( eING_TYPE IngType )
	{
		if ( this->IsSET( c_dwIngFLAG[ IngType ] ) && STATE_PRIFITS_LOSSES( m_nIngSTBIdx[IngType] ) < 2 ) {
			this->ClearSTATUS( IngType );
		}
	}
	void	ExpireSTATUS( eING_TYPE IngType )	{	m_nTICKs[ IngType ] = 0;	}

	//void	ToggleFLAG( DWORD dwIngFLAG )
	//{
	//	if ( this->IsSET( dwIngFLAG ) )
	//		this->ClearFLAG( dwIngFLAG );
	//	else
	//		this->SetFLAG( dwIngFLAG );
	//}

	bool	IsTauntSTATUS( int iAttackObjIDX, CZoneTHREAD *pCurZone );
	bool	IsIgnoreSTATUS ()
	{
		if ( this->IsSET( FLAG_ING_SLEEP | FLAG_ING_FAINTING ) )
			return true;
		if ( this->IsSubSET( FLAG_SUB_STORE_MODE ) )
			return true;
		return false;
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool	IsEnableApplay( short nIngStbIDX, short nAdjValue );
	bool	IsEnableApplayITEM( short nIngStbIDX );

	DWORD	UpdateIngSTATUS( CObjCHAR *pCharOBJ, short nIngStbIDX, short nTick, short nAdjVALUE, short nSkillIDX, int iTargetObjIDX=0 );
	void	UpdateIngPOTION( short nIngStbIDX, short nTotValue, short nAdjPerSEC );

	DWORD	Proc (CObjCHAR *pCharOBJ, DWORD dwPassTIME);
} ;
#pragma pack (pop)

//-------------------------------------------------------------------------------------------------
#endif
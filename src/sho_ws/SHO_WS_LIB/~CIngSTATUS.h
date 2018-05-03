#ifndef	__CINGSTATUS_H
#define	__CINGSTATUS_H
//-------------------------------------------------------------------------------------------------
/*
// Ablitty to ing status 
const eING_TYPE c_AB2IS[] = {
	ING_NULL,			//	0,
	ING_NULL,			//	1,
	ING_NULL,			//	2,
	ING_NULL,			//	AT_BIRTHSTONE = 3,	// 탄생성
	ING_NULL,			//	AT_CLASS = 4,		// 직업
	ING_NULL,			//	AT_UNION = 5,		// 소속
	ING_NULL,			//	AT_RANK = 6,		// 계급
	ING_NULL,			//	AT_FAME = 7,		// 명성
	ING_NULL,			//	AT_FACE = 8,		// 얼굴모양
	ING_NULL,			//	AT_HAIR,
	ING_NULL,			//	AT_STR = 1ING_NULL,
	ING_NULL,			//	AT_DEX,
	ING_NULL,			//	AT_INT,
	ING_NULL,			//	AT_CON,
	ING_NULL,			//	AT_CHARM,
	ING_NULL,			//	AT_SENSE = 15,
	ING_NULL,			//	AT_HP,
	ING_NULL,			//	AT_MP,
	ING_INC_APOWER,		//	AT_ATK,
	ING_INC_DPOWER,		//	AT_DEF,
	ING_INC_HIT,		//	AT_HIT,
	ING_INC_RES,		//	AT_RES = 21,
	ING_INC_AVOID,		//	AT_AVOID,
	ING_INC_MOV_SPD,	//	AT_SPEED,
	ING_INC_ATK_SPD,	//	AT_ATK_SPD,
	ING_NULL,			//	AT_WEIGHT,
	ING_INC_CRITICAL,	//	AT_CRITICAL = 26,
	ING_NULL,			//	AT_RECOVER_HP,
	ING_NULL,			//	AT_RECOVER_MP

	ING_NULL,			//  29
	ING_NULL,			//  AT_EXP = 30,
	ING_NULL,			//  AT_LEVEL,
	ING_NULL,			//  AT_BONUSPOINT = 32,
	ING_NULL,			//  AT_CHAOS = 33,
	ING_NULL,			//  AT_PK_LEV,
	ING_NULL,			//  AT_HEAD_SIZE,
	ING_NULL,			//  AT_BODY_SIZE,
	ING_NULL,			//  AT_SKILLPOINT = 37,

	ING_INC_MAX_HP,		//  AT_MAX_HP=38,		// 2004. 2. 3 추가 
	ING_INC_MAX_MP,		//  AT_MAX_MP,
} ;
inline eING_TYPE GetIngStatusTYPE(int iAbilityType)
{
	_ASSERT( iAbilityType > 0 && iAbilityType <= AT_MAX_MP );
	return c_AB2IS[ iAbilityType ];
}
*/

//-------------------------------------------------------------------------------------------------
class CObjCHAR;
#pragma pack (push, 1)
class CIngSTATUS 
{
private :
	DWORD	m_dwIngStatusFLAG;

	DWORD	m_dwAccTIME;						// 누적 시간
	short	m_nTICKs    [ ING_MAX_STATUS ];		// 남은 시간
	short	m_nIngSKILL [ ING_MAX_STATUS ];		// 적용된 스킬번호
	short	m_nIngSTBIdx[ ING_MAX_STATUS ];		// 중복 적용시 비교위해서...
	short	m_nAdjVALUE [ ING_MAX_STATUS ];		// 적용된 수치

	DWORD	Proc_ING_HP( CObjCHAR *pCharOBJ );
	DWORD	Proc_ING_MP( CObjCHAR *pCharOBJ );

public :
	
	short	Inc_MAX_HP()					{	return m_nAdjVALUE[ ING_INC_MAX_HP ];			}
	short	Inc_MAX_MP()					{	return m_nAdjVALUE[ ING_INC_MAX_MP ];			}

	short	Inc_RUN_SPEED()					{	return m_nAdjVALUE[ ING_INC_MOV_SPD	];			}
	short	Dec_RUN_SPEED()					{	return m_nAdjVALUE[ ING_DEC_MOV_SPD	];			}

	short	Inc_ATK_SPEED()					{	return m_nAdjVALUE[ ING_INC_ATK_SPD	];			}
	short	Dec_ATK_SPEED()					{	return m_nAdjVALUE[ ING_DEC_ATK_SPD	];			}

	short	Adj_RUN_SPEED()					{	return m_nAdjVALUE[ ING_INC_MOV_SPD	] - m_nAdjVALUE[ ING_DEC_MOV_SPD	];		}
	short	Adj_ATK_SPEED()					{	return m_nAdjVALUE[ ING_INC_ATK_SPD	] - m_nAdjVALUE[ ING_DEC_ATK_SPD	];		}
	short	Adj_APOWER()					{	return m_nAdjVALUE[ ING_INC_APOWER	] - m_nAdjVALUE[ ING_DEC_APOWER		];		}
	short	Adj_DPOWER()					{	return m_nAdjVALUE[ ING_INC_DPOWER	] - m_nAdjVALUE[ ING_DEC_DPOWER		];		}
	short	Adj_RES()						{	return m_nAdjVALUE[ ING_INC_RES		] - m_nAdjVALUE[ ING_DEC_RES		];		}
	short	Adj_HIT()						{	return m_nAdjVALUE[ ING_INC_HIT		] - m_nAdjVALUE[ ING_DEC_HIT		];		}
	short	Adj_CRITICAL()					{	return m_nAdjVALUE[ ING_INC_CRITICAL] - m_nAdjVALUE[ ING_DEC_CRITICAL	];		}
	short	Adj_AVOID()						{	return m_nAdjVALUE[ ING_INC_AVOID	] - m_nAdjVALUE[ ING_DEC_AVOID		];		}

	void	ClearALL()
	{
		m_dwAccTIME = 0;
		m_dwIngStatusFLAG = 0;
		::ZeroMemory( m_nAdjVALUE, sizeof(short) * ING_MAX_STATUS );
	}
//	void	SetALL( DWORD dwIngStatus )		{	m_dwIngStatusFLAG = dwIngStatus;				}

	DWORD	IsSET( DWORD dwCheckFLAG )		{	return ( m_dwIngStatusFLAG & dwCheckFLAG );		}
	DWORD	GetFLAGs ()						{	return m_dwIngStatusFLAG;						}
	short	GetSkillIDX( eING_TYPE eTYPE )	{	return m_nIngSKILL[ eTYPE ];					}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void	SetFLAG( DWORD dwIngFLAG )		{	m_dwIngStatusFLAG |= dwIngFLAG;					}
	void	ClearFLAG( DWORD dwIngFLAG )	{	m_dwIngStatusFLAG &= ~dwIngFLAG;				}
	void	ToggleFLAG( DWORD dwIngFLAG )
	{
		if ( this->IsSET( dwIngFLAG ) )
			this->ClearFLAG( dwIngFLAG );
		else
			this->SetFLAG( dwIngFLAG );
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool	IsEnableApplay( short nIngStbIDX );

	void	UpdateIngSTATUS( CObjCHAR *pCharOBJ, short nIngStbIDX, BYTE btTick, short nAdjVALUE, short nSkillIDX );
	DWORD	UpdateIngPOTION( CObjCHAR *pCharOBJ, short nIngStbIDX, BYTE btTick, short nAdjVALUE );
	DWORD	Proc (CObjCHAR *pCharOBJ, DWORD dwPassTIME);
} ;
#pragma pack (pop)

//-------------------------------------------------------------------------------------------------
#endif
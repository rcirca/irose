
#include "stdAFX.h"
#include "IO_STB.h"
#include "CIngSTATUS.h"

// const BYTE CIngSTATUS::m_btSwitchBitMask[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80	};

//-------------------------------------------------------------------------------------------------
bool CIngSTATUS::IsTauntSTATUS( int iAttackObjIDX, CZoneTHREAD *pCurZone )
{
	if ( this->IsSET( c_dwIngFLAG[ ING_TAUNT ] ) ) {
		CObjCHAR *pDestCHAR = g_pObjMGR->Get_CharOBJ( this->m_iTargetOBJ[ ING_TAUNT ], true );
		if ( pDestCHAR ) {
			// 타운트를 건넘이면 타운트 상태가 아닌것으로...
			if ( this->m_iTargetOBJ[ ING_TAUNT ] == iAttackObjIDX )
				return false;

			if ( pCurZone == pDestCHAR->GetZONE() )
				return true;
		}

		// 타겟이 없거나 존을 이동했다...타운트 풀리도록..
		m_nTICKs[ ING_TAUNT ] = 0;
		return false;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
bool CIngSTATUS::IsEnableApplayITEM( short nIngStbIDX )
{
	short nTypeIDX = STATE_TYPE(nIngStbIDX);

	// 이미 적용되어 있는가 ?
	if ( this->IsSET( c_dwIngFLAG[ nTypeIDX ] ) ) {
		if ( 0 == STATE_CAN_DUPLICATED(nIngStbIDX) ) {
			// 갱신 적용 불가면 무조건 취소...
			return false;
		}

		// 적용 되어 있는것 보다 우선 순위가 있는가 ?
		if ( m_nIngSTBIdx[ nTypeIDX ] > nIngStbIDX ) {
			return false;
		}
	}

	return true;
}
bool CIngSTATUS::IsEnableApplay( short nIngStbIDX, short nAdjValue )
{
	short nTypeIDX = STATE_TYPE(nIngStbIDX);

	// 이미 적용되어 있는가 ?
	if ( this->IsSET( c_dwIngFLAG[ nTypeIDX ] ) ) {
		if ( 0 == STATE_CAN_DUPLICATED(nIngStbIDX) ) {
			// 갱신 적용 불가면 무조건 취소...
			return false;
		}

		if ( m_nAdjVALUE[ nTypeIDX ] > nAdjValue ) {
			return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
DWORD CIngSTATUS::UpdateIngSTATUS( CObjCHAR *pCharOBJ, short nIngStbIDX, short nTick, short nAdjVALUE, short nSkillIDX, int iTargetObjIDX )
{
	// 반드시 적용 가능여부 판단한후 호출되어야 한다 !!!
	short nTypeIDX = STATE_TYPE(nIngStbIDX);

	//if ( nTypeIDX <= ING_INC_MP ) {
	//	// 스킬로는 물약으로 변경되는 상태를 변경할수 없도록...
	//	return 0;
	//}

	if ( nTypeIDX > ING_CHECK_END ) {
		switch( nTypeIDX ) {
			case ING_CLEAR_INVISIBLE :	// 투명상태 해지..
			{
				if ( this->IsSET( c_dwIngFLAG[ ING_DISGUISE ] ) ) {
					m_nTICKs[ ING_DISGUISE ] = 0;		// 바로 삭제 하지 말고 다음 처리해 주위에 전송될수 있도록..
				}
				if ( this->IsSET( c_dwIngFLAG[ ING_TRANSPARENT ] ) ) {
					m_nTICKs[ ING_TRANSPARENT ] = 0;	// 바로 삭제 하지 말고 다음 처리해 주위에 전송될수 있도록..
				}
				return 0;
			}
			case ING_CLEAR_GOOD :	// 유리 
			{
				for (short nI=ING_POISONED; nI<=ING_CHECK_END; nI++) {
					if ( this->IsSET( c_dwIngFLAG[ nI ] ) && STATE_PRIFITS_LOSSES( m_nIngSTBIdx[nI] ) == 0 ) {
						m_nTICKs[ nI ] = 0;			// 바로 삭제 하지 말고 다음 처리해 주위에 전송될수 있도록..
					}
				}
				return 0;
			}
			case ING_CLEAR_BAD  :	// 불리
			{
				for (short nI=ING_POISONED; nI<=ING_CHECK_END; nI++) {
					if ( this->IsSET( c_dwIngFLAG[ nI ] ) && STATE_PRIFITS_LOSSES( m_nIngSTBIdx[nI] ) == 1 ) {
						m_nTICKs[ nI ] = 0;			// 바로 삭제 하지 말고 다음 처리해 주위에 전송될수 있도록..
					}
				}
				if ( this->IsSET( FLAG_ING_TAUNT ) ) {
					m_nTICKs[ ING_TAUNT ] = 0;
				}
				return 0;
			}
			case ING_CLEAR_ALL  :
			{
				// 전체 해지시 물약도 해지되기 때문에...
				// this->ClearALL( FLAG_ING_CLEAR );		// 상점 상태는 유지.
				for (short nI=ING_POISONED; nI<=ING_CHECK_END; nI++) {
					if ( this->IsSET( c_dwIngFLAG[ nI ] ) && STATE_PRIFITS_LOSSES( m_nIngSTBIdx[nI] ) < 2 ) {
						this->ClearSTATUS( (eING_TYPE)nI );
					}
				}
				return 0;
			}
			case ING_DEC_LIFE_TIME :
				this->SetFLAG( c_dwIngFLAG[ nTypeIDX ] );
				this->m_nIngSTBIdx[ nTypeIDX ] = nIngStbIDX;
				return 0;
		}

		// 타운트등...
	}
	
	this->SetFLAG( c_dwIngFLAG[ nTypeIDX ] );

	m_nAdjVALUE [ nTypeIDX ] = nAdjVALUE;
	m_nTICKs    [ nTypeIDX ] = nTick;
	m_nIngSTBIdx[ nTypeIDX ] = nIngStbIDX;
	m_nIngSKILL [ nTypeIDX ] = nSkillIDX;
	m_iTargetOBJ[ nTypeIDX ] = iTargetObjIDX;

/*
	삭제~~~
	if ( c_dwIngFLAG[ nTypeIDX ] & ( FLAG_ING_MAX_HP | FLAG_ING_MAX_MP ) ) {
		// 최대 HP,MP가 올라가는 경우 현재 수치까지 같이 올리자...	2004. 4. 12
		// 먼저 MAX_HP를 올려야 아래서 현재 피가 오른다.
		switch( nTypeIDX ) {
			case ING_INC_MAX_HP :	pCharOBJ->Add_HP( nAdjVALUE );	break;
			case ING_INC_MAX_MP :	pCharOBJ->Add_MP( nAdjVALUE );	break;
		}
	}
*/

	return ( c_dwIngFLAG[ nTypeIDX ] & ( FLAG_ING_FAINTING | FLAG_ING_SLEEP ) );
}

void CIngSTATUS::UpdateIngPOTION( short nIngStbIDX, short nTotValue, short nAdjPerSEC )
{
	// 반드시 적용 가능여부 판단한후 호출되어야 한다 !!!
	short nTypeIDX = STATE_TYPE(nIngStbIDX);

	if ( nTypeIDX > FLAG_ING_MP )
		return;
	
	this->SetFLAG( c_dwIngFLAG[ nTypeIDX ] );

	m_nIngSTBIdx[ nTypeIDX ] = nIngStbIDX;

	m_POTION[ nTypeIDX ].m_dwTotTIME = ( 1000 * nTotValue / nAdjPerSEC );	// 전체 필요시간

	assert( m_POTION[ nTypeIDX ].m_dwTotTIME );

	m_POTION[ nTypeIDX ].m_dwAccTIME = 0;									// 누적시간.
	m_POTION[ nTypeIDX ].m_nTotVALUE = nTotValue;
	m_POTION[ nTypeIDX ].m_nAccVALUE = 0;
}

short CIngSTATUS::Proc_IngPOTION(tagIngPOTION *pPOTION, DWORD dwPassTIME)
{
	pPOTION->m_dwAccTIME += dwPassTIME;

	assert( pPOTION->m_dwTotTIME );

	int iCurAccValue = floor_int( pPOTION->m_dwAccTIME * pPOTION->m_nTotVALUE / pPOTION->m_dwTotTIME );

	short nDiff;
	if ( iCurAccValue >= pPOTION->m_nTotVALUE ) {
		// 적용끝...
		nDiff = pPOTION->m_nTotVALUE - pPOTION->m_nAccVALUE;
	} else {
		nDiff = iCurAccValue - pPOTION->m_nAccVALUE;
	}
	pPOTION->m_nAccVALUE += nDiff;

	return nDiff;
}


//-------------------------------------------------------------------------------------------------
#define	ONE_SECOND	1000
DWORD CIngSTATUS::Proc (CObjCHAR *pCharOBJ, DWORD dwPassTIME)
{
	DWORD dwChangedFLAG = 0;

	// 물약 처리... 매 프레임별...
	if ( this->IsSET( FLAG_ING_HP | FLAG_ING_MP ) ) {
		short nAdj;
		tagIngPOTION *pPOTION;
		BYTE btIngEND = 0;
		if ( this->IsSET( FLAG_ING_HP ) ) {
			pPOTION = &m_POTION[ ING_INC_HP ];
			nAdj = this->Proc_IngPOTION( pPOTION, dwPassTIME );

			//LogString( 0xfff, "HP: %d,  %d/%d,   %d  PassTIME:%d \n", nAdj, pPOTION->m_nAccVALUE, pPOTION->m_nTotVALUE, pCharOBJ->Get_HP(), dwPassTIME );
					
			if ( pCharOBJ->Add_HP( nAdj ) >= pCharOBJ->Get_MaxHP() ||
				 pPOTION->m_nAccVALUE >= pPOTION->m_nTotVALUE ) {
				// 처리 완료 or 만땅.
				this->ClearStatusFLAG( FLAG_ING_HP );
				dwChangedFLAG |= FLAG_ING_HP;
				btIngEND = 0x01;
			}
		}

		if ( this->IsSET( FLAG_ING_MP ) ) {
			pPOTION = &m_POTION[ ING_INC_MP ];
			nAdj = this->Proc_IngPOTION( pPOTION, dwPassTIME );

			//LogString( 0xfff, "MP: %d,  %d/%d,   %d  PassTIME:%d \n", nAdj, pPOTION->m_nAccVALUE, pPOTION->m_nTotVALUE, pCharOBJ->Get_MP(), dwPassTIME );

			if ( pCharOBJ->Add_MP( nAdj ) >= pCharOBJ->Get_MaxMP() ||
				 pPOTION->m_nAccVALUE >= pPOTION->m_nTotVALUE ) {
				// 처리 완료 or 만땅.
				this->ClearStatusFLAG( FLAG_ING_MP );
				dwChangedFLAG |= FLAG_ING_MP;
				btIngEND |= 0x02;
			}
		}

		if ( btIngEND ) {
			pCharOBJ->Send_gsv_SET_HPnMP( btIngEND );
		}
	}

	// 1초당 처리 할것들...
	m_dwAccTIME += dwPassTIME;
	if ( m_dwAccTIME < ONE_SECOND )
		return dwChangedFLAG;
	m_dwAccTIME -= ONE_SECOND;

	if ( this->IsSET( FLAG_ING_PROC_PER_TICK ) ) {
		// 중독...
		if ( this->IsSET( FLAG_ING_POISONED ) ) {
			if ( m_nTICKs[ ING_POISONED ]-- > 0 ) {
				// dec hp
				if ( pCharOBJ->Get_HP() > 0 ) {
					short nValue = STATE_APPLY_ABILITY_VALUE( m_nIngSTBIdx[ ING_POISONED ], 0 );

					if ( pCharOBJ->Get_HP() > nValue )
						pCharOBJ->Sub_HP( nValue );
					else
						pCharOBJ->Set_HP( 1 );
				}
			} else {
				this->ClearStatusFLAG( FLAG_ING_POISONED );
				dwChangedFLAG |= FLAG_ING_POISONED;
			}
		}

		// 소환된 몹의 생명 시간....
		if ( this->IsSET( FLAG_ING_DEC_LIFE_TIME ) ) {
			if ( pCharOBJ->Get_HP() > 0 ) {
				short nValue = STATE_APPLY_ABILITY_VALUE( m_nIngSTBIdx[ ING_DEC_LIFE_TIME ], 0 );

				if ( pCharOBJ->Get_HP() > nValue ) {
					pCharOBJ->Sub_HP( nValue );
				} else {
					// 자살 시킨다...
					pCharOBJ->Add_DAMAGE( nValue );
					return 0;
				}
			}
		}
	}

	// 비트 플레그 집합
	if ( this->IsSET( FLAG_ING_CHECK_PER_TICK ) ) {
		// 최대 HP
		if ( this->IsSET( FLAG_ING_MAX_HP ) ) {
			if ( m_nTICKs[ ING_INC_MAX_HP ]-- <= 0 ) {
				this->ClearSTATUS( ING_INC_MAX_HP );
				pCharOBJ->Add_HP( 0 );
				dwChangedFLAG |= FLAG_ING_MAX_HP;
			}
		}

		// 최대 MP
		if ( this->IsSET( FLAG_ING_MAX_MP ) ) {
			if ( m_nTICKs[ ING_INC_MAX_MP ]-- <= 0 ) {
				this->ClearSTATUS( ING_INC_MAX_MP );
				pCharOBJ->Add_MP( 0 );
				dwChangedFLAG |= FLAG_ING_MAX_MP;
			}
		}

		// 유지 시간 체크..
		for (short nI=ING_CHECK_START; nI<=ING_CHECK_END; nI++) {
			if ( !this->IsSET( c_dwIngFLAG[ nI ] ) )
				continue;
			
			if ( m_nTICKs[ nI ]-- <= 0 ) {
				// 시간이 완료 되었으면...
				this->ClearSTATUS( (eING_TYPE)nI );
				dwChangedFLAG |= c_dwIngFLAG[ nI ];
			}
		}

		// 타운트..
		if ( this->IsSET( FLAG_ING_TAUNT ) ) {
			if ( m_nTICKs[ ING_TAUNT ]-- <= 0 ) {
				this->m_iTargetOBJ[ ING_TAUNT ] = 0;
				this->ClearSTATUS( ING_TAUNT );
				dwChangedFLAG |= FLAG_ING_TAUNT;
			}
		}

		// 부활
		if ( this->IsSET( FLAG_ING_REVIVE ) ) {
			if ( m_nTICKs[ ING_REVIVE ]-- <= 0 ) {
				this->ClearSTATUS( ING_REVIVE );
				dwChangedFLAG |= FLAG_ING_REVIVE;
			}
		}
	}

	return dwChangedFLAG;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

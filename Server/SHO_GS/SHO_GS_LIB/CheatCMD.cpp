
#include "LIB_gsMAIN.h"
#include "GS_USER.h"
#include "ZoneLIST.h"
#include "GS_ListUSER.h"
#include "IO_Quest.h"
#include "GS_SocketLSV.h"

//-------------------------------------------------------------------------------------------------

char *l_szAbility[] = {
	"STR",
	"DEX",
	"INT",
	"CON",
	"CHA",
	"SEN",

	NULL
} ;

DWORD classUSER::A_Cheater ()
{
#ifdef	__INC_WORLD
	return this->m_dwRIGHT & 0x7fffffff;
#else
	if ( CLIB_GameSRV::GetInstance()->IsTestServer() ) {
		// 테섭 : 마스타, 개발자
		return this->m_dwRIGHT & ( RIGHT_MASTER | RIGHT_DEV );
	} else {
		// 본섭 : 마스타
		return this->m_dwRIGHT & RIGHT_MASTER;
	}
#endif
}
DWORD classUSER::B_Cheater ()
{
	if ( CLIB_GameSRV::GetInstance()->IsTestServer() ) {
		// 테섭 : 마스타, 개발자, 마스타gm, 일반gm
		return this->m_dwRIGHT & ( RIGHT_MASTER | RIGHT_DEV | RIGHT_MG | RIGHT_NG );
	} else {
		// 본섭 : 마스타, 마스타gm,
		return this->m_dwRIGHT & ( RIGHT_MASTER | RIGHT_MG );
	}
}
DWORD classUSER::C_Cheater ()
{
	// 마스터, 개발자, 마스타gm, 일반 gm
	return this->m_dwRIGHT & ( RIGHT_MASTER | RIGHT_DEV | RIGHT_MG | RIGHT_NG );
}
DWORD classUSER::GM_Cheater ()
{
	if ( CLIB_GameSRV::GetInstance()->IsTestServer() ) {
		// 테섭 : 마스타, 개발자, 마스타gm, 일반gm
		return this->m_dwRIGHT & ( RIGHT_MASTER | RIGHT_DEV | RIGHT_MG | RIGHT_NG );
	} else {
		// 본섭 : 마스타, 마스타gm,
		return this->m_dwRIGHT & ( RIGHT_MASTER | RIGHT_MG | RIGHT_NG );
	}
}

DWORD classUSER::TWGM_Cheater ()
{
	// kchs : 2005-08-17 , 512와 256와 중간 권한
	return this->m_dwRIGHT & ( RIGHT_TWG | RIGHT_MG | RIGHT_DEV | RIGHT_MASTER );
}

//-------------------------------------------------------------------------------------------------
// C등급..
short classUSER::Cheat_where (CStrVAR *pStrVAR, char *pArg1/*szCharName*/, char *pArg2/*szAccount*/, char *szCode )
{
	classUSER *pUSER=NULL;
	if ( pArg2 ) {
		pUSER = g_pUserLIST->Find_ACCOUNT( pArg2 );
	} else {
		pUSER = g_pUserLIST->Find_CHAR( pArg1 );
	}

	if ( pUSER ) {
		if ( pUSER->GetZONE() ) {
			// CStrVAR *pCStr = this->GetZONE()->GetStrVAR ();

			pStrVAR->Printf ("%s:%d ( %.0f, %.0f )", pUSER->GetZONE()->Get_NAME(), pUSER->GetZONE()->Get_ZoneNO(), pUSER->m_PosCUR.x, pUSER->m_PosCUR.y );
			this->Send_gsv_WHISPER( pUSER->Get_NAME(), pStrVAR->Get() );
		}
	} else {
		// 월드 서버로 전송.
		g_pSockLSV->Send_gsv_CHEAT_REQ( this, this->m_dwWSID, 0, szCode );
	}
	return CHEAT_NOLOG;
}
// C등급..
short classUSER::Cheat_account (char *pArg1, char *szCode )
{
	// 케릭 이름으로 계정 얻기
	classUSER *pUSER=NULL;
	pUSER = g_pUserLIST->Find_CHAR( pArg1 );
	if ( pUSER && pUSER->m_dwRIGHT <= RIGHT_DEV ) {
		this->Send_gsv_WHISPER( pUSER->Get_NAME(), pUSER->Get_ACCOUNT() );
	} else {
		// 월드 서버로 전송.
		g_pSockLSV->Send_gsv_CHEAT_REQ( this, this->m_dwWSID, 0, szCode );
	}
	return CHEAT_NOLOG;
}
// C등급..
short classUSER::Cheat_move (char *pArg1, char *pArg2, char *szCode )
{
	classUSER *pUSER=NULL;
	if ( pArg2 ) {
		pUSER = g_pUserLIST->Find_ACCOUNT( pArg2 );
	} else
	if ( pArg1 ) {
		pUSER = g_pUserLIST->Find_CHAR( pArg1 );
	}

	if ( pUSER ) {
		if ( pUSER->GetZONE() ) {
			this->Proc_TELEPORT( pUSER->GetZONE()->Get_ZoneNO(), pUSER->m_PosCUR );	// "/move"
		}
	} else {
		// 월드 서버로 전송.
		g_pSockLSV->Send_gsv_CHEAT_REQ( this, this->m_dwWSID, 0, szCode );
	}
	return CHEAT_NOLOG;
}
// C등급..
short classUSER::Cheat_mm (short nZoneNO, char *pArg2, char *pArg3)
{
	if ( !this->Is_CartGuest() && g_pZoneLIST->IsValidZONE(nZoneNO) ) {
		tPOINTF PosWARP;

		PosWARP.x = atoi( pArg2 ) * 1000.f;
		PosWARP.y = atoi( pArg3 ) * 1000.f;

		POINTS  PosSEC;
		PosSEC.x = (short) ( PosWARP.x / g_pZoneLIST->GetSectorSIZE(nZoneNO) );
		PosSEC.y = (short) ( PosWARP.y / g_pZoneLIST->GetSectorSIZE(nZoneNO) );

		// y축 체크는 안됨 - 로컬존이 아닐경우 g_pZoneLIST->GetZONE(nZoneNO)->Get_SectorYCNT()에서 뻑~~
		if ( PosSEC.x >= 0 && PosSEC.y >= 0 )
		{
			this->Proc_TELEPORT( nZoneNO, PosWARP );		// "/mm"
			return CHEAT_NOLOG;
		}
	}
	return CHEAT_INVALID;
}
// C등급
short classUSER::Cheat_call ( char *pArg1, char *pArg2, char *szCode )
{
	classUSER *pUSER=NULL;
	if ( pArg2 ) {
		pUSER = g_pUserLIST->Find_ACCOUNT( pArg2 );
	} else
	if ( pArg1 ) {
		pUSER = g_pUserLIST->Find_CHAR( pArg1 );
	}
	if ( pUSER ) {
		if ( this->m_dwRIGHT >= pUSER->m_dwRIGHT ) {
			// pUSER에게 바로 Send_gsv_TELEPORT_REPLY을 날리는것은 
			// pUSER의 존이 NULL이 되어 운이 나쁘면 pUSER루프시 뻑~~~
			pUSER->Send_gsv_RELAY_REQ( RELAY_TYPE_RECALL, this->GetZONE()->Get_ZoneNO(), this->m_PosCUR );
		}
	} else {
		// 월드 서버로 전송.
		// 나의 존/좌표를 전송하여 상대방으로 하여금 오게끔...
		g_pSockLSV->Send_gsv_CHEAT_REQ( this, this->m_dwWSID, 0, szCode );
	}
	return CHEAT_INVALID;
}
// C등급..
short classUSER::Cheat_out ( char *pArg1, char *pArg2, char *szCode )
{
	classUSER *pUSER=NULL;
	if ( pArg2 )	pUSER = g_pUserLIST->Find_ACCOUNT( pArg2 );
	else
	if ( pArg1 )	pUSER = g_pUserLIST->Find_CHAR( pArg1 );

	if ( pUSER ) {
		if ( this->m_dwRIGHT >= pUSER->m_dwRIGHT ) {
			// 권한이 같거나 높아야...
			pUSER->Send_gsv_GM_COMMAND( this->Get_NAME(), GM_CMD_LOGOUT );
			return CHEAT_PROCED;
		}
	} else {
		g_pSockLSV->Send_gsv_CHEAT_REQ( this, this->m_dwWSID, 0, szCode );
	}
	return CHEAT_INVALID;
}
// C등급..
short classUSER::Cheat_shut ( char *pArg1, char *pArg2, char *pArg3, char *szCode )
{
	WORD wBlockTIME = 1;
	if ( pArg2 ) {
		wBlockTIME = (WORD)atoi( pArg2 );
		if ( wBlockTIME > 20 )
			wBlockTIME = 20;
	}

	classUSER *pUSER=NULL;
	if ( pArg3 )	pUSER = g_pUserLIST->Find_ACCOUNT( pArg3 );
	else 
	if ( pArg1 )	pUSER = g_pUserLIST->Find_CHAR( pArg1 );

	if ( pUSER ) {
		if ( this->m_dwRIGHT >= pUSER->m_dwRIGHT ) {
			pUSER->Send_gsv_GM_COMMAND( this->Get_NAME(), GM_CMD_SHUT, wBlockTIME );
			return CHEAT_PROCED;
		} 
	} else {
		g_pSockLSV->Send_gsv_CHEAT_REQ( this, this->m_dwWSID, 0, szCode );
	}
	return CHEAT_INVALID;
}

// 선택적...
short classUSER::Cheat_npc ( CStrVAR *pStrVAR, CObjNPC *pNPC, int iNpcIDX, char *pArg2, char *pArg3, char *pArg4 )
{
	if ( pArg2 ) {
		if ( !strcmpi(pArg2, "ENABLE" ) ) {
			;
		} else 
		if ( !strcmpi(pArg2, "DISABLE" ) ) {
			;
		} else
		if ( !strcmpi(pArg2, "MOVE") ) {	// 모두 가능
			if ( pNPC->GetZONE() && pNPC->GetZONE()->Get_ZoneNO() ) {
				this->Proc_TELEPORT( pNPC->GetZONE()->Get_ZoneNO(), pNPC->m_PosCUR );
			}
			return CHEAT_NOLOG;
		} else
		if ( !strcmpi(pArg2, "ADD") ) {
			if ( A_Cheater() && pArg3 ) {
				int iQuestIDX = atoi( pArg3 );
				if ( iQuestIDX >= 0 ) {
					pNPC = new CObjNPC;
					// float fModelDIR = quaternionToModelDirection( fQW, fQX, fQY, fQZ );
					// ( (CObjNPC*)pNewCHAR )->Set_DIR( fModelDIR );
					if ( !pNPC->Init( this->GetZONE(), iNpcIDX, this->m_PosCUR.x, this->m_PosCUR.y, TEAMNO_NPC, NULL, iQuestIDX ) ) {
						SAFE_DELETE( pNPC );
					} else
						g_pZoneLIST->Add_LocalNPC( pNPC );
				}
			}
		} else
		if ( !strcmpi(pArg2, "CALL")  ) {
			if ( A_Cheater() && this->GetZONE() == pNPC->GetZONE() ) {
				this->GetZONE()->Sub_DIRECT( pNPC );
				pNPC->m_PosCUR = this->m_PosCUR;
				this->GetZONE()->Add_OBJECT( pNPC );
			}
		} else
		if ( !strcmpi(pArg2, "DEL") ) {
			if ( A_Cheater() && this->GetZONE() == pNPC->GetZONE() ) {
				this->GetZONE()->Sub_DIRECT( pNPC );
			    SAFE_DELETE( pNPC );
			}
		} else
		if ( !strcmpi(pArg2, "EVENT") ) {
			if ( B_Cheater() && pArg3 ) {
				int iQuestIDX = atoi( pArg3 );
				if ( iQuestIDX >= 0 )
					pNPC->m_nQuestIDX = iQuestIDX;
			}
		} else
		if ( !strcmpi(pArg2, "DIR") ) {
			if ( B_Cheater() && pArg3 ) {
				float fDir = (float)atof( pArg3 );
				pNPC->Set_DIR( fDir );
			}
		} else 
		if ( !strcmpi(pArg2, "VAR") && pArg3 && pArg4 ) {
			if ( B_Cheater() && pArg4 ) {
				int iVarIDX   = atoi( pArg3 );
				int iVarVALUE = atoi( pArg4 ); 
				pNPC->Set_ObjVAR( iVarIDX, iVarVALUE );
			}
		}
		/*
		else
		if ( !strcmpi(pArg2, "getVAR") && pArg3 ) {
			int iVarIDX   = atoi( pArg3 );
			xxx = pNPC->Get_ObjVAR( iVarIDX, iVarVALUE );
			pStrVAR->Printf ("%s Zone:%d, Pos(%.0f,%.0f)", pNPC->Get_NAME(), pNPC->GetZONE()->Get_ZoneNO(), pNPC->m_PosCUR.x, pNPC->m_PosCUR.y );
			this->Send_gsv_WHISPER( "SERVER", pStrVAR->Get() );
		}
		*/
		return CHEAT_PROCED;
	}

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return CHEAT_INVALID;

	pCPacket->m_HEADER.m_wType = GSV_SERVER_DATA;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SERVER_DATA );
	pCPacket->m_gsv_SERVER_DATA.m_btDataTYPE = SERVER_DATA_NPC;
	pCPacket->AppendData( pNPC->m_pVAR, sizeof( tagObjVAR ) );

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	
	pStrVAR->Printf ("%s Zone:%d, Pos(%.0f,%.0f)", pNPC->Get_NAME(), pNPC->GetZONE()->Get_ZoneNO(), pNPC->m_PosCUR.x, pNPC->m_PosCUR.y );
	this->Send_gsv_WHISPER( "SERVER", pStrVAR->Get() );

	return CHEAT_PROCED;
}

short classUSER::Cheat_add ( char *pArg1, char *pArg2, char *pArg3, char *szCode )
{
	if ( B_Cheater() ) {
		if ( NULL == pArg2 ) {
			if ( !strcmpi(pArg1, "arua") ) {
				this->Del_Goddess ();
				//this->Add_Goddess ();
				return CHEAT_PROCED;
			}
		} else {
			classUSER *pUSER=NULL;
			if ( pArg3 ) {
				// 올려줄 대상이 있는가 ?
				pUSER = g_pUserLIST->Find_CHAR( pArg3 );
				if ( NULL == pUSER ) {
					// 월드 서버로 전송.
					g_pSockLSV->Send_gsv_CHEAT_REQ( this, this->m_dwWSID, 0, szCode );
					return CHEAT_PROCED;
				}
			} else
				pUSER = this;

			if ( NULL == pUSER->GetZONE() ) {
				return 0;
			}
			if ( !strcmpi(pArg1, "EXP") ) {
				int iExp = atoi(pArg2);
				if ( iExp < 0 ) iExp = 1;

				// 경험치를 올려줄 대상이 있는가 ?
				pUSER->Add_EXP( iExp, true, 0 );
				return CHEAT_PROCED;
			} else
			if ( !strcmpi(pArg1, "BP") ) {
				int iValue = atoi( pArg2 );
				pUSER->AddCur_BonusPOINT( iValue );

				if ( pUSER->GetCur_BonusPOINT() < 0	   ) pUSER->SetCur_BonusPOINT( 0 );
				else
				if ( pUSER->GetCur_BonusPOINT() > 9999 ) pUSER->SetCur_BonusPOINT( 9999 );
				return CHEAT_PROCED;
			} else
			if ( !strcmpi(pArg1, "SP") ) {
				int iValue = atoi( pArg2 );
				pUSER->AddCur_SkillPOINT( iValue );

				if ( pUSER->GetCur_SkillPOINT() < 0	   ) pUSER->SetCur_BonusPOINT( 0 );
				else
				if ( pUSER->GetCur_SkillPOINT() > 9999 ) pUSER->SetCur_BonusPOINT( 9999 );
				return CHEAT_PROCED;
			} else
			if ( !strcmpi(pArg1, "UP") ) {
				if ( pUSER->GetCur_JOHAP() ) {
					int iValue = atoi( pArg2 );
					pUSER->AddCur_JoHapPOINT( pUSER->GetCur_JOHAP(), iValue );

					if ( pUSER->GetCur_JoHapPOINT( pUSER->GetCur_JOHAP() ) < 0	   ) pUSER->SetCur_JoHapPOINT( pUSER->GetCur_JOHAP(), 0 );
					else
					if ( pUSER->GetCur_JoHapPOINT( pUSER->GetCur_JOHAP() ) > 99999 ) pUSER->SetCur_JoHapPOINT( pUSER->GetCur_JOHAP(), 99999 );
				}
				return CHEAT_PROCED;
			} else
			if ( !strcmpi(pArg1, "MONEY") ) {
				pUSER->Add_CurMONEY( atoi(pArg2) );
				return CHEAT_PROCED;
			} else
			if ( !strcmpi(pArg1, "SKILL") ) {	// 스킬을 배운것으로...
				short nSkillIDX = atoi( pArg2 );
				if ( nSkillIDX >= 1 && nSkillIDX < g_SkillList.Get_SkillCNT() ) {
					if ( SKILL_ICON_NO( nSkillIDX ) ) {
						pUSER->Send_gsv_SKILL_LEARN_REPLY( nSkillIDX );
					}
				}
				return CHEAT_PROCED;
			} else
			if ( !strcmpi(pArg1, "QUEST") ) {
				short nQuestIDX = atoi(pArg2);
				short nSlot = this->Quest_Append( nQuestIDX );
				if ( nSlot >= 0 ) 
					this->Send_gsv_QUEST_REPLY( RESULT_QUEST_REPLY_ADD_SUCCESS, (BYTE)nSlot, nQuestIDX );
				else
					this->Send_gsv_QUEST_REPLY( RESULT_QUEST_REPLY_ADD_FAILED, 0, nQuestIDX );

				return CHEAT_PROCED;
			}
		}
	}
	return CHEAT_INVALID;
}

short classUSER::Cheat_del ( CStrVAR *pStrVAR, char *pArg1, char *pArg2, char *pArg3 )
{
	if ( !strcmpi(pArg1, "arua" ) ) {
		this->Del_Goddess ();
		return CHEAT_PROCED;
	}

	if ( A_Cheater() && pArg2 ) {
		classUSER *pUSER=NULL;
		if ( pArg3 ) {
			pUSER = g_pUserLIST->Find_CHAR( pArg3 );
			if ( NULL == pUSER || this->GetZONE() != pUSER->GetZONE() )
				return CHEAT_INVALID;
		} else
			pUSER = this;

		if ( !strcmpi(pArg1, "HP") ) {
			pUSER->Set_HP( 1 );
			pUSER->Set_MP( 0 );
			return CHEAT_PROCED;
		} else
		if ( !strcmpi(pArg1, "EXP") ) {
			pUSER->m_GrowAbility.m_lEXP = 0;
			pUSER->Add_EXP( 0, false, 0 );
			return CHEAT_PROCED;
		} else
		if ( !strcmpi(pArg1, "BP") ) {
			pUSER->m_GrowAbility.m_nBonusPoint = 0;
			return CHEAT_SEND;
		} else
		if ( !strcmpi(pArg1, "SP") ) {
			pUSER->m_GrowAbility.m_nSkillPoint = 0;
			return CHEAT_SEND;
		} else
		if ( !strcmpi(pArg1, "SKILL") ) {
			if ( !strcmpi( pArg2, "ALL" ) ) {
				pUSER->Reward_InitSKILL();
			} else {
				short nSkillIDX = atoi( pArg2 );
				for (short nSlot=0; nSlot<MAX_LEARNED_SKILL_CNT; nSlot++) {
					if ( 0 == nSkillIDX || pUSER->m_Skills.m_nSkillINDEX[ nSlot ] == nSkillIDX )
						pUSER->m_Skills.m_nSkillINDEX[ nSlot ] = 0;
				}
			}
			return CHEAT_SEND;
		} else
		if ( !strcmpi(pArg1, "MONEY") ) {
			pUSER->SetCur_MONEY( 0 );
			pUSER->Send_gsv_SET_MONEYONLY ( GSV_SET_MONEY_ONLY );
			return CHEAT_PROCED;
		} else
		if ( !strcmpi(pArg1, "QUEST") && pUSER->GetZONE() ) {
			// CStrVAR *pCStr = this->GetZONE()->GetStrVAR ();
			short nQuestIDX = atoi( pArg2 );
			if ( pUSER->Quest_Delete( nQuestIDX ) ) {
				pStrVAR->Printf ("%d quest delete success", nQuestIDX );
			} else {
				pStrVAR->Printf ("%d quest delete failed", nQuestIDX );
			}

			pUSER->Send_gsv_WHISPER( "SERVER", pStrVAR->Get() );
			return CHEAT_PROCED;
		}
	}
	return CHEAT_INVALID;
}


short classUSER::Cheat_get ( CStrVAR *pStrVAR, char *pArg1, char *pArg2, char *szCode )
{
	if ( !strcmpi(pArg1, "HP") || !strcmpi(pArg1, "TG") ) {
		CObjCHAR *pUSER=NULL;
		if ( pArg2 ) {
			pUSER = g_pUserLIST->Find_CHAR( pArg2 );
		} else {
			if ( !strcmpi(pArg1, "TG") ) {
				pUSER = (CObjCHAR*)this->Get_TARGET(true);
			} else
				pUSER = this;
		}

		if ( pUSER ) {
			if ( this->Get_RIGHT() >= pUSER->Get_RIGHT() && this->GetZONE() ) {
				short nMovSpeed = (short)pUSER->Get_MoveSPEED();
				short nAtkSpeed = pUSER->Get_nAttackSPEED();
				BYTE  btGndATT  = ( pUSER->GetZONE() ) ? pUSER->GetZONE()->IsMovablePOS((int)(pUSER->m_PosCUR.x), (int)(pUSER->m_PosCUR.y)) : 0;

				if ( pUSER->IsUSER() ) {
					pStrVAR->Printf ("HP:%d/%d, MP:%d/%d, LEV:%d, EXP:%d/%d, JOB:%d, SPD(M:%d,A:%d), ANI(M:%1.1f,A:%d), Hit:%d, Crt:%d, AP:%d, DP:%d, AVD:%d, ATTR:%d, bCST:%d, SKL:%d, FLG:%x, Summon:%d/%d, BP:%d, SP:%d, UP:%d, STMNA:%d \\:%I64d, Team:%d, Clan(%d,L:%d,P:%d,S:%d), Recover(%d,%d)",
							pUSER->Get_HP(), pUSER->Get_MaxHP(), 
							pUSER->Get_MP(), pUSER->Get_MaxMP(),
							pUSER->Get_LEVEL(), pUSER->Get_EXP(), ( (classUSER*)pUSER )->Get_NeedEXP( pUSER->Get_LEVEL() ), pUSER->Get_JOB(), 
							nMovSpeed, nAtkSpeed, 
							m_fRunAniSPEED,m_nAtkAniSPEED,
							pUSER->Get_HIT(), pUSER->Get_CRITICAL(), pUSER->Get_ATK(), pUSER->Get_DEF(), pUSER->Get_AVOID(),
							btGndATT, pUSER->m_bCastingSTART, 
							pUSER->Get_ActiveSKILL(), pUSER->m_IngSTATUS.GetFLAGs(), 
							(classUSER*)pUSER->Get_SummonCNT(), (classUSER*)pUSER->Max_SummonCNT(), 
							((classUSER*)pUSER)->GetCur_BonusPOINT(),
							((classUSER*)pUSER)->GetCur_SkillPOINT(), 
							((classUSER*)pUSER)->GetCur_JoHapPOINT( ((classUSER*)pUSER)->GetCur_JOHAP() ),
							((classUSER*)pUSER)->GetCur_STAMINA(),
							((classUSER*)pUSER)->Get_MONEY(), pUSER->Get_TeamNO(), ((classUSER*)pUSER)->GetClanID(), ((classUSER*)pUSER)->GetClanLEVEL(), ((classUSER*)pUSER)->GetClanSCORE(), ((classUSER*)pUSER)->GetClanPOS(),
							((classUSER*)pUSER)->GetAdd_RecoverHP(), ((classUSER*)pUSER)->GetAdd_RecoverMP()
							);
				} else {
					pStrVAR->Printf ("HP:%d/%d, MP:%d/%d, LEV:%d, EXP:%d, JOB:%d, SPD(M:%d,A:%d), Hit:%d, Crt:%d, AP:%d, DP:%d, AVD:%d, ATTR:%d, bCST:%d, SKL:%d, FLG:%x",
							pUSER->Get_HP(), pUSER->Get_MaxHP(), pUSER->Get_MP(), pUSER->Get_MaxMP(),
							pUSER->Get_LEVEL(), pUSER->Get_EXP(), pUSER->Get_JOB(), 
							nMovSpeed, nAtkSpeed, pUSER->Get_HIT(), pUSER->Get_CRITICAL(), pUSER->Get_ATK(), pUSER->Get_DEF(), pUSER->Get_AVOID(),
							btGndATT, pUSER->m_bCastingSTART, pUSER->Get_ActiveSKILL(), pUSER->m_IngSTATUS.GetFLAGs() );
				}
				this->Send_gsv_WHISPER( pUSER->Get_NAME(), pStrVAR->Get() );
			}
		} else {
			// 월드 서버로 전송.
			g_pSockLSV->Send_gsv_CHEAT_REQ( this, this->m_dwWSID, 0, szCode );
		}
		return CHEAT_NOLOG;
	}
		
	if ( !strcmpi(pArg1, "USER") && this->GetZONE() ) {
		pStrVAR->Printf ("Server users:%d, Zone user/char: %d/%d",
			g_pUserLIST->GetUsedSocketCNT(),
			this->GetZONE()->Get_UserCNT(),
			this->GetZONE()->GetGameObjCNT() );
		this->Send_gsv_WHISPER( "<SERVER>::", pStrVAR->Get() );
		return CHEAT_NOLOG;
	} else
	if ( !strcmpi(pArg1, "PARTY" ) ) {
		if ( this->GetPARTY() ) {
			pStrVAR->Printf ("Party users:%d, Lev:%d, Exp:%d/%d, AvgLEV:%d, Option: %x",
				this->GetPARTY()->GetMemberCNT(),
				this->GetPARTY()->GetPartyLEV(),
				this->GetPARTY()->GetPartyEXP(), CCal::Get_PartyLevelUpNeedEXP( this->GetPARTY()->GetPartyLEV() ),
				this->GetPARTY()->GetAverageLEV(),
				this->GetPARTY()->m_btPartyRULE );
			this->Send_gsv_WHISPER( "<SERVER>::", pStrVAR->Get() );
		}
		return CHEAT_NOLOG;
	} else
	if ( B_Cheater() && !strcmpi(pArg1, "EC") ) {
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( !pCPacket )
			return 0;

		pCPacket->m_HEADER.m_wType = GSV_SERVER_DATA;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_SERVER_DATA );
		pCPacket->m_gsv_SERVER_DATA.m_btDataTYPE = SERVER_DATA_ECONOMY;
		pCPacket->AppendData( this->GetZONE()->m_Economy.m_pEconomy, sizeof( tagECONOMY ) );

		this->SendPacket( pCPacket );
		Packet_ReleaseNUnlock( pCPacket );
		
		return CHEAT_NOLOG;
	} else
	if ( B_Cheater() && !strcmpi(pArg1, "WORLD") && this->GetZONE() ) {
		pStrVAR->Printf ("wDrop:%d, wDropM:%d, wEXP:%d, wPROD:%d, wRATE:%d, wREWARD:%d, wSTAMINA:%d, QUEST(Enable:%d, Ref:%d)", 
			Get_WorldDROP(), Get_WorldDROP_M(), Get_WorldEXP(), Get_WorldPROD(), Get_WorldRATE(), Get_WorldREWARD(), Get_WorldSTAMINA(),
			g_QuestList.IsEnable(), g_QuestList.GetRefCNT () );
		this->Send_gsv_WHISPER( "<WORLD>::", pStrVAR->Get() );
		return CHEAT_NOLOG;
	}
#ifdef	__INC_WORLD
	else if( B_Cheater() && !strcmpi(pArg1, "PROPOINT") ) {
		pStrVAR->Printf("PRO1:%f  PRO2:%f  PRO3:%f  PRO4:%f, SUC1:%f, SUC2:%f, SUC3:%f, SUC4:%f", 
			m_fPROPOINT[ 0 ] , m_fPROPOINT[ 1 ] , m_fPROPOINT[ 2 ] , m_fPROPOINT[ 3 ] , 
			m_fSUCPOINT[ 0 ] , m_fSUCPOINT[ 1 ] , m_fSUCPOINT[ 2 ] , m_fSUCPOINT[ 3 ]);
		this->Send_gsv_WHISPER( "<<SERVER>>::", pStrVAR->Get() );
		return CHEAT_NOLOG;
	}
#endif
#ifdef __KCHS_BATTLECART__
	else if( !strcmpi( pArg1, "pathp" ) ) {
		pStrVAR->Printf ("Pat MaxHP : %d, Pat HP : %d, PatCoolTime:%u", this->GetCur_PatMaxHP(), this->GetCur_PatHP() , 
			this->GetCur_PatCoolTIME() );
		this->Send_gsv_WHISPER( "<Server>::", pStrVAR->Get() );
		return CHEAT_NOLOG;
	}
#endif
	
	return CHEAT_INVALID;
}

// A 등급
short classUSER::Cheat_item ( char *pArg1, char *pArg2, char *pArg3, char *pArg4 )
{
	if ( B_Cheater() ) {
		int iItemTYPE = atoi( pArg1 );
		int iItemNO   = atoi( pArg2 );

		if ( iItemTYPE < 1 || iItemTYPE >= ITEM_TYPE_QUEST ) {
			if ( iItemTYPE > ITEM_TYPE_QUEST && iItemTYPE <= ITEM_TYPE_RIDE_PART ) {
				;
			} else
				return CHEAT_INVALID;
		}
		if ( iItemNO >= g_pTblSTBs[ iItemTYPE ]->m_nDataCnt )
			return CHEAT_INVALID;
		int iDupCNT   = atoi( pArg3 );

		tagITEM sITEM;
		sITEM.Clear();

		sITEM.m_cType = iItemTYPE;
		if ( sITEM.m_cType > 0 && sITEM.m_cType < ITEM_TYPE_MONEY ) {
			sITEM.m_nItemNo = iItemNO;

			if ( sITEM.m_cType >= ITEM_TYPE_USE && sITEM.m_cType != ITEM_TYPE_RIDE_PART ) {
				if ( ITEM_ICON_NO( iItemTYPE, iItemNO ) <= 0 )
					return CHEAT_INVALID;

				// 소모 기타...
				if ( iDupCNT > 100 )	iDupCNT = 100;
				else if ( iDupCNT < 1 )	iDupCNT = 1;

				sITEM.m_uiQuantity = iDupCNT;
			} else {
				if ( ITEM_ICON_NO( iItemTYPE, iItemNO ) <= 0 )
					return CHEAT_INVALID;

				// 장비 ...
				sITEM.m_nLife = MAX_ITEM_LIFE;
				sITEM.m_cDurability = ITEM_DURABITY( sITEM.m_cType, sITEM.m_nItemNo );
				sITEM.m_nGEM_OP = iDupCNT % 301;
				sITEM.m_bIsAppraisal = 1;
				if ( 0 == sITEM.m_nGEM_OP ) {
					// 옵션이 없다..
					if( pArg4 && ITEM_RARE_TYPE( sITEM.GetTYPE(), sITEM.GetItemNO() ) ) {
						// 0이 아니면 소켓이 붙을수 있는 아이템이다..
						if ( atoi(pArg4) )
							sITEM.m_bHasSocket = 1;
					}
				}
			}

			short nInvIDX = this->Add_ITEM( sITEM );
			if ( nInvIDX >= 0 ) {
				// nInvIDX == 0 or sITEM.m_cType == ITEM_TYPE_MONEY 이면 돈...
				this->Send_gsv_SET_INV_ONLY( (BYTE)nInvIDX, &sITEM );
				return CHEAT_PROCED;
			}
		}
	}
	return CHEAT_INVALID;
}


// B등급..
short classUSER::Cheat_mon ( char *pArg1, char *pArg2 )
{
	if ( B_Cheater() ) {
		int iMobIDX = atoi( pArg1 );
		int iMobCNT = atoi( pArg2 );
	#ifdef	__INC_WORLD
		if ( iMobCNT > 2000 ) iMobCNT = 2000;
	#else
		if ( iMobCNT > 100 ) iMobCNT = 100;
	#endif
		this->GetZONE()->RegenCharacter( this->m_PosCUR.x, this->m_PosCUR.y, 1500, iMobIDX, iMobCNT, TEAMNO_MOB, true);
	}
	return CHEAT_PROCED;
}

// 
short classUSER::Cheat_mon2 ( char *pArg1, char *pArg2, char *pArg3, char *pArg4 )
{
	if( !pArg1 || !pArg2 || !pArg3 || !pArg4 )
		return CHEAT_INVALID;

	if ( TWGM_Cheater() ) {
		int iMobIDX = atoi( pArg1 );
		int iX = atoi( pArg2 );
		int iY = atoi( pArg3 );
		int iMobCNT = atoi( pArg4 );
	#ifdef	__INC_WORLD
		if ( iMobCNT > 2000 ) iMobCNT = 2000;
	#else
		if ( iMobCNT > 100 ) iMobCNT = 100;
	#endif
		this->GetZONE()->RegenCharacter( iX * 1000, iY * 1000, 1500, iMobIDX, iMobCNT, TEAMNO_MOB, true);
		return CHEAT_PROCED;
	}
	return CHEAT_INVALID;
}

// B등급..
short classUSER::Cheat_damage( char *pArg1, char *pArg2, char *pArg3 )
{
	if ( B_Cheater() ) {
		int iDistance = atoi( pArg1 );
		int iDamage = atoi( pArg2 );

		if ( iDistance < 1000 ) iDistance = 1000;


		uniDAMAGE sDamage;
		if ( iDamage < 0 || iDamage >= MAX_DAMAGE ) 
			sDamage.m_wDamage = MAX_DAMAGE;
		else
			sDamage.m_wDamage = iDamage;

		if ( pArg3 ) {
			int iDamageType = atoi( pArg3 );
			switch( iDamageType ) {
				case 1 : sDamage.m_wDamage |= DMG_BIT_HITTED;	break;	// 맞는 동작을 한다.
				case 2 : sDamage.m_wDamage |= DMG_BIT_CRITICAL;	break;	// 크리티컬 데미지다
			}
		}

		CObjCHAR *pTarget;
		pTarget = (CObjCHAR*)this->AI_FindFirstOBJ( iDistance );
		while( pTarget ) {
			if ( pTarget->IsA( OBJ_MOB ) ) {
				if ( iDamage >= pTarget->Get_HP() )
					pTarget->Set_HP( 1 );
				this->Give_DAMAGE(pTarget, sDamage);
			}

			pTarget = (CObjCHAR*)this->AI_FindNextOBJ ();
		}
	}
	return CHEAT_PROCED;
}


short classUSER::Cheat_quest( CStrVAR *pStrVAR, char *pArg1 )
{
	if ( B_Cheater() ) {
		t_HASHKEY HashQst = StrToHashKey( pArg1 );

		// CStrVAR *pCStr;
		if ( QST_RESULT_SUCCESS == g_QuestList.CheckQUEST( this, HashQst, true ) ) {
			pStrVAR->Printf ("quest check success: %s", pArg1 );
			this->Send_gsv_QUEST_REPLY(RESULT_QUEST_REPLY_TRIGGER_SUCCESS, 0, (int)HashQst );
		} else {
			pStrVAR->Printf ("quest check failed: %s", pArg1 );
			this->Send_gsv_QUEST_REPLY(RESULT_QUEST_REPLY_TRIGGER_FAILED, 0, (int)HashQst );
		}
		this->Send_gsv_WHISPER( "SERVER", pStrVAR->Get() );
	}
	return CHEAT_PROCED;
}


short classUSER::Cheat_set ( classUSER *pUSER, char *pArg1, char *pArg2, char *pArg3 )
{
	if ( NULL == pUSER )
		pUSER = this;

	if ( B_Cheater() ) {
		for (short nI=0; l_szAbility[ nI ]; nI++) {
			if ( !strcmpi(pArg1, l_szAbility[ nI ]) ) {
				int iValue = atoi( pArg2 );

				if ( iValue < 10 )
					iValue = 10;
				if ( iValue > MAX_BASIC_ABLITY )
					iValue = MAX_BASIC_ABLITY;

				pUSER->m_BasicAbility.m_nBasicA[ nI ] = iValue;
				pUSER->Update_SPEED();
				return CHEAT_SEND;
			}
		}

		if ( !strcmpi(pArg1, "JOB") ) {
			short nWantJob = atoi(pArg2);
			short nJob, nJob10, nJob100;

			nJob = ( nWantJob % 10 );		// 1단위 0~
			if ( nJob && nJob < 3 ) {
				nJob10  = ( ( nWantJob % 100 ) / 10 ) % 4;	// 10 단위
				nJob100 = ( nWantJob / 100 ) % 5;
				if ( nJob10 && nJob100 ) {
					nJob += ( nJob10 * 10 );
					nJob += ( nJob100 * 100 );
				}
			} else nJob = 0;
			pUSER->SetCur_JOB( nJob );
			return CHEAT_SEND;
		} else
		if ( !strcmpi(pArg1, "LEV") ) {
			pUSER->Set_LEVEL( atoi(pArg2) );
			return CHEAT_SEND;
		/*	존 분할로 운영시 월드 서버에서 파티 운영될수 있도록 전송하던...
		#ifndef	__INC_WORLD
			g_pSockLSV->Send_gsv_LEVEL_UP( LEVELUP_OP_USER, pUSER->m_dwWSID, pUSER->Get_LEVEL(), pUSER->m_GrowAbility.m_lEXP );
		#endif
		*/
		} else 
		if ( !strcmpi(pArg1, "FAME" ) ) {
			int iValue = atoi(pArg2);
			if ( iValue < 0 )
				iValue = 0;
			if ( iValue > 99 )
				iValue = 99;

			pUSER->SetCur_FAME( iValue );
			return CHEAT_SEND;
		} else 
		if ( !strcmpi( pArg1, "MONEY" ) ) {
			__int64 i64Money;
			i64Money = _atoi64( pArg2 );
			pUSER->SetCur_MONEY( i64Money );
			return CHEAT_SEND;
		} else
		if ( !strcmpi( pArg1, "grade" ) && pArg3 ) {
			int iPos = atoi(pArg2);
			if ( iPos > 0 && iPos < MAX_EQUIP_IDX ) {
				if ( this->m_Inventory.m_ItemEQUIP[ iPos ].GetHEADER() ) {
					int iGrade = atoi(pArg3);
					if ( iGrade < 0 ) iGrade = 0;
					if ( iGrade > 9 ) iGrade = 9;

					this->m_Inventory.m_ItemEQUIP[ iPos ].m_cGrade = iGrade;
				}
			}
			return CHEAT_PROCED;
		}
	}

	if ( B_Cheater() ) {
		if ( !strcmpi( pArg1, "worldDROP_M" ) ) {
			int iValue = atoi(pArg2);
			if ( RIGHT_MG == this->m_dwRIGHT ) {
				if ( iValue < 50  ) iValue = 50;
				if ( iValue > 300 ) iValue = 300;
			}
			::Set_WorldDROP_M( iValue );
			return CHEAT_NOLOG;
		} else
		if ( !strcmpi( pArg1, "worldDROP" ) ) {
			int iValue = atoi(pArg2);
			if ( RIGHT_MG == this->m_dwRIGHT ) {
				if ( iValue < 50  ) iValue = 50;
				if ( iValue > 200 ) iValue = 200;
			}
			::Set_WorldDROP( iValue );
			return CHEAT_NOLOG;
		} else
		if ( !strcmpi( pArg1, "worldEXP" ) ) {
			int iValue = atoi(pArg2);
			if ( RIGHT_MG == this->m_dwRIGHT ) {
				if ( iValue < 50  ) iValue = 50;
				if ( iValue > 300 ) iValue = 300;
			}
			::Set_WorldEXP( iValue );
			return CHEAT_NOLOG;
		} else
		if ( !strcmpi( pArg1, "worldPROD" ) ) {
			int iValue = atoi(pArg2);
			if ( RIGHT_MG == this->m_dwRIGHT ) {
				if ( iValue < 70  ) iValue = 70;
				if ( iValue > 130 ) iValue = 130;
			}
			::Set_WorldPROD( iValue );
			return CHEAT_NOLOG;
		} else
		if ( !strcmpi( pArg1, "worldRATE" ) ) {
			int iValue = atoi(pArg2);
			if ( RIGHT_MG == this->m_dwRIGHT ) {
				if ( iValue < 70  ) iValue = 70;
				if ( iValue > 120 ) iValue = 120;
			}
			::Set_WorldRATE( iValue );
			return CHEAT_NOLOG;
		} else
		if ( !strcmpi( pArg1, "worldREWARD" ) ) {
			int iValue = atoi(pArg2);
			if ( RIGHT_MG == this->m_dwRIGHT ) {
				if ( iValue < 50  ) iValue = 50;
				if ( iValue > 300 ) iValue = 300;
			}
			::Set_WorldREWARD( iValue );
			return CHEAT_NOLOG;
		} else 
		if ( !strcmpi( pArg1, "worldSTAMINA" ) ) {
			int iValue = atoi(pArg2);
			if ( RIGHT_MG == this->m_dwRIGHT ) {
				if ( iValue < 0   ) iValue = 0;
				if ( iValue > 200 ) iValue = 200;
			}
			::Set_WorldSTAMINA( iValue );
			return CHEAT_NOLOG;
		} else
		if ( !strcmpi( pArg1, "union" ) ) {
			int iValue = atoi(pArg2);
			if ( iValue >= 0 && iValue < MAX_UNION_COUNT ) {
				this->SetCur_UNION( iValue );
				return CHEAT_SEND;
			}
		}
#ifdef __KCHS_BATTLECART__
		else if ( !strcmpi( pArg1, "maxpathp" ) ) {
			int iValue = atoi(pArg2);
			this->Set_MaxPatHP( (short)iValue );
			return CHEAT_SEND;
		}
		else if ( !strcmpi( pArg1, "pathp" ) ) {
			int iValue = atoi(pArg2);
			this->SetCur_PatHP( (short)iValue );
			return CHEAT_SEND;
		}
		else if ( !strcmpi( pArg1, "pattime" ) ) {
			int iValue = atoi(pArg2);
			this->SetCur_PatCoolTIME( iValue );
			Send_gsv_PATSTATE_CHAGE( 0, this->GetCur_PatCoolTIME() ); 
			return CHEAT_SEND;
		}
#endif
	}
	return CHEAT_INVALID;
}


//-------------------------------------------------------------------------------------------------
short classUSER::Cheat_toggle ( CStrVAR *pStrVAR, char *pArg1 )
{
	if ( B_Cheater() ) {
		if ( !strcmpi( pArg1, "PK" ) && this->GetZONE() ) {
			DWORD dwFLAG = this->GetZONE()->Toggle_PK_FLAG ();

			// CStrVAR *pCStr = this->GetZONE()->GetStrVAR ();
			pStrVAR->Printf ("PK Mode: %s ", (dwFLAG & ZONE_FLAG_PK_ALLOWED) ? "ON" : "OFF" );
			this->Send_gsv_WHISPER( "SERVER", pStrVAR->Get() );
			return CHEAT_NOLOG;
		} else
		if ( !strcmpi( pArg1, "QUEST" ) ) {
			g_QuestList.ToggleEnable ();
		} else
		if ( !strcmpi( pArg1, "REGEN" ) ) {
			pStrVAR->Printf ("Zone %d Regen mode: %s ", this->GetZONE()->Get_ZoneNO(), this->GetZONE()->Toggle_REGEN () ? "ON" : "OFF" );
			this->Send_gsv_WHISPER( "SERVER", pStrVAR->Get() );
		}
	}

	return CHEAT_INVALID;
}


short classUSER::Cheat_regen ( CStrVAR *pStrVAR, char *pArg1, char *pArg2 )
{
	if ( A_Cheater() ) {
		if ( !strcmpi( pArg1, "COUNT" ) ) {
			pStrVAR->Printf ("RegenPointCNT: %d", this->GetZONE()->GetRegenPointCNT() );
			this->Send_gsv_WHISPER( "<SERVER>::", pStrVAR->Get() );
		} else
		if ( !strcmpi( pArg1, "INFO" ) ) {
			if ( pArg2 ) {
				int iPointIDX = atoi(pArg2);
				CRegenPOINT *pRegen = this->GetZONE()->GetRegenPOINT( iPointIDX );
				if ( pRegen ) {
					pStrVAR->Printf ("%dth RegenINFO(%.0f,%.0f):: Live:%d/%d, Tactics:%d/%d", 
						iPointIDX,
						pRegen->m_fXPos, pRegen->m_fYPos,
						pRegen->GetLiveCNT (), pRegen->m_iLimitCNT,
						pRegen->m_iCurTactics, pRegen->m_iTacticsPOINT );
					this->Send_gsv_WHISPER( "<SERVER>::", pStrVAR->Get() );
				}
			}
		} else
		if ( !strcmpi( pArg1, "MOB" ) && pArg2 ) {
			int iPointIDX = atoi(pArg2);
			this->GetZONE()->GetRegenPointMOB( iPointIDX, this );
		} else 
		if ( !strcmpi( pArg1, "CALL" ) && pArg2 ) {
			int iPointIDX = atoi(pArg2);
			int iCallCNT = this->GetZONE()->CallRegenPointMOB( iPointIDX );
			pStrVAR->Printf ("%dth RegenCALL: %d mobs", iPointIDX, iCallCNT );
			this->Send_gsv_WHISPER( "<SERVER>::", pStrVAR->Get() );
		} else
		if ( !strcmpi( pArg1, "NEAR" ) ) {
			pStrVAR->Printf ("Nearst RegenPNT:: %dth", this->GetZONE()->GetNearstRegenPOINT( this->m_PosCUR ) );
			this->Send_gsv_WHISPER( "<SERVER>::", pStrVAR->Get() );
		}
	}

	return CHEAT_NOLOG;
}

//-------------------------------------------------------------------------------------------------
// 2005-08-17 : kchs , 현재속도를 fRate(비율)만큼 증가시킴
short classUSER::Cheat_speed ( char * pArg1 )
{
	float fRate = (float)atof( pArg1 );
	//m_nRunSPEED *= fRate;
	float calcspeed = (float)m_nRunSPEED;
	calcspeed *= fRate;
	if(calcspeed > MAX_SHORT)
	{
		m_nRunSPEED = (MAX_SHORT);
	}
	else if(calcspeed < 100)
	{
		m_nRunSPEED = 100;
	}
	else
	{
		m_nRunSPEED = (short)calcspeed;
	}
	Send_gsv_SPEED_CHANGED( false );

	return CHEAT_PROCED;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

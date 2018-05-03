
#include "StdAfx.h"
#include "QueryDLG.h"
#include "../CSplitHangul.h"
#include "../../Game.h"
//#include "..\\LookUpImageIndex.h"
#include "../../../TGameCtrl/ResourceMgr.h"
#include "../it_Mgr.h"
#include "../IO_ImageRes.h"
#include "../CTDrawImpl.h"
#include "../../System/CGame.h"
#include "../../Object.h"

#define QUERY_START_X					16
#define QUERY_START_Y					24
#define	QUERY_HEIGHT					18


CQueryDLG::CQueryDLG()
{
	m_nTargetClientIdx  = 0;
	m_nMaxLN			= 0;
	m_nExamCNT			= 0;
	m_nLastExamID		= 0;
	m_nMaxExamCNT		= NULL;
	m_pQuery			= NULL;

	Free_EXAM ();
	fpClose = NULL;

	m_sQuestion.m_pQuestionPage = NULL;
	m_sQuestion.m_nCurPageNo    = 0;
	m_sQuestion.m_nPageCNT      = 0;

	m_nMaxLength = 50; //한라인에 최대 50자까지 ... 
	
	m_iFontNo_q  = 2;
	m_dCOL_q	 = D3DCOLOR_RGBA(0,0,0,255);

	m_iFontNo_e	 = 3;
	m_dCOL_e	 = D3DCOLOR_RGBA(0,0,255,255);
	
	fpClose		 = NULL;

}

CQueryDLG::~CQueryDLG()
{
	if ( m_pQuery ) {
		delete[] m_pQuery;
		m_pQuery = NULL;
	}

	// *-----------------------------------------------------* //
	ReleaseQuestion();
	ReleaseExam();
//	SetEnable( false );
	fpClose = NULL;

}


void CQueryDLG::Draw()
{
	if(!IsVision()) return ;
	CTDialog::Draw();
	DrawQuery();
}

unsigned int CQueryDLG::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if(!IsVision()) return 0;

	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_KEYDOWN:
			if(On_KeyDown(wParam))
				return uiMsg;
			break;
		case WM_LBUTTONDOWN:
			if(On_LButtonDN(iProcID))
				return uiMsg;
			break;
		default:
			break;
		}
		return uiMsg;
	}
	/*
	else {
		nProcID = GetLastProcessChildID();
		switch(uiMsg)
		{
		case WM_KEYDOWN:
			if(On_KeyDown(wParam))
				return uiMsg;
			break;
		case WM_LBUTTONDOWN:
			if(On_LButtonDN(nProcID))
				return uiMsg;
			break;
		case WM_MOUSEMOVE:
			
		 	break;
		}
	}
	*/
	return 0;
}

bool CQueryDLG::OpenDLG(short nSX,short nSY,char* szQuery,short nMaxCharLength,int iEventID,void (*fpCloseProc)(int iEventID))
{
	//기존의 데이타 클리어
	ReleaseQuestion();
	ReleaseExam();
	//질문
	AppendQuestion(szQuery,iEventID);
	//예시
	Free_EXAM ();
	m_nExamCNT    = 0;
	m_nMaxExamCNT = 0;

	m_pQuery = new char[ strlen(szQuery)+1 ];
	strcpy(m_pQuery, szQuery);

	fpClose = fpCloseProc;
	Show();
	POINT pt = { nSX,nSY };
	MoveWindow( pt );

	CObjCHAR *pTarget = (CObjCHAR*)g_pAVATAR->Get_TARGET();
	if( pTarget )
	{
		m_nTargetClientIdx = pTarget->Get_INDEX();
	}
	return true;
}

void CQueryDLG::Close()
{
	if ( m_pQuery ) {
		delete[] m_pQuery;
		m_pQuery = NULL;
	}

	// *-----------------------------------------------------* //
	ReleaseQuestion();
	ReleaseExam();
//	SetEnable( false );
	fpClose = NULL;
	if( IsVision() )
		Hide();
	m_nTargetClientIdx = 0;
	// *-----------------------------------------------------* //
}


void CQueryDLG::DrawQuery()
{
	if(m_sQuestion.m_pQuestionPage==NULL)   return;
	
	short nI;
	// *--------------------------------------------------------------------* //
	//기본창을 그린다(창은 3단계로 구분되어 있다)
    short nYOffset = 0;

	CResourceMgr*   pResourceMgr = CResourceMgr::GetInstance();
	CTDrawImpl*		pDrawMgr     = g_itMGR.GetDrawImplPtr();
	if( pResourceMgr == NULL || pDrawMgr == NULL )
		return;

	POINT ptDlg = GetPosition();
	//맨위
	nYOffset +=31;

	//중간
	for(nI=0;nI< m_nMaxLN + m_nExamCNT + 1;nI++) 
	{
		pDrawMgr->Draw( ptDlg.x, ptDlg.y + nYOffset+(nI*19), IMAGE_RES_UI, pResourceMgr->GetImageNID(IMAGE_RES_UI,"ID_QUERY_MIDDLE"));
	}

	//아래
	pDrawMgr->Draw( ptDlg.x, ptDlg.y + nYOffset+(nI*19), IMAGE_RES_UI, pResourceMgr->GetImageNID(IMAGE_RES_UI,"ID_QUERY_BOTTOM"));	
	pDrawMgr->Draw( ptDlg.x, ptDlg.y , IMAGE_RES_UI, pResourceMgr->GetImageNID(IMAGE_RES_UI,"ID_QUERY_TOP"));

	//전체크기를 등록해준다 
	SetHeight( nYOffset+(nI*19)+11 );

	// *--------------------------------------------------------------------* //
	//질문들은 출력한다 
	for(nI=0;nI<m_nMaxLN;nI++) 
	{
		//텍스트 출력 
		//if(nI<m_nMaxLN) 
		{
			char *pTXT = m_sQuestion.m_pQuestionPage[m_sQuestion.m_nCurPageNo].m_szQuestion[nI];
			if(strlen(pTXT))
				::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true,QUERY_START_X, ( nI * QUERY_HEIGHT ) + QUERY_START_Y, m_dCOL_q, "%s", pTXT );
		}
	}

	// *--------------------------------------------------------------------* //
	//마지막으로 찍한 위치 
	m_nLastPosY  = nI*20;
	short nSelLN = getMouseSelectLN();
	//예시들을 출력한다 
	for(nI=0;nI<m_nExamCNT;nI++) 
	{
		char* pTXT = m_vecEXAM[nI].m_pExam;
		if(strlen(pTXT)) 
		{
			if( m_vecEXAM[nI].m_nLnCNT==nSelLN)
				::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
									QUERY_START_X,m_nLastPosY + (nI*QUERY_HEIGHT) + QUERY_START_Y, 
									g_dwRED, "%s", pTXT );	

			else
				::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ],true,
									QUERY_START_X,m_nLastPosY + (nI*QUERY_HEIGHT) + QUERY_START_Y, 
									m_dCOL_e, "%s", pTXT );					
		}	
	}
}

bool CQueryDLG::AppendQuestion(char* szQuestion,int iEventID)
{
	//기존 질문이 있으면 삭제
	ReleaseQuestion();
	
	//들어온 질문을 분할해서 등록 
	CSplitHangul	splitHAN(szQuestion,m_nMaxLength);
	short	nLineCNT   = splitHAN.GetLineCount();
	short   nAllocSize = (nLineCNT/MAX_QUESTION_LN)+1;	//페이지 개수 
	m_nMaxLN = nLineCNT;
	m_sQuestion.m_pQuestionPage = new t_question_page[nAllocSize];

	if(m_sQuestion.m_pQuestionPage) 
	{
		ZeroMemory(m_sQuestion.m_pQuestionPage,sizeof(t_question_page)*nAllocSize);
		m_sQuestion.m_iEventID = iEventID;
		short nPageNo;
		m_sQuestion.m_nPageCNT = nAllocSize;
		for(short nI=0;nI<nLineCNT;nI++) 
		{
			//분리된 질문들을 페이지에 등록 시킨다 
			nPageNo = nI/MAX_QUESTION_LN ;
			strcpy(m_sQuestion.m_pQuestionPage[nPageNo].m_szQuestion[nI%MAX_QUESTION_LN],(char*)splitHAN.GetString(nI));

			m_nExamCNT++;
		}
	}
	else 
		return false;

	return true;
}

void CQueryDLG::ReleaseQuestion()
{
	if(m_sQuestion.m_pQuestionPage) {
		delete [] m_sQuestion.m_pQuestionPage;
		m_sQuestion.m_nCurPageNo = 0;
		m_sQuestion.m_nPageCNT   = 0;
		m_sQuestion.m_pQuestionPage = NULL;
	}
	m_nMaxLN  = 0;
}

bool CQueryDLG::AppendExam(char* szExam,int iEventID,void (*fpExamEvent)(int iEventID))
{
	CSplitHangul	splitHAN(szExam,m_nMaxLength-4);
	short nLineCNT = splitHAN.GetLineCount();
	
	short nLnID = m_nLastExamID; //아이디를 부여해준다.....
	m_nLastExamID++;
	for(short nI=0;nI<nLineCNT;nI++) {
		t_query_exam		sQuery;
		sQuery.fpEventHandle = fpExamEvent;
		sQuery.m_iEventID    = iEventID;
		sQuery.m_nLnCNT      = nLnID;
		sQuery.m_pExam = new char[ strlen((char*)splitHAN.GetString(nI))+5 ];
		
		if(nI==0)
			sprintf(sQuery.m_pExam,"%d.%s",sQuery.m_nLnCNT+1, splitHAN.GetString(nI));
		else
			sprintf(sQuery.m_pExam,"  %s",splitHAN.GetString(nI));
		m_vecEXAM.push_back(sQuery);
		int iTemp = m_vecEXAM.size();
		m_nExamCNT++;

	}

	return true;
}

void CQueryDLG::Free_EXAM()
{
	for(unsigned short nI=0;nI<m_vecEXAM.size();nI++) {
		if ( m_vecEXAM[nI].m_pExam ) {
			delete[] m_vecEXAM[nI].m_pExam;
			m_vecEXAM[nI].m_pExam = NULL;
		}
	}

	m_vecEXAM.clear();
	m_nExamCNT = 0;
	m_nLastExamID = 0;
}

void CQueryDLG::ReleaseExam()
{
	Free_EXAM ();
	m_nExamCNT    = 0;
	m_nMaxExamCNT = 0;
}

short CQueryDLG::getMouseSelectLN()
{
	short nTextWidth;
	short nWidth;
	POINT sPT;
	RECT  sRGN;
	CGame::GetInstance().Get_MousePos(sPT );

	POINT ptDlg = GetPosition();
	for(short nI=0;nI<m_nExamCNT;nI++) 
	{
		nWidth		= strlen(m_vecEXAM[nI].m_pExam);
		nTextWidth  = nWidth*8;
		if(nTextWidth>384) 
			nTextWidth = 384;

		SetRect(&sRGN,ptDlg.x,
					  ptDlg.y+m_nLastPosY + ((nI) * QUERY_HEIGHT) + QUERY_START_Y,
					  ptDlg.x+nTextWidth,
					  ptDlg.y+m_nLastPosY + ((nI) * QUERY_HEIGHT) + QUERY_START_Y + QUERY_HEIGHT );
		
		if(PtInRect(&sRGN,sPT)) 
			return m_vecEXAM[nI].m_nLnCNT;
	}
  	return -1;
}



void CQueryDLG::ProcessEvent(short nNum)
{
		
	if(!m_nExamCNT)		return;
	if(nNum>=m_nExamCNT) return ;

	if(m_vecEXAM[nNum].fpEventHandle) {
		m_vecEXAM[nNum].fpEventHandle(m_vecEXAM[nNum].m_iEventID);
	}
}
///*-------------------------------------------------------------------------
/// 2004/3/30:수정 nAvy 
/// 1.nSelNo을 직접 m_vecEXAM의 첨자로 사용함 ( Exam이 2줄이상일 경우 문제 발생 )
/// => Loop를 돌아서 m_nLnCNT와 같은 것을 처리하도록 한다.
bool CQueryDLG::MouseClickEventProc()
{
	if(!m_nExamCNT)		return false;
	short nSelNo = getMouseSelectLN();
	if(nSelNo<0)		return false;
	
	vec_query_exam_itr iter;
	for( iter = m_vecEXAM.begin(); iter != m_vecEXAM.end(); ++iter )
	{
		if( iter->m_nLnCNT == nSelNo )
		{
			iter->fpEventHandle( iter->m_iEventID );
			break;
		}
	}
	return true;	
}

bool CQueryDLG::KeyDownEventProc(WPARAM wParam)
{
	//예시선택 처리
	switch(wParam)
	{
	case '1':
		ProcessEvent(0);
		break;
	case '2':
		ProcessEvent(1);
		break;
	case '3':
		ProcessEvent(2);
		break;
	case '4':
		ProcessEvent(3);
		break;
	case '5':
		ProcessEvent(4);
		break;
	case '6':
		ProcessEvent(5);
		break;
	case '7':
		ProcessEvent(6);
		break;
	case '8':
		ProcessEvent(7);
		break;
	case '9':
		ProcessEvent(8);
		break;
	default:
		return false;
	}
	return true;
}
// *---------------------------------------------------------------------------* //



bool CQueryDLG::On_LButtonDN(unsigned iProcID)
{
	MouseClickEventProc();
	return true;
}
bool CQueryDLG::On_KeyDown(WPARAM wParam)
{
	return KeyDownEventProc(wParam);
}

void CQueryDLG::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	CTDialog::Update( ptMouse );
	CObjCHAR* pObjChar = g_pObjMGR->Get_CharOBJ( m_nTargetClientIdx, false );

	///NPC Object가 없거나 있는데 거리가 일정 거리 이상이라면 
	if( pObjChar == NULL )
		Close();
	else if( g_pAVATAR->Get_DISTANCE( pObjChar ) >= 1000 )
		Close();

}

int	 CQueryDLG::IsInValidShow()
{
	if( g_pAVATAR && g_pAVATAR->Get_HP() <= 0 )
		return 99;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_DEAL ) )
		return DLG_TYPE_DEAL;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_EXCHANGE ) )
		return DLG_TYPE_EXCHANGE;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_QUERY ))
		return DLG_TYPE_QUERY;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_MAKE ))
		return DLG_TYPE_MAKE;

	return 0;
}
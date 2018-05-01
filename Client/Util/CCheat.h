
#ifndef	__CHEAT_H
#define	__CHEAT_H
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

bool DoCheatFunc(char* pStr);


enum enumCHEATRESULT
{
	CHEAR_RESULT_FAIL = 0,
	CHEAT_RESULT_SERVER = 1,
	CHEAT_RESULT_CLIENT = 2,
};


class classPARSESTR {
private :
	char		m_szBUF[MAX_PATH];	

public  :
	classPARSESTR ();
	~classPARSESTR ();

	//초기화 
	void  CheatInit();
	//문자열을 얻어준다 (nStrNo==어디부터 얻을것인가)
	char* ParseString(char* pStr,short nStrNo);
	//치트번호를 얻어줌 
	short ParseCheatCodeIdx(char* pStr);
	
	//치트실행
	int  DoCheat(char* pCheatCode, bool bServerCheate = true );
	//special 치트실행
	int  DoSpecialCheat(char* pCheatCode, bool bServerCheate = true );


	//set
	bool DoSET(char* pStr);
	short GetSET_CodeIdx(char* pStr);
	//add
	bool DoADD(char* pStr);
	short GetADD_CodeIdx(char* pStr);
	//full
	bool DoFULL(char* pStr);
	short GetFULL_CodeIdx(char* pStr);
	//skill
	bool DoSKILL(char* pStr);
	//item
	bool DoITEM(char* pStr);
	//money
	bool DoMONEY(char* pStr);
	//ser
	bool DoSER(char* pStr);
	//fame
	bool DoFAME(char* pStr);
	//meet
	bool DoMEET(char* pStr);
	//fly_mode
	bool DoFLY_MODE(char* pStr);
	//warp
	bool DoWARP(char* pStr);
	//warp_m
	bool DoWARP_M(char* pStr);
	//effect
	bool DoEFFECT(char* pStr);
	//sound
	bool DoSOUND(char* pStr);

	//Dagnarus
	bool DoSpeed(char* pStr);

	//진행 수치
	bool DoProcVal( char* pStr );
	//소속 명성
	bool DoUnionFame( char* pStr );
	//퀘스트 변수
	bool DoQuestVal( char* pStr );
	// 직업 전환
	bool DoJob( char* pStr );

	// 스페셜 치트
	bool DoSpecial( char* pStr );

	/// Client자체적으로 퀘스트 컨디션 체크
	bool DoCheckQuestCondition( char* pszStr );

	bool DoFree( char* pStr );

	bool DoClan( char* pStr );

	void DoFee();
} ;

extern classPARSESTR g_Cheat;
//-------------------------------------------------------------------------------------------------
#endif

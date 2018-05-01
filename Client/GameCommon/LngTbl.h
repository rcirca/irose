#ifndef __LNGTBL_H__

#include <stdio.h>
#include "StringManager.h"
#include "Util\\VFSManager.h"

// AI 스트링 테이블 파일
#define LANGSTB_AI_NAME			"3Ddata\\AI\\ulngtb_ai.ltb"

// Event (대화툴에서 만든 데이터) 스트링 테이블 파일
#define LANGSTB_CON_NAME		"3Ddata\\Event\\ulngtb_con.ltb"

// Quest 스트링 테이블 파일
#define LANGSTB_QST_NAME		"3Ddata\\QuestData\\ulngtb_qst.ltb"


enum LTB_TYPE 
{
	LTB_AI		= 0 , 
	LTB_QUEST	= 1 , 
	LTB_EVENT	= 2
};


struct tagStrTblDATA {
	long	m_lFilePOS;
	short	m_nStrLEN;
	WCHAR   *m_pStr;

	void Set(const WCHAR *szStr, short nLen);
	void Clear (void);
	tagStrTblDATA ();
	~tagStrTblDATA ();
} ;

// 파일을 저장할때는 아래 Pre Define을 제거해야 한다.
#define __USEBLOCK__

class AStringTable
{
private:
	int				m_iRowCNT;
	int				m_iColCNT;
	tagStrTblDATA **m_ppFilePOS;
	// FILE		   *m_FP;
	
	CFileSystem*	m_pFS;

	WCHAR *			m_pTmpStr;
	short			m_nTmpStrLen;
	WCHAR *			m_pData;

	//
	// MBCS String 용 버퍼와 버퍼 길이
	//
	char *			m_szMbcBuffer;
	short			m_nMbcsTmpStrLen;

public :
	AStringTable ();
	~AStringTable ();

	/*******************************************************************
	 * 사용된 메모리 리소스를 해제한다
	 */
	void Clear ();

	/*******************************************************************
	 * 파일을 오픈하고, 파일앞쪽에 있는 할당테이블을 읽는다
	 * 파일은 Close 하지 않는다
	 */
	bool Open (const char *szFileName);

	/*******************************************************************
	 * 파일이 오픈되었으면 Close(fclose) 한다
	 */
	void  Close ();

	/*******************************************************************
	 * 주어진 위치에 있는 스트링의 길이를 조사한다. 길이가 0이면 비어 있음
	 * wColIDX : Column Index
	 * wRowIDX : Row Index
	 * Return : 길이가 0이면 비어 있음. 0보다 작으면 Column과 Row가 범위
	 * 를 벗어 났음
	 */
	short GetLength(WORD wColIDX, WORD wRowIDX);

	/*******************************************************************
	 * 주어진 위치에 있는 스트링을 파일로부터 읽어온다
	 * wColIDX : Column Index
	 * wRowIDX : Row Index
	 * Return : 범위를 벗어난 경우 NULL 리턴, 문자열의 길이 0인 경우 
	 * "\0"을 리턴
	 */
	char *ReadString(WORD wColIDX, WORD wRowIDX);

	/*******************************************************************
	 * 파일에서 스트링을 읽어서 메모리에 올려 놓는다.
	 * 파일은 Close 된 다음 리턴된다. Load ()로 파일을 오픈한 경우, 
	 * GetString () 을 사용해서 스트링을 참조 할 것.
	 *
	 * Return : 실패하면 false
	 */
	bool Load (const char *szFileName);

	/*******************************************************************
	 * 메모리상의 스트링 테이블의 줄수를 조사한다
	 */
	int GetRowCNT (void);

	/*******************************************************************
	 * 컬럼수를 조사한다
	 */
	int GetClmnCNT (void);

	/*******************************************************************
	 * 메모리상의 스트링테이블에서 문자열을 조사한다.
	 * wColIDX : Column Index
	 * wRowIDX : Row Index
	 */
	WCHAR * GetWString (WORD wColIDX, WORD wRowIDX);
	char * GetMbcsString (WORD wColIDX, WORD wRowIDX);

	/*******************************************************************
	 * 메모리상의 스트링테이블에서 문자열을 지운다.
	 * wColIDX : Column Index
	 * wRowIDX : Row Index
	 */
	bool ClearString (WORD wColIDX, WORD wRowIDX);

	/*******************************************************************
	 * 메모리상의 스트링테이블에서 행을 지운다.
	 * wRowIDX : Row Index
	 */
	bool ClearRow (WORD wRowIDX);

	/*******************************************************************
	 * 메모리상의 스트링테이블에서 빈행을 찾는다
	 * iClmnIDX : 검사할 컬럼
	 * Return : Row Index , 찾지 못하면 -1를 리턴한다
	 */
	int FindEmptyClmn (int iClmnIDX);

	/*******************************************************************
	 * 메모리상의 스트링테이블에서 빈열을 찾는다
	 * Return : Row Index , 찾지 못하면 -1를 리턴한다
	 */
	int FindEmptyRow (void);

} ;


class CLtbManager
{
private:
	LANGUAGE_TYPE	m_LngType;
	AStringTable * 	m_pAI;
	AStringTable * 	m_pQuest;
	AStringTable * 	m_pEvent;

public:
	CLtbManager ();
	~CLtbManager ();
	
	bool Load ( int iLngType );
	///const char * GetAiString (int StrID);
	//const char * GetQuestString (int StrID);
	const char * GetEventString (int StrID);
};

extern CLtbManager g_LngTBL;

#endif

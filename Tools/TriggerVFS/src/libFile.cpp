#include "stdafx.h"
#include <stdio.h>
#include <io.h>
#include <windows.h>
#include <sys/stat.h>
#include <Shlwapi.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "libFile.h"


/*********************************************************************************************
 * 
 */
long __fseek (FILE *fp, int lOff, int iOrg)
{
	fseek (fp, lOff, iOrg);
	return ftell (fp);
}


/*********************************************************************************************
 * 현재 디렉토리를 알아낸다
 */
std::string __GetCurDir (void)
{
	char szDir [255];
	std::string sRet;

	ZeroMemory (szDir, 255);
	GetCurrentDirectory (255, szDir);

	sRet = szDir;

	return sRet;
}


/*********************************************************************************************
 * 파일의 크기를 알아낸다
 */
long __GetFileSize (const char *FileName)
{
	struct _stat buffer;

	if(_stat (FileName, &buffer) == 0)
	{ return (long)buffer.st_size; }

	return 0;
}


/*********************************************************************************************
 * 파일 블록 이동 시키기
 * @param lStartOff		올길 부분의 시작 Offset
 * @param stLength		블록의 길이
 * @param lTargetOff	옮길 Offset
 * @param sAllocSize	옮기는 단위 . 메모리 할당량이 됨
 * @param bFlush		true 이면 옮기고 나서 fflush를 하고 false이면 안 한다.
 * @param fp			파일 핸들
 */
bool MoveFileBlock (long lStartOff, size_t stLength, long lTargetOff, int sAllocSize, FILE * fp, bool bFlush)
{
	BYTE *btBuff	= NULL;		/// 버퍼
	int iMove		= 0;		/// 옮겨지는 길이
	size_t stCount	= stLength; /// while 루프를 위한 Count
	int stRead		= 0;		/// 현재 읽을 바이트수
	int	stReaded	= 0;		/// 읽혀진 바이트수
	long lOldSize	= 0;		/// 원래 파일의 크기

	if(stLength < (unsigned)sAllocSize) { sAllocSize = (int)stLength; }
	/// 현재 미결된 쓰기작업을 모두 Device로 밀어넣는다
	fflush (fp);
	/// 앞으로
	if(lTargetOff - lStartOff < 0)
	{
		iMove = lStartOff - lTargetOff;
		if((btBuff = new BYTE[ sAllocSize ]))
		{
			fseek (fp, lTargetOff, SEEK_SET);
			while (stCount > 0)
			{
				/// 읽을 양을 결정
				stRead = stCount > (unsigned)sAllocSize ? sAllocSize : (unsigned)stCount;
				/// 읽을 위치로 이동
				fseek (fp, iMove, SEEK_CUR);
				/// 읽는다
				stReaded = (int)fread (btBuff, sizeof (BYTE), stRead, fp);
				/// 읽을 양에서 읽은 만큼 뺀다
				stCount -= stReaded;
				/// 읽은 만큼 앞으로 가면 원래 위치 그리고 imove 만큼 앞으로
				fseek (fp, (long)(-iMove - stReaded), SEEK_CUR);
				fwrite (btBuff, sizeof (BYTE), stReaded, fp);
			}
			if(bFlush) { fflush (fp); }
			lOldSize = __fseek (fp, 0, SEEK_END);
			/// 파일크기를 줄이는 것은 이 함수를 호출한 쪽에서 해야 한다. 
			/// ::__ftruncate (lOldSize - iMove, fp);
			/// 메모리 해제
			delete [] btBuff;
			return true;
		}
	}
	/// 뒤로 밀때
	else if (lTargetOff - lStartOff > 0)
	{
		if((btBuff = new BYTE[ sAllocSize ]))
		{
			// lOldSize = ::__fseek (fp, 0, SEEK_END);
			/// 뒷쪽 데이터의 크기
			// size_t szLengthOfletter = lOldSize - lStartOff;
			/// 가장 뒷쪽 = 옮길 시작 위치 + 옮길 블럭의 길이
			fseek (fp, lStartOff + (long)stLength, SEEK_SET);
			/// 옮길 길이
			iMove = lTargetOff - lStartOff;
			while (stCount > 0)
			{
				/// 읽을 길이 =  읽는 블럭보다 남은 크기가 작으면 남은 크기만큼 아니면 읽는 블록
				stRead = stCount < (unsigned)sAllocSize ? (int)stCount : sAllocSize;
				fseek (fp, (long)(0 - stRead), SEEK_CUR);
				stReaded = (int)fread (btBuff, sizeof (BYTE), stRead, fp);
				stCount -= stReaded;
				fseek (fp, (long)(iMove - stReaded), SEEK_CUR);
				fwrite (btBuff, sizeof (BYTE), stReaded, fp);
				fseek (fp, (long)(- iMove - stReaded), SEEK_CUR);
			}
			if(bFlush) { fflush (fp); }
			/// 메모리 해제
			delete [] btBuff;
			return true;
		}
	}
	/// 똑같을때
	else 
	{ return true; }

	return false;
}


/*************************************************************************************
 * MakeFileHole : 파일의 중간에 구멍을 만든다
 */
bool MakeFileHole (long lPos, size_t stSize, FILE *fp, bool bFlush)
{
	long	lFileSize = __fseek (fp, 0, SEEK_END);				/// 파일의 크기
	size_t	stLength = lFileSize - lPos;						/// 뒷쪽에 있는 데이터의 크기

	if(stSize < 0 || fp == NULL || lPos < 0) { return false; }	/// Checking

	return ::MoveFileBlock (lPos, stLength, lPos + (long)stSize, 1000000/*1MB*/, fp, bFlush);
}



/// 파일 검색 결과를 저장할 전역변수
struct stFindFiles  g_stFindFile;


/*************************************************************************************
 * 디렉토리에 있는 파일명을 가지고 온다
 */
stFindFiles * GetFileList (const char * pBaseDir, const char *FilterExt, const char *FilterDir)
{
	std::string sPrefix = pBaseDir;

	g_stFindFile.dwFinded		= 0;
	g_stFindFile.dwTotalSize	= 0;
	g_stFindFile.FileList.clear ();
	g_stFindFile.vecExcDir.clear ();
	g_stFindFile.vecExcExt.clear ();
    
	char * buff = NULL;

	if(FilterExt != NULL && strlen (FilterExt) > 0 && (buff = new char [ strlen (FilterExt)  + 1 ]))
	{
		strcpy (buff, FilterExt);
		char * token = strtok (buff, ";");
		while (token)
		{
			/// 대문자로 바꾼다
			_strupr (token);
			g_stFindFile.vecExcExt.push_back (token);
			token = strtok (NULL, ";");
		}

		delete [] buff;
	}
	
	g_stFindFile.vecExcDir.push_back (".");
	g_stFindFile.vecExcDir.push_back ("..");
	if(FilterDir != NULL && strlen (FilterDir) > 0 && (buff = new char [ strlen (FilterDir)  + 1 ]))
	{
		strcpy (buff, FilterDir);
		char * token = strtok (buff, ";");
		while (token)
		{
			/// 대문자로 바꾼다
			_strupr (token);
			g_stFindFile.vecExcDir.push_back (token);
			token = strtok (NULL, ";");
		}

		delete [] buff;
	}
	
	__GetFileNames (pBaseDir, sPrefix.c_str ());
	
	g_stFindFile.dwFinded = (DWORD)g_stFindFile.FileList.size ();

	return &g_stFindFile;
}


/// 제외시킬 디렉토리인지 검사
bool __is_excdir (char * strDir)
{
	/// 검색할 Dir가 없으면 바로 false 리턴
	if(g_stFindFile.vecExcDir.empty ()) { return false; }

	std::vector<std::string>::iterator iv = g_stFindFile.vecExcDir.begin ();
	/// 스트링에 카피. 원래 문자열을 바꾸지 않기 위해서
	char * strDir2 = NULL;
	if((strDir2 = new char[ strlen (strDir) + 1 ]))
	{
		strcpy (strDir2, strDir);
		/// 대문자로 바꿈
		strDir2 = _strupr (strDir2);
		for(; iv != g_stFindFile.vecExcDir.end (); iv++)
		{
			if(strlen(strDir2) != (*iv).size ()) { continue; }
			if(strcmp (strDir2, (*iv).c_str ()) == 0) { return true; }
		}
		delete [] strDir2;
	}
	return false;
}


/// 제외시킬 디렉토리인지 검사
bool __is_excext (const char * stFileName)
{
	/// 검색할 확장자가 없으면 바로 false 리턴
	if(g_stFindFile.vecExcExt.empty ()) { return false; }
	/// .을 찾는다
	const char * Dot = strchr (stFileName, '.');
	if(!Dot) { return false; }

	/// 제외시킬 확장자가 저장된 vector
	std::vector<std::string>::iterator iv = g_stFindFile.vecExcExt.begin ();
	char * sFileName2 = NULL;

	if((sFileName2 = new char[strlen (stFileName) + 1]))
	{
		/// ".xxx"에서 "." 다음 확장자만 넣는다
		strcpy (sFileName2, Dot + 1);
		/// 대문자로 바꿈
		sFileName2 = _strupr (sFileName2);
		for(; iv != g_stFindFile.vecExcExt.end (); iv++)
		{
			if(strlen (sFileName2) != (*iv).size ()) { continue; }
			if(strcmp (sFileName2, (*iv).c_str ()) == 0) { return true; }
		}
		/// 스트링을 위해 할당한 메모리를 해제
		delete [] sFileName2;
	}

	return false;
}


/**
 * 디렉토리로부터 파일이름을 가져온다
 * @param pBaseDir 파일이름을 검색할 디렉토리
 * @param FilterEx 제외시킬 파일
 * @param FilterInc 포함시킬 파일
 */
void __GetFileNames (const char * pBaseDir, const char * prefix)
{
	std::string finded;
	std::string sCurDir;
	std::string sPrefix = prefix;
	
	sCurDir = ::__GetCurDir ();

	if(strlen (pBaseDir) > 0 ) { /*BOOL bChange =*/ SetCurrentDirectory (pBaseDir); }
	std::string sCurDir2 = ::__GetCurDir ();

	WIN32_FIND_DATA FileData; 
	HANDLE hSearch; 

	BOOL fFinished = FALSE; 
	 
	hSearch = FindFirstFile("*.*", &FileData); 
	if (hSearch == INVALID_HANDLE_VALUE) { return /*SkelList*/;	} 
	 
	while (!fFinished) 
	{
		/// 디렉토리이고 제외시킬 
		if(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && 
			!__is_excdir (FileData.cFileName))
		{
			std::string sSubPrefix = sPrefix;
			/// \이 마지막에 없으면 추가함
			if(sPrefix.size () > 0 && sPrefix[sPrefix.size () - 1] != '\\') { sSubPrefix.append ("\\"); }
			sSubPrefix.append (FileData.cFileName);
			sSubPrefix.append ("\\");
			::__GetFileNames (FileData.cFileName, sSubPrefix.c_str ());
		}
		else if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && 
			!__is_excext (FileData.cFileName))
		{
			// finded = pBaseDir;
			finded = sPrefix;
			/// \이 마지막에 없으면 추가함
			if(finded.size () > 0 && finded[finded.size () - 1] != '\\') { finded.append ("\\"); }
			finded.append (FileData.cFileName);
			/// 파일크기를 알아내서 더한다
			g_stFindFile.dwTotalSize += ::__GetFileSize (FileData.cFileName);
			g_stFindFile.FileList.push_back (finded);
		}
		
		if (!FindNextFile(hSearch, &FileData)) 
		{ 
			if (GetLastError() == ERROR_NO_MORE_FILES) { fFinished = TRUE; }
		}
	} 
	 
	// Close the search handle. 
	 
	FindClose(hSearch);

	SetCurrentDirectory (sCurDir.c_str ());
}


/********************************************************************************************
 * 디렉토리내에 있는 파일의 갯수를 조사한다. 하위디렉토리는 검사 안 한다
 */
int GetFileNumInDir (const char *Dir)
{
	int nNum = 0;
	BOOL fFinished = FALSE;							/// 더 이상의 파일이 없으면 여기에 TRUE값

	WIN32_FIND_DATA FileData; 
	HANDLE hSearch = NULL;

	hSearch = FindFirstFile ("*.*", &FileData); 
	if (hSearch == INVALID_HANDLE_VALUE) { return 0; } 

	while (!fFinished) 
	{
		/// 디렉토리이고 제외시킬 
		if(!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			nNum++;
		}
				
		if (!FindNextFile (hSearch, &FileData)) 
		{ 
			if (GetLastError() == ERROR_NO_MORE_FILES) { fFinished = TRUE; }
		}
	} 

	return nNum;
}


/********************************************************************************************
 * 디렉토리내에 있는 파일이름을 조사한다
 * @param Dir		조사할 디렉토리
 * @param ppFile	저장할 메모리 포인터
 * @param nNum		할당된 메모리의 최대갯수
 * @param nMaxName	최대 파일이름 길이. NULL포함
 *
 * @return			조사한 파일의 갯수
 */
int GetFileNameInDir (const char *Dir, char **ppFile, int nNum, int nMaxName)
{
	int nRetNum = 0;
	int i = 0;
	BOOL fFinished = FALSE;							/// 더 이상의 파일이 없으면 여기에 TRUE값

	WIN32_FIND_DATA FileData; 
	HANDLE hSearch = NULL;

	hSearch = FindFirstFile ("*.*", &FileData); 
	if (hSearch == INVALID_HANDLE_VALUE) { return 0; } 

	while (!fFinished) 
	{
		/// 디렉토리이고 제외시킬 ".", ".."도 제외
		if(!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			*(ppFile[ i ] + nMaxName - 1) = 0;						/// strncpy를 사용할 경우 주의
			strncpy (ppFile[ i++ ], FileData.cFileName, nMaxName);	/// 파일이름을 카피, FileData.cFileName은 char [260]
			nRetNum++;													/// 갯수 증가
		}
				
		if (!FindNextFile (hSearch, &FileData)) 
		{ 
			if (GetLastError() == ERROR_NO_MORE_FILES) { fFinished = TRUE; }
		}
	} 

	return nRetNum;
}


char g_szOldDir[ 1024 ];
/********************************************************************************************
 * 디렉토리내에 있는 첫번째 파일이름을 조사한다.
 * @param Dir		조사할 디렉토리
 * @param FirstName 첫번째 파일명
 * @return			핸들. 실패하면 NULL
 */
HANDLE __FindFirstFileName (const char *Dir, const char **FirstName)
{
	HANDLE hSearch = NULL;
	WIN32_FIND_DATA FileData;
	
	GetCurrentDirectory (1024, g_szOldDir);
	SetCurrentDirectory (Dir);

	hSearch = FindFirstFile ("*.*", &FileData); 
	if (hSearch == INVALID_HANDLE_VALUE) 
	{ 
		FirstName = NULL;
		return NULL;
	} 

	while (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		FindNextFile (hSearch, &FileData);
		if (GetLastError() == ERROR_NO_MORE_FILES)					/// 검색은 성공했지만 파일이 하나도 없는 경우
		{ 
			FirstName = NULL; 
			return hSearch;											/// 파일이 하나도 없는 경우 여기가 실행된다
		}
	}
	*FirstName = (const char *)FileData.cFileName;

	return hSearch;
}


/********************************************************************************************
 * 디렉토리내에 있는 다음 파일이름을 리턴해 준다
 * @param Dir		조사할 디렉토리
 * @param hSearch	GetFileNameInDir에 의해 만들어진 핸들
 *
 * @return			파일이름
 * 디렉토리이고 제외시킬 ".", ".."도 제외
 */
const char * __FindNextFileName (HANDLE hSearch)
{
	WIN32_FIND_DATA FileData; 

	if(!hSearch) { return NULL; }									/// hSearch가 유효한 값이 아니면 NULL을 리턴

	FindNextFile (hSearch, &FileData);
    if(GetLastError() == ERROR_NO_MORE_FILES) { return NULL; }		/// 더 이상의 파일이 없다

	while (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		if(!FindNextFile (hSearch, &FileData)) 
		{
			if(GetLastError() == ERROR_NO_MORE_FILES) { return NULL; }	/// 더 이상의 파일이 없다
		}
	}

	return (const char *)FileData.cFileName;
}


/****************************************************************************************************
 * 파일 검색을 닫는다. 그리고 디렉토리도 원위치 시킨다
 */
void __CloseFindFileName (HANDLE hSearch)
{
	FindClose (hSearch);
	SetCurrentDirectory (::g_szOldDir);
	g_szOldDir[ 0 ] = 0;
}


/****************************************************************************************************
 * 경로가 디렉토리인지 아닌지 검사
 */
bool __IsDirectory (const char * Path)
{
	// return PathIsDirectory (Path) == TRUE; /// or next code

	if(_access (Path, 0) != 0) { return false; } /// 이게 없으면 없는 경로인 경우 디렉토리로 해석된다

	struct _stat s;

	_stat (Path , &s);

	return ((s.st_mode & _S_IFDIR) != 0);
}


/****************************************************************************************************
 * 경로가 파일인지 아닌지 검사
 */
bool __IsFile (const char * Path)
{
	if(_access (Path, 0) != 0) { return false; } /// 이게 없으면 없는 경로인 경우 이상하게 작동한다

	struct _stat s;

	_stat (Path , &s);

	return ((s.st_mode & _S_IFREG) != 0);
}


/****************************************************************************************************
 * 경로1에서 경로2를 제외한 문자열을 리턴한다
 * 메모리할당에 실패해도 NULL을 리턴하므로 사용할 때 주의할것
 */
const char * GetRelativePath (const char *Path1 , const char * Path2)
{
	char * sPath1 = new char[ strlen (Path1) + 1 ];
	char * sPath2 = new char[ strlen (Path2) + 1 ];
	if(!sPath1 || !sPath2) { return NULL; }					/// 메모리할당에 실패하면 그냥 NULL을 리턴.
	strcpy (sPath1, Path1);									/// 대문자로 변환하기 위해
	strcpy (sPath2, Path2);									/// 대문자로 변환하기 위해

	if (strlen(Path1) < strlen (Path2)) { return NULL; }	/// 경로1이 경로2보다 작으면 NULL을 리턴
	_strupr (sPath1);
	_strupr (sPath2);

	for(unsigned int i = 0; i < strlen(sPath2); i++)
	{
		if(sPath1[ i ] != sPath2[ i ]) { return NULL; }
	}

	delete [] sPath1;
	delete [] sPath2;

	return &Path1[ strlen(Path2) + 1 ];
}


/**************************************************************************
 * Base Name을 알아낸다
 */
char * GetBaseName (const char *Path)
{
	char Drive [8];
	char Dir [255];
	char FileName [255];
	char Ext [255];

	_splitpath (Path, Drive, Dir, FileName, Ext);
	
	return (char *)(Path + strlen (Path) - ( strlen (FileName) + strlen (Ext))) ;
}


/**************************************************************************
 * 디렉토리 경로를 알아낸다
 */
int GetDirectory (const char *Path, char *buff, int nMax)
{
	char Drive [8];
	char Dir [255];
	char FileName [255];
	char Ext [255];

	_splitpath (Path, Drive, Dir, FileName, Ext);
	if(buff) 
	{ 
		strncpy (buff, Dir, nMax); 
	}
	
	return (int)(strlen (Path) - strlen (FileName) - strlen (Ext));
}


/**************************************************************************
 * 디렉토리 경로를 알아낸다 
 * @return std::string
 */
std::string GetDirectory (const char *Path)
{
	char Drive [8];
	char Dir [255];
	char FileName [255];
	char Ext [255];

	_splitpath (Path, Drive, Dir, FileName, Ext);
	
	std::string sDir = Drive;
	sDir.append (Dir);
	
	return sDir;
}


/**************************************************************************
 * 확장자까 빠진 Base File명을 조사한다
 */
std::string GetFileNameWithoutExt (const char *Path)
{
	std::string strName;

	char Drive [8];
	char Dir [255];
	char FileName [255];
	char Ext [255];

	_splitpath (Path, Drive, Dir, FileName, Ext);
	
	strName = FileName;
	
	return strName;
}


/*********************************************************************************
 * File의 사이즈를 변경한다
 * @param lNewSize 새롭게 변경할 파일의 크기
 * @param fp 크기를 변경할 파일의 파일포인터
 */
bool __ftruncate (long lNewSize, FILE * fp)
{
	int hFile = _fileno (fp);
	/// _chsize returns the value 0 if the file size is successfully changed
	
	return (_chsize (hFile, lNewSize) == 0);
}


/*********************************************************************************
 * 파일 indicator를 이동시키고 그 위치를 리턴한다
 * parameter는 fseek와 동일하다
 * @param origin SEEK_SET SEEK_CUR SEEK_END
 */
long __vfseek (FILE *stream, long offset, int origin)
{
	int iRet = fseek (stream, offset, origin);

	fpos_t pos ;
	fgetpos (stream, &pos);

	return (long)pos;
}


/*********************************************************************************
 * 파일의 특정 위치에 데이터를 삽입한다
 */
bool __InSertFileBlock (void *buff, size_t sSize, long lPos, FILE *fp, bool bFlush)
{
	long lFileSize = __vfseek (fp, 0, SEEK_END);
	if(__MoveFileBlock (lPos, lFileSize - lPos, lPos + (long)sSize, 1000000/*1MB*/, fp, true))
	{
		fseek (fp, lPos, SEEK_SET);
		fwrite (buff, sizeof (char), sSize, fp);
		return true;
	}
	if(bFlush) { fflush (fp); }

	return false;
}


/*********************************************************************************
 * MakeFileHole : 파일의 중간에 구멍을 만든다
 */
bool __MakeFileHole (long lPos, size_t stSize, FILE *fp, bool bFlush)
{
	long	lFileSize = __vfseek (fp, 0, SEEK_END);				/// 파일의 크기
	size_t	stLength = lFileSize - lPos;						/// 뒷쪽에 있는 데이터의 크기

	if(stSize < 0 || fp == NULL || lPos < 0) { return false; }	/// Checking

	return ::__MoveFileBlock (lPos, stLength, lPos + (long)stSize, 1000000/*1MB*/, fp, bFlush);
}


/*********************************************************************************
 * 파일 블록 이동 시키기
 * @param lStartOff		올길 부분의 시작 Offset
 * @param stLength		블록의 길이
 * @param lTargetOff	옮길 Offset
 * @param sAllocSize	옮기는 단위 . 메모리 할당량이 됨
 * @param bFlush		true 이면 옮기고 나서 fflush를 하고 false이면 안 한다.
 * @param fp			파일 핸들
 */
bool __MoveFileBlock (long lStartOff, size_t stLength, long lTargetOff, int sAllocSize, FILE * fp, bool bFlush)
{
	BYTE *btBuff	= NULL;		/// 버퍼
	int iMove		= 0;		/// 옮겨지는 길이
	size_t stCount	= stLength; /// while 루프를 위한 Count
	int stRead		= 0;		/// 현재 읽을 바이트수
	int	stReaded	= 0;		/// 읽혀진 바이트수
	long lOldSize	= 0;		/// 원래 파일의 크기

	if(stLength < (unsigned)sAllocSize) { sAllocSize = (int)stLength; }
	/// 현재 미결된 쓰기작업을 모두 Device로 밀어넣는다
	fflush (fp);
	/// 앞으로
	if(lTargetOff - lStartOff < 0)
	{
		iMove = lStartOff - lTargetOff;
		if((btBuff = new BYTE[ sAllocSize ]))
		{
			fseek (fp, lTargetOff, SEEK_SET);
			while (stCount > 0)
			{
				/// 읽을 양을 결정
				stRead = stCount > (unsigned)sAllocSize ? sAllocSize : (int)stCount;
				/// 읽을 위치로 이동
				fseek (fp, iMove, SEEK_CUR);
				/// 읽는다
				stReaded = (int)fread (btBuff, sizeof (BYTE), stRead, fp);
				if(stReaded > 0)
				{
					/// 읽을 양에서 읽은 만큼 뺀다
					stCount -= stReaded;
					/// 읽은 만큼 앞으로 가면 원래 위치 그리고 imove 만큼 앞으로
					fseek (fp, (long)(-iMove - stReaded), SEEK_CUR);
					fwrite (btBuff, sizeof (BYTE), stReaded, fp);
				}
				else
				{
					delete [] btBuff;
					return false;
				}
			}
			if(bFlush) { fflush (fp); }
			lOldSize = ::__vfseek (fp, 0, SEEK_END);
			/// 파일크기를 줄이는 것은 이 함수를 호출한 쪽에서 해야 한다. 
			/// ::__ftruncate (lOldSize - iMove, fp);
			/// 메모리 해제
			delete [] btBuff;
			return true;
		}
	}
	/// 뒤로 밀때
	else if (lTargetOff - lStartOff > 0)
	{
		if((btBuff = new BYTE[ sAllocSize ]))
		{
			// lOldSize = ::__vfseek (fp, 0, SEEK_END);
			/// 뒷쪽 데이터의 크기
			// size_t szLengthOfletter = lOldSize - lStartOff;
			/// 가장 뒷쪽 = 옮길 시작 위치 + 옮길 블럭의 길이
			fseek (fp, lStartOff + (long)stLength, SEEK_SET);
			/// 옮길 길이
			iMove = lTargetOff - lStartOff;
			while (stCount > 0)
			{
				/// 읽을 길이 =  읽는 블럭보다 남은 크기가 작으면 남은 크기만큼 아니면 읽는 블록
				stRead = stCount < (unsigned)sAllocSize ? (int)stCount : sAllocSize;
				fseek (fp, (long)(0 - stRead), SEEK_CUR);
				stReaded = (int)fread (btBuff, sizeof (BYTE), stRead, fp);
				if(stReaded > 0)
				{
					stCount -= stReaded;
					fseek (fp, (long)(iMove - stReaded), SEEK_CUR);
					fwrite (btBuff, sizeof (BYTE), stReaded, fp);
					fseek (fp, (long)(- iMove - stReaded), SEEK_CUR);
				}
				else
				{
					delete [] btBuff;
					return false;
				}
			}
			if(bFlush) { fflush (fp); }
			/// 메모리 해제
			delete [] btBuff;
			return true;
		}
	}
	/// 똑같을때
	else 
	{ return true; }

	return false;
}
 


/*********************************************************************************
 *
 * File Mode 
 *
 */

bool CFileMode::CheckMode (const char * FileName, int iChkMode, bool bForce)
{
	bool bAccess = (_access (FileName, iChkMode ) == 0);
	
	if(bAccess == false && bForce)
	{
		switch (iChkMode)
		{
		case CFileMode::MODE_READ:
			return (_chmod ( FileName, _S_IREAD  ) == 0);

		case CFileMode::MODE_WRITE:
			return (_chmod ( FileName, _S_IWRITE  ) == 0);

		case CFileMode::MODE_READWRITE:
			return (_chmod ( FileName, _S_IREAD | _S_IWRITE  ) == 0);
		}
	}
	
	return bAccess;
}

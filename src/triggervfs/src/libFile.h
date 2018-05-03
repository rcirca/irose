#ifndef __LIBFILE_H__
#define __LIBFILE_H__

#include <string>
#include <vector>


/// 이동시키고 위치 리턴
long __fseek (FILE *fp, int lOff, int iOrg);
/// 파일 블럭 이동
bool MoveFileBlock (long lStartOff, size_t stLength, long lTargetOff, int sAllocSize, 
					  FILE * fp, bool bFlush = true);
/// MakeFileHole : 파일의 중간에 구멍을 만든다
bool MakeFileHole (long lPos, size_t stSize, FILE *fp, bool bFlush);

/// 파일 검색결과 저장
struct stFindFiles 
{
	DWORD						dwFinded;
	DWORD						dwTotalSize;
	std::vector<std::string>	FileList;
	std::vector<std::string>	vecExcExt;	/// 제외시킬 확장자
	std::vector<std::string>	vecExcDir;	/// 제외시킬 디렉토리
};

stFindFiles * GetFileList (const char * pBaseDir, const char *FilterExt, const char *FilterDir);

/// 폴더로부터 파일리스트를 가져온다. GetFileList에 의해 호출
void __GetFileNames (const char * pBaseDir, const char * prefix);
/// 디렉토리내에 있는 파일의 갯수를 조사한다. 하위디렉토리는 검사 안 한다
int GetFileNumInDir (const char *Dir);
/// 디렉토리내에 있는 파일이름을 조사한다
int GetFileNameInDir (const char *Dir, char **ppFile, int nNum, int nMaxName);
/// 디렉토리내에 있는 첫번째 파일이름을 조사한다.
HANDLE __FindFirstFileName (const char *Dir, const char **FirstName);
/// 디렉토리내에 있는 다음 파일이름을 리턴해 준다
const char * __FindNextFileName (HANDLE hSearch);
/// 파일 Find를 닫는다
void __CloseFindFileName (HANDLE hSearch);
/// 경로가 파일인지 아닌지 검사
bool __IsFile (const char * Path);
/// 경로가 디렉토리인지 아닌지 검사
bool __IsDirectory (const char * Path);

/// 경로1에서 경로2를 제외한 문자열을 리턴한다
const char * GetRelativePath (const char *Path1 , const char * Path2);


/**************************************************************************************************
 * 파일 경로와 문자열 관련
 */

/// Full Path에서 Base Name을 알아낸다
char * GetBaseName (const char *Path);

/// Full Path에서 디렉토리 경로를 알아낸다
int GetDirectory (const char *Path, char *buff, int nMax);

/// Full Path에서 디렉토리 경로를 알아낸다 
std::string GetDirectory (const char *Path);

/// 확장자까 빠진 Base File명을 조사한다
std::string GetFileNameWithoutExt (const char *Path);

bool __ftruncate (long lNewSize, FILE * fp);

long __vfseek (FILE *stream, long offset, int origin);

bool __InSertFileBlock (void *buff, size_t sSize, long lPos, FILE *fp, bool bFlush);

bool __MakeFileHole (long lPos, size_t stSize, FILE *fp, bool bFlush);

bool __MoveFileBlock (long lStartOff, size_t stLength, long lTargetOff, int sAllocSize, FILE * fp, bool bFlush = true);


/***********************************************************
 * 권한 관련
 */
class CFileMode
{
public:
	static const int MODE_EXISTS	= 0; // Existence only 
	static const int MODE_WRITE		= 2; // Write permission 
	static const int MODE_READ		= 4; // Read permission 
	static const int MODE_READWRITE	= 6; // Read and write permission 


public:
	static bool CheckMode (const char * FileName, int iChkMode, bool bForce = false);
};


#endif

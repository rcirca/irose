#ifndef ____FILE_H__
#define ____FILE_H__

#include <vector>
#include <string>
#include <windows.h>

void __GetFileNames (const char * pBaseDir, const char * prefix);

/// 현재 디렉토리를 알아낸다
std::string GetCurDir (void);
void GetCurDir (char *buff, int iMax);
/// 파일 길이를 알아낸다
long GetFileLength (const char *FileName);

/// 파일 검색결과 저장
struct stFindFiles 
{
	DWORD						dwFinded;
	DWORD						dwTotalSize;
	std::vector<std::string>	FileList;
	std::vector<std::string>	vecExcExt;	/// 제외시킬 확장자
	std::vector<std::string>	vecExcDir;	/// 제외시킬 디렉토리
};

/// 디렉토리로부터 파일이름을 찾아내서 리턴한다
stFindFiles * FindFiles (const char * pBaseDir, const char *FilterEx=NULL, const char *FilterInc=NULL);
/// 내부적으로 FindFiles ()가 파일 검색할 준비를 끝내고 이 함수를 호출한다
void __FindFiles (const char * pBaseDir, const char * prefix);



#endif

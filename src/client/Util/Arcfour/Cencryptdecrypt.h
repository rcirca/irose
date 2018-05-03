#ifndef _Encrypt_Decrypt_
#define _Encrypt_Decrypt_
#include <string>


const unsigned int KEYSIZEPOSITION = 15; // 키의 자릿수. 아스키코드 범위가 0~127이다..orz 
const unsigned int KEYINFORMATION = 394; // 키의 실제 사이즈.
const unsigned int KEYPOSITION = 135; // 키가 시작되는 위치.

const unsigned int SORT = 255; // 랜덤으로 생성할 숫자의 범위.

const int MAXSTRING  = 512; // 최대 대입 개수 설정.


struct Crypt
{
	unsigned int size; // 키 사이즈
	bool bKeyJudgment; // 실행파일에 키가 들어있는지 유무.
	std::string sCyrptKey; // 키
};

class CEncryptDecrypt
{
public:
	CEncryptDecrypt();
	~CEncryptDecrypt();

	int KeyPositionInformation(std::string sKey); // 실제 키의 사이즈를 리턴.

	Crypt EncryptDataLoad(char* filename);  
	Crypt EncryptDataLoad_Server(char* filename);

	std::string Encrypt(std::string pEncrypt);  // 키를 암호화
	std::string DecryptFromFile(std::string pDecrypt);   // File에서 뽑아낸 걸 암호화 해제.
	std::string DecryptFromString(std::string pDecrypt);  // 암호화 스트링만 뽑아서 암호화 해제.
};

extern CEncryptDecrypt g_EncryptDecrypt;

#endif
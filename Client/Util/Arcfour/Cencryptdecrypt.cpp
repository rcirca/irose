#include "stdafx.h"
#include "Cencryptdecrypt.h"
#include <time.h>
#include <math.h>

CEncryptDecrypt g_EncryptDecrypt;

const unsigned int MAXKEY = 15; // 최대 키개수 설정.

// KEY - 실제 키 변환용, KEY2 - 키 사이즈 변환용, KEY3 - 쓰레기데이터 변환용
static KEY[MAXKEY] = {0x34, 0x5c, 0x3d, 0x52, 0xce, 0xf3, 0x12, 0xd4, 0x05, 0x91, 0xee, 0xff, 0x49, 0xc2, 0xd2}; 
static KEY2[3] = {0x94, 0x20, 0x5f};
static KEY3[MAXKEY] = {0x53, 0x14, 0xf2, 0x61, 0x11, 0x54, 0xff, 0x72, 0x99, 0x44, 0xf4, 0xa1, 0xaa, 0xba, 0xc2};

CEncryptDecrypt::CEncryptDecrypt()
{
	
}

CEncryptDecrypt::~CEncryptDecrypt()
{
	
}

std::string CEncryptDecrypt::Encrypt(std::string pEncrypt)
{

	char ch;
	
	int iSizeEncrpt = (int)pEncrypt.size();

	unsigned int count = 0;
	unsigned int CharCount = KEYPOSITION; // 키가 들어갈 최초 위치.

	std::string conclusion;
	conclusion.clear();
	srand((unsigned int)GetTickCount());

	for(int k =0; k < 512; k++)
	{

		ch = rand() % SORT;

		ch ^=KEY3[count % MAXKEY];

		conclusion += ch;

	}

	count = 0;

	conclusion[3] = 'B';
	conclusion[4] = 'o';
	conclusion[5] = 'x';

	char Temp_Keysize[5];
	memset(Temp_Keysize, 0, 5);
	sprintf(Temp_Keysize, "%d", iSizeEncrpt);

	conclusion[KEYSIZEPOSITION] = (int)strlen(Temp_Keysize);

	for(unsigned int m = KEYINFORMATION; m < KEYINFORMATION + strlen(Temp_Keysize); m++)
	{
		ch = Temp_Keysize[count];
		ch ^=KEY2[count];

		conclusion[m] = ch;
		count++;
	}

	count = 0;

	for(int i =0 ; i < iSizeEncrpt; i++)
	{

		ch = pEncrypt[i];

		ch ^=KEY[count % MAXKEY];

		conclusion[CharCount]= ch;

		CharCount++;
		count++;
	}

/*
	for(int j = conclusion.size() - 1; j > -1; j--)
	{
		Temp += conclusion[j];
	}
*/

	return conclusion;
}


std::string CEncryptDecrypt::DecryptFromFile(std::string pEncrypt)
{
	char ch;
	int iSizeEncrpt = KeyPositionInformation(pEncrypt);

	std::string Temp;	

	unsigned int count = 0;
/*
	for(int j = iSizeEncrpt - 1; j > -1; j--)
	{
		Temp += pEncrypt[j];

	}
*/
	std::string conclusion;
	for(int i =0 ; i < iSizeEncrpt; i++)
	{

		ch = pEncrypt[i + KEYPOSITION];

		ch ^=KEY[count % MAXKEY];

		conclusion += ch;
		count++;
	}
	
	return conclusion;
	
}


std::string CEncryptDecrypt::DecryptFromString(std::string pEncrypt)
{
	char ch;
	size_t iSizeEncrpt = pEncrypt.size();
	std::string Temp;	

	unsigned int count = 0;
	/*
	for(int j = iSizeEncrpt - 1; j > -1; j--)
	{
	Temp += pEncrypt[j];

	}
	*/
	std::string conclusion;
	for(size_t i =0 ; i < iSizeEncrpt; i++)
	{

		ch = pEncrypt[i];

		ch ^=KEY[count % MAXKEY];

		conclusion += ch;
		count++;
	}

	return conclusion;
}

int CEncryptDecrypt::KeyPositionInformation(std::string sKey)
{
	char ch;
	char Temp[5];
	memset(Temp, 0, 5);
	char aTempSize[5];
	memset(aTempSize, 0, 5);
	int keysize = (int)sKey[15];

	unsigned int count = 0;
	for(int i = 0; i < keysize; i++)
	{
		ch = sKey[KEYINFORMATION + i];
		ch ^= KEY2[count];
		Temp[i]= ch;
		count++;
	}
	

	int len = (int)strlen(Temp);
	if( len != keysize)
	{
		return 0;
	}

	int j = 0;

	for(i=0;i< len ;i++)
	{
		if ( (Temp[i] & 0xf0) == 0x30) 
		{
			aTempSize[j]=Temp[i];
			j++;           
		} 
	}

	len = j;

	int ivalue = 0;

	for(i=0;i< len ; i++)
	{
		ivalue += (int)( (aTempSize[i] & 0x0f)* (int)pow(10,len-(i+1)) );
	}

	return ivalue;
}

Crypt CEncryptDecrypt::EncryptDataLoad(char* filename)
{
	FILE* encryptData;

	encryptData = fopen (filename, "rb");

	char temp[MAXSTRING];
	memset(temp, 0, MAXSTRING);
	long pos = 0L;

	Crypt s_Key;
	s_Key.sCyrptKey.clear();
	s_Key.size = 0;
	s_Key.bKeyJudgment = false;

	if(encryptData)
	{	

		// 파일의 크기를 알아내서 내가 긁어야하는 최소 단위보다 작으면 리턴~
		fseek(encryptData,0,SEEK_END); 
		pos = ftell(encryptData);
		if(pos < MAXSTRING)
		{
//			char temp1[MAXSTRING];
//			sprintf(temp1, "Size of file which try to read is smaller then MAXSTRING %d .", MAXSTRING);
//			MessageBox(NULL, temp1, "에러", MB_OK);
			return s_Key;
		}

		//fseek는 성공을 하면 0을 리턴한다..
		if (0 != fseek(encryptData, -MAXSTRING, SEEK_CUR))
		{
//			MessageBox(NULL, "File Seek Error~", "Error", MB_OK);
			return s_Key;
		}

		//긁어!!
		size_t ddd = fread(temp, sizeof(char), MAXSTRING , encryptData);

		std::string KeyFile;
		for(int e= 0; e < MAXSTRING; e++)
		{
			KeyFile += temp[e];
		}

		if(KeyFile[3] == 'B' && KeyFile[4] == 'o' && KeyFile[5] == 'x')
			s_Key.bKeyJudgment = true;

		s_Key.sCyrptKey = DecryptFromFile(KeyFile); // 나온 키
		s_Key.size = KeyPositionInformation(KeyFile); // 키 크기

	}

	fclose(encryptData);

	return s_Key;
}


Crypt CEncryptDecrypt::EncryptDataLoad_Server( char* filename )
{
	FILE *fp;

	char szKey[1024];
	int nKeyLen = 0;

	if((fp = fopen( filename, "rb")) != NULL)
	{		
		fseek(fp, 0, SEEK_END);
		nKeyLen = ftell(fp);

		if(nKeyLen > sizeof(szKey))
			nKeyLen = sizeof(szKey);
		fseek(fp, 0, SEEK_SET);
		fread(szKey, sizeof(char), nKeyLen, fp);

		fclose(fp);
	}
	else
	{
		strcpy(szKey, "1234567890123456");	// 디폴트 키값 사용.
		nKeyLen = 16;
	}

	Crypt crypt;
	crypt.sCyrptKey = szKey;
	crypt.size = nKeyLen;
	crypt.bKeyJudgment = true;

	return crypt;

}
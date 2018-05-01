#ifndef _BYTE_DEFINED
#define _BYTE_DEFINED
typedef unsigned char BYTE;
#endif

#ifndef _WORD_DEFINED
#define _WORD_DEFINED
typedef unsigned short WORD;
#endif

/// 메모리에 있는 데이터로부터 CRC값을 계산한다
unsigned long getcrc_32 (const BYTE * buff, DWORD dwSize);
/// 파일을 읽어서 32비트 CRC를 계산한다
unsigned long getcrc_32_file (const char * FileName);
#include "stdafx.h"
#include "libCrc.h"
#include "classCRC.h"


// Crc 값을 조사한다
DWORD CLibCrc::GetIcarusCrc ( const unsigned char * pbtData, int lLength)
{
	return classCRC::DataCRC32 (pbtData, lLength, true );
};
#ifndef __LIBCRC_H__
#define __LIBCRC_H__

/*******************************************************************
 *
 * CRC Library
 *
 */
class CLibCrc
{
public:
	// Crc 값을 조사한다
	static DWORD GetIcarusCrc ( const unsigned char * pbtData, int iLength);
};

#endif

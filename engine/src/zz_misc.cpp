/** 
 * @file zz_misc.cpp
 * @brief misc class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    30-apr-2003
 *
 * $Header: /engine/src/zz_misc.cpp 4     04-10-14 10:37a Zho $
 * $History: zz_misc.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-10-14   Time: 10:37a
 * Updated in $/engine/src
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-28   Time: 5:44p
 * Updated in $/engine/src
 * riva tnt2 enabled.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include <stdlib.h>
#include "zz_misc.h"

int random_number(int imin, int imax)
{
	if (imin == imax) return(imin);
	return((rand() % (abs(imax-imin)+1))+imin);
}

float random_number(float fmin, float fmax)
{
	if (fmin == fmax) return(fmin);
	float frandom = (float)rand() / (float)RAND_MAX;
	return((frandom * (float)fabs(fmax-fmin))+fmin);
}

/*
// [REF] http://www.flipcode.org/cgi-bin/fcarticles.cgi?show=4&id=64182

// Returns the least power of 2 greater than or equal to "x".
// Note that for x=0 and for x>2147483648 this returns 0!
#ifdef __GNUC__
unsigned ceilPowerOf2(unsigned x) {
    unsigned eax;
    __asm__(
        "xor eax,eax\n"
        "dec ecx\n"
        "bsr ecx,ecx\n"
        "cmovz ecx,eax\n"
        "setnz al\n"
        "inc eax\n"
        "shl eax,cl\n"
        : "=a" (eax)
        : "c" (x)
    );
    return eax;
}
#else
__declspec(naked) unsigned __fastcall ceilPowerOf2(unsigned x) {
    __asm {
        xor eax,eax
        dec ecx
        bsr ecx,ecx
        cmovz ecx,eax
        setnz al
        inc eax
        shl eax,cl
        ret
    }
}
#endif// Returns the greatest power of 2 less than or equal to "x".
// Note that for x=0 this returns 0!
#ifdef __GNUC__
unsigned floorPowerOf2(unsigned x) {
    unsigned eax;
    __asm__(
        "xor eax,eax\n"
        "bsr ecx,ecx\n"
        "setnz al\n"
        "shl eax,cl\n"
        : "=a" (eax)
        : "c" (x)
    );
    return eax;
}
#else
__declspec(naked) unsigned __fastcall floorPowerOf2(unsigned x) {
    __asm {
        xor eax,eax
        bsr ecx,ecx
        setnz al
        shl eax,cl
        ret
    }
}
#endif
*/

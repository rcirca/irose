/** 
 * @file zz_gamma.h
 * @brief gamma control class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-mar-2003
 *
 * $Header: /engine/include/zz_gamma.h 1     03-11-30 8:05p Zho $
 * $History: zz_gamma.h $
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:05p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:53p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:47p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:19p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:55p
 * Updated in $/znzin11/engine/include
 * cleanup #include structure
 */

#ifndef __ZZ_GAMMA_H__
#define __ZZ_GAMMA_H__

//--------------------------------------------------------------------------------
class zz_gamma {
	float gamma_value;

public:
    uint16 red[256];
    uint16 green[256];
    uint16 blue[256];

	zz_gamma (int type = 0); // type 0 : normal, 1 : white, 2 : black
	~zz_gamma ();

	// load()
	// save()
	void reset ();

	// change to black ramp
	void to_black ();

	// change to white ramp
	void to_white ();

	void copy_from (const zz_gamma& from);

	void interpolate (const zz_gamma& from, const zz_gamma& to, float t);

	void set_gamma_value (float gamma_value_in);
	float get_gamma_value ();
};

#endif // __ZZ_GAMMA_H__
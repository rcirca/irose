/** 
 * @file zz_gamma.cpp
 * @brief gamma control class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-mar-2003
 *
 * $Header: /engine/src/zz_gamma.cpp 2     04-02-11 2:05p Zho $
 * $History: zz_gamma.cpp $
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
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include <math.h>
#include "zz_type.h"
#include "zz_algebra.h"
#include "zz_gamma.h"
#include "zz_log.h"

zz_gamma::zz_gamma (int type)
{
	switch (type) {
		case 1:
			to_white();
			break;
		case 2:
			to_black();
			break;
		default:
			reset();
	}
}

zz_gamma::~zz_gamma ()
{
}

void zz_gamma::reset ()
{
	for (int i = 0; i < 256; ++i) {
		red[i]   = (i+1)*256 - 1;
		green[i] = (i+1)*256 - 1;
		blue[i]  = (i+1)*256 - 1;
	}
}

void zz_gamma::to_black ()
{
	for (int i = 0; i < 256; i++) {
		red[i]   = 0;
		green[i] = 0;
		blue[i]  = 0;
	}
}

void zz_gamma::to_white ()
{
	for (int i = 0; i < 256; i++) {
		red[i]   = 65535;
		green[i] = 65535;
		blue[i]  = 65535;
	}
}

void zz_gamma::copy_from (const zz_gamma& from)
{
	for (int i = 0; i < 256; ++i) {
		this->red[i] = from.red[i];
		this->green[i] = from.green[i];
		this->blue[i] = from.blue[i];
	}
}

float zz_gamma::get_gamma_value ()
{
	return gamma_value;
}

void zz_gamma::set_gamma_value (float gamma_value_in)
{
	gamma_value = gamma_value_in;

	if (gamma_value <= -4.0f) {
		gamma_value = -3.9f;
	}
	uint16 value;
	for (int i = 0; i < 256 ; ++i) {
		value = (uint16)(65535.f*powf((float)i/255.0f, 1.0f/((gamma_value/4.0f) + 1.0f)));
		this->red[i]   = value;
		this->green[i] = value;
		this->blue[i]  = value;
	}
}

void zz_gamma::interpolate (const zz_gamma& from, const zz_gamma& to, float t)
{
	t = t*t;
	for (int i = 0; i < 256; i++) {
		// [red] component
		// ...if same, just skip
		if (from.red[i] != to.red[i]) 
			red[i] = uint16(float(from.red[i])*(1.0f - t) + float(to.red[i])*t);

		// [green] component
		// ...if same, just skip
		if (from.green[i] != to.green[i]) 
			green[i] = uint16(float(from.green[i])*(1.0f - t) + float(to.green[i])*t);

		// [blue] component
		// ...if same, just skip
		if (from.blue[i] != to.blue[i])
            blue[i] = uint16(float(from.blue[i])*(1.0f - t) + float(to.blue[i])*t);
	}
}
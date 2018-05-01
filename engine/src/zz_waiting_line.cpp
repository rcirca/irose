/** 
 * @file zz_waiting_line.CPP
 * @brief waiting line mesh class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    27-oct-2003
 *
 * $Header: /engine/src/zz_waiting_line.cpp 10    04-08-11 6:20p Zho $
 * $History: zz_waiting_line.cpp $
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-08-11   Time: 6:20p
 * Updated in $/engine/src
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-06-22   Time: 9:51a
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-06-03   Time: 11:18a
 * Updated in $/engine/src
 * glow code updated.
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-04-30   Time: 12:10p
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-04-25   Time: 1:16p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-13   Time: 1:22a
 * Updated in $/engine/src
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-09   Time: 7:42p
 * Updated in $/engine/src
 * added thread-based texture loading and acceleration-based camera
 * following system(buggable)
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
 * User: Zho          Date: 03-11-30   Time: 7:49p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-11-30   Time: 7:12p
 * Updated in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * some type casting problem fixed
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_type.h"
#include "zz_log.h"
#include "zz_waiting_line.h"
#include "zz_node.h"

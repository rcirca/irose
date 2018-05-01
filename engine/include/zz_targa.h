/** 
 * @file zz_targa.h
 * @brief TGA file manipulation.
 * @author	Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date 18-apr-2002
 *
 * $Header: /engine/include/zz_targa.h 1     03-11-30 8:06p Zho $
 * $History: zz_targa.h $
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_TARGA_H__
#define __ZZ_TARGA_H__

// targa header
struct zz_targa_header {
	uchar id_length, colormap_type, image_type;
    uint16 colormap_index, colormap_length;
    uchar colormap_size;
    uint16 x_origin, y_origin, width, height;
    uchar pixel_size, attributes;
};

// "pixels" will be allocated in this function, and you have to free it manually later.
bool zz_targa_load (const char * name, byte **pixels, int * width, int * height, int * format);

#endif __ZZ_TARGA_H__
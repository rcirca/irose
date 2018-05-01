/** 
 * @file zz_targa.cpp
 * @brief TGA file manipulation.
 * @author	Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date 18-apr-2002
 *
 * $Header: /engine/src/zz_targa.cpp 4     04-02-11 2:05p Zho $
 * $History: zz_targa.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-02   Time: 9:20p
 * Updated in $/engine/src
 * do not use zz_vfs_pkg as defaulst filesystem 
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-02   Time: 6:28p
 * Updated in $/engine/src
 * In type.h, added "zz_vfs.h"
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
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "zz_mem.h"
#include "zz_type.h"
#include "zz_vfs.h"
#include "zz_log.h"
#include "zz_targa.h"

// TGA image file load
// name : filename, pixels : data
// width : TGA width, height : TGA height
// format : pixel size ( 24 or 32 )
bool zz_targa_load (const char * name, byte **pixels, int * width, int * height, int * format)
{
    int columns, rows, numPixels;
    byte * pixbuf;
    int row, column;
    char buf_char;

	zz_vfs fin;

    byte * targa_rgba;

    zz_targa_header targa_header;

    if (!fin.open(name, zz_vfs::ZZ_VFS_READ))
        return false;

	targa_header.id_length = fin.read_char(buf_char);
    targa_header.colormap_type = fin.read_char(buf_char);
	targa_header.image_type = fin.read_char(buf_char);
	fin.read_uint16(targa_header.colormap_index);
    fin.read_uint16(targa_header.colormap_length);
    targa_header.colormap_size = fin.read_char(buf_char);
    fin.read_uint16(targa_header.x_origin);
    fin.read_uint16(targa_header.y_origin);
    fin.read_uint16(targa_header.width);
    fin.read_uint16(targa_header.height);
    targa_header.pixel_size = fin.read_char(buf_char);
    targa_header.attributes = fin.read_char(buf_char);

    if (targa_header.image_type!=2 && targa_header.image_type!=10)
        ZZ_LOG("LoadTGA: Only type 2 and 10 targa RGB images supported\n");

    if (targa_header.colormap_type !=0 || (targa_header.pixel_size!=32 && targa_header.pixel_size!=24))
        ZZ_LOG("Texture_LoadTGA: Only 32 or 24 bit images supported (no colormaps)\n");

    *format = targa_header.pixel_size;

    columns = targa_header.width;
    rows = targa_header.height;
    numPixels = columns * rows;

    if (width)
        *width = columns;
    if (height)
        *height = rows;

    targa_rgba = zz_new byte[numPixels*4];
    //  targa_rgba = new unsigned char[numPixels*4];
    *pixels = targa_rgba;

    if (targa_header.id_length != 0)
		fin.seek(targa_header.id_length, zz_vfs::ZZ_VFS_SEEK_CUR); // skip TARGA image comment

    if (targa_header.image_type==2) {  // Uncompressed, RGB images
        for(row=rows-1; row>=0; row--) {
            pixbuf = targa_rgba + row*columns*4;
            for(column=0; column<columns; column++) {
                uchar red,green,blue,alphabyte;
                switch (targa_header.pixel_size) {
                    case 24:
                        fin.read_uchar(blue);
                        fin.read_uchar(green);
                        fin.read_uchar(red);
                        *pixbuf++ = red;
                        *pixbuf++ = green;
                        *pixbuf++ = blue;
                        *pixbuf++ = 255;
                        break;
                    case 32:
                        fin.read_uchar(blue);
                        fin.read_uchar(green);
                        fin.read_uchar(red);
                        fin.read_uchar(alphabyte);
                        *pixbuf++ = red;
                        *pixbuf++ = green;
                        *pixbuf++ = blue;
                        *pixbuf++ = alphabyte;
                        break;
                }
            }
        }
    }
    else if (targa_header.image_type==10) {   // Runlength encoded RGB images
        uchar red,green,blue,alphabyte,packetHeader,packetSize,j;
        for(row=rows-1; row>=0; row--) {
            pixbuf = targa_rgba + row*columns*4;
            for(column=0; column<columns; ) {
                fin.read_uchar(packetHeader);
                packetSize = 1 + (packetHeader & 0x7f);
                if (packetHeader & 0x80) {        // run-length packet
                    switch (targa_header.pixel_size) {
                        case 24:
							fin.read_uchar(blue);
							fin.read_uchar(green);
							fin.read_uchar(red);
                            alphabyte = 255;
                            break;
                        case 32:
                            fin.read_uchar(blue);
							fin.read_uchar(green);
							fin.read_uchar(red);
                            fin.read_uchar(alphabyte);
                            break;
                    }
                    for(j=0;j<packetSize;j++) {
                        *pixbuf++=red;
                        *pixbuf++=green;
                        *pixbuf++=blue;
                        *pixbuf++=alphabyte;
                        column++;
                        if (column==columns) { // run spans across rows
                            column=0;
                            if (row>0)
                                row--;
                            else
                                goto breakOut;
                            pixbuf = targa_rgba + row*columns*4;
                        }
                    }
                }
                else {                            // non run-length packet
                    for(j=0;j<packetSize;j++) {
                        switch (targa_header.pixel_size) {
                            case 24:                                                        
								fin.read_uchar(blue);
								fin.read_uchar(green);
								fin.read_uchar(red);
								*pixbuf++ = red;
                                *pixbuf++ = green;
                                *pixbuf++ = blue;
                                *pixbuf++ = 255;
                                break;
                            case 32:
								fin.read_char((char&)blue);
								fin.read_char((char&)green);
								fin.read_char((char&)red);
								fin.read_char((char&)alphabyte);
                                *pixbuf++ = red;
                                *pixbuf++ = green;
                                *pixbuf++ = blue;
                                *pixbuf++ = alphabyte;
                                break;
                        }
                        column++;
                        if (column==columns) { // pixel packet run spans across rows
                            column=0;
                            if (row>0)
                                row--;
                            else
                                goto breakOut;
                            pixbuf = targa_rgba + row*columns*4;
                        }                                               
                    }
                }
            }
breakOut:;
        }
    }
    fin.close();
    return true;
}

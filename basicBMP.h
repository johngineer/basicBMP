/************************************************
*
*   Basic BMP Library v0.1
*
*   A library for basic loading/saving and
*   manipulating bitmaps and .BMP files.
*
*   current BMP versions supported:
*   1-bit monochrome, 8-bit greyscale/palette,
*   and 24-bit RGB.
*
*   J. M. De Cristofaro, March 2015
*
*   BSD License
*
*   Text above must be included in all
*   redistributions
*
************************************************/

#ifndef BASICBMP_H_INCLUDED
#define BASICBMP_H_INCLUDED

#define BASICBMP_VERSION            0.1

#include <iostream>
#include <fstream>
#include <string>
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>


#define BMPHEADERSIZE               54
#define BMP_MAGICNUMBER             0x4D42
#define BMP_V3HEADERSIZE            40

#define BMPHEADER_MAGICNUMOFFSET    0
#define BMPHEADER_FILESIZEOFFSET    2
#define BMPHEADER_RESERVED1         6
#define BMPHEADER_RESERVED2         8
#define BMPHEADER_IMGDATAOFFSET     10
#define BMPHEADER_DIBHEADEROFFSET   14
#define BMPHEADER_IMGWIDTHOFFSET    18
#define BMPHEADER_IMGHEIGHTOFFSET   22
#define BMPHEADER_COLORPLNOFFSET    26
#define BMPHEADER_BPPOFFSET         28
#define BMPHEADER_COMPRESSION       30
#define BMPHEADER_IMGSIZEOFFSET     34
#define BMPHEADER_HRESOFFSET        38
#define BMPHEADER_VRESOFFSET        42
#define BMPHEADER_PALETTESIZE       46
#define BMPHEADER_IMPORTANTCOLOR    50

#define RED                         2
#define GREEN                       1
#define BLUE                        0

using namespace std;

// types of BMP formats
// 1-bit monochrome
// 8-bit palettized
// 24-bit RGB color (8 bits per channel)
enum bmp_format { color1bit, color8bit, color24bit };

// pixel channels
typedef enum rgbchan
    { red = 2,
      green = 1,
      blue = 0
    };

enum verbose { off, on, all };

// structure for a BMP file holding all the useful info
// data: the raw bitmap data, in msb->BGR<-lsb format
// header: the header block, which is parsed for other info about
// the file.
// format (see above): the color depth & format of the file
// palette: an array of values for palletized and greyscale images
// height, width and total size (in pixels) of the image itself
// hres, vres = resolution of the image in pixels per meter
struct BMP_struct {
    //string filename;
    uint8_t* data;
    //uint8_t* header;
    //bmp_format format;
    //uint8_t* palette;
    uint32_t height, width;
    //uint32_t imagesize, hres, vres;
    uint32_t hres, vres;
    //uint32_t palettesize;
    uint16_t bpp;
};

// a class of BMP file operations
class BMP_file_ops {
    public:

        // returns size of bitmap if successful, -1 if invalid bitmap or disk error
        int32_t load(char* srcfilename, BMP_struct &bitmap);

        // returns size of file if successful, -1 if invalid bitmap or disk error
        int32_t save(char* destfilename, BMP_struct &bitmap);

    private:

        // reads value of (size <= 4) bytes from (offset)
        uint32_t read_from_offset(uint8_t data[], uint16_t offset, uint16_t size);

        // reads (value) of (size <= 4) bytes from (offset)
        uint32_t store_from_offset(uint8_t data[], uint32_t value, uint16_t offset, uint16_t size);

        uint32_t padded_row_size(uint32_t w);
};

// a class of bitmap data operations
class BMP_data_ops {
    public:
        uint8_t getpixel(BMP_struct &bitmap, uint16_t x, uint16_t y, rgbchan color);
        uint8_t putpixel(BMP_struct &bitmap, uint16_t x, uint16_t y, rgbchan color, uint8_t val);
        void init(BMP_struct &bitmap, uint16_t w, uint16_t h, uint16_t bpp);
        void clear(BMP_struct &bitmap);
        int8_t print(BMP_struct &bitmap);
        void fill(BMP_struct &bitmap, rgbchan color, uint8_t val);
        void invert(BMP_struct &bitmap, rgbchan color);
        void clone(BMP_struct &source, BMP_struct &target);

};

#endif // BASICBMP_H_INCLUDED

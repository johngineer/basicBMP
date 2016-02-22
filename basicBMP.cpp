/************************************************
*
*   Basic BMP Library v0.1
*
*   A library for basic loading/saving and
*   manipulating bitmaps and .BMP files.
*
*   current BMP versions supported:
*   v3 header, 24-bit RGB uncompressed
*
*   J. M. De Cristofaro, March 2015
*
*   BSD License
*
*   Text above must be included in all
*   redistributions
*
************************************************/

#include "basicBMP.h"

using namespace std;

uint32_t BMP_file_ops::read_from_offset(uint8_t data[], uint16_t offset, uint16_t size)
{
    uint32_t temp = 0;

    if (size > 4) { return 0; }
    else
    {
        for (uint16_t cp = 0; cp < size; cp++)
        {
            temp += (data[offset + cp]) << (8 * cp);
        }
    }
    return temp;
}

uint32_t BMP_file_ops::store_from_offset(uint8_t data[], uint32_t value, uint16_t offset, uint16_t size)
{
    uint32_t temp = 0;

    if (size > 4) { return 0; }
    else
    {
        for (uint16_t cp = 0; cp < size; cp++)
        {
            data[offset + cp] = ((value >> (8 * cp)) & 0xFF);
        }
    }
    return temp;
}

uint32_t BMP_file_ops::padded_row_size(uint32_t w)
{
    if (((w*3) % 4) == 0) { return w*3; }
    else { return ((4-((w*3) % 4)) + (w*3)); }
}

int32_t BMP_file_ops::load(char* srcfilename, BMP_struct &bitmap, uint8_t verbose = 0)
{
    int32_t bitmap_filesize, bitmap_location;
    //int32_t bitmap_width_rgb;
    int16_t bitmap_magicnumber;
    uint32_t bitmap_dibheadersize;
    int32_t w_temp, h_temp;
    uint8_t rowpad;
    uint32_t tempdata_size;

    uint8_t header[BMPHEADERSIZE];
    //uint8_t* tempdata;


    //uint32_t bitmap_data_size;
    //FILE* source;
    //ifstream source;

    if (verbose > 0)
    {
        printf(ANSI_COLOR_RED "\nOPENING: " ANSI_COLOR_CYAN "%s\n\n" ANSI_COLOR_RESET, srcfilename);
    }
    //source = fopen(srcfilename, "r+b");
    ifstream source(srcfilename, ios::binary);

    //bitmap.filename = srcfilename;

    // read in header info, configure bitmap structure
    source.read((char*)header, BMPHEADERSIZE);

    // check for BM magic number
    bitmap_magicnumber = (uint16_t)read_from_offset(header, BMPHEADER_MAGICNUMOFFSET, 2);
    if (bitmap_magicnumber != BMP_MAGICNUMBER) { return -1; }

    // read in filesize and image data offset
    bitmap_filesize = read_from_offset(header, BMPHEADER_FILESIZEOFFSET, 4);
    bitmap_location = read_from_offset(header, BMPHEADER_IMGDATAOFFSET, 4);

    // check that file is using BMP_V3 header
    bitmap_dibheadersize = read_from_offset(header, BMPHEADER_DIBHEADEROFFSET, 4);
    if (bitmap_dibheadersize != BMP_V3HEADERSIZE) { return -1; }

    // read in width and height. these fields can be negative for some reason
    w_temp = read_from_offset(header, BMPHEADER_IMGWIDTHOFFSET, 4);
    if (w_temp < 0) { bitmap.width = (w_temp * (-1)); }
    else { bitmap.width = (uint32_t)w_temp; }

    h_temp = read_from_offset(header, BMPHEADER_IMGHEIGHTOFFSET, 4);
    if (h_temp < 0) { bitmap.height = (h_temp * (-1)); }
    else { bitmap.height = (uint32_t)h_temp; }

    // check pixel depth = 24bpp (basicBMP v0.1 only reads 24bit file)
    bitmap.bpp = (uint16_t)read_from_offset(header, BMPHEADER_BPPOFFSET, 2);
    if (bitmap.bpp != 24) { return -1; }

    // read in vertical and horizontal resolution (pixels per meter)
    // conversion factor 1ppm = 39.37ppi
    bitmap.vres = read_from_offset(header, BMPHEADER_VRESOFFSET, 4);
    bitmap.hres = read_from_offset(header, BMPHEADER_HRESOFFSET, 4);

    // ignore palette crap for now.

    if (verbose > 1) {
        printf(ANSI_COLOR_YELLOW "bitmap size:" ANSI_COLOR_CYAN "        %8d total bytes\n", bitmap_filesize);
        printf(ANSI_COLOR_YELLOW "bitmap location:" ANSI_COLOR_CYAN "      0x%04x bytes\n", bitmap_location);
        printf(ANSI_COLOR_YELLOW "bitmap width:" ANSI_COLOR_CYAN "       %8d pixels\n", bitmap.width);
        printf(ANSI_COLOR_YELLOW "bitmap height:" ANSI_COLOR_CYAN "      %8d pixels\n", bitmap.height);
        printf(ANSI_COLOR_YELLOW "hor-> res.:" ANSI_COLOR_CYAN "         %8d ppi\n", (bitmap.hres / 39));
        printf(ANSI_COLOR_YELLOW "vert. res.:" ANSI_COLOR_CYAN "         %8d ppi\n", (bitmap.vres / 39));
        printf(ANSI_COLOR_YELLOW "color depth:" ANSI_COLOR_CYAN "        %8d bpp\n\n\n", bitmap.bpp);
        printf(ANSI_COLOR_RESET);
    }


    if ((bitmap.width % 4) == 0) { rowpad = 0; }
    else { rowpad = (4-((bitmap.width*3) % 4)); }

    tempdata_size = (bitmap.width*3) + rowpad;

    if (verbose > 1) {
        printf(ANSI_COLOR_YELLOW "row padding: " ANSI_COLOR_MAGENTA " %d\n", rowpad);
        printf(ANSI_COLOR_YELLOW " array size: " ANSI_COLOR_MAGENTA " %d bytes\n", tempdata_size);
        printf(ANSI_COLOR_RESET);
    }

    bitmap.data = new uint8_t[(bitmap.height * bitmap.width * 3)];

    uint8_t tempdata[tempdata_size];

    source.seekg(bitmap_location, ios::beg);

    for (uint32_t y = bitmap.height; y > 0; y--)
    {
        source.read((char*)tempdata, tempdata_size);
        for (uint32_t x = 0; x < (bitmap.width*3); x++)
        {
            bitmap.data[(y-1)*(bitmap.width*3) + x] = tempdata[x];
        }
    }

    if (verbose > 0) { printf(ANSI_COLOR_RED "\nCLOSING: " ANSI_COLOR_CYAN "%s\n\n" ANSI_COLOR_RESET, srcfilename); }
    source.close();

    //return (bitmap.width * bitmap.height);
    return (bitmap_filesize);
}

int32_t BMP_file_ops::save(char* destfilename, BMP_struct &bitmap)
{
    uint32_t tempdata_size = 0;
    uint8_t header[BMPHEADERSIZE];
    uint8_t rowpad;
    uint32_t temp32;

    // write magic number
    store_from_offset(header, BMP_MAGICNUMBER, BMPHEADER_MAGICNUMOFFSET, 2);

    // write BMP file size
    temp32 = padded_row_size(bitmap.width) * bitmap.height;
    temp32 += BMPHEADERSIZE + 2;
    store_from_offset(header, temp32, BMPHEADER_FILESIZEOFFSET, 4);

    // write dummy zeroes
    store_from_offset(header, 0, BMPHEADER_RESERVED1, 2);
    store_from_offset(header, 0, BMPHEADER_RESERVED2, 2);

    // write starting address for BMP data
    store_from_offset(header, BMPHEADERSIZE, BMPHEADER_IMGDATAOFFSET, 4);

    // write v3 header size
    store_from_offset(header, BMP_V3HEADERSIZE, BMPHEADER_DIBHEADEROFFSET, 4);

    // write width
    store_from_offset(header, bitmap.width, BMPHEADER_IMGWIDTHOFFSET, 4);

    // write height
    store_from_offset(header, bitmap.height, BMPHEADER_IMGHEIGHTOFFSET, 4);

    // write color planes
    store_from_offset(header, 1, BMPHEADER_COLORPLNOFFSET, 2);

    // write bits per pixel
    store_from_offset(header, bitmap.bpp, BMPHEADER_BPPOFFSET, 2);

    // write compression method (using 0 for uncompressed RGB)
    store_from_offset(header, 0, BMPHEADER_COMPRESSION, 4);

    // write raw bitmap size
    store_from_offset(header, padded_row_size(bitmap.width)*bitmap.height, BMPHEADER_IMGSIZEOFFSET, 4);

    // write hres and vres
    store_from_offset(header, bitmap.hres, BMPHEADER_HRESOFFSET, 4);
    store_from_offset(header, bitmap.vres, BMPHEADER_VRESOFFSET, 4);
    // write number of palette colors
    store_from_offset(header, 0, BMPHEADER_PALETTESIZE, 4);
    store_from_offset(header, 0, BMPHEADER_PALETTESIZE + 4, 4);

    // create bitmap row padded with zeroes for rows not a multiple of 4.
    rowpad = (bitmap.width * 3) % 4;
    tempdata_size = (bitmap.width * 3);
    if (rowpad > 0)
    {
        tempdata_size += (4-rowpad);
    }

    uint8_t tempdata[tempdata_size];

    #ifdef DEBUG
    cout << "Opening " << destfilename << " for saving...";
    #endif // DEBUG
    ofstream dest(destfilename, ios::binary);

    //write header to file
    dest.write((char*)header, BMPHEADERSIZE);

    for (uint16_t y = (bitmap.height); y > 0; y--)
    {
        for (uint16_t x = 0; x < tempdata_size; x++)
        {
            if (x < (bitmap.width*3))
            {
                tempdata[x] = bitmap.data[((y-1) * (bitmap.width * 3)) + x];
            }
            else
            {
                tempdata[x] = 0;
            }
        }
        dest.write((char*)tempdata, tempdata_size);
        #ifdef DEBUG
        cout << ".";
        #endif // DEBUG
    }
    #ifdef DEBUG
    cout << "\n";
    cout << "Closing " << destfilename << "\n";
    #endif // DEBUG
    dest.close();

    return 0;
}

uint32_t BMP_data_ops::height(BMP_struct &bitmap)
{
    return bitmap.height;
}

uint32_t BMP_data_ops::width(BMP_struct &bitmap)
{
    return bitmap.width;
}

uint8_t BMP_data_ops::putpixel(BMP_struct &bitmap, uint16_t x, uint16_t y, rgbchan color, uint8_t val)
{
    if (x >= bitmap.width) { return 1; }
    else if (y >= bitmap.height) { return 1; }
    else
    {
        (bitmap.data[((y * bitmap.width) + x)*3 + color]) = val;
        return 0;
    }
}

uint8_t BMP_data_ops::putpixel(BMP_struct &bitmap, uint16_t x, uint16_t y, rgbpixel pixel)
{
    if (x >= bitmap.width) { return 1; }
    else if (y >= bitmap.height) { return 1; }
    else
    {
        (bitmap.data[((y * bitmap.width) + x)*3 + BLUE]) = pixel.b;
        (bitmap.data[((y * bitmap.width) + x)*3 + GREEN]) = pixel.g;
        (bitmap.data[((y * bitmap.width) + x)*3 + RED]) = pixel.r;
        return 0;
    }
}

uint8_t BMP_data_ops::getpixel(BMP_struct &bitmap, uint16_t x, uint16_t y, rgbchan color)
{
    if (x >= bitmap.width) { return 0; }
    else if (y >= bitmap.height) { return 0; }
    else { return (bitmap.data[((y * bitmap.width) + x)*3 + color]); }
}

rgbpixel BMP_data_ops::getpixel(BMP_struct &bitmap, uint16_t x, uint16_t y)
{
    rgbpixel temp;
    temp.r = 0;
    temp.g = 0;
    temp.b = 0;
    if (x >= bitmap.width) { return temp; }
    else if (y >= bitmap.height) { return temp; }
    else {
        temp.b = (bitmap.data[((y * bitmap.width) + x)*3 + BLUE]);
        temp.g = (bitmap.data[((y * bitmap.width) + x)*3 + GREEN]);
        temp.r = (bitmap.data[((y * bitmap.width) + x)*3 + RED]);
        return temp;
    }
}

void BMP_data_ops::init(BMP_struct &bitmap, uint16_t w, uint16_t h, uint16_t bpp)
{
    uint32_t bitmap_size;

    bitmap.width = w;
    bitmap.height = h;

    bitmap_size = (w * h);

    bitmap.bpp = bpp;

    bitmap.hres = 300 * 39.37;
    bitmap.vres = 300 * 39.37;
    bitmap.data = new uint8_t[(bitmap_size * 3)];
}

void BMP_data_ops::clear(BMP_struct &bitmap)
{
    bitmap.width = 0;
    bitmap.height = 0;
    bitmap.hres = 300;
    bitmap.vres = 300;
    delete [] bitmap.data;
}

int8_t BMP_data_ops::print(BMP_struct &bitmap)
{
    //rgbchan color;
    if (bitmap.height > 16 || bitmap.width > 16) { return -1; }

    for (uint8_t y = 0; y < bitmap.height; y++)
    {
        printf("%04X:: ", (y*bitmap.width));
        for (uint8_t x = 0; x < bitmap.width; x++)
        {
            printf("%02X.", getpixel(bitmap, x, y, blue));
            printf("%02X.", getpixel(bitmap, x, y, green));
            printf("%02X ", getpixel(bitmap, x, y, red));
        }
        printf("\n");
    }
    printf("\n");

    return 0;
}

void BMP_data_ops::fill(BMP_struct &bitmap, rgbchan color, uint8_t val)
{
    for (uint16_t y = 0; y < bitmap.height; y++)
    {
        for (uint16_t x = 0; x < bitmap.width; x++)
        {
            bitmap.data[((y * bitmap.width) + x)*3 + color] = val;
        }
    }
}

void BMP_data_ops::invert(BMP_struct &bitmap, rgbchan color)
{
    uint8_t temp = 0;
    for (uint32_t y = 0; y < bitmap.width; y++)
    {
        for (uint32_t x = 0; x < bitmap.height; x++)
        {
            temp = getpixel(bitmap, x, y, color) ^ 0xFF;
            putpixel(bitmap, x, y, color, temp);
        }
    }
}

void BMP_data_ops::clone(BMP_struct &source, BMP_struct &target)
{
    init(target, source.width, source.height, source.bpp);
    for (uint32_t cp = 0; cp < source.width*target.height*3; cp++)
    {
        target.data[cp] = source.data[cp];
    }
}

//        // pastes source on to destination
//        void overlay(BMP_struct &source, BMP_struct &destination, uint16_t x, uint8_t y);
//
//        // pastes source on to destination, but does not paste in areas where source = mask_color
//        void mask(BMP_struct &source, BMP_struct &destination, uint16_t x, uint16_t y, rgbpixel mask_color);
//
//        // adds source to destination. values are clipped at 0xFF
//        void add(BMP_struct &source, BMP_struct &destination, uint16_t x, uint16_t y);

bool BMP_data_ops::same_color(rgbpixel a, rgbpixel b)
{
    if ((a.r == b.r) && (a.g == b.g) && (a.b == b.b)) { return true; }
    else { return false; }
}

rgbpixel BMP_data_ops::add_pixel(rgbpixel a, rgbpixel b)
{
    rgbpixel temp;
    uint16_t temp16;

    temp16 = a.r + b.r;
    if (temp16 > 0xFF) { temp.r = 0xFF; }
    else { temp.r = (uint8_t)temp16; }

    temp16 = a.g + b.g;
    if (temp16 > 0xFF) { temp.g = 0xFF; }
    else { temp.g = (uint8_t)temp16; }

    temp16 = a.b + b.b;
    if (temp16 > 0xFF) { temp.b = 0xFF; }
    else { temp.b = (uint8_t)temp16; }

    return temp;
}

uint8_t BMP_data_ops::overlay(BMP_struct &source, BMP_struct &destination, uint16_t x, uint8_t y)
{
    uint16_t x_terminus, y_terminus;
    rgbpixel temp;
    uint8_t return_val = 0;

    x_terminus = x + source.width;
    y_terminus = y + source.height;

    if (x_terminus > source.width)
    {
        x_terminus = source.width;
        return_val = 1;
    }
    if (y_terminus > source.height)
    {
        y_terminus = source.height;
        return_val = 1;
    }

    for (uint16_t row = y; row < y_terminus; row++)
    {
        for (uint16_t col = x; col < x_terminus; col++)
        {
            temp = getpixel(source, row - x, col - y);
            putpixel(destination, row, col, temp);
        }
    }

    return return_val;
}

uint8_t BMP_data_ops::mask(BMP_struct &source, BMP_struct &destination, uint16_t x, uint16_t y, rgbpixel mask_color)
{
    uint16_t x_terminus, y_terminus;
    rgbpixel temp;
    uint8_t return_val = 0;

    x_terminus = x + source.width;
    y_terminus = y + source.height;

    if (x_terminus > source.width)
    {
        x_terminus = source.width;
        return_val = 1;
    }
    if (y_terminus > source.height)
    {
        y_terminus = source.height;
        return_val = 1;
    }

    for (uint16_t row = y; row < y_terminus; row++)
    {
        for (uint16_t col = x; col < x_terminus; col++)
        {
            temp = getpixel(source, row - x, col - y);
            if (!same_color(temp, mask_color)) { putpixel(destination, row, col, temp); }
        }
    }

    return return_val;
}

uint8_t BMP_data_ops::add(BMP_struct &source, BMP_struct &destination, uint16_t x, uint16_t y)
{
    uint16_t x_terminus, y_terminus;
    rgbpixel temp1, temp2;
    uint8_t return_val = 0;

    x_terminus = x + source.width;
    y_terminus = y + source.height;

    if (x_terminus > source.width)
    {
        x_terminus = source.width;
        return_val = 1;
    }
    if (y_terminus > source.height)
    {
        y_terminus = source.height;
        return_val = 1;
    }

    for (uint16_t row = y; row < y_terminus; row++)
    {
        for (uint16_t col = x; col < x_terminus; col++)
        {
            temp1 = getpixel(source, row - x, col - y);
            temp2 = getpixel(destination, row, col);
            putpixel(destination, row, col, add_pixel(temp1, temp2));
        }
    }

    return return_val;
}

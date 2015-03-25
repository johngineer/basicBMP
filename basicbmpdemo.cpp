/******************************************************************************
*
*   Basic BMP Library Demo
*
*   Inputs a BMP file, inverts the green channel
*   and outputs it.
*
*   Usage: basicbmpdemo.exe <input file> <output file>
*
*   For Basic BMP Library v0.1
*
*   J. M. De Cristofaro, March 2015
*
*   BSD License
*
*   Text above must be included in all redistributions
*
******************************************************************************/

#include "lib\basicBMP.h"

using namespace std;

BMP_file_ops bmpfile;
BMP_data_ops bmpdata;

int main(int argc, char *argv[])
{
    BMP_struct my_bitmap;

    bmpfile.load(argv[1], my_bitmap);
    bmpdata.invert(my_bitmap, blue);
    bmpfile.save(argv[2], my_bitmap);
    bmpdata.clear(my_bitmap);

    return 0;
}

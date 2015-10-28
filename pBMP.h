#ifndef PBMP_H
#define PBMP_H
#include <iostream>
#include <cmath>

using BYTE = unsigned char;
using WORD = unsigned short;
using DWORD = unsigned int;
using LONG = unsigned int;

// const BYTE value 255 for upper limit of the color value
const BYTE CLAMP = (BYTE)~0;

const double PI = acos(-1);

/**
 * Header block of .bmp file, at the first parts
 */
class BitmapFileHeader {

public:
    /**
     * display infomation
     * @author piratf
     */
    void display();

    // WORD bfType; // must be 0x4d42 for checking real type of input file
    DWORD bfSize;   // size of .bmp file
    WORD bfReserved1;   // reserved, skipped
    WORD bfReserved2;   // reserved, skipped
    DWORD bfOffBits;    // actual offset bytes
};

/**
 * info of the picture, the second part
 */
class BitmapFileInfoHeader {

public:
    /**
     * display infomation
     * @author piratf
     */
    void display();

    DWORD biSize;       // must be 40, the size of info header.
    LONG biWidth;       // width of the picture, pixel for unit
    LONG biHeight;      // height of the picture, pixel for unit
    WORD biPlanes;      // must be 1, the count of planes.
    WORD biBitCount;    // count for bits of each pixel, 1 for black-white, n for 2 ^ n different colors picture, now it could be 32 for the most large oe.
    DWORD biCompression;    // type of compression, constant value in windows: BI_RGB，BI_RLE8，BI_RLE4，BI_BITFIELDS, the first one means no compression
    DWORD biSizeImage;      // size of bytes of the compression image
    LONG biXPelsPerMeter;   // pixels per meter in horizontal
    LONG biYPelsPerMeter;   // pixels per meter in vertical
    DWORD biClrUsed;        // use RGB pad
    DWORD biClrImportant;   // the count for important colors in the picture, 0 means all colors are improtant
};

/** RGB palette
 * for some of the .BMP file
 */
class RGBQuad {

public:
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
};

/**
 * data of the each pixel.
 */
struct ImgData
{
    BYTE red;
    BYTE green;
    BYTE blue;

    ImgData(): red(0), green(0), blue(0) {
    }

    ImgData(BYTE _red, BYTE _green, BYTE _blue) {
        red = _red;
        green = _green;
        blue = _blue;
    }

    /**
     * use int value to initialize ImgData object.
     */
    ImgData(int color): red(color >> 16), green(color >> 8), blue(color) {
    }

    ImgData operator += (const ImgData &other) {
        red += other.red;
        green += other.green;
        blue += other.blue;
        return *this;
    }

    ImgData operator / (const long &other) {
        red /= other;
        green /= other;
        blue /= other;
        return *this;
    }

    operator int() {
        return (0 | (red << 16) | (green << 8) || (blue));
    }

    void clear() {
        red = green = blue = 0;
    }
};

class pBMP
{
public:

    pBMP operator = (const pBMP &other) {
        fileHeader = other.fileHeader;
        infoHeader = other.infoHeader;
        quad = nullptr;
        imgData = nullptr;
        height = other.height;
        width = other.width;
        blockSize = other.blockSize;
        loadSuccess = other.loadSuccess;
        return *this;
    }

    /**
     * print infomation of this picture
     * @author piratf
     */
    void display();

    /**
     * read palette from file pointer
     * @author piratf
     * @param  quad       palette array
     * @param  infoHeader BigmapFileInfoHeader object
     * @param  fpi        file stream pointer
     */
    void readQuad(RGBQuad *quad, BitmapFileInfoHeader &infoHeader, FILE *fpi);

    /**
     * write palette to file
     * @author piratf
     * @param  strPla     palette array
     * @param  infoHeader BigmapFileInfoHeader object
     * @param  fpi        file stream pointer
     */
    void writeQuad(RGBQuad *strPla, BitmapFileInfoHeader &infoHeader, FILE *fpi);

    /**
     * read .bmp file, fill data of current pBMP object;
     * @author piratf
     * @param  filePath path to the source file
     * @return          0 - success : -1 :failed
     */
    int read(const char *filePath);

    /**
     * write pBMP to .bmp file
     * @author piratf
     * @param  filePath path of .bmp file
     * @return          0 - success : -1 - failed
     */
    int write(const char* filePath);

    /**
     * rotate the img and return a new one.
     * @author piratf
     * @param  targetAngle angle between two img
     * @return             new pBMP object
     */
    pBMP rot(double targetAngle);

    /**
     * one dimensional gaussian blur algorithm
     * @author piratf
     * @param  radius radius of the gaussian mask, could affect the running time
     * @return        a new pBMP object will be return
     */
    pBMP blur(const long radius);


    pBMP zoom(LONG targetHeight, LONG targetWidth);

    ~pBMP() {
        delete [] quad;
        delete [] imgData;
    }

    static const WORD TYPE_IDENTIFIER = 0x4d42;
    BitmapFileHeader fileHeader;
    BitmapFileInfoHeader infoHeader;
    RGBQuad *quad;
    ImgData *imgData;
    LONG height;
    LONG width;
    unsigned long blockSize;
    bool loadSuccess = 0;
};

#endif
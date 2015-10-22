#ifndef PBMP_H
#define PBMP_H
#include <iostream>
#include <cmath>

using BYTE = unsigned char;
using WORD = unsigned short;
using DWORD = unsigned int;
using LONG = long;

const BYTE CLAMP = (BYTE)~0;

const double PI = acos(-1);

//位图文件头定义;
//其中不包含文件类型信息（由于结构体的内存结构决定，
//要是加了的话将不能正确读取文件信息）
class BitmapFileHeader {

public:
    /**
     * display infomation
     * @author piratf
     */
    void display();

    // WORD bfType;//文件类型，必须是0x424D，即字符“BM”
    DWORD bfSize;//文件大小
    WORD bfReserved1;//保留字
    WORD bfReserved2;//保留字
    DWORD bfOffBits;//从文件头到实际位图数据的偏移字节数
};

class BitmapFileInfoHeader {

public:
    /**
     * display infomation
     * @author piratf
     */
    void display();

    DWORD biSize;       //信息头大小, 需要是40
    LONG biWidth;       //图像宽度, 单位是象素。
    LONG biHeight;      //图像高度, 单位是象素。
    WORD biPlanes;      //位平面数, 必须为1
    WORD biBitCount;    //每像素位数, 常用的值为1(黑白二色图), 4(16色图), 8(256色), 24(真彩色图)(新的.bmp格式支持32位色)
    DWORD biCompression;    //压缩类型, 指定位图是否压缩，有效的值为BI_RGB，BI_RLE8，BI_RLE4，BI_BITFIELDS
    DWORD biSizeImage;      //压缩图像大小字节数
    LONG biXPelsPerMeter;   //水平分辨率, 单位是每米的象素个数
    LONG biYPelsPerMeter;   //垂直分辨率, 单位是每米的象素个数
    DWORD biClrUsed;        //位图实际用到的色彩数. 如果该值为零, 则用到的颜色数为2 ^ biBitCount
    DWORD biClrImportant;   //本位图中重要的色彩数, 如果该值为零，则认为所有的颜色都是重要的。
}; //位图信息头定义

/** 调色板
 * 有些位图文件需要用到调色板
 * 而对于真彩色图等较大的图，不需要调色板
 */
class RGBQuad {

public:
    BYTE rgbBlue;   // 该颜色的蓝色分量
    BYTE rgbGreen;  // 该颜色的绿色分量
    BYTE rgbRed;    // 该颜色的红色分量
    BYTE rgbReserved;   // 保留值
}; // 调色板定义

// 像素信息
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
        return *this;
    }

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

    pBMP blur(const long radius);

    pBMP gaussBlur_2 (long radius);

    ~pBMP() {
        delete [] quad;
        delete [] imgData;
    }

    static const WORD TYPE_IDENTIFIER = 0x4d42;
    BitmapFileHeader fileHeader;
    BitmapFileInfoHeader infoHeader;
    RGBQuad *quad;
    ImgData *imgData;
    unsigned long height;
    unsigned long width;
    unsigned long blockSize;
private:
    void boxBlur_2 (ImgData *blurData, long radius);
};

#endif
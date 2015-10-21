#include <iostream>
#include <cmath>
#include <cstring>
#include <cassert>

using BYTE = unsigned char;
using WORD = unsigned short;
using DWORD = unsigned int;
using LONG = long;

const double PI = acos(-1);

//位图文件头定义;
//其中不包含文件类型信息（由于结构体的内存结构决定，
//要是加了的话将不能正确读取文件信息）
class BitmapFileHeader {

public:
    //显示位图文件头信息
    void display() {
        std::cout << "位图文件头: " << std::endl;
        std::cout << "文件大小: " << bfSize << std::endl;
        std::cout << "保留字_1: " << bfReserved1 << std::endl;
        std::cout << "保留字_2: " << bfReserved2 << std::endl;
        std::cout << "实际位图数据的偏移字节数: " << bfOffBits << std::endl << std::endl;
    }

    // WORD bfType;//文件类型，必须是0x424D，即字符“BM”
    DWORD bfSize;//文件大小
    WORD bfReserved1;//保留字
    WORD bfReserved2;//保留字
    DWORD bfOffBits;//从文件头到实际位图数据的偏移字节数
};

struct BitmapFileInfoHeader {

public:
    void display() {
        std::cout << "============位图信息头: ============" << std::endl;
        std::cout << "biSize - 结构体的长度: " << biSize << std::endl;
        std::cout << "biWidth - 位图宽: " << biWidth << std::endl;
        std::cout << "biHeight - 位图高: " << biHeight << std::endl;
        std::cout << "biPlanes - 平面数: " << biPlanes << std::endl;
        std::cout << "biBitCount - 采用颜色位数: " << biBitCount << std::endl;
        std::cout << "biCompression - 压缩方式: " << biCompression << std::endl;
        std::cout << "biSizeImage实际位图数据占用的字节数: " << biSizeImage << std::endl;
        std::cout << "biXPelsPerMeter - X方向分辨率: " << biXPelsPerMeter << std::endl;
        std::cout << "biYPelsPerMeter - Y方向分辨率: " << biYPelsPerMeter << std::endl;
        std::cout << "biClrUsed - 使用的颜色数: " << biClrUsed << std::endl;
        std::cout << "biClrImportant - 重要颜色数: " << biClrImportant << std::endl;
    }

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
struct RGBQuad {
    BYTE rgbBlue;   //该颜色的蓝色分量
    BYTE rgbGreen;  //该颜色的绿色分量
    BYTE rgbRed;    //该颜色的红色分量
    BYTE rgbReserved;   //保留值
}; //调色板定义

//像素信息
struct ImgData
{
    BYTE red;
    BYTE green;
    BYTE blue;
};

class pBMP
{
public:

    //读取调色板
    void readQuad(RGBQuad *quad, BitmapFileInfoHeader &infoHeader, FILE *fpi) {
        for (unsigned int nCounti = 0; nCounti < infoHeader.biClrUsed; nCounti++)
        {
            //存储的时候，一般去掉保留字rgbReserved
            fread((char *)&quad[nCounti].rgbBlue, 1, sizeof(BYTE), fpi);
            fread((char *)&quad[nCounti].rgbGreen, 1, sizeof(BYTE), fpi);
            fread((char *)&quad[nCounti].rgbRed, 1, sizeof(BYTE), fpi);
        }
    }

    //写调色板到文件
    void writeQuad(RGBQuad *strPla, BitmapFileInfoHeader &infoHeader, FILE *fpi) {
        for (unsigned int nCounti = 0; nCounti < infoHeader.biClrUsed; nCounti++)
        {
            //存储的时候，一般去掉保留字rgbReserved
            fwrite((char *)&strPla[nCounti].rgbBlue, 1, sizeof(BYTE), fpi);
            fwrite((char *)&strPla[nCounti].rgbGreen, 1, sizeof(BYTE), fpi);
            fwrite((char *)&strPla[nCounti].rgbRed, 1, sizeof(BYTE), fpi);
        }
    }

    int read(const char *filePath) {
        FILE *fin = fopen(filePath, "rb");
        if (!fin) {
            std::cerr << "ERROR** imgFIleName -> can't read BMP file." << std::endl;
            return -1;
        }
        WORD fileType = 0;
        fread(&fileType, 1, sizeof(WORD), fin);
        // 读取文件结构头
        if (TYPE_IDENTIFIER == fileType) {
            fread(&fileHeader, 1, sizeof(BitmapFileHeader), fin);
            fread(&infoHeader, 1, sizeof(BitmapFileInfoHeader), fin);
            fileHeader.display();
            infoHeader.display();

            quad = nullptr;
            if (infoHeader.biClrUsed) {
                quad = new RGBQuad[256]; // 256 色调色板
                readQuad(quad, infoHeader, fin);
            }

            width = infoHeader.biWidth;
            height = infoHeader.biHeight;

            // 读入图像数据
            imgData = (ImgData*)malloc(sizeof(ImgData) * width * height);
            fread(imgData, sizeof(ImgData) * width, height, fin);
            fclose(fin);
        }
        else {
            std::cerr << "It is not a .bmp file" << std::endl;
            return -1;
        }
        return 0;
    }

    int write(const char* filePath) {
        FILE *fout = fopen(filePath, "wb");
        if (!fout) {
            std::cerr << "ERROR** fopen -> Can't create output file." << std::endl;
            return -1;
        }
        WORD fileType = TYPE_IDENTIFIER;
        fwrite(&fileType, 1, sizeof(WORD), fout);
        fwrite(&fileHeader, 1, sizeof(BitmapFileHeader), fout);
        fwrite(&infoHeader, 1, sizeof(BitmapFileInfoHeader), fout);
        if (infoHeader.biClrUsed) {
            writeQuad(quad, infoHeader, fout);
        }
        delete[] quad;
        //保存像素数据
        for (unsigned i = 0; i < height; ++i)
        {
            for (unsigned j = 0; j < width; ++j)
            {
                fwrite( &(*(imgData + i * width + j)).red, 1, sizeof(BYTE), fout); //注意三条语句的顺序：否则颜色会发生变化
                fwrite( &(*(imgData + i * width + j)).green, 1, sizeof(BYTE), fout);
                fwrite( &(*(imgData + i * width + j)).blue, 1, sizeof(BYTE), fout);
            }
        }
        fclose(fout);
        return 0;
    }

    static const WORD TYPE_IDENTIFIER = 0x4d42;
    BitmapFileHeader fileHeader;
    BitmapFileInfoHeader infoHeader;
    RGBQuad *quad;
    ImgData *imgData;
    unsigned height;
    unsigned width;

};





double inline angle2Radian(double angle) {
    return PI * angle / 180;
}

ImgData * rot(ImgData *imgData, const unsigned width, const unsigned height, double angle) {
    // 输入的角度转弧度
    // double radian = angle2Radian(angle);
    // 准备输出数据
    ImgData *result = (ImgData*)malloc(sizeof(ImgData) * width * height);
    memset(result, 0, sizeof(ImgData) * width * height);
    return imgData;
}

ImgData * readBMP() {
    return NULL;
}


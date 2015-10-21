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
};

double inline angle2Radian(double angle) {
    return PI * angle / 180;
}

double inline radian2Angle (double radian) {
    return radian * 180 / PI;
}

double inline dmod(double var, unsigned MOD) {
    return (var - floor(var)) + ((int)floor(var) % MOD);
}

class pBMP
{
public:

    // read palette
    void readQuad(RGBQuad *quad, BitmapFileInfoHeader &infoHeader, FILE *fpi) {
        for (unsigned int nCounti = 0; nCounti < infoHeader.biClrUsed; nCounti++)
        {
            fread((char *)&quad[nCounti].rgbBlue, 1, sizeof(BYTE), fpi);
            fread((char *)&quad[nCounti].rgbGreen, 1, sizeof(BYTE), fpi);
            fread((char *)&quad[nCounti].rgbRed, 1, sizeof(BYTE), fpi);
        }
    }

    // write quad to file
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
            // fileHeader.display();
            // infoHeader.display();

            quad = nullptr;
            if (infoHeader.biClrUsed) {
                quad = new RGBQuad[256]; // palette need 256 different color.
                readQuad(quad, infoHeader, fin);
            }

            width = infoHeader.biWidth;
            height = infoHeader.biHeight;

            // read imgData
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
            // save quad
            writeQuad(quad, infoHeader, fout);
        }
        // now save the imgData
        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                fwrite( &(*(imgData + i * width + j)).red, 1, sizeof(BYTE), fout);
                fwrite( &(*(imgData + i * width + j)).green, 1, sizeof(BYTE), fout);
                fwrite( &(*(imgData + i * width + j)).blue, 1, sizeof(BYTE), fout);
            }
        }
        fclose(fout);
        return 0;
    }

    pBMP rot(double targetAngle) {
        double angle = dmod(targetAngle, 180);
        if (angle > 90) {
            angle = 180 - angle;
        }

        // length of diagonal
        double diagonal = sqrt(width * width + height * height);
        printf("diagonal Width = %lf\n", diagonal);
        double baseAngleHeight = asin(height / diagonal);
        printf("baseAngleHeight = %lf\n", radian2Angle(baseAngleHeight));
        // this is the sum radian of rotation angle and base angel between diagonal and height
        double radianHeight = angle2Radian(dmod(angle + radian2Angle(baseAngleHeight), 180));
        printf("radianHeight = %lf\n", radian2Angle(radianHeight));
        // get the height of target img.
        double targetHeight = diagonal * sin(radianHeight);
        printf("targetHeight = %lf\n", targetHeight);

        // sum radian of rotation angle and base angel between diagonal and width
        double radianWidth = angle2Radian(dmod(angle + (90 - radian2Angle(baseAngleHeight)), 180));
        // width of the target img.
        double targetWidth = diagonal * sin(radianWidth);
        printf("target Width = %lf\n", targetWidth);
        fflush(stdout);

        // At rotate process, because of the diagonal is longer of both height and width, the output picture may has different size with the original img. The codes above want to figure the best size of output img, not too big to wasting memory, and just enough to contain the target picture.

        // the output image
        pBMP result = *this;
        // the width and height of .BMP need to be a multiple of 4
        result.width = ((LONG)ceil(targetWidth) + 3) / 4 * 4;
        result.height = ((LONG)ceil(targetHeight) + 3) / 4 * 4;

        result.infoHeader.biWidth = result.width;
        result.infoHeader.biHeight = result.height;
        unsigned block = sizeof(ImgData) * result.height * result.width;
        ImgData *rotData = (ImgData*)malloc(block);
        memset(rotData, 0x3f3f3f3f, block);

        // start rot process
        // middle position of original img.
        unsigned imid = height / 2,
                 jmid = width / 2,
                 // middle position of result img.
                 imidr = result.height / 2,
                 jmidr = result.width / 2;
        int x = 0,
            y = 0;
        unsigned xn = 0,
                 yn = 0;

        printf("angle = %lf\n", angle);
        printf("result height = %u\n", result.height);
        printf("result width = %u\n", result.width);
        fflush(stdout);
        angle = angle2Radian(targetAngle);

        // Traverse point of the result image, Reverse out original point coordinates. Could be slower.
        // for (unsigned i = 0; i < result.height; ++i) {
        //     for (unsigned j = 0; j < result.width; ++j) {
        //         x = i - imidr;
        //         y = j - jmidr;
        //         xn = x * cos(angle) - y * sin(angle) + imid;
        //         yn = x * sin(angle) + y * cos(angle) + jmid;
        //         if (xn <= height && yn <= width) {
        //             *(rotData + i * result.width + j) = *(imgData + xn * width + yn);
        //         }
        //     }
        // }

        // Traverse point of the original image, calculate result point and check the position. Could be faster.
        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                x = i - imid;
                y = j - jmid;
                x = x * cos(angle) + y * sin(angle);
                y = y * cos(angle) - x * sin(angle);
                xn = x + imidr;
                yn = y + jmidr;
                if (xn <= result.height && yn <= result.width) {
                    *(rotData + result.width * xn + yn) = *(imgData + i * width + j);
                }
            }
        }

        // put rotData on the imgData pointer
        result.imgData = rotData;
        return result;
    }

    static const WORD TYPE_IDENTIFIER = 0x4d42;
    BitmapFileHeader fileHeader;
    BitmapFileInfoHeader infoHeader;
    RGBQuad *quad;
    ImgData *imgData;
    unsigned height;
    unsigned width;
};




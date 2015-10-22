#include "pBMP.h"
#include <iostream>
#include <cmath>
#include <cstring>
#include <cassert>
#include <vector>
#include <algorithm>

/**
 * display infomation
 * @author piratf
 */
void BitmapFileHeader::display() {
    std::cout << "位图文件头: " << std::endl;
    std::cout << "文件大小: " << bfSize << std::endl;
    std::cout << "保留字_1: " << bfReserved1 << std::endl;
    std::cout << "保留字_2: " << bfReserved2 << std::endl;
    std::cout << "实际位图数据的偏移字节数: " << bfOffBits << std::endl << std::endl;
}

/**
 * display infomation
 * @author piratf
 */
void BitmapFileInfoHeader::display() {
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

double inline angle2Radian(double angle) {
    return PI * angle / 180;
}

double inline radian2Angle (double radian) {
    return radian * 180 / PI;
}

double inline dmod(double var, unsigned MOD) {
    if (var < 0) {
        var = var + ((abs(var) / MOD) + 1) * MOD;
        // printf("var = %lf\n", var);
    }
    if (var < MOD) {
        return var;
    }
    return (var - floor(var)) + ((int)floor(var) % MOD);
}

/**
 * read palette from file pointer
 * @author piratf
 * @param  quad       palette array
 * @param  infoHeader BigmapFileInfoHeader object
 * @param  fpi        file stream pointer
 */
void pBMP::readQuad(RGBQuad *quad, BitmapFileInfoHeader &infoHeader, FILE *fpi) {
    for (unsigned int nCounti = 0; nCounti < infoHeader.biClrUsed; nCounti++)
    {
        fread((char *)&quad[nCounti].rgbBlue, 1, sizeof(BYTE), fpi);
        fread((char *)&quad[nCounti].rgbGreen, 1, sizeof(BYTE), fpi);
        fread((char *)&quad[nCounti].rgbRed, 1, sizeof(BYTE), fpi);
    }
}

/**
 * write quad to file
 * @author piratf
 * @param  strPla     battle array
 * @param  infoHeader BigmapFileInfoHeader object
 * @param  fpi        file stream pointer
 */
void pBMP::writeQuad(RGBQuad *strPla, BitmapFileInfoHeader &infoHeader, FILE *fpi) {
    for (unsigned int nCounti = 0; nCounti < infoHeader.biClrUsed; nCounti++)
    {
        //存储的时候，一般去掉保留字rgbReserved
        fwrite((char *)&strPla[nCounti].rgbBlue, 1, sizeof(BYTE), fpi);
        fwrite((char *)&strPla[nCounti].rgbGreen, 1, sizeof(BYTE), fpi);
        fwrite((char *)&strPla[nCounti].rgbRed, 1, sizeof(BYTE), fpi);
    }
}

/**
 * read .bmp file, fill data of current pBMP object;
 * @author piratf
 * @param  filePath path to the source file
 * @return          0 - success : -1 :failed
 */
int pBMP::read(const char *filePath) {
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
        blockSize = width * height;

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

/**
 * write pBMP to .bmp file
 * @author piratf
 * @param  filePath path of .bmp file
 * @return          0 - success : -1 - failed
 */
int pBMP::write(const char* filePath) {
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

/**
 * rotate the img and return a new one.
 * @author piratf
 * @param  targetAngle angle between two img
 * @return             new pBMP object
 */
pBMP pBMP::rot(double targetAngle) {
    double angle = dmod(targetAngle, 180);
    if (angle > 90) {
        angle = 180 - angle;
    }

    // At rotate process, because of the diagonal is longer of both height and width, the output picture may has different size with the original img. The codes above want to figure the best size of output img, not too big to wasting memory, and just enough to contain the target picture.

    // length of diagonal
    double diagonal = sqrt(width * width + height * height);
    // printf("diagonal Width = %lf\n", diagonal);
    double baseAngleHeight = asin(height / diagonal);
    // printf("baseAngleHeight = %lf\n", radian2Angle(baseAngleHeight));
    // this is the sum radian of rotation angle and base angel between diagonal and height
    double radianHeight = angle2Radian(dmod(angle + radian2Angle(baseAngleHeight), 180));
    // printf("radianHeight = %lf\n", radian2Angle(radianHeight));
    // get the height of target img.
    double targetHeight = diagonal * sin(radianHeight);
    // printf("targetHeight = %lf\n", targetHeight);

    // sum radian of rotation angle and base angel between diagonal and width
    double radianWidth = angle2Radian(dmod(angle + (90 - radian2Angle(baseAngleHeight)), 180));
    // width of the target img.
    double targetWidth = diagonal * sin(radianWidth);
    // printf("target Width = %lf\n", targetWidth);
    // fflush(stdout);

    // the output image
    pBMP result = *this;
    // the width and height of .BMP need to be a multiple of 4
    result.width = ((LONG)ceil(targetWidth) + 3) / 4 * 4;
    result.height = ((LONG)ceil(targetHeight) + 3) / 4 * 4;
    result.blockSize = result.width * result.height;

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

    // printf("angle = %lf\n", angle);
    // printf("result height = %lu\n", result.height);
    // printf("result width = %lu\n", result.width);
    // fflush(stdout);
    angle = angle2Radian(targetAngle);

    // Traverse point of the result image, Reverse out original point coordinates. Could be slower. But could save all possible pixels even if the point between to img could not correspondence
    for (unsigned i = 0; i < result.height; ++i) {
        for (unsigned j = 0; j < result.width; ++j) {
            x = i - imidr;
            y = j - jmidr;
            xn = x * cos(angle) - y * sin(angle) + imid;
            yn = x * sin(angle) + y * cos(angle) + jmid;
            if (xn <= height && yn <= width) {
                *(rotData + i * result.width + j) = *(imgData + xn * width + yn);
            }
        }
    }

    // Traverse point of the original image, calculate result point and check the position. Could be faster. But will lost some points when the coordinates can not correspondence
    // for (unsigned i = 0; i < height; ++i) {
    //     for (unsigned j = 0; j < width; ++j) {
    //         x = i - imid;
    //         y = j - jmid;
    //         xn = x * cos(angle) + y * sin(angle) + imidr;
    //         yn = y * cos(angle) - x * sin(angle) + jmidr;
    //         if (xn <= result.height && yn <= result.width) {
    //             *(rotData + result.width * xn + yn) = *(imgData + i * width + j);
    //         }
    //     }
    // }

    // put rotData on the imgData pointer
    result.imgData = rotData;
    return result;
}

/**
 * 获取直径
 * @author piratf
 * @param  radius 半径
 * @return        直径 = 半径 x 2 + 1
 */
long getDiameter(const int radius) {
    return (radius << 1) + 1;
}

/**
 * 获取一维高斯蒙版
 * @author piratf
 * @param  radius 高斯蒙版半径
 * @return        高斯蒙版指针
 */
double * oneDimensionalGaussianFilter(const long radius) {
    double *kernel = (double *)malloc(sizeof(double) * getDiameter(radius));

    const double SQRT2PI = sqrt(2 * PI);

    const double sigma = radius / 3.0;
    const double sigma2 = sigma * 2.0 * sigma;
    const double sigmap = sigma * SQRT2PI;

    for (long n = 0, i = -radius; i <= radius; ++i, ++n)
        kernel[n] = exp(-(double)(i * i) / sigma2) / sigmap;

    return kernel;
}

/**
 * get two dimensional gaussian filter mask
 * @author piratf
 * @param  radius radius of the mask
 * @return        pointer of the new mask
 */
double * twoDimensionalGaussianFilter(const long radius) {
    // diameter length
    unsigned long diameter = getDiameter(radius);
    double *kernel = (double *)malloc(sizeof(double) * diameter * diameter);

    const double sigma = radius / 3.0;
    const double sigma2 = sigma * 2.0 * sigma;
    const double sigmap = sigma2 * PI;

    unsigned r = 0;

    for (long i = -radius, n = 0; i <= radius; ++i) {
        r = i * i;
        for (long j = -radius; j <= radius; ++j, ++n) {
            kernel[n] = exp( -(double)(r + j * j) / sigma2) / sigmap;
        }
    }
    return kernel;
}

/**
 * filp the coordinate close to the edge.
 * @author piratf
 * @param  i coordinate in mask
 * @param  x coordinate in the target picture
 * @param  w width of the target picture
 * @return   the new coordinate(position)
 */
long long edgeFilp(unsigned long mask, unsigned long img, unsigned long width) {
    unsigned long i_k = img + mask;
    return (i_k < 0) ? -img
           : (i_k >= width) ? (width - 1 - img)
           : mask;
}

/**
 * clamp color value inside 0 - 255
 * @author piratf
 * @param  color some value
 * @return       BYTE const BYTE value 255
 */
BYTE inline clampColor(double color) {
    return (color < CLAMP) ? color : CLAMP;
}

int clamp(int x, int l, int h) {
    return (x < l) ? x
           : (x > h) ? h
           : x;
}

/**
 * normalization the gaussian mask, so the result picture will not too bright or feel dark.
 * @author piratf
 * @param  mask pointer of the gaussian mask
 * @param  size total size of the gaussian mask matrix (height * width)
 */
void normalizationMask(double *mask, const unsigned long size) {
    double sum = 0.0;
    for (unsigned long n = 0; n < size; ++n)
        sum += mask[n];
    if (sum - 1.0 < 1e-12) return;
    for (unsigned long n = 0; n < size; ++n)
        mask[n] = mask[n] / sum;
}

/**
 * one dimensional gaussian blur algorithm
 * @author piratf
 * @param  radius radius of the gaussian mask, could affect the running time
 * @return        a new pBMP object will be return
 */
pBMP pBMP::blur(const long radius) {
    assert(radius > 0);

    // use one dimensional gaussian filter mask
    double *mask = oneDimensionalGaussianFilter(radius);

    ImgData *blurData = (ImgData *)malloc(blockSize * sizeof(ImgData));

    normalizationMask(mask, radius);

    unsigned long n = 0;
    long x = 0;
    unsigned long t, i, j, i_k, inx_k;
    for (t = 0, i = 0; i < height; ++i) {
        for (j = 0; j < width; ++j, ++t) {
            for (n = 0, x = -radius; x < radius; ++x, ++n) {
                i_k = edgeFilp(x, j, width);
                inx_k = t + i_k;
                blurData[t].red += imgData[inx_k].red * mask[n];
                blurData[t].green += imgData[inx_k].green * mask[n];
                blurData[t].blue += imgData[inx_k].blue * mask[n];
            }
        }
    }

    double r = 0.0, g = 0.0, b = 0.0;
    for (t = 0, i = 0; i < width; ++i) {
        for (j = 0; j < height; ++j) {
            t = j * width + i;
            r = g = b = 0;
            for (n = 0, x = -radius; x < radius; ++x, ++n) {
                i_k = edgeFilp(x, j, height);
                inx_k = t + i_k * width;
                r += blurData[inx_k].red * mask[n];
                g += blurData[inx_k].green * mask[n];
                b += blurData[inx_k].blue * mask[n];
            }
            blurData[t].red = clampColor(r);
            blurData[t].green = clampColor(g);
            blurData[t].blue = clampColor(b);
        }
    }

    pBMP blurImg = *this;
    blurImg.imgData = blurData;

    delete [] mask;
    return blurImg;
}
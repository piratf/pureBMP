#include "pBMP.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include <ctime>

const unsigned int FILE_NAME_LENGTH = 30;

int main() {
// #ifdef linux
//     printf("linux OS.\n");
// #endif
// #ifdef _UNIX
//     printf("UNIX OS.\n");
// #endif
// #ifdef _WIN32
//     printf("Win32 OS.\n");
// #endif

    // nice(-20);
    pBMP img;
    img.read("Image/11.BMP");
    // 打印图像信息
    img.display();
    // printf("sizeof info: %u %u\n", sizeof(img.infoHeader), sizeof(BitmapFileInfoHeader));
    // printf("sizeof file: %u %u\n", sizeof(img.fileHeader), sizeof(BitmapFileHeader));
    // int bit = (img.blockSize * sizeof(ImgData) + sizeof(img.infoHeader) + sizeof(img.fileHeader));
    // double imgSize = bit / 1024 / 1024;
    // printf("total imgSize = %.4lf\n", imgSize);
    //==============================
    //准备旋转图像
    // img.rot(0).write("Image/2.bmp");
    // char filePath[FILE_NAME_LENGTH] = {};
    // for (int i = -500; i < 500; i += 45) {
    //     sprintf(filePath, "Image\\%d.BMP", i);
    //     time_t start = clock();
    //     img.rot(i).write(filePath);
    //     time_t end = clock();
    //     printf("the running time is : %f\n", double(end - start) / CLOCKS_PER_SEC);
    // }

    //==============================
    // 高斯模糊
    // time_t start = clock();
    img.blur(15).write("Image/3.bmp");
    // time_t end = clock();
    // printf("the running time is : %f\n", double(end - start) / CLOCKS_PER_SEC);

    //==============================
    // 输出图像
    // img.write("Image\\3.bmp");

    return 0;
}
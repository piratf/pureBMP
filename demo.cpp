#include "pBMP.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include <ctime>

const unsigned int FILE_NAME_LENGTH = 30;

int main() {
    char filePath[FILE_NAME_LENGTH] = {};
    pBMP img;
    img.read("Image\\1.BMP");
    // 打印图像信息
    // img.infoHeader.display();
    //==============================
    //准备旋转图像
    // img.rot(-123).write("Image\\2.BMP");
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
    // pBMP blur = img.blur(10);
    // time_t end = clock();
    // printf("the running time is : %f\n", double(end - start) / CLOCKS_PER_SEC);

    //==============================
    // 输出图像
    // blur.write("Image\\3.bmp");

    return 0;
}
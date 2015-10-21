#include "pBMP.h"
#include <iostream>
#include <cstring>
#include <cmath>

const unsigned int FILE_NAME_LENGTH = 30;

int main() {
    pBMP img;
    img.read("1.BMP");
    // 打印图像信息
    // img.infoHeader.display();
    //==============================
    //准备旋转图像
    double angle = 725;
    img.rot(angle).write("2.BMP");

    //==============================
    // 输出图像
    // img.write("imgFilePath.bmp");

    return 0;
}
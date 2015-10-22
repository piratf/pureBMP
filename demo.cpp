#include "pBMP.h"
#include <iostream>
#include <cstring>
#include <cmath>

const unsigned int FILE_NAME_LENGTH = 30;

int main() {
    // char filePath[FILE_NAME_LENGTH] = {};
    pBMP img;
    img.read("Image\\1.BMP");
    // 打印图像信息
    // img.infoHeader.display();
    //==============================
    //准备旋转图像
    img.rot(-123).write("Image\\2.BMP");
    // for (int i = -500; i < 500; i += 45) {
    //     sprintf(filePath, "Image\\%d.BMP", i);
    //     img.rot(i).write(filePath);
    // }

    //==============================
    // 高斯模糊
    img.blur(20).write("Image\\3.bmp");

    //==============================
    // 输出图像
    // img.write("Image\\2.bmp");

    return 0;
}
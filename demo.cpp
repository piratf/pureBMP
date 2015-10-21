#include "pBMP.h"
#include <iostream>
#include <cstring>
#include <cmath>

const unsigned int FILE_NAME_LENGTH = 30;

int main() {
    pBMP img;
    img.read("Image\\1.BMP");
    // 打印图像信息
    // img.infoHeader.display();
    //==============================
    //准备旋转图像
    char filePath[FILE_NAME_LENGTH] = {};
    for (int i = -500; i < 500; i += 45) {
        sprintf(filePath, "Image\\%d.BMP", i);
        img.rot(i).write(filePath);
    }

    //==============================
    // 输出图像
    // img.write("imgFilePath.bmp");

    return 0;
}
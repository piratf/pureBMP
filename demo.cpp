#include "pBMP.h"
#include <iostream>
#include <cstring>
#include <cmath>

const unsigned int FILE_NAME_LENGTH = 30;

int main() {
    freopen("demo.txt", "r", stdin);
    pBMP img;
    img.read("1.BMP");
    //==============================
    //准备旋转图像
    // double angle = 0;
    // std::cout << "Please input the angle of this rot (0 - 360):" << std::endl;
    // std::cin >> angle;
    // ImgData *result = rot(img, width, height, angle);

    //==============================
    // 输出图像
    img.write("imgFilePath.bmp");

    return 0;
}
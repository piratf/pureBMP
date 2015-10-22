# A pure C++ BMP Library

---

Usage: use `make` command to compile files

complier version: `g++ 4.9.2`
- Use `-std=c++11`(recommend: `-pedantic -Wall`) flag for compiling.

You can modify it in `makefile`.

run `demo.exe` to test data files.

more details in file `demo.cpp`

---

Algorithm can be used:

> + rot

    rotate BMP files, could calcule the minimum size of the output image file, and fast enough.
    return a new picture.

> + blur

    fuzzy picture with one dimensional gaussian mask. 
    return a new picture.

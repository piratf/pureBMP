targets = demo.exe
objects = pBMP.o
relyCode = pBMP.h pBMP.cpp demo.cpp
cppFlags = -std=c++11 -pedantic -Wall

$(targets): $(objects)
	g++ $(cppFlags) demo.cpp $(objects) -o demo.exe

pBMP.o: $(relyCode)
	g++ $(cppFlags) -c pBMP.cpp

.PHONY: clean

clean:
	rm *.o
	rm *.exe
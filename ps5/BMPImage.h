#ifndef BMPIMAGE_H
#define BMPIMAGE_H


#include <stdio.h>

class BMPImage {
public:
	//Create a 0x0 image
	BMPImage();
	//Create an undefined image of the specified size
	BMPImage(unsigned long xSize, unsigned long ySize);

	~BMPImage();

	//Load and save .bmp files (24 bit BGR only!)
	//Return 1 on success, 0 on failure
	int load(char *filename);
	int save(char *filename);

	//Read or write the rgb color of the pixel at the given location
	//unsigned chars are in [0, 255], floats are in [0, 1]
	void readPixel(unsigned long x, unsigned long y, unsigned char &r, unsigned char &g, unsigned char &b);
	void readPixel(unsigned long x, unsigned long y, float &r, float &g, float &b);
	void writePixel(unsigned long x, unsigned long y, unsigned char r, unsigned char g, unsigned char b);
	void writePixel(unsigned long x, unsigned long y, float r, float g, float b);

	//Data is stored first by row, then by column
	//(3 bytes per pixel, RGB order)
	//For example, for the image
	//
	// 000 000 123 000 000
	// 000 234 234 234 000
	// 000 000 345 000 000
	//
	//sizeX = 5
	//sizeY = 3
	//data = {{000 000 123 000 000} {000 234 234 234 000} {000 000 345 000 000}}
	unsigned long getXSize() {return sizeX;}
	unsigned long getYSize() {return sizeY;}
	unsigned char *getData() {return data;}

private:
	unsigned int getint(FILE *fp);
	unsigned int getshort(FILE *fp);

    unsigned long sizeX;
    unsigned long sizeY;
    unsigned char *data;
};

#endif
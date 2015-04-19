#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "BMPImage.h"

BMPImage::BMPImage() {
	sizeX = 0;
	sizeY = 0;
	data = NULL;
}

BMPImage::BMPImage(unsigned long xSize, unsigned long ySize) {
	sizeX = xSize;
	sizeY = ySize;
	data = (unsigned char *)malloc(xSize*ySize*3);
}

BMPImage::~BMPImage() {
	free(data);
}

void BMPImage::readPixel(unsigned long x, unsigned long y, unsigned char &r, unsigned char &g, unsigned char &b) {
	r = data[y*sizeX*3 + x*3 + 0];
	g = data[y*sizeX*3 + x*3 + 1];
	b = data[y*sizeX*3 + x*3 + 2];
}

void BMPImage::readPixel(unsigned long x, unsigned long y, float &r, float &g, float &b) {
	unsigned char ucharR, ucharB, ucharG;

	readPixel(x, y, ucharR, ucharG, ucharB);

	r = ucharR/255.0f;
	g = ucharG/255.0f;
	b = ucharB/255.0f;
}

void BMPImage::writePixel(unsigned long x, unsigned long y, unsigned char r, unsigned char g, unsigned char b) {
	data[y*sizeX*3 + x*3 + 0] = r;
	data[y*sizeX*3 + x*3 + 1] = g;
	data[y*sizeX*3 + x*3 + 2] = b;
}

void BMPImage::writePixel(unsigned long x, unsigned long y, float r, float g, float b) {
	unsigned char ucharR, ucharB, ucharG;

	ucharR = (unsigned char)(r*255);
	ucharG = (unsigned char)(g*255);
	ucharB = (unsigned char)(b*255);

	writePixel(x, y, ucharR, ucharG, ucharB);
}

/*
 * getint and getshort are help functions to load the bitmap byte by byte
 */

unsigned int BMPImage::getint(FILE *fp)
{
  int c, c1, c2, c3;

  /*  get 4 bytes */ 
  c = getc(fp);  
  c1 = getc(fp);  
  c2 = getc(fp);  
  c3 = getc(fp);
  
  return ((unsigned int) c) +   
    (((unsigned int) c1) << 8) + 
    (((unsigned int) c2) << 16) +
    (((unsigned int) c3) << 24);
}

unsigned int BMPImage::getshort(FILE *fp)
{
  int c, c1;
  
  /* get 2 bytes*/
  c = getc(fp);  
  c1 = getc(fp);

  return ((unsigned int) c) + (((unsigned int) c1) << 8);
}

/*  quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.  */

int BMPImage::load(char *filename) 
{
    FILE *file;
    unsigned long size;                 /*  size of the image in bytes. */
    unsigned long i;                    /*  standard counter. */
    unsigned short int planes;          /*  number of planes in image (must be 1)  */
    unsigned short int bpp;             /*  number of bits per pixel (must be 24) */
    unsigned char temp;                 /*  used to convert bgr to rgb color. */

    /*  make sure the file is there. */
    if ((file = fopen(filename, "rb"))==NULL) {
      printf("File Not Found : %s\n",filename);
      return 0;
    }
    
    /*  seek through the bmp header, up to the width height: */
    fseek(file, 18, SEEK_CUR);

    /*  No 100% errorchecking anymore!!! */

    /*  read the width */    
	this->sizeX = getint (file);
	//fixes error for width not multiples of 4
	int remainder = (int) fmodl(this->sizeX, 4L);
	if (remainder != 0) {
		this->sizeX += (4 - remainder);
	}
    
    /*  read the height */ 
    this->sizeY = getint (file);

	/*  calculate the size (assuming 24 bits or 3 bytes per pixel). */
	size = sizeX*sizeY*3;

    /*  read the planes */    
    planes = getshort(file);
    if (planes != 1) {
	printf("Planes from %s is not 1: %u\n", filename, planes);
	return 0;
    }

    /*  read the bpp */    
    bpp = getshort(file);
    if (bpp != 24) {
      printf("Bpp from %s is not 24: %u\n", filename, bpp);
      return 0;
    }
	
    /*  seek past the rest of the bitmap header. */
    fseek(file, 24, SEEK_CUR);

    /*  read the data.  */
    this->data = (unsigned char *)realloc(this->data, size);
    if (this->data == NULL) {
	printf("Error allocating memory for color-corrected image data");
	return 0;	
    }

    if ((i = fread(this->data, size, 1, file)) != 1) {
	printf("Error reading image data from %s.\n", filename);
	return 0;
    }

    for (i=0;i<size;i+=3) { /*  reverse all of the colors. (bgr -> rgb) */
      temp = this->data[i];
      this->data[i] = this->data[i+2];
      this->data[i+2] = temp;
    }

    fclose(file); /* Close the file and release the filedes */

    /*  we're done. */
    return 1;
}

/*  quick and dirty bitmap saver...for 24 bit bitmaps with 1 plane only.  */

struct BMPFILEHEADER {
        unsigned char     bfType[2];
        unsigned long     bfSize;
        unsigned short    bfReserved1;
        unsigned short    bfReserved2;
        unsigned long     bfOffBits;
};

struct BMPINFOHEADER{
        unsigned long        biSize;
        long                 biWidth;
        long                 biHeight;
        unsigned short       biPlanes;
        unsigned short       biBitCount;
        unsigned long        biCompression;
        unsigned long        biSizeImage;
        long                 biXPelsPerMeter;
        long                 biYPelsPerMeter;
        unsigned long        biClrUsed;
        unsigned long        biClrImportant;
};

int BMPImage::save(char *filename)
{
	FILE *file;
    unsigned long size;                 /*  size of the image in bytes. */
    unsigned long i;                    /*  standard counter. */

	BMPFILEHEADER fileHeader;
	BMPINFOHEADER infoHeader;

    /*  make sure the file is there. */
    if ((file = fopen(filename, "wb"))==NULL) {
      printf("Could not open file for writing : %s\n",filename);
      return 0;
    }
    
	size = this->sizeX * this->sizeY * 3;//4;

	/* set up header info */
	fileHeader.bfType[0] = 'B';
	fileHeader.bfType[1] = 'M';
	fileHeader.bfSize = size + 54;//sizeof(BMPFILEHEADER) + sizeof(BMPINFOHEADER);
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = 54;// * 8;//(sizeof(BMPFILEHEADER) + sizeof(BMPINFOHEADER)) * 8;

	infoHeader.biSize = 40;//sizeof(BMPINFOHEADER);
	infoHeader.biWidth = this->sizeX;
	infoHeader.biHeight = this->sizeY;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 24;
	infoHeader.biCompression = 0;
	infoHeader.biSizeImage = 0;
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;

	/* write header info */
	//fwrite(&fileHeader, sizeof(fileHeader), 1, file);
	fwrite(&(fileHeader.bfType), 2, 1, file);
	fwrite(&(fileHeader.bfSize), 4, 1, file);
	fwrite(&(fileHeader.bfReserved1), 2, 1, file);
	fwrite(&(fileHeader.bfReserved2), 2, 1, file);
	fwrite(&(fileHeader.bfOffBits), 4, 1, file);

	//fwrite(&infoHeader, sizeof(infoHeader), 1, file);
	fwrite(&(infoHeader.biSize), 4, 1, file);
	fwrite(&(infoHeader.biWidth), 4, 1, file);
	fwrite(&(infoHeader.biHeight), 4, 1, file);
	fwrite(&(infoHeader.biPlanes), 2, 1, file);
	fwrite(&(infoHeader.biBitCount), 2, 1, file);
	fwrite(&(infoHeader.biCompression), 4, 1, file);
	fwrite(&(infoHeader.biSizeImage), 4, 1, file);
	fwrite(&(infoHeader.biXPelsPerMeter), 4, 1, file);
	fwrite(&(infoHeader.biYPelsPerMeter), 4, 1, file);
	fwrite(&(infoHeader.biClrUsed), 4, 1, file);
	fwrite(&(infoHeader.biClrImportant), 4, 1, file);

	/* write image data */
	for (i = 0; i < this->sizeX * this->sizeY * 3; i += 3) {
		/* reverse RGB */
		fwrite(&(this->data[i+2]), 1, 1, file);
		fwrite(&(this->data[i+1]), 1, 1, file);
		fwrite(&(this->data[i+0]), 1, 1, file);
		//fwrite("", 1, 1, file);
	}

    fclose(file); /* Close the file and release the filedes */

    /*  we're done. */
    return 1;
}
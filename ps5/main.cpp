#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "BMPImage.h"

using namespace std;

//If a pixel has values[r, g, b], take the greyscale value to be .299*r + .587g + .114b
void greyscaleFunction(BMPImage *image) {
	
	float r, g, b;

	//for each pixel in the image, apply the above function
	for (int y = 0; y < image->getYSize(); y++) {
		for (int x = 0; x < image->getXSize(); x++) {
			image->readPixel(x, y, r, g, b);
			float grey = (0.299 * r) + (0.587 * g) + (0.114) * b;
			image->writePixel(x, y, grey, grey, grey);
		}
	}
}

void correlationFunction(BMPImage *image, float filter, int m, int n) {

	float greyVal, pixelVal, filterVal;
	filterVal = sqrt(filter);

	//for each pixel in the image
	for (int y = 0; y < image->getYSize(); y++) {
		for (int x = 0; x < image->getXSize(); x++) {

			pixelVal = 0;
			greyVal = 0;

			//filter across x
			for (int i = -1 * (n / 2); i <= (n / 2); i++) {

				int paddingXCoord = x + i;

				if (paddingXCoord <= 0) {
					//use left most value in row
					image->readPixel(0, y, greyVal, greyVal, greyVal);
					pixelVal += greyVal * filterVal;
				}
				else if (paddingXCoord >= image->getXSize() - 1) {
					//use right most value in row
					image->readPixel(image->getXSize() - 1, y, greyVal, greyVal, greyVal);
					pixelVal += greyVal * filterVal;
				}
				else {
					//use value at position i in row
					image->readPixel(paddingXCoord, y, greyVal, greyVal, greyVal);
					pixelVal += greyVal * filterVal;
				}

			}
			image->writePixel(x, y, pixelVal, pixelVal, pixelVal);
		}
	}

	//for each pixel in the image
	for (int y = 0; y < image->getYSize(); y++) {
		for (int x = 0; x < image->getXSize(); x++) {

			pixelVal = 0;
			greyVal = 0;

			//filter across y
			for (int j = -1 * (n / 2); j <= (n / 2); j++) {

				int paddingYCoord = y + j;

				if (paddingYCoord <= 0) {
					//use top most value in column
					image->readPixel(x, 0, greyVal, greyVal, greyVal);
					pixelVal += greyVal * filterVal;
				}
				else if (paddingYCoord >= image->getYSize() - 1) {
					//use top most value in column
					image->readPixel(x, image->getYSize() - 1, greyVal, greyVal, greyVal);
					pixelVal += greyVal * filterVal;
				}
				else {
					//use value at position i in row
					image->readPixel(x, paddingYCoord, greyVal, greyVal, greyVal);
					pixelVal += greyVal * filterVal;
				}

			}
			image->writePixel(x, y, pixelVal, pixelVal, pixelVal);
		}
	}

	/*
	//for each pixel in the image
	for (int y = 0; y < image->getYSize(); y++) {
		for (int x = 0; x < image->getXSize(); x++) {

			pixelVal = 0;
			greyVal = 0;

			//for each item in the filter
			for (int j = -1 * (n / 2); j <= (m / 2); j++) {
				for (int i = -1 * (m / 2); i <= (m / 2); i++) {

					int paddingXCoord = x + i;
					int paddingYCoord = y + j;
					
					if ((paddingXCoord) <= 0 && (paddingYCoord) <= 0) {
						//use top left value in image
						image->readPixel(0, 0, greyVal, greyVal, greyVal);
						pixelVal += greyVal * filter;
					}
					else if (paddingXCoord >= image->getXSize() - 1 && paddingYCoord <= 0) {
						//use top right value in image
						image->readPixel(image->getXSize() - 1, 0, greyVal, greyVal, greyVal);
						pixelVal += greyVal * filter;
					}
					else if (paddingXCoord <= 0 && paddingYCoord >= image->getYSize() - 1) {
						//use bottom left value in image
						image->readPixel(0, image->getYSize() - 1, greyVal, greyVal, greyVal);
						pixelVal += greyVal * filter;
					}
					else if (paddingXCoord >= image->getXSize() - 1 && paddingYCoord >= image->getYSize() - 1) {
						//use bottom right value in image
						image->readPixel(image->getXSize() - 1, image->getYSize() - 1, greyVal, greyVal, greyVal);
						pixelVal += greyVal * filter;
					}
					else if (paddingXCoord <= 0 && paddingYCoord > 0) {
						//use left value in image
						image->readPixel(0, paddingYCoord, greyVal, greyVal, greyVal);
						pixelVal += greyVal * filter;
					}
					else if (paddingXCoord >= image->getXSize() - 1 && paddingYCoord > 0) {
						//use right value in image
						image->readPixel(image->getXSize() - 1, paddingYCoord, greyVal, greyVal, greyVal);
						pixelVal += greyVal * filter;
					}
					else if (paddingXCoord > 0 && paddingYCoord <= 0) {
						//use top value in image
						image->readPixel(paddingXCoord, 0, greyVal, greyVal, greyVal);
						pixelVal += greyVal * filter;
					}
					else if (paddingXCoord > 0 && paddingYCoord >= image->getYSize() - 1) {
						//use bottom value in image
						image->readPixel(paddingXCoord, image->getYSize() - 1, greyVal, greyVal, greyVal);
						pixelVal += greyVal * filter;
					}
					else {
						//use value in image
						image->readPixel(paddingXCoord, paddingYCoord, greyVal, greyVal, greyVal);
						pixelVal += greyVal * filter;
					}
					
				}
			}
			
			image->writePixel(x, y, pixelVal, pixelVal, pixelVal);

		}
	}
	*/
}

void correlationFunction(BMPImage *image, float *xFilter, float *yFilter, int numX, int numY) {
	
}

void filterGaussianFunction(BMPImage *image, float sigma) {
	
	float filterDimen = 2 * (ceilf(3 * sigma)) + 1;

	float *xFilter = new float[(int) filterDimen];
	float *yFilter = new float[(int)filterDimen];

	for (int i = 0; i < )
}

void filterSharpeningFunction() {

}

void resizeFunction() {

}

//argv[0] - program name
//argv[1] - function number
//argv[2] - read image file name
//argv[3] - write image file name
int main(int argc, char** argv) {

	if (argc != 4) {
		cout << "Incorrect usage - proper usage: " << argv[0] << " functionNumber inputFile outputFile\n";
		cout << "\nfunctionNumber options:\n1. greyscaleFunction\n2. correlationFunction\n3. filterGuassianFunction\n4. filterShapeningFunction\n5. resizeFunction\n";
		return 0;
	}

	int functionNumber = atoi(argv[1]);
	BMPImage image = BMPImage();
	image.load(argv[2]);
	greyscaleFunction(&image);

	switch (functionNumber) {
	case 1:
		break;
	case 2:
		float filterVal = float(1.0f / 49.0f);
		correlationFunction(&image, filterVal, 7, 7);
		break;
	case 3:
		float sigma = 1.5;
		filterGaussianFunction(&image, sigma);
		break;
	}
	image.save(argv[3]);
	

}
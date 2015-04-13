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

	float greyVal, pixelVal;
	
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
	
}

void filterGaussianFunction() {

}

void filterSharpeningFunction() {

}

void resizeFunction() {

}

void sameImageTest(BMPImage *image1, BMPImage *image2) {
	bool result = false;
	float image1Val, image2Val;

	for (int y = 0; y < image1->getYSize(); y++) {
		for (int x = 0; x < image1->getXSize(); x++) {
			image1->readPixel(x, y, image1Val, image1Val, image1Val);
			image2->readPixel(x, y, image2Val, image2Val, image2Val);

			if (image1Val != image2Val) {
				cout << "values don't match\n";
				break;
			}
		}
	}
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
	float filterVal;

	switch (functionNumber) {
	case 1:
		image.save(argv[3]);
		break;
	case 2:
		filterVal = float(1.0f / 49.0f);
		correlationFunction(&image, filterVal, 7, 7);
		image.save(argv[3]);
		break;
	case 9:
		BMPImage image1 = BMPImage();
		image1.load(argv[2]);
		BMPImage image2 = BMPImage();
		image2.load(argv[3]);
		greyscaleFunction(&image1);
		greyscaleFunction(&image2);
		sameImageTest(&image1, &image2);
		break;
	}
	

}
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "BMPImage.h"

using namespace std;

/*
	This function takes in a color BMPImage and outputs a greyscale version of the image.
*/
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

/*
	Correlates a filter value on an image. This function takes in a greyscale BMPImage, and a separable 
	filter with n x n dimensions and a uniform filter value. The filter value parameter should be the 
	value before separation. For instance, if the box filter has all values of 1/49, then 1/49 should be 
	used as input, as this function will separate the box filter into two 1D filters with the value of 1/7. 
	The function then applies the two 1D filters to the image.
*/
void correlationFunction(BMPImage *image, float filter, int n) {

	float readVal, writeVal, filterVal;
	filterVal = sqrt(filter);

	//for each pixel in the image
	for (int y = 0; y < image->getYSize(); y++) {
		for (int x = 0; x < image->getXSize(); x++) {

			writeVal = 0;
			readVal = 0;

			//filter across x
			for (int i = -1 * (n / 2); i <= (n / 2); i++) {

				int offsetXCoord = x + i;

				if (offsetXCoord <= 0) {
					//use left most value in row
					image->readPixel(0, y, readVal, readVal, readVal);
					writeVal += readVal * filterVal;
				}
				else if (offsetXCoord >= image->getXSize() - 1) {
					//use right most value in row
					image->readPixel(image->getXSize() - 1, y, readVal, readVal, readVal);
					writeVal += readVal * filterVal;
				}
				else {
					//use value at offset in row
					image->readPixel(offsetXCoord, y, readVal, readVal, readVal);
					writeVal += readVal * filterVal;
				}

			}
			image->writePixel(x, y, writeVal, writeVal, writeVal);
		}
	}

	//for each pixel in the image
	for (int y = 0; y < image->getYSize(); y++) {
		for (int x = 0; x < image->getXSize(); x++) {

			writeVal = 0;
			readVal = 0;

			//filter across y
			for (int i = -1 * (n / 2); i <= (n / 2); i++) {

				int offsetYCoord = y + i;

				//padding left
				if (offsetYCoord <= 0) {
					//use top most value in column
					image->readPixel(x, 0, readVal, readVal, readVal);
					writeVal += readVal * filterVal;
				}
				//padding right
				else if (offsetYCoord >= image->getYSize() - 1) {
					//use top most value in column
					image->readPixel(x, image->getYSize() - 1, readVal, readVal, readVal);
					writeVal += readVal * filterVal;
				}
				else {
					//use value at offset in column
					image->readPixel(x, offsetYCoord, readVal, readVal, readVal);
					writeVal += readVal * filterVal;
				}

			}
			image->writePixel(x, y, writeVal, writeVal, writeVal);
		}
	}
}

/*
	Correlates a 1D filter on an image. This function takes in a greyscale BMPImage, a pointer 
	to a 1D filter, and the dimension of the 1D filter. This function uses the 1D filter twice 
	to simulate the affect of applying a separable filter.
*/
void correlationFunction(BMPImage *image, float *filter, int n) {

	float readVal, writeVal;

	for (int x = 0; x < image->getXSize() - 1; x++) {
		for (int y = 0; y < image->getYSize() - 1; y++) {

			writeVal = 0;
			readVal = 0;

			int j = 0; //filter value location
			//filter across x
			for (int i = -1 * (n / 2); i <= (n / 2); i++) {
				
				int offsetXCoord = x + i;

				//padding left
				if (offsetXCoord <= 0) {
					//use left most value in row
					image->readPixel(0, y, readVal, readVal, readVal);
					writeVal += readVal * filter[j];
				}
				//padding right
				else if (offsetXCoord >= image->getXSize() - 1) {
					//use right most value in row
					image->readPixel(image->getXSize() - 1, y, readVal, readVal, readVal);
					writeVal += readVal * filter[j];
				}
				else {
					//use value at offset in row
					image->readPixel(offsetXCoord, y, readVal, readVal, readVal);
					writeVal += readVal * filter[j];
				}
				j++; //next location in filter
			}
			image->writePixel(x, y, writeVal, writeVal, writeVal);
		}
	}
	
	for (int x = 0; x < image->getXSize() - 1; x++) {
		for (int y = 0; y < image->getYSize() - 1; y++) {

			writeVal = 0;
			readVal = 0;

			int j = 0; //filter value location
			//filter across y
			for (int i = -1 * (n / 2); i <= (n / 2); i++) {

				int offsetYCoord = y + i;

				//padding top
				if (offsetYCoord <= 0) {
					//use top most value in column
					image->readPixel(x, 0, readVal, readVal, readVal);
					writeVal += readVal * filter[j];
				}
				//padding bottom
				else if (offsetYCoord >= image->getYSize() - 1) {
					//use bottom most value in column
					image->readPixel(x, image->getYSize() - 1, readVal, readVal, readVal);
					writeVal += readVal * filter[j];
				}
				else {
					//use value at offset in column
					image->readPixel(x, offsetYCoord, readVal, readVal, readVal);
					writeVal += readVal * filter[j];
				}
				j++; //next location in filter
			}
			image->writePixel(x, y, writeVal, writeVal, writeVal);
		}
	}
}

/*
	Creates a gaussian filter. This function takes in a value for sigma and a pointer to an integer.
	The function will create a new 1D filter and populate it with values for the gaussian filter. The
	function will return a pointer to the 1D filter, and will modify the integer pointer to reflect the
	dimension of the filter.
*/
float* filterGaussianFunction(float sigma, int *dimen) {
	const float PI = 3.1415927;

	int filterDimen = 2 * (ceilf(3 * sigma)) + 1;

	float *filter = new float[filterDimen];

	int j = 0; //index in the filter
	//populate the filter
	for (int i = -1 * (filterDimen / 2); i <= (filterDimen / 2); i++) {
		//calculate filter value and place in filter
		filter[j] = sqrt(1 / (2 * sigma * sqrt(2 * PI))) *exp(-1 * (i * i) / (2 * sigma * sigma));
		j++;
	}

	//normalize the filter values
	float sum = 0.0f;
	for (int i = 0; i < filterDimen; i++) {
		sum += filter[i];
	}
	for (int i = 0; i < filterDimen; i++) {
		filter[i] = (filter[i] / sum);
	}
	
	*dimen = filterDimen; //"return" the dimension of the filter
	return filter; //return the filter
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

	BMPImage image = BMPImage(); //new image to reflect greyscale and any filters applied
	float filterVal = 0.0f;
	float sigma = 0.0f;
	float *filter = NULL;
	int filterDimen = 0;

	if (argc != 4) {
		cout << "Incorrect usage - proper usage: " << argv[0] << " functionNumber inputFile outputFile\n";
		cout << "\nfunctionNumber options:\n1. greyscaleFunction\n2. correlationFunction\n3. filterGuassianFunction\n4. filterShapeningFunction\n5. resizeFunction\n";
		return 0;
	}

	image.load(argv[2]);
	greyscaleFunction(&image); //apply greyscale function

	switch (atoi(argv[1])) {
	case 1:
		//greyscale applied, we're done
		break;
	case 2:
		filterVal = float(1.0f / 49.0f);
		filterDimen = 7;
		//apply box filter to image
		correlationFunction(&image, filterVal, filterDimen);
		break;
	case 3:
		sigma = 1.5;
		//get filter and dimensions of the filter
		filter = filterGaussianFunction(sigma, &filterDimen);
		//apply filter to the image
		correlationFunction(&image, filter, filterDimen);
		delete[] filter;
		filter = NULL;
		break;
	case 4:
		sigma = 1.5;
		filter = filterGaussianFunction(sigma, &filterDimen);
	}
	image.save(argv[3]);
	

}
/*
	CMSC427 Spring 2015 Problem Set 5
	Author: Brian Summers summers.brian.cs@gmail.com
	110656609
	4/21/15
*/
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "BMPImage.h"

using namespace std;



void bilateralFilterFunction(BMPImage *image, float sigma1, float sigma2, int width);

void clampValues(float *val);

void greyscaleFunction(BMPImage *image);

void correlationFunction_1D(BMPImage *image, float filter, int n);
void correlationFunction(BMPImage *image, float filter, int n);
/*
3. 高斯功能
创建高斯滤波器。 此函数接受sigma和a的值
指向整数的指针。 该函数将创建一个新的1D滤波器并填充它
与高斯滤波器的值。 该函数将返回一个指向1D的指针
过滤器，并将修改整数指针以反映过滤器的维度。
*/
float* filterGaussianFunction(float sigma, int *dimen);

/*
4.锐化功能
创建锐化滤镜。 此函数接受sigma的值，a
指向1D高斯滤波器的指针和滤波器的维度。 功能
将从具有中心值的全通滤波器中减去高斯滤波器
为2，其余为零。 该函数返回过滤器T-G。
*/
float* filterSharpeningFunction(float sigma, float *filter, int dimen);
/*
5.调整函数大小
根据缩放值调整图像大小。 此函数接收一个BMPImage，a
缩放和保存新图像的文件名。 该功能将拍摄图像
参数并创建作为原始缩放版本的新图像，以及
将保存新缩放的图像到saveName参数。 该函数使用
双线性插值以近似新图像中的像素值
在原来。
*/
BMPImage* resizeFunction(BMPImage *image, float scale, char *saveName);


/*
	在图像上应用双边滤镜
*/
void bilateralFilterFunction(BMPImage *image, float sigma1, float sigma2, int width);

float* filterSharpeningFunction(float sigma, float *filter, int dimen);

float gaussianWeight(BMPImage *image, int i, int j, int k, int l, float sigma1, float sigma2);

BMPImage* resizeFunction(BMPImage *image, float scale, char *saveName);


void clampValues(float *val) {

	if (*val > 1.0f) {
		*val = 1.0f;
	}
	if (*val < 0.0f) {
		*val = 0.0f;
	}
}

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

				//padding left
				if (offsetXCoord <= 0) {
					//use left most value in row
					image->readPixel(0, y, readVal, readVal, readVal);
					writeVal += readVal * filterVal;
				}
				//padding right
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
			clampValues(&writeVal);
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

				//padding top
				if (offsetYCoord <= 0) {
					//use top most value in column
					image->readPixel(x, 0, readVal, readVal, readVal);
					writeVal += readVal * filterVal;
				}
				//padding bottom
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

			}clampValues(&writeVal);
			image->writePixel(x, y, writeVal, writeVal, writeVal);
		}
	}
}

/*
	Correlates a 1D filter on an image. This function takes in a greyscale BMPImage, a pointer 
	to a 1D filter, and the dimension of the 1D filter. This function uses the 1D filter twice 
	to simulate the affect of applying a separable filter.
*/
void correlationFunction_1D(BMPImage *image, float *filter, int n) {

	float readVal, writeVal;

	for (int x = 0; x < image->getXSize(); x++) {
		for (int y = 0; y < image->getYSize(); y++) {

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
			clampValues(&writeVal);
			image->writePixel(x, y, writeVal, writeVal, writeVal);
		}
	}
	
	for (int x = 0; x < image->getXSize(); x++) {
		for (int y = 0; y < image->getYSize(); y++) {

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
			clampValues(&writeVal);
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
		//note: I square root the constant because I apply this filter twice, once in x direction
		//and once in the y, in the correlation function to simulate a separable filter
		filter[j] = sqrt(1 / (sigma * sqrt(2 * PI))) * exp(-1 * (i * i) / (2 * sigma * sigma));
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

/*
	Creates a sharpening filter. This function takes in a value for sigma, a pointer to a 1D Gaussian filter
	and the dimension of the filter. The function will subtract the Gaussian filter from an all-pass filter
	with a center value of 2, with the rest being zero. The function returns the filter T-G.
*/
float* filterSharpeningFunction(float sigma, float *filter, int dimen) {

	for (int i = 0; i < dimen; i++) {
		if (i == (dimen / 2)) {
			filter[i] = 2 - filter[i]; //center value of filter T is 2, subtract the center gaussian filter value
		}
		else {
			filter[i] *= -1.0f; //all other values of T are 0, so subtract gaussian filter value at this position
		}
	}
	return filter;
}

/*
	Resizes an image to the scale value. This function takes in a BMPImage, a scale, and a filename to save the
	new image. The function will take the image parameter and create a new image that is a scaled version of
	the original, and will save the new scaled image to the saveName parameter. The function uses bilinear
	interpolation to approximate the pixel values in the new image based on the original.
*/
BMPImage* resizeFunction(BMPImage *image, float scale, char *saveName) {
	float temp1 = image->getXSize() * scale;
	float temp2 = image->getYSize() * scale;

	/* compute the image size */
	int remainder = (int)fmodl(temp1, 4L);
	if (remainder != 0) {
		temp1 += (4 - remainder);
	}
	BMPImage *scaledImage = new BMPImage(temp1, temp2);

	int xLeft, xRight, yTop, yBot;
	float topLeftVal, topRightVal, botLeftVal, botRightVal;
	float weightX, weightY;
	float leftBorderVal, rightBorderVal;
	float finalVal;
	//for all pixels in the scaled image
	for (int scaledX = 0; scaledX < scaledImage->getXSize(); scaledX++) {
		for (int scaledY = 0; scaledY < scaledImage->getYSize(); scaledY++) {
			
			//get coordinates of surrounding pixels in original image
			xLeft = scaledX / scale;
			//make sure we don't exceed bounds of image
			if (xLeft == image->getXSize() - 1) {
				xRight = xLeft;
			}
			else {
				xRight = xLeft + 1;
			}
			
			yTop = scaledY / scale;
			//make sure we don't exceed bounds of image
			if (yTop == image->getYSize() - 1) {
				yBot = yTop;
			}
			else {
				yBot = yTop + 1;
			}

			//get color value at surrounding pixels in original image
			image->readPixel(xLeft, yTop, topLeftVal, topLeftVal, topLeftVal); //top left value
			image->readPixel(xRight, yTop, topRightVal, topRightVal, topRightVal); //top right value
			image->readPixel(xLeft, yBot, botLeftVal, botLeftVal, botLeftVal); //bot left value
			image->readPixel(xRight, yBot, botRightVal, botRightVal, botRightVal); //bot right value

			//get weight in the y direction
			weightY = (scaledY / scale) - yTop;

			//get color value on left border at the y value
			leftBorderVal = (weightY * (botLeftVal - topLeftVal)) + topLeftVal;
			//get color value on right border at the y value
			rightBorderVal = (weightY * (botRightVal - topRightVal)) + topRightVal;

			//get weight in the x direction
			weightX = (scaledX / scale) - xLeft;

			//get color between the two weighted border values
			finalVal = (weightX * (rightBorderVal - leftBorderVal)) + leftBorderVal;

			scaledImage->writePixel(scaledX, scaledY, finalVal, finalVal, finalVal);
		}
	}
	return scaledImage;
}

void bilateralFilterFunction(BMPImage *image, float sigma1, float sigma2, int width) {

	const float PI = 3.1415927;
	const float k = (1 / (sigma2 * sqrt(2 * PI)));
	float gValDist = 0.0f;
	float gValIntense = 0.0f;
	float pixelReadVal = 0.0f;
	float pixelWriteVal = 0.0f;
	float neighborVal = 0.0f;
	float normalizationSum = 0.0f;
	float neighborReadVal = 0.0f;
	float intenseDiff = 0.0f;

	for (int x = 0; x < image->getXSize(); x++) {
		for (int y = 0; y < image->getYSize(); y++) {
			
			image->readPixel(x, y, pixelReadVal, pixelReadVal, pixelReadVal); //get intensity at the pixel

			normalizationSum = 0.0f;
			neighborVal = 0.0f;
			pixelWriteVal = 0.0f;

			float pVal = 0.0f;
			int weightSum = 0.0f;
			for (int i = x - width; i < x + width; i++) {
				for (int j = y - width; j < y + width; j++) {
					weightSum += gaussianWeight(image, x, y, i, j, sigma1, sigma2);
				}
			}
			// ZY 添加参数
			float i2;
			int l = image->getYSize();

			for (int i = x - width; i < x + width; i++) {
				for (int j = y - width; j < y + width; j++) {
					float nVal = 0.0f;

					if (k <= 0 && l <= 0) {
						// 后面三个参数是 R G B,两种
						image->readPixel(0, 0, i2, i2, i2);
					}
					else if (k >= image->getXSize() - 1 && l <= 0) {
						image->readPixel(image->getXSize() - 1, 0, i2, i2, i2);
					}
					else if (k <= 0 && l >= image->getYSize() - 1) {
						image->readPixel(0, image->getYSize() - 1, i2, i2, i2);
					}
					else if (k >= image->getXSize() - 1 && l >= image->getYSize() - 1) {
						image->readPixel(image->getXSize() - 1, image->getYSize() - 1, i2, i2, i2);
					}
					else if (k <= 0) {
						image->readPixel(0, l, i2, i2, i2);
					}
					else if (k >= image->getXSize() - 1) {
						image->readPixel(image->getXSize() - 1, l, i2, i2, i2);
					}
					else if (l <= 0) {
						image->readPixel(k, 0, i2, i2, i2);
					}
					else if (l >= image->getYSize() - 1) {
						image->readPixel(k, image->getYSize() - 1, i2, i2, i2);
					}
					else {
						image->readPixel(k, l, i2, i2, i2);
					}
				}
			}

			

			/*
			//go through neighboring pixels
			for (int i = -1 * width; i <= width; i++) {
				
				int offsetXCoord = x + i;

				for (int j = -1 * width; j <= width; j++) {

					int offsetYCoord = y + j;

					gValDist = (1 / (sigma1 * sqrt(2 * PI))) * exp(-1 * (i * i) / (2 * sigma1 * sigma1)) * exp(-1 * (j * j) / (2 * sigma1 * sigma1));


					
					if (offsetXCoord <= 0 && offsetYCoord <= 0) {
						//top left value
						image->readPixel(0, 0, neighborReadVal, neighborReadVal, neighborReadVal);
						intenseDiff = fabsf(pixelReadVal - neighborReadVal);
						gValIntense = k * exp(-1 * (intenseDiff * intenseDiff) / (2 * sigma2 * sigma2));
						neighborVal = neighborReadVal * gValDist * gValIntense;
						normalizationSum += gValDist * gValIntense;
					}
					else if (offsetXCoord >= image->getXSize() - 1 && offsetYCoord <= 0) {
						//top right value
						image->readPixel(image->getXSize() - 1, 0, neighborReadVal, neighborReadVal, neighborReadVal);
						intenseDiff = fabsf(pixelReadVal - neighborReadVal);
						gValIntense = k * exp(-1 * (intenseDiff * intenseDiff) / (2 * sigma2 * sigma2));
						neighborVal = neighborReadVal * gValDist * gValIntense;
						normalizationSum += gValDist * gValIntense;
					}
					else if (offsetXCoord <= 0 && offsetYCoord >= image->getYSize() - 1) {
						//bot left value
						image->readPixel(0, image->getYSize() - 1, neighborReadVal, neighborReadVal, neighborReadVal);
						intenseDiff = fabsf(pixelReadVal - neighborReadVal);
						gValIntense = k * exp(-1 * (intenseDiff * intenseDiff) / (2 * sigma2 * sigma2));
						neighborVal = neighborReadVal * gValDist * gValIntense;
						normalizationSum += gValDist * gValIntense;
					}
					else if (offsetXCoord >= image->getXSize() - 1 && offsetYCoord >= image->getYSize() - 1) {
						//bot right value
						image->readPixel(image->getXSize() - 1, image->getYSize() - 1, neighborReadVal, neighborReadVal, neighborReadVal);
						intenseDiff = fabsf(pixelReadVal - neighborReadVal);
						gValIntense = k * exp(-1 * (intenseDiff * intenseDiff) / (2 * sigma2 * sigma2));
						neighborVal = neighborReadVal * gValDist * gValIntense;
						normalizationSum += gValDist * gValIntense;
					}
					else if (offsetXCoord <= 0) {
						//use left most value in row
						image->readPixel(0, offsetYCoord, neighborReadVal, neighborReadVal, neighborReadVal);
						intenseDiff = fabsf(pixelReadVal - neighborReadVal);
						gValIntense = k * exp(-1 * (intenseDiff * intenseDiff) / (2 * sigma2 * sigma2));
						neighborVal = neighborReadVal * gValDist * gValIntense;
						normalizationSum += gValDist * gValIntense;
					}
					else if (offsetXCoord >= image->getXSize() - 1) {
						//use right most value in row
						image->readPixel(image->getXSize() - 1, offsetYCoord, neighborReadVal, neighborReadVal, neighborReadVal);
						intenseDiff = fabsf(pixelReadVal - neighborReadVal);
						gValIntense = k * exp(-1 * (intenseDiff * intenseDiff) / (2 * sigma2 * sigma2));
						neighborVal = neighborReadVal * gValDist * gValIntense;
						normalizationSum += gValDist * gValIntense;
					}
					else if (offsetYCoord <= 0) {
						//use top most value in column
						image->readPixel(offsetXCoord, 0, neighborReadVal, neighborReadVal, neighborReadVal);
						intenseDiff = fabsf(pixelReadVal - neighborReadVal);
						gValIntense = k * exp(-1 * (intenseDiff * intenseDiff) / (2 * sigma2 * sigma2));
						neighborVal = neighborReadVal * gValDist * gValIntense;
						normalizationSum += gValDist * gValIntense;
					}
					else if (offsetYCoord >= image->getYSize() - 1) {
						//use bottom most value in column
						image->readPixel(offsetXCoord, image->getYSize() - 1, neighborReadVal, neighborReadVal, neighborReadVal);
						intenseDiff = fabsf(pixelReadVal - neighborReadVal);
						gValIntense = k * exp(-1 * (intenseDiff * intenseDiff) / (2 * sigma2 * sigma2));
						neighborVal = neighborReadVal * gValDist * gValIntense;
						normalizationSum += gValDist * gValIntense;
					}
					else {
						//use value in image
						image->readPixel(offsetXCoord, offsetYCoord, neighborReadVal, neighborReadVal, neighborReadVal);
						intenseDiff = fabsf(pixelReadVal - neighborReadVal);
						gValIntense = k * exp(-1 * (intenseDiff * intenseDiff) / (2 * sigma2 * sigma2));
						neighborVal = neighborReadVal * gValDist * gValIntense;
						normalizationSum += gValDist * gValIntense;
					}
					
					pixelWriteVal += neighborVal;
					
				}
			}
			
			pixelWriteVal /= normalizationSum;
			*/

			image->writePixel(x, y, pixelWriteVal, pixelWriteVal, pixelWriteVal);
		}
	}
}

float gaussianWeight(BMPImage *image, int i, int j, int k, int l, float sigma1, float sigma2) {
	float i1 = 0.0f, i2 = 0.0f;

	if (k <= 0 && l <= 0) {
		image->readPixel(0, 0, i2, i2, i2);
	}
	else if (k >= image->getXSize() - 1 && l <= 0) {
		image->readPixel(image->getXSize() - 1, 0, i2, i2, i2);
	}
	else if (k <= 0 && l >= image->getYSize() - 1) {
		image->readPixel(0, image->getYSize() - 1, i2, i2, i2);
	}
	else if (k >= image->getXSize() - 1 && l >= image->getYSize() - 1) {
		image->readPixel(image->getXSize() - 1, image->getYSize() - 1, i2, i2, i2);
	}
	else if (k <= 0) {
		image->readPixel(0, l, i2, i2, i2);
	}
	else if (k >= image->getXSize() - 1) {
		image->readPixel(image->getXSize() - 1, l, i2, i2, i2);
	}
	else if (l <= 0) {
		image->readPixel(k, 0, i2, i2, i2);
	}
	else if (l >= image->getYSize() - 1) {
		image->readPixel(k, image->getYSize() - 1, i2, i2, i2);
	}
	else {
		image->readPixel(k, l, i2, i2, i2);
	}
	image->readPixel(i, j, i1, i1, i1);
	return exp(-1.0f * (((powf((i - k),2) + powf((j - l),2))/(2 * powf(sigma1,2))) - ((powf((i1 - i2),2))/(2 * powf(sigma2,2)))));
}

//argv[0] - program name
//argv[1] - read image file name
//argv[2] - write image file name
//argv[3] - function number
//argv[4] - value (sigma or box filter dimension)
int main(int argc, char** argv) {

	BMPImage image = BMPImage(); //new image to reflect greyscale and any filters applied
	BMPImage *tempImage = NULL;
	int functionNumber = 0;
	float filterVal = 0.0f;
	float sigma1 = 0.0f;
	float sigma2 = 0.0f;
	float scale = 0.0f;
	float *filter = NULL;
	int filterDimen = 0;
	int width = 0;

	if (argc < 3) {
		cout << "Incorrect usage - proper usage: " << argv[0] << " inputFile outputFile\n";
		return 0;
	}
	
	cout << "Which function would you like to run?\n1. Greyscale function\n2. Correlation function\n3. Gaussian filter function\n";
	cout << "4. Sharpening function\n5. Resize function\n6. Bilateral filter function\n\n";
	cout << "Function number: ";
	cin >> functionNumber;

	cout << "\nLoading input image...\n";
	image.load(argv[1]);
	cout << "Applying greyscale function...\n";
	greyscaleFunction(&image); //apply greyscale function

	switch (functionNumber) {
	case 1:
		//greyscale applied, we're done
		break;
	case 2:
		//get filter value and dimension
		float num, denom;
		cout << "This function assumes a box filter with same value everywhere in the filter\n";
		cout << "Filter value numerator: ";
		cin >> num;
		cout << "Filter value denominator: ";
		cin >> denom;
		filterVal = (num / denom);
		cout << "Dimension of filter (assumes nxn - integer value): ";
		cin >> filterDimen;
		//apply box filter to image
		cout << "Correlating filter onto image...\n";
		correlationFunction(&image, filterVal, filterDimen);
		break;
	case 3:
		cout << "Sigma (float value): ";
		cin >> sigma1;
		//get filter and dimensions of the filter
		cout << "Creating Gaussian filter with sigma...\n";
		filter = filterGaussianFunction(sigma1, &filterDimen);
		//apply filter to the image
		cout << "Correlating Gaussian filter onto image...\n";

		//correlationFunction(&image, filter, filterDimen);
		correlationFunction(&image, filter[0], filterDimen);
		delete[] filter;
		filter = NULL;
		break;
	case 4:
		//get sigma value
		cout << "Sigma (float value): ";
		cin >> sigma1;
		//get gaussian filter
		cout << "Creating Gaussian filter with sigma...\n";
		filter = filterGaussianFunction(sigma1, &filterDimen);
		//get sharpening filter (T-G)
		cout << "Creating sharpening filter with Gaussian filter...\n";
		filter = filterSharpeningFunction(sigma1, filter, filterDimen);
		//apply filter
		cout << "Correlating sharpening filter onto image...\n";
		// correlationFunction(&image, filter[0], filterDimen);
		correlationFunction(&image, filter[0], filterDimen);
		delete[] filter;
		filter = NULL;
		break;
	case 5:
		//get the scale value
		cout << "Scale (float value): ";
		cin >> scale;
		cout << "Resizing image...\n";
		//resize the image
		tempImage = resizeFunction(&image, scale, argv[2]);
		cout << "Saving image to output...\n";
		tempImage->save(argv[2]);
		tempImage->~BMPImage();
		tempImage = NULL;
		break;
	case 6:
		//get the sigma values
		cout << "Sigma1 (float value): ";
		cin >> sigma1;
		cout << "Sigma2 (float value): ";
		cin >> sigma2;
		cout << "Width (integer value): ";
		cin >> width;
		//apply bilateral filter function
		cout << "Applying Bilateral filter onto image...\n";
		bilateralFilterFunction(&image, sigma1, sigma2, width);
		break;
	}
	if (functionNumber != 5) {
		cout << "Saving image to output...\n";
		image.save(argv[2]);
	}
	cout << "Done!\n";
}
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
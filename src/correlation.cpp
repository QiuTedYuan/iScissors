#include "correlation.h"

/************************ TO BE DONE 2 **************************/
/*
 *	INPUT: 
 *		origImg:		the original image, 
 *		imgWidth:		the width of the image
 *		imgHeight:		the height of the image
 *						the image is arranged such that 
 *						origImg[3*(row*imgWidth+column)+0], 
 *						origImg[3*(row*imgWidth+column)+1], 
 *						origImg[3*(row*imgWidth+column)+2]
 *						are R, G, B values for pixel at (column, row).
 *
 *      kernel:			the 2D filter kernel,
 *		knlWidth:		the width of the kernel
 *		knlHeight:		the height of the kernel
 *		
 *		scale, offset:  after correlating the kernel with the origImg, 
 *						each pixel should be divided by scale and then added by offset
 *      
 *		selection:      a byte array of the same size as the image, 
 *						indicating where in the original image should be filtered, e.g., 
 *						selection[k] == 1 ==> pixel k should be filtered
 *                      selection[k] == 0 ==> pixel k should NOT be filtered
 *                      a special case is selection is a NULL pointer, which means all the pixels should be filtered. 
 *
 *  OUTPUT:
 *		rsltImg:		the filtered image of the same size as original image.
 *						it is a valid pointer ( allocated already ).
 */

void image_filter(double *rsltImg, const unsigned char *origImg, const unsigned char *selection, 
			int imgWidth, int imgHeight, 
			const double *kernel, int knlWidth, int knlHeight,
			double scale, double offset)
{
	//copy origImg to rsltImg is NOT the solution, it does nothing!, 
	//you need to figure sth out.
	//printf("image_filter: to be done in correlation.cpp\n");

	for (int i = 0; i < imgHeight; ++i) {
		for (int j = 0; j < imgWidth; ++j) {
			double result[3] = { 0.0,0.0,0.0 };	
			if (selection == NULL || selection[3 * (i*imgWidth + j)] == 1) {
				double result[3] = { 0.0,0.0,0.0 };
				pixel_filter(result, j, i, origImg, imgWidth, imgHeight, kernel, knlWidth, knlHeight, scale, offset);
				for (int k = 0; k < 3; ++k) {
					rsltImg[3 * (i*imgWidth + j) + k] = result[k];
				}
			}
			else {
				for (int k = 0; k < 3; ++k) {
					rsltImg[3 * (i*imgWidth + j) + k] = origImg[3 * (i*imgWidth + j) + k];
				}
			}

		}
	}
}

/************************ END OF TBD 2 **************************/


/************************ TO BE DONE 3 **************************/
/*
 *	INPUT: 
 *      x:				a column index,
 *      y:				a row index,
 *		origImg:		the original image, 
 *		imgWidth:		the width of the image
 *		imgHeight:		the height of the image
 *						the image is arranged such that 
 *						origImg[3*(row*imgWidth+column)+0], 
 *						origImg[3*(row*imgWidth+column)+1], 
 *						origImg[3*(row*imgWidth+column)+2]
 *						are R, G, B values for pixel at (column, row).
 *
 *      kernel:			the 2D filter kernel,
 *		knlWidth:		the width of the kernel
 *		knlHeight:		the height of the kernel
 *
 *		scale, offset:  after correlating the kernel with the origImg, 
 *						the result pixel should be divided by scale and then added by offset
 *
 *  OUTPUT:
 *		rsltPixel[0], rsltPixel[1], rsltPixel[2]:		
 *						the filtered pixel R, G, B values at row y , column x;
 */

void pixel_filter(double rsltPixel[3], int x, int y, const unsigned char *origImg, int imgWidth, int imgHeight, 
			  const double *kernel, int knlWidth, int knlHeight,
			  double scale, double offset)
{

	//printf("pixel_filter: to be done in correlation.cpp\n");
	if (knlWidth % 2 != 1 || knlHeight % 2 != 1) {
		printf("pixel_filter assumes an odd matrix!");
		return;
	}
	//i is the row# and j is the col# in the kernel
	for (int i = 0; i < knlHeight; ++i) {
		for (int j = 0; j < knlWidth; ++j) {
			int xCord = x - knlWidth / 2 + j;
			int yCord = y - knlHeight / 2 + i;
			//k iterate through RGB
			for (int k = 0; k < 3; ++k) {
				double knlV = kernel[i*knlWidth + j];
				double origV = 0.0;
				if ((xCord >= 0) && (xCord < imgWidth) && (yCord >= 0) && (yCord < imgHeight)) {
					origV = origImg[3 * (yCord*imgWidth + xCord) + k];
				}
				rsltPixel[k] += origV*knlV;
			}
		}
	}
	for (int i = 0; i < 3; ++i) {
		rsltPixel[i] = rsltPixel[i] * scale + offset;
	}

}

/************************ END OF TBD 3 **************************/


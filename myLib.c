/*@file myLib.c
 *@author Nick Liccini
 */

#include "myLib.h"

/* Initialize External Variables */
volatile unsigned short *videoBuffer = (unsigned short *) 0x6000000;

/* waitForVblank
* Wait until the scanline counter is in the vblank zone
*/
void waitForVblank() {
	// Wait until the scanline is in the vblank zone to do computations
	while (SCANLINECOUNTER > GBA_SCREEN_HEIGHT);
	while (SCANLINECOUNTER < GBA_SCREEN_HEIGHT);
}

/* setPixel
* Set a pixel on the screen at (x, y)
*/
void setPixel(int x, int y, u16 pixel) {
	videoBuffer[OFFSET(y, x, GBA_SCREEN_WIDTH)] = pixel;
}

/* drawFilledRectangle
* Draw a solid rectangle on the screen with the top left corner at (x ,y)
*/
void drawFilledRectangle(int x, int y, int width, int height, u16 pixel) {
	for (int row = 0; row < height; ++row) {
		DMA[3].src = &pixel;
		DMA[3].dst = &videoBuffer[OFFSET(y + row, x, GBA_SCREEN_WIDTH)];
		DMA[3].cnt = (width) | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_FIXED;
	}
}

/* drawimage3
* A function that will draw an arbitrary sized image
* onto the screen (with DMA).
* @param r row to draw the image
* @param c column to draw the image
* @param width width of the image
* @param height height of the image
* @param image Pointer to the first element of the image.
*/
void drawImage3 (int r, int c, int width, int height, const u16* image) {
	for (int row = 0; row < height; ++row) {
		// Put the specific image pixel in the DMA Source Register
		DMA[3].src = &image[OFFSET(row, 0, width)];
		// Put the next draw location in the DMA Destination Register
		DMA[3].dst = &videoBuffer[OFFSET(r + row, c, GBA_SCREEN_WIDTH)];
		// Tell the DMA to draw (width) pixels, incrementing from the source and dest
		DMA[3].cnt = (width) | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_INCREMENT;
	}
}

/* drawImageFlippedLR
* A function that will draw an arbitrary sized image and flip it vertically
* onto the screen (with DMA).
* @param r row to draw the image
* @param c column to draw the image
* @param width width of the image
* @param height height of the image
* @param image Pointer to the first element of the image.
*/
void drawImageFlippedLR(int row, int col, int width, int height, const u16* image) {
    for (int r = 0; r < height; r++) {
        DMA[3].src = &image[r * width + width - 1];
        DMA[3].dst = &videoBuffer[(row + r) * 240 + col];
        DMA[3].cnt = (width) | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_DECREMENT;
    }
}
/*@file text.h
 *@author Nick Liccini
 */

#ifndef TEXT_H
#define TEXT_H

/* External Variables */
extern const unsigned char fontdata_6x8[12288];

/* Prototypes */
void drawChar(int row, int col, char ch, u16 pixel);
void drawString(int row, int col, char *str, u16 color);

#endif
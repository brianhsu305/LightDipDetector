// segDisplay.h
// Module to display the number of dips on 14-segment display
#ifndef _DISPLAYDIGITS_H_
#define _DISPLAYDIGITS_H_

// Called to Turn off all lights on the 14 Segment Display
void Seg_turnOffSegDisplay(void);

// Called to Display Number of Dips on the 14 Segment Display
void Seg_displayDigits(int, char *);

// Called to Initialize the Segment Display
// Configure the I2C and GPIO initialization Setup
void Seg_init(void);


#endif
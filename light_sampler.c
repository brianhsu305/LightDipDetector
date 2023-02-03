// The will be the main program
#include <stdio.h>
#include "sampler.h"


int main(int argc, char* args[]) {
    // Tasks:
    // Analog to digital (A2D) reading (potentiometer and light sensor)
    int reading = getVoltage1Reading();
    printf("Value %5d \n", reading); 
    // Output on a 14-segment display using I2C
    // UDP networking
    // pthread threads and thread synchronization
    // C-programming with malloc() / free()
    return 0;
}
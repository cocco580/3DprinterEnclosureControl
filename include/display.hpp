#ifndef DISPLAY_OLED_hpp
#define DISPLAY_OLED_hpp

//#include <Arduino.h>

void beginDisplayPV(void);
void printDisplayPV(float, float, int, float, float, float, float);
void beginDisplayFAN(void);
void printDisplayFAN_AUTO(float, int);
void printDisplayFAN_MAN(int);
void printDisplayFAN_STOP(void);

#endif
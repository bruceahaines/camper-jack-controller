#ifndef GLOBAL_DEFINES
#define GLOBAL_DEFINES

#define DEBUG // Comment out this line to disable debug serial prints

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTF(x) Serial.printf(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(x)
#endif

// Define GPIO for ESP32
// Left side of DOIT Devkit v1 as seen aligned with top writing.
#define LF_UP_PIN 27 // Left Front Up. Up corresponds to +12V on the RED wire (control board).
#define LF_DOWN_PIN 14
#define RF_UP_PIN 12
#define RF_DOWN_PIN 13
// Right side of DOIT Devkit v1 as seen aligned with top writing.
#define LR_UP_PIN 21
#define LR_DOWN_PIN 19
#define RR_UP_PIN 18
#define RR_DOWN_PIN 5

#endif /*GLOBAL_DEFINES*/
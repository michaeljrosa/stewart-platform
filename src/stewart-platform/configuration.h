#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// control/feedback pins
#define IN1_A1  32
#define IN2_A1  33
#define ENA_A1   8
#define FEED_A1 A0  

#define IN1_A2  30
#define IN2_A2  31
#define ENA_A2   9
#define FEED_A2 A1

#define IN1_B1  28
#define IN2_B1  29
#define ENA_B1  10
#define FEED_B1 A2

#define IN1_B2  26
#define IN2_B2  27
#define ENA_B2  11
#define FEED_B2 A3

#define IN1_C1  24
#define IN2_C1  25
#define ENA_C1  12
#define FEED_C1 A4

#define IN1_C2  22
#define IN2_C2  23
#define ENA_C2  13
#define FEED_C2 A5

// constants for sensor readings/movement
#define SMOOTH      5  // smoothing factor, 5-50
#define TOLERANCE  30  // 3% of 1024, the resolution of the ADC

// other constants
#define NUM_ACTUATORS 6
#define NUM_CALIB_STAGES 9
#define LENGTH 12  // inches
#define SPEED 0.5  // inches/sec
#define CALIB_SPEED_RATIO 0.25 // calibration speed / full speed

#endif

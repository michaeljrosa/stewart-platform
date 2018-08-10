#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// verbose debug output
#define VERBOSE     0  // 0, not verbose; >0 verbose

// constants for sensor readings/movement
#define SMOOTH      5  // smoothing factor, 5-50
#define TOLERANCE  30  // 3% of 1024, can probably be lowered moving foward with speed control

#define LENGTH 12  // inches
#define SPEED 0.5  // inches/sec
#define CALIB_SPEED_RATIO 0.25    // calibration speed relative to full speed
#define SECOND_PROBE_LENGTH 0.1   // how much the actuator should retract/extend for the second calibration probe (0.1-0.25)
#define EXTRA_SECONDS 0.5         // if the actuators aren't reaching the limits, increase this value until they do

// calibration settings storage
#define EEPROM_VERSION 1 // 0 to calibrate every time, 1 to load from EEPROM
#define MAX_CYCLES 10    // how many cycles before recalibrating

#define ADDR_VERSION  0                    // 1 byte, eeprom version
#define ADDR_CYCLES   (ADDR_VERSION + 1)   // 1 byte, how many cycles have passed

#define ADDR_CALIB_A1 (ADDR_CYCLES + 1)    // calibration settings, 2 16 bit integers (4 bytes) each
#define ADDR_CALIB_A2 (ADDR_CALIB_A1 + 4) 
#define ADDR_CALIB_B1 (ADDR_CALIB_A2 + 4) 
#define ADDR_CALIB_B2 (ADDR_CALIB_B1 + 4) 
#define ADDR_CALIB_C1 (ADDR_CALIB_B2 + 4) 
#define ADDR_CALIB_C2 (ADDR_CALIB_C1 + 4)

#define ADDR_CRC      (ADDR_CALIB_C2 + 4)  // cyclic redundancy check to see if EEPROM was changed, 1 long (4 bytes)


// control/feedback pins
// less likely to need to be changed
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

// other constants
#define NUM_ACTUATORS 6
#define NUM_CALIB_STAGES 9

#endif

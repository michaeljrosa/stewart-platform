/**
 * stewart-platform
 * 
 * Authors: Leroy Sibanda, Michael Rosa
 * Date: August 2018
 * 
 * Program to test and calibrate an Stewart platform based on an Arduino Mega and motor drivers
 */

#include "configuration.h"
#include "Actuator.h"
#include "StewartPlatform.h"

// graphing 
#define GRAPH 0  // 0, don't graph; >0, graph
#define RAW   0  // 0, filtered data; >0, raw data
#define WHICH_ACTUATOR 0  // which actuator to graph data from, 0-5

bool isGoingUp = true;
double  high[] = {0.9, 0.9, 0.9, 0.9, 0.9, 0.9};
double  low[] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
double  mid[] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};

// ------ main program ------ //
StewartPlatform platform;
  
void setup() {
  Serial.begin(9600);

  // set the analog ref (5V regulator)
  analogReference(EXTERNAL);

  platform.setup();

  Serial.println("Actuator info");
  for (int i = 0; i < NUM_ACTUATORS; i++) {
    Serial.print("Actuator "); Serial.print(i); 
    Serial.print(" position: ");
    Serial.println(platform.getActuatorPosition(i));
    Serial.print("min: "); 
    Serial.println(platform.getActuatorMinPosition(i));
    Serial.print("max: "); 
    Serial.println(platform.getActuatorMaxPosition(i));
    Serial.print("target: ");
    Serial.println(platform.getActuatorTarget(i));
    Serial.print("ready: ");
    Serial.println(platform.getActuatorReady(i));
    Serial.println();
  } 
}

void loop() {
  platform.loop();

  #if GRAPH > 0
    #if (WHICH_ACTUATOR >= 0 && WHICH_ACTUATOR < NUM_ACTUATORS) 
      #if RAW > 0
        Serial.println(platform.getActuatorRawPosition(WHICH_ACTUATOR));
      #else
        Serial.println(platform.getActuatorPosition(WHICH_ACTUATOR));
      #endif
    #endif
  #endif

  /*if (platform.getPlatformReady()) {
        if(isGoingUp) {
          platform.setPlatformLengths(high);
          isGoingUp = false;
        } else {
          platform.setPlatformLengths(low);
          isGoingUp = true;
        }
   }*/
   
   if (platform.getPlatformReady()) {
    platform.setPlatformLengths(mid);
   }
}

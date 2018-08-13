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
#include "eeprom.h"


#define MAX_COUNT 5
int count = 0;
bool finished = false;
bool positionFlag = true;
double high[] = {0.9, 0.9, 0.9, 0.9, 0.9, 0.9};
double low[] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
double mid[] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};


// ------ main program ------ //
StewartPlatform platform;
  
void setup() {
  Serial.begin(9600);

  // set the analog ref (5V regulator)
  analogReference(EXTERNAL);

  platform.setup();

  #if VERBOSE > 0
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
  #endif

  #if GRAPH > 0
    // print the min and max positions of each actuator
    for (int i = 0; i < NUM_ACTUATORS; i++) {
      Serial.print(platform.getActuatorMaxPosition(i));
      Serial.print(',');
    }
    Serial.println();
    for (int i = 0; i < NUM_ACTUATORS; i++) {
      Serial.print(platform.getActuatorMinPosition(i));
      Serial.print(',');
    }
    Serial.println();
    Serial.println();
  #endif
  
  platform.setPlatformLengths(mid);
}

void loop() {
  platform.loop();

  #if GRAPH > 0
    // print the current raw and filtered positions of each actuator
    for (int i = 0; i < NUM_ACTUATORS; i++) {
      Serial.print(platform.getActuatorPosition(i));
      Serial.print(',');
      Serial.print(platform.getActuatorRawPosition(i));
      Serial.print(',');
    } 
    Serial.println();
  #endif

  if (!finished) {
    if (platform.isPlatformReady() && count < MAX_COUNT) {
      if(positionFlag) {
        platform.setPlatformLengths(high);
      } else {
        platform.setPlatformLengths(low);
      }
      positionFlag = !positionFlag;
      count++;
     } else if (count >= MAX_COUNT && platform.isPlatformReady()) {
      platform.setPlatformLengths(mid);
      finished = true;
     }
  }
}

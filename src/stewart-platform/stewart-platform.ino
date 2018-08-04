/**
 * stewart-platform
 * 
 * Authors: Leroy Sibanda, Michael Rosa
 * Date: August 2018
 * 
 * Program to test and calibrate an Stewart platform based on an Arduino Micro and relays
 * Will be updated in the future to use an Arduino Mega and motor drivers, with  the
 * additional functionality that those entail (e.g. PWM)
 */

// ------ constants and defines ------ //
// graphing 
#define IS_GRAPHING true  // graph sensor values?
#define IS_RAW_DATA true  // graph the raw sensor data? if not, will graph filtered
#define WHICH_ACTUATOR 1  // which actuator to graph data from, 1-6

// control pins
#define FORW_1  12  // 1st linear actuator forward enable
#define BACK_1  11  // 1st linear actuator backward enable
#define FORW_2  10  // 2nd linear actuator forward enable
#define BACK_2   9  // 2nd linear actuator backward enable
#define FORW_3   8  // 3rd linear actuator forward enable
#define BACK_3   7  // 3rd linear actuator backward enable
#define FORW_4   6  // 4th linear actuator forward enable
#define BACK_4   5  // 4th linear actuator backward enable
#define FORW_5   4  // 5th linear actuator forward enable
#define BACK_5   3  // 5th linear actuator backward enable
#define FORW_6   0  // 6th linear actuator forward enable
#define BACK_6   1  // 6th linear actuator backward enable

// feedback pins
#define FEED_1  A0  // 1st linear actuator feedback
#define FEED_2  A1  // 2nd linear actuator feedback
#define FEED_3  A2  // 3rd linear actuator feedback
#define FEED_4  A3  // 4th linear actuator feedback
#define FEED_5  A4  // 5th linear actuator feedback
#define FEED_6  A5  // 6th linear actuator feedback

// constants for sensor readings/movement
#define SMOOTH      5  // smoothing factor, 5-50
#define TOLERANCE  30  // 3% of 1024, the resolution of the ADC

// other constants
#define NUM_CALIB_STAGES 9
#define NUM_ACTUATORS 6


// ------ classes ------ //
class Actuator {
  private:
    const byte extendPin;
    const byte retractPin;
    const byte feedbackPin;
    
    int maxPosition;
    int minPosition;
    bool isCalibrated;
    int calibrationStage;
    
    bool isReady;
    int rawPosition;
    int filtPosition;
    int targetPosition;
    int readings[SMOOTH];
    int index;
    int total;
    
    void extend() {
      digitalWrite(extendPin, LOW);
      digitalWrite(retractPin, HIGH);
    }

    void retract() {
      digitalWrite(extendPin, HIGH);
      digitalWrite(retractPin, LOW);
    }

    void halt() {
      digitalWrite(extendPin, HIGH);
      digitalWrite(retractPin, HIGH);
    }

    void readPosition() {
      total -= readings[index];                // subtract the last value
      rawPosition = analogRead(feedbackPin);   // read the sensor
      readings[index] = rawPosition;     // record the reading
      total += rawPosition;              // add the value 
      filtPosition = total / SMOOTH;     // average reading and update the filtered position
      
      if (index++ >= SMOOTH) {  // reset index if at end of array
        index = 0;
      }
    }

  public:    
    Actuator(byte attachToExtendPin, byte attachToRetractPin, byte attachToFeedbackPin) :
      extendPin(attachToExtendPin),
      retractPin(attachToRetractPin),
      feedbackPin(attachToFeedbackPin) {
         
      isCalibrated = false;
      calibrationStage = 0; 
      maxPosition = 0;
      minPosition = 0;
      
      isReady = false;
      memset(readings, 0, sizeof(readings));
      index = 0;
      total = 0;
      targetPosition = 512;
      readPosition();
    }
    
    void setup() {
      pinMode(extendPin, OUTPUT);
      pinMode(retractPin, OUTPUT);
      pinMode(feedbackPin, INPUT);
      halt();
    }
    
    void loop() {
      readPosition();
      if (isCalibrated && !isReady) {
        if (filtPosition <= targetPosition + TOLERANCE || filtPosition >= targetPosition - TOLERANCE) {
          halt();
          isReady = true;
        } else if (filtPosition > targetPosition + TOLERANCE) {
          retract();
        } else if (filtPosition < targetPosition - TOLERANCE) {
          extend();
        }
      }
    }

    /* calibration stages
       0 - extend for initial max 
       1 - set initial max 
       2 - retract for final max
       3 - extend for final max
       4 - set final max
       5 - retract for initial min
       6 - set initial min
       7 - extend for final min
       8 - retract for final min
       9+ - set final min, done   */
    bool calibrate() {
      switch (calibrationStage) {
        case 0: 
        case 3:
        case 7: extend();
                break;

        case 2:
        case 5:
        case 8: retract();
                break;
                
        case 1:
        case 4:
        case 6:
        case 9: for(int i = 0; i < SMOOTH; i++)
                  readPosition();
                halt();
                break;
        
        default:break;
      }

      switch (calibrationStage) {
        case 1: maxPosition = filtPosition;
                break;

        case 4: maxPosition = min(maxPosition, filtPosition);
                break;

        case 6: minPosition = filtPosition;
                break;

        case 9: minPosition = max(minPosition, filtPosition);
                break;

        default: break;
      }
      if(calibrationStage++ > NUM_CALIB_STAGES) {
        calibrationStage = 0;
        isCalibrated = true;
        isReady = true;
      }
      
      return isCalibrated;
    }

    void goToPosition(double relativePosition) {
      if (isCalibrated && isReady) {
        isReady = false;
        targetPosition = minPosition + (maxPosition - minPosition) * relativePosition;
      }
    }

    bool isActuatorReady() {
      return isReady;
    }

    int getRawPosition() {
      return rawPosition;
    }

    int getPosition() {
      return filtPosition;
    }

    int getTargetPosition() {
      return targetPosition;
    }

    int getMaxPosition() {
      return maxPosition;
    }

    int getMinPosition() {
      return minPosition;
    }
};


class StewartPlatform {
  bool isCalibrated;
  Actuator actuators[NUM_ACTUATORS] = {
    Actuator(FORW_1, BACK_1, FEED_1),
    Actuator(FORW_2, BACK_2, FEED_2),
    Actuator(FORW_3, BACK_3, FEED_3),
    Actuator(FORW_4, BACK_4, FEED_4),
    Actuator(FORW_5, BACK_5, FEED_5),
    Actuator(FORW_6, BACK_6, FEED_6),
  };
  
  public:
    StewartPlatform() {
      isCalibrated = false;
    }

    void setup() {
      for (int i = 0; i < NUM_ACTUATORS; i++) {
        actuators[i].setup();
      }
      calibrate();
    }

    void loop() {
       for (int i = 0; i < NUM_ACTUATORS; i++) {
          actuators[i].loop();
       }
    }

    void calibrate() {
      for (int stage = 0; stage < NUM_CALIB_STAGES; stage++) {
        for (int i = 0; i < NUM_ACTUATORS; i++) {
          actuators[i].calibrate();
        }

        switch (stage) {
          case 0: 
          case 5: delay(30000); // wait 30 secs for actuators to fully extend/retract fully
                  break;

          case 2:
          case 7: delay(3000); // wait 3 secs for actuators to extend/retract to prepare sanity check
                  break;

          case 3:
          case 8: delay(5000); // wait 5 secs for actuators to extend/retract to make sanity check
                  
          case 1: 
          case 4:
          case 6:
          case 9: delay(500); // changeover pause 
                  break;
        }
      }
      isCalibrated = true;
       for (int i = 0; i < NUM_ACTUATORS; i++) {
          actuators[i].goToPosition(0.5);
       }
    }

    int getActuatorRawPosition(int actuator) {
      if (actuatorIsValid(actuator)) {
        return actuators[actuator - 1].getRawPosition();
      } else {
        return 0;
      }
    }

    int getActuatorPosition(int actuator) {
      if (actuatorIsValid(actuator)) {
        return actuators[actuator - 1].getPosition();
      } else {
        return 0;
      }
    }

    int getActuatorMaxPosition(int actuator) {
      if (actuatorIsValid(actuator)) {
        return actuators[actuator - 1].getMaxPosition();
      } else {
        return 0;
      }
    }

    int getActuatorMinPosition(int actuator) {
      if (actuatorIsValid(actuator)) {
        return actuators[actuator - 1].getMinPosition();
      } else {
        return 0;
      }
    }

    private:
      bool actuatorIsValid(int actuator) {
        return actuator > 0 && actuator <= NUM_ACTUATORS;
      }
};


// ------ main program ------ //
StewartPlatform platform;
  
void setup() {
  Serial.begin(9600);

  // set the analog ref (5V)
  analogReference(EXTERNAL); // make sure external reference is actually connected

  platform.setup();

  if (IS_GRAPHING) {
    Serial.println(platform.getActuatorMaxPosition(WHICH_ACTUATOR));
    Serial.println(platform.getActuatorMinPosition(WHICH_ACTUATOR));
  } else {
    Serial.end();
  }
}

void loop() {
  platform.loop();

  if (IS_GRAPHING) {
    if (WHICH_ACTUATOR > 0 && WHICH_ACTUATOR <= NUM_ACTUATORS) {
      if (IS_RAW_DATA) {
        Serial.println(platform.getActuatorRawPosition(WHICH_ACTUATOR));
      } else {
        Serial.println(platform.getActuatorPosition(WHICH_ACTUATOR));
      }
    }
  }
}

/**
 * stewart-platform
 * 
 * Authors: Leroy Sibanda, Michael Rosa
 * Date: August 2018
 * 
 * Program to test and calibrate an Stewart platform based on an Arduino Mega and motor drivers
 */

// ------ constants and defines ------ //
// graphing 
#define IS_GRAPHING true  // graph sensor values?
#define IS_RAW_DATA false  // graph the raw sensor data? if not, will graph filtered
#define WHICH_ACTUATOR 0  // which actuator to graph data from, 0-5

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
#define NUM_CALIB_STAGES 9
#define NUM_ACTUATORS 6
#define LENGTH 12  // inches
#define SPEED 0.5  // inches/sec

// ------ classes ------ //
class Actuator {
  private:
    const byte in1Pin;
    const byte in2Pin;
    const byte enaPin;
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
      extend(255);
    }
    
    void extend(uint8_t pwm) {
      digitalWrite(in1Pin, HIGH);
      digitalWrite(in2Pin, LOW);
      analogWrite(enaPin, pwm);
    }

    void retract() {
      retract(255);
    }
    
    void retract(uint8_t pwm) {
      digitalWrite(in1Pin, LOW);
      digitalWrite(in2Pin, HIGH);
      analogWrite(enaPin, pwm);
    }

    void off() { // leave motor floating
      digitalWrite(in1Pin, HIGH);
      digitalWrite(in2Pin, HIGH);
      digitalWrite(enaPin, LOW);
    }

    void brake() {
      digitalWrite(in1Pin, LOW);
      digitalWrite(in2Pin, LOW);
      digitalWrite(enaPin, LOW);
    }

    void readPosition() {
      total -= readings[index];                // subtract the last value
      rawPosition = analogRead(feedbackPin);   // read the sensor
      readings[index] = rawPosition;     // record the reading
      total += readings[index];              // add the value
      filtPosition = total / SMOOTH;     // average reading and update the filtered position

      index++;
      if (index >= SMOOTH) {  // reset index if at end of array
        index = 0;
      }
    }

  public:    
    Actuator(byte attachToIn1Pin, byte attachToIn2Pin, byte attachToEnaPin, byte attachToFeedbackPin) :
      in1Pin(attachToIn1Pin),
      in2Pin(attachToIn2Pin),
      enaPin(attachToEnaPin),
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
      pinMode(in1Pin, OUTPUT);
      pinMode(in2Pin, OUTPUT);
      pinMode(enaPin, OUTPUT);
      pinMode(feedbackPin, INPUT);
      off();
    }
    
    void loop() {
      readPosition();
      if (isCalibrated && !isReady) {
        /*
        Serial.print("Upper limit: "); Serial.println(targetPosition + TOLERANCE);
        Serial.print("Lower limit: "); Serial.println(targetPosition - TOLERANCE);
        Serial.print("Current position: "); Serial.println(filtPosition);
        Serial.print("Raw position: "); Serial.println(rawPosition);
        */
        if (filtPosition <= targetPosition + TOLERANCE && filtPosition >= targetPosition - TOLERANCE) { 
          //Serial.println("stop");
          brake();
          isReady = true;
        } else if (filtPosition > targetPosition + TOLERANCE) { 
          //Serial.println("extend"); 
          extend();
        } else if (filtPosition < targetPosition - TOLERANCE) { 
          //Serial.println("retract");
          retract();
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
        case 7: extend();
                break;

        case 3: extend(127);
                break;

        case 2:
        case 5: retract();
                break;
        
        case 8: retract(127);
                break;
                
        case 1:
        case 4:
        case 6:
        case 9: for(int i = 0; i < SMOOTH; i++)
                  readPosition();
                brake();
                break;
        
        default:break;
      }

      switch (calibrationStage) {
        case 1: minPosition = filtPosition;
                break;

        case 4: minPosition = max(minPosition, filtPosition);
                break;

        case 6: maxPosition = filtPosition;
                break;

        case 9: maxPosition = min(maxPosition, filtPosition);
                break;

        default: break;
      }
      calibrationStage++;
      if(calibrationStage > NUM_CALIB_STAGES) {
        calibrationStage = 0;
        isCalibrated = true;
        isReady = true;
      }
      
      return isCalibrated;
    }

    void setLength(double relativeLength) {
      if (isCalibrated && isReady && relativeLength >= 0 && relativeLength <= 1) {
        isReady = false;
        targetPosition = minPosition + (maxPosition - minPosition) * relativeLength;
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
  bool isReady;
  Actuator actuators[NUM_ACTUATORS] = {
    Actuator(IN1_A1, IN2_A1, ENA_A1, FEED_A1),
    Actuator(IN1_A2, IN2_A2, ENA_A2, FEED_A2),
    Actuator(IN1_B1, IN2_B1, ENA_B1, FEED_B1),
    Actuator(IN1_B2, IN2_B2, ENA_B2, FEED_B2),
    Actuator(IN1_C1, IN2_C1, ENA_C1, FEED_C1),
    Actuator(IN1_C2, IN2_C2, ENA_C2, FEED_C2),
  };
  
  public:
    StewartPlatform() {
      isCalibrated = false;
      isReady = false;
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
      for (int stage = 0; stage <= NUM_CALIB_STAGES; stage++) {
        for (int i = 0; i < NUM_ACTUATORS; i++) {
          actuators[i].calibrate();
        }

        switch (stage) {
          case 0: 
          case 5: delay(LENGTH / SPEED * 1000 + 1000); // wait for actuators to fully extend/retract
                  break;

          case 2:
          case 7: delay(0.125 * LENGTH / SPEED * 1000); // wait 2 secs for actuators to extend/retract to prepare second check
                  break;

          case 3:
          case 8: delay(0.125 * LENGTH / (SPEED * 0.5) * 1000 + 1000); // wait 5 secs for actuators to extend/retract to make second check
                  break;
                  
          default: break;
        }
      }
      isCalibrated = true;
      isReady = true;
    }

    bool setPlatformLengths(double (&lengths)[6]) {
      if (getPlatformReady()) {
        for (int i = 0; i < NUM_ACTUATORS; i++) {
          actuators[i].setLength(lengths[i]);
        }
        return true;
      } else {
        return false;
      }
    }
    
    int getActuatorRawPosition(int actuator) {
      if (actuatorIsValid(actuator)) {
        return actuators[actuator].getRawPosition();
      } else {
        return 0;
      }
    }

    int getActuatorPosition(int actuator) {
      if (actuatorIsValid(actuator)) {
        return actuators[actuator].getPosition();
      } else {
        return 0;
      }
    }

    int getActuatorTarget(int actuator) {
      if (actuatorIsValid(actuator)) {
        return actuators[actuator].getTargetPosition();
      } else {
        return 0;
      }
    }

    int getActuatorMaxPosition(int actuator) {
      if (actuatorIsValid(actuator)) {
        return actuators[actuator].getMaxPosition();
      } else {
        return 0;
      }
    }

    int getActuatorMinPosition(int actuator) {
      if (actuatorIsValid(actuator)) {
        return actuators[actuator].getMinPosition();
      } else {
        return 0;
      }
    }

    bool getActuatorReady(int actuator) {
      if (actuatorIsValid(actuator)) {
        return actuators[actuator].isActuatorReady();
      } else {
        return 0;
      }
    }

    bool getPlatformReady() {
      isReady = true;
      for (int i = 0; i < NUM_ACTUATORS; i++) {
        isReady = isReady && actuators[i].isActuatorReady();
      }
      return isReady;
    }

    private:
      bool actuatorIsValid(int actuator) {
        return actuator >= 0 && actuator < NUM_ACTUATORS;
      }
};


// ------ main program ------ //
StewartPlatform platform;
  
void setup() {
  Serial.begin(9600);

  // set the analog ref (5V regulator)
  analogReference(EXTERNAL);

  platform.setup();
  
  if (IS_GRAPHING) {
    Serial.println(platform.getActuatorMaxPosition(WHICH_ACTUATOR));
    Serial.println(platform.getActuatorMinPosition(WHICH_ACTUATOR));
  }

  /*
  Serial.println("Actuator info");
  for (int i = 1; i <= NUM_ACTUATORS; i++) {
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
  } 
  */
  
}

bool isGoingUp = true;
double high[] = {0.9, 0.9, 0.9, 0.9, 0.9, 0.9};
double low[] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1};

void loop() {
  platform.loop();

  if(platform.getPlatformReady()) {
    if(isGoingUp) {
      platform.setPlatformLengths(high);
      isGoingUp = false;
    } else {
      platform.setPlatformLengths(low);
      isGoingUp = true;
    }
  }

  if (IS_GRAPHING) {
    if (WHICH_ACTUATOR >= 0 && WHICH_ACTUATOR < NUM_ACTUATORS) {
      if (IS_RAW_DATA) {
        Serial.println(platform.getActuatorRawPosition(WHICH_ACTUATOR));
      } else {
        Serial.println(platform.getActuatorPosition(WHICH_ACTUATOR));
      }
    }
  }
}

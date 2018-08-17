#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "configuration.h"

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

    void extend();
    void extend(uint8_t pwm);
    void retract();
    void retract(uint8_t pwm);
    void off();
    void brake();
    
    void readPosition();

  public:
  	Actuator(byte attachToIn1Pin, byte attachToIn2Pin, byte attachToEnaPin, byte attachToFeedbackPin);
  	
    void setup();
    void loop();
  	
  	void calibrate();
    void calibrate(uint16_t (&settings)[2]);
  	void setLength(float relativeLength);
    
  	bool isActuatorReady();
    int getRawPosition();
    int getPosition();
    int getTargetPosition();
    int getMaxPosition();
    int getMinPosition();
};

#endif

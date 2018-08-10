#ifndef STEWARTPLATFORM_H
#define STEWARTPLATFORM_H

#include "configuration.h"
#include "Actuator.h"

class StewartPlatform {
  private:
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

    bool actuatorIsValid(int actuator);

  public:
    StewartPlatform();

    void setup();
    void loop();

    void calibrate();
    void calibrate(uint16_t (&settings)[NUM_ACTUATORS][2]);
    bool setPlatformLengths(double (&lengths)[NUM_ACTUATORS]);
    
    int getActuatorRawPosition(int actuator);
    int getActuatorPosition(int actuator);
    int getActuatorTarget(int actuator);
    int getActuatorMaxPosition(int actuator);
    int getActuatorMinPosition(int actuator);

    bool isActuatorReady(int actuator);
    bool isPlatformReady();
};

#endif

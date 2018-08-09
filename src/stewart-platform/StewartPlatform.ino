#include "StewartPlatform.h"

StewartPlatform::StewartPlatform() {
isCalibrated = false;
isReady = false;
}

void StewartPlatform::setup() {
for (int i = 0; i < NUM_ACTUATORS; i++) {
  actuators[i].setup();
}
calibrate();
}

void StewartPlatform::loop() {
 for (int i = 0; i < NUM_ACTUATORS; i++) {
    actuators[i].loop();
 }
}

void StewartPlatform::calibrate() {
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

bool StewartPlatform::setPlatformLengths(double (&lengths)[6]) {
if (getPlatformReady()) {
  for (int i = 0; i < NUM_ACTUATORS; i++) {
    actuators[i].setLength(lengths[i]);
  }
  return true;
} else {
  return false;
}
}

int StewartPlatform::getActuatorRawPosition(int actuator) {
if (actuatorIsValid(actuator)) {
  return actuators[actuator].getRawPosition();
} else {
  return 0;
}
}

int StewartPlatform::getActuatorPosition(int actuator) {
if (actuatorIsValid(actuator)) {
  return actuators[actuator].getPosition();
} else {
  return 0;
}
}

int StewartPlatform::getActuatorTarget(int actuator) {
if (actuatorIsValid(actuator)) {
  return actuators[actuator].getTargetPosition();
} else {
  return 0;
}
}

int StewartPlatform::getActuatorMaxPosition(int actuator) {
if (actuatorIsValid(actuator)) {
  return actuators[actuator].getMaxPosition();
} else {
  return 0;
}
}

int StewartPlatform::getActuatorMinPosition(int actuator) {
if (actuatorIsValid(actuator)) {
  return actuators[actuator].getMinPosition();
} else {
  return 0;
}
}

bool StewartPlatform::getActuatorReady(int actuator) {
if (actuatorIsValid(actuator)) {
  return actuators[actuator].isActuatorReady();
} else {
  return 0;
}
}

bool StewartPlatform::getPlatformReady() {
isReady = true;
for (int i = 0; i < NUM_ACTUATORS; i++) {
  isReady = isReady && actuators[i].isActuatorReady();
}
return isReady;
}

bool StewartPlatform::actuatorIsValid(int actuator) {
  return actuator >= 0 && actuator < NUM_ACTUATORS;
}

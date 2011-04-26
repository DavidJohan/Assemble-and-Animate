 /**
  * B: read software version
  * D: set speed
  * G: set position counter
  * H: read position
  * L: change LED state
  * N: read proximity sensors
  * O: read ambient light sensors
 **/
#include <stdint.h>

int kheperaApi_readSoftwareVersion(uint16_t* sw_version);
int kheperaApi_setSpeed(int16_t left_speed, int16_t right_speed);
int kheperaApi_setPositionCounter(int16_t left, int16_t right);
int kheperaApi_readPosition(int16_t* position);
int kheperaApi_changeLedState(uint8_t led_number, uint8_t led_action);
int kheperaApi_readProximitySensors(uint16_t* values);
int kheperaApi_readAmbientLightSensors(uint16_t* values);
int kheperaApi_exit();
int kheperaApi_setup();

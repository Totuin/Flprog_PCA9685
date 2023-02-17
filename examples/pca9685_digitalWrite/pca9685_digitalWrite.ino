#include "flprog_PCA9685.h"

FLProgI2C wireDevice(0);

FLProgPCA9685 dac(&wireDevice);
byte chanel = 2;
bool value;

void setup(void)
{
  wireDevice.begin();
  dac.begin(488);
}

void loop(void)
{
  value = !value;
  dac.digitalWrite(chanel, value);
  delay(10);
}


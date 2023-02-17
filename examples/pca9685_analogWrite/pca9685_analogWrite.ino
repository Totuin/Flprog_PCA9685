#include "flprog_PCA9685.h"

FLProgI2C wireDevice(0);

FLProgPCA9685 dac(&wireDevice);

void setup(void)
{
    wireDevice.begin();
    dac.begin(488);
}

void loop(void)
{
    uint32_t counter;
    for (counter = 0; counter < 4095; counter++)
    {
        dac.analogWrite(0, counter);
        writeData();
        delay(2);
    }
    for (counter = 4095; counter > 0; counter--)
    {
        dac.analogWrite(0, counter);
        delay(2);
        writeData();
    }
}

void writeData()
{
    analogWrite(5, (map((dac.analogRead(0)), (0), (4096), (0), (255))));
}
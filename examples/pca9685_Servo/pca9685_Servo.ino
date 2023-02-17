#include "flprog_PCA9685.h"

FLProgI2C wireDevice(0);

FLProgPCA9685 dac(&wireDevice);
byte chanel = 1;

void setup(void)
{
    wireDevice.begin();
    dac.begin(488);
    dac.servoSet(chanel, FLPROG_PCA9685_SERVO_MG996R);
}

void loop(void)
{
    uint32_t counter;
    for (counter = 0; counter < 180; counter++)
    {
        dac.servoWrite(chanel, counter);
        writeData();
        delay(10);
    }
    for (counter = 180; counter > 0; counter--)
    {
        dac.servoWrite(chanel, counter);
        delay(10);
        writeData();
    }
}

void writeData()
{
    analogWrite(5, (map((dac.analogRead(chanel)), (0), (4096), (0), (255))));
}
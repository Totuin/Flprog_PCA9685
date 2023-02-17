#include "flprog_PCA9685.h"

FLProgPCA9685::FLProgPCA9685(FLProgI2C *device, uint8_t i2c_address)
{
    i2cDevice = device;
    addres = i2c_address;
}

void FLProgPCA9685::begin(uint16_t workPwm)
{
    pwm = workPwm;
    setMode();
    digitalWrite(FLPROG_PCA9685_SERVO_ALL, LOW);
}

void FLProgPCA9685::setMode()
{
    uint16_t j = (osc / (4096 * uint32_t(pwm))) - 1;
    if (j < 3)
    {
        j = 3;
    }
    if (j > 255)
    {
        j = 255;
    } //	определяем значение предделителя

    write8(0, 0x30);                             //	отправляем байт данных в регистр MODE1     (устанавливаем флаги AL и SLEEP, остальные флаги сброшены)
    write8(0xFE, j);                             //	отправляем байт данных в регистр PRE_SCALE (устанавливаем предделитель для частоты ШИМ)
    write8(1, invrt << 4 | outdrvVal << 2 | outne); //	отправляем байт данных в регистр MODE2     (записываем флаги INVRT, OUTDRV, OUTNE, флаг OCH сброшен)
    write8(0, extclk << 6 | 0x30);               //	отправляем байт данных в регистр MODE1     (записываем флаг EXTCLK, флаги AL и SLEEP установлены, остальные флаги сброшены)
    write8(0, 0x20);                             //	отправляем байт данных в регистр MODE1     (сбрасываем флаг SLEEP, остальные флаги без изменений) / флаг EXTCLK не сбрасывается записью нуля
    delayMicroseconds(500);                      //	ждём выполнение действий по сбросу флага SLEEP
    write8(0, 0xA0);
}

void FLProgPCA9685::analogWrite(uint8_t pin, uint16_t value, uint16_t faze = 0)
{
    if (pin >= 16 && pin != FLPROG_PCA9685_SERVO_ALL)
    {
        return;
    }
    if (value == 0)
    {
        digitalWrite(pin, LOW);
        return;
    } //	устанавливаем логический 0
    if (value > 4095)
    {
        digitalWrite(pin, HIGH);
        return;
    } //	устанавливаем логическую 1
    if (faze > 4095)
    {
        faze = 4095;
    }
    value += faze;
    if (value > 4095)
    {
        value -= 4096;
    }
    //	отодвигаем спад импульса в зависимости от фазового сдвига
    uint8_t buffer[5];
    buffer[0] = pin == FLPROG_PCA9685_SERVO_ALL ? 0xFA : (pin * 4 + 6); //	определяем адрес первого регистра для записи данных
    buffer[1] = faze;
    buffer[2] = faze >> 8;
    buffer[3] = value;
    buffer[4] = value >> 8; //	определяем данные для записи в 4 регистра, начиная с первого определённого
    codeError = i2cDevice->fullWrite(addres, buffer, 5);
}

void FLProgPCA9685::digitalWrite(uint8_t pin, bool value)
{
    if (pin >= 16 && pin != FLPROG_PCA9685_SERVO_ALL)
    {
        return;
    }
    uint8_t buffer[5];
    buffer[0] = pin == FLPROG_PCA9685_SERVO_ALL ? 0xFA : (pin * 4 + 6); //	определяем адрес первого регистра для записи данных
    buffer[1] = 0;
    buffer[2] = value ? 16 : 0;
    buffer[3] = 0;
    buffer[4] = value ? 0 : 16; //	определяем данные для записи в 4 регистра, начиная с первого определённого
    codeError = i2cDevice->fullWrite(addres, buffer, 5);
}

uint16_t FLProgPCA9685::analogRead(uint8_t pin)
{
    if (pin >= 16)
    {
        return 0;
    }
    codeError = i2cDevice->fullWrite(addres, (pin * 4 + 6));
    uint8_t buffer[5];
    codeError = i2cDevice->fullRead(addres, buffer, 5);
    uint16_t j = 0, k = 0;
    if (buffer[3] & 0x10)
    {
        return 0;
    }
    if (buffer[1] & 0x10)
    {
        return 4096;
    }
    k = (buffer[1] << 8) + buffer[0];
    j = (buffer[3] << 8) + buffer[2];
    if (j < k)
    {
        j += 4096;
    }
    j -= k;
    return j;
}

void FLProgPCA9685::servoWrite(uint8_t pin, uint16_t angle)
{
    uint8_t k = pin;
    if (pin == FLPROG_PCA9685_SERVO_ALL)
    {
        pin = 15;
        k = 0;
    }
    if (pin >= 16 || angle > 360)
    {
        return;
    }
    for (int n = k; n <= pin; n++)
    {
        analogWrite(n, map((angle <= angles[n] ? angle : angles[n]), 0, angles[n], angles_min[n], angles_max[n]));
    }
}

void FLProgPCA9685::servoSet(uint8_t pin, uint16_t angle, uint16_t angle_min, uint16_t angle_max)
{
    uint8_t k = pin;
    if (pin == FLPROG_PCA9685_SERVO_ALL)
    {
        pin = 15;
        k = 0;
    }
    if (pin >= 16 || angle == 0 || angle > 360 || angle_min > 4095 || angle_max > 4095 || angle_max == 0)
    {
        return;
    }
    for (int n = k; n <= pin; n++)
    {
        angles[n] = angle;
        angles_min[n] = angle_min;
        angles_max[n] = angle_max;
    }
}

void FLProgPCA9685::servoSet(uint8_t pin, uint8_t type)
{
    switch (type)
    {
    case FLPROG_PCA9685_SERVO_SG90:
        servoSet(pin, 180, 127, 540);
        break;
    case FLPROG_PCA9685_SERVO_MG90:
        servoSet(pin, 180, 150, 555);
        break;
    case FLPROG_PCA9685_SERVO_MG996R:
        servoSet(pin, 180, 130, 515);
        break;
    case FLPROG_PCA9685_SERVO_FutabaS3003:
        servoSet(pin, 180, 140, 490);
        break;
    }
}

void FLProgPCA9685::invert(bool value)
{
    invrt = value;
    setMode();
}

void FLProgPCA9685::outdrv(bool value)
{
    outdrvVal = value;
    setMode();
}

void FLProgPCA9685::outState(uint8_t value)
{
    if (value > 2)
    {
        return;
    }
    outne = value;
    setMode();
}

void FLProgPCA9685::extClock(uint16_t value)
{
    if (value)
    {
        extclk = true;
        osc = value;
        osc *= 1000;
        setMode();
    }
    else
    {
        extclk = false;
        osc = 25000000;
        setMode();
        digitalWrite(FLPROG_PCA9685_SERVO_ALL, LOW);
    }
}

void FLProgPCA9685::write8(uint8_t addr, uint8_t data)
{
    i2cDevice->beginTransmission(addres);
    i2cDevice->write(addr);
    i2cDevice->write(data);
    codeError = i2cDevice->endTransmission();
}
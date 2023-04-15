#pragma once
#include "Arduino.h"
#include "flprogUtilites.h"
#include "flprogI2C.h"

#define FLPROG_PCA9685_DEFAULT_I2CADDR 0x40

#define FLPROG_PCA9685_IMPEDANCE 2
#define FLPROG_PCA9685_SERVO_ALL 255

#define FLPROG_PCA9685_SERVO_SG90 1
#define FLPROG_PCA9685_SERVO_MG90 2
#define FLPROG_PCA9685_SERVO_MG996R 3
#define FLPROG_PCA9685_SERVO_FutabaS3003 4

class FLProgPCA9685 : public FLProgI2cBasicSensor
{
public:
    FLProgPCA9685(AbstractFLProgI2C *device, uint8_t i2c_address = FLPROG_PCA9685_DEFAULT_I2CADDR);
    void begin() { begin(50); };
    void begin(uint16_t workPwm);                                                       // инициализ-я чипа (ID адрес, частота ШИМ в Гц)
    void analogWrite(uint8_t pin, uint16_t value, uint16_t faze = 0);                   // установка	ШИМ на выходе (№ выхода от 0 до 15, коэффициент заполнения от 0 до 4095, фазовый сдвиг от 0 до 4095)
    void digitalWrite(uint8_t pin, bool value);                                         // установка	логического уровня на выходе (№ выхода от 0 до 15, LOW / HIGH)
    uint16_t analogRead(uint8_t pin);                                                   // чтение коэффициента заполнения на выходе (№ выхода от 0 до 15)
    void servoWrite(uint8_t pin, uint16_t angle);                                       // установка	угла поворота сервопривода (№ выхода от 0 до 15, угол поворота от 0° до 360°)
    void servoSet(uint8_t pin, uint16_t angle, uint16_t angle_min, uint16_t angle_max); // установка	параметров для сервопривода	(№ выхода от 0 до 15, максимальный угол поворота от 1° до 360°, коэффициент заполнения от 0 до 4095 для угла 0°, коэффициент заполнения от 0 до 4095 для максимального угла поворота)                            // чтение коэффициента заполнения на выходе (№ выхода от 0 до 15)
    void servoSet(uint8_t pin, uint8_t type);                                           // установка	параметров для сервопривода	(№ выхода от 0 до 15, название сервопривода)

    void invert(bool value = true);    // инверсия сигналов на всех выходах (true / false)
    void outdrv(bool value = true);    // выбор схемы	подключения всех выходов внутри чипа		(true - схема с каскадным выходом / false - схема с открытым стоком)
    void outState(uint8_t value = 0);  // состояние на всех выходах при подаче «1» на вход OE	(LOW / HIGH / IMPEDANCE) примечание: при указании HIGH и выборе схемы с открытым стоком, на выходах будет состояние высокого импеданса
    void extClock(uint16_t value = 0); // установка частоты внешнего источника тактирования (частота в кГц, от 1 до 50000)

protected:
    void setMode();
    void write8(uint8_t addr, uint8_t d);
    uint32_t osc = 25000000;
    uint16_t pwm;
    bool extclk = false;                                                                                        //	флаг		состояния бита EXTCLK
    bool invrt = false;                                                                                         //	флаг		состояния бита INVRT
    bool outdrvVal = true;                                                                                      //	флаг		состояния бита OUTDRV
    uint8_t outne = 0;                                                                                          //	значение	двух битов OUTNE
    uint16_t angles[16] = {180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180};     //	максимальный угол поворота
    uint16_t angles_min[16] = {170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170}; //	коэффициент заполнения для угла 0°
    uint16_t angles_max[16] = {470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470}; //	коэффициент заполнения для максимального угла
};
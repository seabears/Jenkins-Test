// hvac_ctl.h
#ifndef HVAC_CTL_H
#define HVAC_CTL_H

#include "IfxPort.h"
#include "IfxPort_PinMap.h"

#include "Platform_Types.h"
#include "PWM.h"
#include "asclin.h"

// Print Color
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

// Thresholds Values
#define EXT_AIR_TH 3200 //2660
#define INT_CO2_TH 600
#define INT_TEMP_TH 29.5 //.0
#define INT_TEMP_TOO_HIGH_TH 30.5
#define INT_HUMIDITY_TH 63.0 //60.0 

// 10, 11, 13
#define RED_LED IfxPort_P10_5
#define YELLOW_LED IfxPort_P10_3
#define GREEN_LED IfxPort_P10_2

typedef enum {
    MANUAL_CTL = 0, // manual control
    AUTOMATIC_CTL = 1, // automatic control
}Control_Mode; // control mode

typedef enum {
    EXT_MODE = 0,
    INT_MODE = 1,
}Mode;

typedef enum {
    STOP = 0,
    LOW_SPEED = 1,
    MID_SPEED = 2,
    HIGH_SPEED = 3,
}Fan_Speed;

typedef struct {
    Control_Mode control; // 0: manual control, 1: automatic control
    Mode mode; // 0: external air quality, 1: internal air quality
    Fan_Speed speed; // 0: stop, 1: low speed, 2: mid speed, 3: high speed
}Hvac;

typedef struct{
    uint16 ext_air;         // external air quality
    uint16 int_co2;         // internal co2 level
    double int_temperature; // internal temperature
    double int_humidity;    // internal humidity
}Sensor_Data;


void hvac_init(Hvac* hvac);           // HVAC 초기화 함수
void sensor_init(Sensor_Data* data); // 센서 데이터 초기화 함수
uint16 havc_control(Hvac* hvac, const Sensor_Data data);  // 센서 데이터 바탕으로 팬 제어 함수

#endif // HVAC_CTL_H

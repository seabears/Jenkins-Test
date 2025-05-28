// hvac_ctl.c
#include "hvac_ctl.h"

// 함수 프로토타입
static Fan_Speed select_fan_speed(uint16 co2, double humi, double temp);

// 함수 정의
void hvac_init(Hvac* hvac) {
    hvac->control = AUTOMATIC_CTL;  // 자동 제어 모드로 초기화
    hvac->mode = EXT_MODE;
    hvac->speed = STOP;
}

void sensor_init(Sensor_Data* data) {
    data->ext_air = 0;
    data->int_co2 = 0;
    data->int_temperature = 0.0;
    data->int_humidity = 0.0;
}

uint16 havc_control(Hvac* hvac, const Sensor_Data data) {


    switch(hvac->mode){
        case EXT_MODE:
            // 1. 모드 판단
            if(data.ext_air > EXT_AIR_TH) {  // 외부 공기질 안좋으면 내부 순환모드로 전환
                hvac->mode = INT_MODE;
                changeMode(INT_MODE);  // 내부 순환 모드로 전환
            }
            if(hvac->control == MANUAL_CTL) {  // 수동 제어 모드일 경우
                return 1;  // 팬 제어를 하지 않음
            }

            // 2. fan 스피드 판단
            hvac->speed = select_fan_speed(data.int_co2, data.int_humidity, data.int_temperature);
            controlFan(hvac->speed);  // 팬 속도 제어 함수 호출
            break;

        case INT_MODE:
            // 1. 모드 판단
            if(data.ext_air <= EXT_AIR_TH - 200) {  // 외부 공기질 좋으면 외부 유입모드로 전환
                hvac->mode = EXT_MODE;
                changeMode(EXT_MODE);  // 외부 유입 모드로 전환
            }
            if(hvac->control == MANUAL_CTL) {  // 수동 제어 모드일 경우
                return 1;  // 팬 제어를 하지 않음
            }

            // 2. fan 스피드 판단
            hvac->speed = select_fan_speed(data.int_co2, data.int_humidity, data.int_temperature);
            controlFan(hvac->speed);  // 팬 속도 제어 함수 호출
            break;

        default:
            print("default\r\n");
            return -1;
            break;
    }
    return 0;  // 정상 종료
}

static Fan_Speed select_fan_speed(uint16 co2, double humi, double temp){
    if(co2 >= INT_CO2_TH || temp >= INT_TEMP_TOO_HIGH_TH) {  // CO2 농도가 기준치 이상, 온도가 너무 높은 경우 팬 작동
        // IfxPort_setPinHigh(RED_LED.port, RED_LED.pinIndex);  
        // IfxPort_setPinLow(YELLOW_LED.port, YELLOW_LED.pinIndex);  
        // IfxPort_setPinLow(GREEN_LED.port, GREEN_LED.pinIndex); 
        return HIGH_SPEED;
    }
    else if(temp >= INT_TEMP_TH) {  // 온도가 기준치 이상이면 팬 작동
        // IfxPort_setPinLow(RED_LED.port, RED_LED.pinIndex);  
        // IfxPort_setPinHigh(YELLOW_LED.port, YELLOW_LED.pinIndex);  
        // IfxPort_setPinLow(GREEN_LED.port, GREEN_LED.pinIndex); 
        return MID_SPEED;
    }
    else if(humi >= INT_HUMIDITY_TH) {  // 습도가 기준치 이상이면 팬 작동
        // IfxPort_setPinLow(RED_LED.port, RED_LED.pinIndex);  
        // IfxPort_setPinLow(YELLOW_LED.port, YELLOW_LED.pinIndex);  
        // IfxPort_setPinHigh(GREEN_LED.port, GREEN_LED.pinIndex); 
        return LOW_SPEED;
    }
    else {
        // IfxPort_setPinLow(RED_LED.port, RED_LED.pinIndex);  
        // IfxPort_setPinLow(YELLOW_LED.port, YELLOW_LED.pinIndex);  
        // IfxPort_setPinLow(GREEN_LED.port, GREEN_LED.pinIndex); 
        return STOP;  // 모든 조건을 만족하지 않으면 팬 정지
    }
}

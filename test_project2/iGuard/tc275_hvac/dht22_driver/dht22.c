// dht22.c

#include "dht22.h"

#define DHT22_PORT IfxPort_P02_0  // DHT22 데이터 핀
#define TIMEOUT 10000  // 타임아웃 값

void delay_us(uint32 us) {
    Ifx_TickTime ticks = IfxStm_getTicksFromMicroseconds(BSP_DEFAULT_TIMER, us);
    waitTime(ticks);
}

uint8 DHT22_ReadByte(void) {

     uint8 result = 0;
     for (int i = 0; i < 8; i++) {
         while (IfxPort_getPinState(DHT22_PORT.port, DHT22_PORT.pinIndex) == 0); // LOW면 대기
         delay_us(30);  // 26~28us면 '0', 70us면 '1'

         if (IfxPort_getPinState(DHT22_PORT.port, DHT22_PORT.pinIndex)) {    //70us이상일 때 High기록
             result |= (1 << (7 - i));   //MSB부터 write
         }

         while (IfxPort_getPinState(DHT22_PORT.port, DHT22_PORT.pinIndex) == 1); // HIGH면 대기
     }
     return result;
}

int DHT22_process(DHT22_Data* data) {

    // MCU → DHT22: Start Signal
    IfxPort_setPinModeOutput(DHT22_PORT.port, DHT22_PORT.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinLow(DHT22_PORT.port, DHT22_PORT.pinIndex);
    delay_us(8000);  // 최소 8ms 동안 LOW 유지  // modified
    IfxPort_setPinHigh(DHT22_PORT.port, DHT22_PORT.pinIndex);
    delay_us(40);  // 20~40us 동안 HIGH 유지

    // MCU ← DHT22: 응답 대기
    //print("DHT22 응답 대기...\n\r");
    IfxPort_setPinModeInput(DHT22_PORT.port, DHT22_PORT.pinIndex, IfxPort_Mode_inputPullUp);
    //print("DHT22 응답.\n\r");

    //DHT22에서 Low -> High -> Low 보내옴
    uint32 timeout = TIMEOUT;
    while (IfxPort_getPinState(DHT22_PORT.port, DHT22_PORT.pinIndex) == 1) {
        if (--timeout == 0) return -1;  // 응답 없음
    }
    timeout = TIMEOUT;
    while (IfxPort_getPinState(DHT22_PORT.port, DHT22_PORT.pinIndex) == 0) {
        if (--timeout == 0) return -1;
    }
    timeout = TIMEOUT;
    while (IfxPort_getPinState(DHT22_PORT.port, DHT22_PORT.pinIndex) == 1) {
        if (--timeout == 0) return -1;
    }

    // 데이터 수신 (40비트 = 5바이트)
    uint8 buffer[5] = {0};
    for (int i = 0; i < 5; i++) {
        buffer[i] = DHT22_ReadByte();
    }

    // 체크섬 검증
    if (buffer[4] != (buffer[0] + buffer[1] + buffer[2] + buffer[3])) {
        if ((buffer[4] + 256 != (buffer[0] + buffer[1] + buffer[2] + buffer[3])) && buffer[4] + 256*2 != (buffer[0] + buffer[1] + buffer[2] + buffer[3])) {   // uint8 오버플로우도 허용
            print("Checksum error\n\r");
            print("%d waiting...  ", buffer[4]);
            print("but %d\n\r", (buffer[0] + buffer[1] + buffer[2] + buffer[3]));
            return -2;  // 체크섬 오류
        }
    }

    // 온도, 습도 데이터 임시 저장
    DHT22_Data temp = {0};
    temp.humidity = (uint32)(((uint16_t)buffer[0] << 8) | buffer[1]);
    temp.temperature = (uint32)(((uint16_t)buffer[2] << 8) | buffer[3]);
    
    // 데이터 범위 체크
    if(temp.humidity > 1000) {
        print("Error : Humidity, Out Of Range\n\r");
        return -3;  // 습도 범위 오류
    }
    if(temp.temperature > 800) {
        print("Error : Temperature, Out Of Range\n\r");
        return -4;  // 온도 범위 오류
    }

    // 데이터 저장
    //data->humidity = (uint32)(((uint16_t)buffer[0] << 8) | buffer[1]);
    //data->temperature = (uint32)(((uint16_t)buffer[2] << 8) | buffer[3]);
    data->humidity = temp.humidity;
    data->temperature = temp.temperature;

    return 0;
}

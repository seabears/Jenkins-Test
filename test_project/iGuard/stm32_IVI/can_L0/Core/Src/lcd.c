#include "lcd.h"

static void lcd_pulse_enable(void)
{
	HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);
  HAL_Delay(2);  
	HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
  HAL_Delay(2);     
}


static void lcd_write4bits(uint8_t data)
{
    // 각 핀에 맞는 값 설정
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    lcd_pulse_enable();
}


static void lcd_command(uint8_t cmd)
{
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);  // RS 핀 Low (명령 모드)
    lcd_write4bits(cmd >> 4);  // 상위 4비트 전송
    lcd_write4bits(cmd & 0x0F); // 하위 4비트 전송
    HAL_Delay(5);  // 명령 후 딜레이
}


static void lcd_data(uint8_t data)
{
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);  // RS 핀 High (데이터 모드)
    lcd_write4bits(data >> 4);  // 상위 4비트 전송
    lcd_write4bits(data & 0x0F); // 하위 4비트 전송
    HAL_Delay(5);  // 데이터 전송 후 딜레이
}


void lcd_clear(void)
{
    lcd_command(0x01);  // Clear 화면 명령
    HAL_Delay(5);  // 클리어 후 딜레이
}

void lcd_init(void)
{
    // GPIO 핀 초기화는 STM32CubeMX에서 자동으로 생성된 코드에서 설정되어 있어야 함

    HAL_Delay(100);  // 초기화 딜레이

    // LCD 초기화 시 필요한 명령어 순서
    lcd_write4bits(0x03);
    HAL_Delay(10);  // 딜레이
    lcd_write4bits(0x03);
    HAL_Delay(10);  // 딜레이
    lcd_write4bits(0x03);
    HAL_Delay(5);   // 딜레이
    lcd_write4bits(0x02);
    HAL_Delay(5);   // 딜레이

    lcd_command(0x28);  // 4비트 모드, 2라인, 5x8 크기
    lcd_command(0x0C);  // 디스플레이 ON, 커서 OFF
    lcd_command(0x06);  // 입력 모드 설정 (자동 증가, 커서 이동)
    lcd_clear();  // 화면 지우기
}


void lcd_goto(uint8_t row, uint8_t col)
{
    uint8_t address = (row == 0) ? 0x00 : 0x40;  // 2라인 LCD의 주소 매핑
    address += col;  // 열 번호에 맞게 주소 조정
    lcd_command(0x80 | address);  // 주소 설정 명령
}

void lcd_print(const char *str)
{
    while (*str)
    {
        lcd_data(*str++);  // 문자열을 하나씩 출력
    }
}


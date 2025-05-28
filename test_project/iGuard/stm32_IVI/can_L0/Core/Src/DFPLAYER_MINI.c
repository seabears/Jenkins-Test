#include  "DFPLAYER_MINI.h"
#include "string.h"
#include "hw_vcom.h"
#include <stdio.h>

extern UART_HandleTypeDef huart4;

void Send_Cmd (uint8_t cmd, uint8_t Parameter1, uint8_t Parameter2)
{
	uint16_t Checksum = Version + Cmd_Len + cmd + Feedback + Parameter1 + Parameter2;
  Checksum = 0 - Checksum;

	uint8_t CmdSequence[10] = { Start_Byte, Version, Cmd_Len, cmd, Feedback, Parameter1, Parameter2, (Checksum>>8)&0x00ff, (Checksum&0x00ff), End_Byte};

	HAL_UART_Transmit(DF_UART, CmdSequence, 10, HAL_MAX_DELAY);
}

void Sound_Track(uint8_t nums){ // before use this func, you need to use DF_Resume() first.
    if(nums < 255) {
		Send_Cmd(0x03, 0x00, nums);
	}
}

void DF_Pause (void) {
	Send_Cmd(0x0E, 0x00, 0x00);
}

void DF_Resume (void) {
	Send_Cmd(0x0D, 0x00, 0);
}

void DF_Volume_Up(void)
{
	Send_Cmd(0x04, 0x00, 0);
}

void DF_Volume_Down(void)
{
	Send_Cmd(0x05, 0x00, 0);
}

void DF_Init (uint8_t volume)
{
	Printf("DF_Init \r\n");
	
	Send_Cmd(0x0C, 0x00, 0x00); // reset
	Printf("DF Reset \r\n");	
	HAL_Delay(2000);
	
	Send_Cmd(0x3F, 0x00, Source); // initialize parameters
	Printf("Send Cmd that initialize \r\n");
	HAL_Delay(200);
	
	Send_Cmd(0x06, 0x00, volume); // initialize volume
	Printf("Send Cmd that change volume \r\n");
	HAL_Delay(500);
}
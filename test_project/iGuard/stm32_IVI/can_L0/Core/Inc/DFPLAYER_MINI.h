#ifndef __DFPLAYER_MINI_H__
#define __DFPLAYER_MINI_H__

#include "main.h"

#define DF_UART                      &huart4
#define Source                       0x02        // TF CARD

# define Start_Byte                  0x7E
# define Version                     0xFF
# define Cmd_Len                     0x06
# define Feedback                    0x00        // if need Feedback: 0x01,  No Feedback: 0
# define End_Byte                    0xEF

void Send_Cmd(uint8_t cmd, uint8_t Parameter1, uint8_t Parameter2);
void DF_Init(uint8_t volume);
void DF_Pause(void);
void DF_Resume (void);
void Sound_Track(uint8_t nums);
void DF_Volume_Up(void);
void DF_Volume_Down(void);

#endif
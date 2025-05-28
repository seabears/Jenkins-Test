// mhz19b.h
#ifndef MH_Z19B_H
#define MH_Z19B_H

#include "IfxStdIf_DPipe.h"

#define ISR_PRIORITY_ASCLIN0_TX      16                                       /* Priority for interrupt ISR Transmit  */
#define ISR_PRIORITY_ASCLIN0_RX      9                                       /* Priority for interrupt ISR Receive   */
#define ISR_PRIORITY_ASCLIN0_ER      24                                      /* Priority for interrupt ISR Errors    */
#define ASC_TX_BUFFER_SIZE          256                                     /* Define the TX buffer size in byte    */
#define ASC_RX_BUFFER_SIZE          256                                     /* Define the RX buffer size in byte    */


#define MHZ19B_TX IfxPort_P15_2  // MH-Z19B tx
#define MHZ19B_RX IfxPort_P15_3  // MH-Z19B rx
#define MH_Z19B_CMD_LEN 9
#define MH_Z19B_RESP_LEN 9



void Driver_MHZ19B_Init(void);    // ASCLIN 인터페이스 초기화
int MHZ19B_requestCO2(uint16* ret_val);     // CO2 농도 요청 및 응답 처리


#endif // MH_Z19B_H

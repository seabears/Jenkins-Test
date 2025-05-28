// mhz19b.c
#include "mhz19b.h"

#include "Ifx_Types.h"
#include "IfxAsclin_Asc.h"
#include "Ifx_Shell.h"
#include "Ifx_Console.h"
#include "IfxPort.h"
#include "asclin_driver/asclin.h"

static const uint8 mh_z19b_cmd[MH_Z19B_CMD_LEN] = { 0xFF, 0x01, 0x86, 0, 0, 0, 0, 0, 0x79 };    // CO2 데이터 요청 명령어

// Global variables
static IfxStdIf_DPipe  g_ascStandardInterface0;                                     /* Standard interface object            */
static IfxAsclin_Asc   g_asclin0;                                                   /* ASCLIN module object                 */

static uint8 g_uartTxBuffer0[ASC_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
static uint8 g_uartRxBuffer0[ASC_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];

// Function Prototype
static void initSerialInterface0(void);

// IFX_INTERRUPT
IFX_INTERRUPT(asc0TxISR, 0, ISR_PRIORITY_ASCLIN0_TX);

void asc0TxISR(void)
{
    IfxStdIf_DPipe_onTransmit(&g_ascStandardInterface0);
}

IFX_INTERRUPT(asc0RxISR, 0, ISR_PRIORITY_ASCLIN0_RX);

void asc0RxISR(void)
{
    IfxStdIf_DPipe_onReceive(&g_ascStandardInterface0);
}

IFX_INTERRUPT(asc0ErrISR, 0, ISR_PRIORITY_ASCLIN0_ER);

void asc0ErrISR(void)
{
    IfxStdIf_DPipe_onError(&g_ascStandardInterface0);
}


// Function Implementations
void Driver_MHZ19B_Init(void)
{
    /* Initialize the hardware peripherals */
    initSerialInterface0();

    /* Initialize the Standard Interface */
    IfxAsclin_Asc_stdIfDPipeInit(&g_ascStandardInterface0, &g_asclin0);

}

void initSerialInterface0(void)
{
    IfxAsclin_Asc_Config ascConf0;

    /* Set default configurations */
    IfxAsclin_Asc_initModuleConfig(&ascConf0, &MODULE_ASCLIN0); /* Initialize the structure with default values      */

    /* Set the desired baud rate */
    ascConf0.baudrate.baudrate = 9600;                                   /* Set the baud rate in bit/s       */
    ascConf0.baudrate.oversampling = IfxAsclin_OversamplingFactor_16;            /* Set the oversampling factor      */

    /* Configure the sampling mode */
    ascConf0.bitTiming.medianFilter = IfxAsclin_SamplesPerBit_three;             /* Set the number of samples per bit*/
    ascConf0.bitTiming.samplePointPosition = IfxAsclin_SamplePointPosition_8;    /* Set the first sample position    */

    /* ISR priorities and interrupt target */
    ascConf0.interrupt.txPriority = ISR_PRIORITY_ASCLIN0_TX;  /* Set the interrupt priority for TX events             */
    ascConf0.interrupt.rxPriority = ISR_PRIORITY_ASCLIN0_RX;  /* Set the interrupt priority for RX events             */
    ascConf0.interrupt.erPriority = ISR_PRIORITY_ASCLIN0_ER;  /* Set the interrupt priority for Error events          */
    ascConf0.interrupt.typeOfService = IfxSrc_Tos_cpu0;

    /* Pin configuration */
    const IfxAsclin_Asc_Pins pins = {
            .cts        = NULL_PTR,                         /* CTS pin not used                                     */
            .ctsMode    = IfxPort_InputMode_pullUp,
            .rx         = &IfxAsclin0_RXB_P15_3_IN ,        /* Select the pin for RX connected to the USB port      */
            .rxMode     = IfxPort_InputMode_pullUp,         /* RX pin                                               */
            .rts        = NULL_PTR,                         /* RTS pin not used                                     */
            .rtsMode    = IfxPort_OutputMode_pushPull,
            .tx         = &IfxAsclin0_TX_P15_2_OUT,         /* Select the pin for TX connected to the USB port      */
            .txMode     = IfxPort_OutputMode_pushPull,      /* TX pin                                               */
            .pinDriver  = IfxPort_PadDriver_cmosAutomotiveSpeed1
    };
    ascConf0.pins = &pins;

    /* FIFO buffers configuration */
    ascConf0.txBuffer = g_uartTxBuffer0;                      /* Set the transmission buffer                          */
    ascConf0.txBufferSize = ASC_TX_BUFFER_SIZE;              /* Set the transmission buffer size                     */
    ascConf0.rxBuffer = g_uartRxBuffer0;                      /* Set the receiving buffer                             */
    ascConf0.rxBufferSize = ASC_RX_BUFFER_SIZE;              /* Set the receiving buffer size                        */

    /* Init ASCLIN module */
    IfxAsclin_Asc_initModule(&g_asclin0, &ascConf0);          /* Initialize the module with the given configuration   */
}
int MHZ19B_requestCO2(uint16* ret_val)
{
    #define TIMEOUT_LIMIT 8000000   // 타임아웃 시간
    #define MH_Z19B_RESPONSE_LEN 9  // 수신 패킷 길이

    // 1. 명령 전송
    Ifx_SizeT txLength = MH_Z19B_CMD_LEN;
    IfxStdIf_DPipe_write(&g_ascStandardInterface0, (void *)mh_z19b_cmd, &txLength, TIMEOUT_LIMIT);

    // 2. 센서로부터 수신
    uint8 response[MH_Z19B_RESPONSE_LEN] = {0};
    Ifx_SizeT rxLength = MH_Z19B_RESPONSE_LEN;

    // 수신 (TIMEOUT_LIMIT: 내부적으로 대기)
    IfxStdIf_DPipe_read(&g_ascStandardInterface0, response, &rxLength, TIMEOUT_LIMIT);

    if (rxLength != MH_Z19B_RESPONSE_LEN) {
        print("Error : RX length mismatch, expected %d, got %d\n\r", MH_Z19B_RESPONSE_LEN, rxLength);
        return -1;  // 수신 길이 오류
    }

    // 3. 체크섬 확인
    uint8 checksum = 0;
    for(int i = 1; i <= 7; i++) {
        checksum += response[i];
    }
    checksum = 0xFF - checksum + 1;
    if(checksum != response[8]){
        print("Error : Checksum, expected %d, got %d\n\r", checksum, response[8]);
        return -2;  // 체크섬 오류
    }

    // 4. CO2 농도 계산
    if (response[0] == 0xFF && response[1] == 0x86)
    {
        uint16 ppm = (response[2] << 8) | response[3];
        if(ppm > 5000) {
            print("Error : Out Of Range\n\r");
            return -3;  // CO2 값 범위 초과
        }
        *ret_val = ppm;
        return 0;   // 정상 종료
    }
    else {
        print("Error : Invalid response\n\r");
        return -4;  // 응답 오류
    }
    return -4;
}

// uint16 MHZ19B_requestCO2(uint16* ret_val)
// {
//     // 1. 명령 전송
//     for (int i = 0; i < MH_Z19B_CMD_LEN; i++)
//     {
//         IfxAsclin_Asc_blockingWrite(&g_asclin0, mh_z19b_cmd[i]); // UART TX
//     }

//     // 2. 센서로부터 수신
//     #define UART_TIMEOUT_LIMIT 8000000  // 적절한 대기시간
//     #define MH_Z19B_RESPONSE_LEN 9
//     uint8 response[MH_Z19B_RESPONSE_LEN] = {0};
//     for (int i = 0; i < MH_Z19B_RESPONSE_LEN; i++)
//     {
//         // 수신 데이터가 들어올 때까지 대기
//         volatile int timeout = 0;
//         while (IfxAsclin_Asc_getReadCount(&g_asclin0) == 0)
//         {   
//             //print("waitint...\n\r");
//             if (++timeout > UART_TIMEOUT_LIMIT)
//             {
//                 // 타임아웃 발생
//                 print("UART RX timeout at byte %d\n", i);
//                 return -1;
//             }
//         }
//         response[i] = IfxAsclin_Asc_blockingRead(&g_asclin0); // UART RX
//     }

//     // 3. 체크섬 확인
//     uint8 checksum = 0;
//     for(int i = 1;i <= 7;i++){
//         //print("response[%d] = %d\n\r", i, response[i]);   // 패킷 확인용
//         checksum += response[i];
//     }
//     checksum = 0xff - checksum + 1; // 체크섬 계산
//     if(checksum != response[8]){
//         print("Checksum error: expected %d, got %d\n\r", checksum, response[8]);
//         //return -1;
//     }

//     // 3. CO2 농도 계산
//     if (response[0] == 0xFF && response[1] == 0x86) // && response[8] == (0xff - (response[0] + response[1] + response[2] + response[3]) + 0x01))
//     {
//         uint16 ppm = (response[2] << 8) | response[3];
//         *ret_val = ppm; // CO2 농도 값 저장
//         return 0;
//     }
//     else {
//         print("Invalid response: \n\r");
//         return -1;
//     }

//     return -2;
// }

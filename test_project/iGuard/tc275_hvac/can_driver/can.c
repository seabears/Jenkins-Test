// can.c
#include "can.h"


static uint32 swapEndian(uint32 value) {
    return ((value & 0x000000FF) << 24) |
           ((value & 0x0000FF00) << 8)  |
           ((value & 0x00FF0000) >> 8)  |
           ((value & 0xFF000000) >> 24);
}

IFX_INTERRUPT(canRxInterruptHandler, 0, ISR_PRIORITY_CAN_RX);
void canRxInterruptHandler(void)
{
    IfxCpu_enableInterrupts();
    exit_sleep_mode(); // 슬립 모드 해제

    //IfxSrc_disable(IfxMultican_getSrcPointer(&MODULE_CAN, IfxMultican_SrcId_0));
}

/* 초기화 함수 */
void initCan(void)
{
  /* 1. CAN 모듈 초기화 */
  IfxMultican_Can_Config canConfig;
  IfxMultican_Can_initModuleConfig(&canConfig, &MODULE_CAN);
  IfxMultican_Can_initModule(&g_can, &canConfig);

  /* 2. CAN 노드 초기화 */
  IfxMultican_Can_NodeConfig nodeConfig;
  IfxMultican_Can_Node_initConfig(&nodeConfig, &g_can);

  nodeConfig.nodeId = IfxMultican_NodeId_0;       // Node0 사용
  nodeConfig.baudrate = 250000;                   // 250kbps 설정
  nodeConfig.samplePoint = 8000;                  // 샘플 포인트 80%
  nodeConfig.rxPin = &IfxMultican_RXD0B_P20_7_IN; // RX 핀 (P20.7)
  nodeConfig.txPin = &IfxMultican_TXD0_P20_8_OUT; // TX 핀 (P20.8)
  nodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
  nodeConfig.txPinMode = IfxPort_OutputMode_pushPull;
  nodeConfig.pinDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1; // 패드 드라이버 설정

  IfxMultican_Can_Node_init(&g_canNode, &nodeConfig);

  /* 3. 송신 메시지 오브젝트 초기화 */
  IfxMultican_Can_MsgObjConfig txMsgConfig;
  IfxMultican_Can_MsgObj_initConfig(&txMsgConfig, &g_canNode);

  txMsgConfig.msgObjId = 0;      // 오브젝트 ID 0
  txMsgConfig.messageId = 0x100; // 송신 ID: 0x100
  txMsgConfig.frame = IfxMultican_Frame_transmit;
  txMsgConfig.control.messageLen = IfxMultican_DataLengthCode_8;

  IfxMultican_Can_MsgObj_init(&g_txMsgObj, &txMsgConfig);

  /* 4. 수신 메시지 오브젝트 초기화 */
  IfxMultican_Can_MsgObjConfig rxMsgConfig;
  IfxMultican_Can_MsgObj_initConfig(&rxMsgConfig, &g_canNode);

  rxMsgConfig.msgObjId = 1;      // 오브젝트 ID 1
  rxMsgConfig.messageId = 0; // 수신 ID는 의미 없음
  rxMsgConfig.acceptanceMask = 0; // 0이면 '모든 ID' 허용
  rxMsgConfig.frame = IfxMultican_Frame_receive;
  rxMsgConfig.control.messageLen = IfxMultican_DataLengthCode_8;

  /* 5. 수신 인터럽트 활성화 */
  rxMsgConfig.rxInterrupt.enabled = TRUE;
  rxMsgConfig.rxInterrupt.srcId = IfxMultican_SrcId_0;


  IfxMultican_Can_MsgObj_init(&g_rxMsgObj, &rxMsgConfig);

  /* 6. 송신 메시지 데이터 준비 */
  IfxMultican_Message_init(&g_txMsg, 0x00a, 0x11223344, 0x55667788, 8); // ID 0x100, 데이터 8 bytes

  /* 6. 인터럽트 서비스 등록 */
  IfxSrc_init(IfxMultican_getSrcPointer(&MODULE_CAN, IfxMultican_SrcId_0), IfxSrc_Tos_cpu0, ISR_PRIORITY_CAN_RX);
  IfxSrc_enable(IfxMultican_getSrcPointer(&MODULE_CAN, IfxMultican_SrcId_0));
}

/* 송신 함수 */
IfxMultican_Status sendCanMessage(void)
{
    IfxMultican_Status status = IfxMultican_Can_MsgObj_sendMessage(&g_txMsgObj, &g_txMsg);
    return status;
}

/* 수신 대기 및 출력 함수 */
void canReceiveLoop(Hvac* hvac)
{
    Ifx_CAN_MO *hwObj = IfxMultican_MsgObj_getPointer(g_rxMsgObj.node->mcan, g_rxMsgObj.msgObjId);

    // 메시지가 수신되었는지 확인
    if (IfxMultican_MsgObj_isRxPending(hwObj))
    {
        // 수신 메시지 읽기
        IfxMultican_Message_init(&g_rxMsg, 0, 0, 0, 8); // ID, data 초기화
        IfxMultican_Status status = IfxMultican_MsgObj_readMessage(hwObj, &g_rxMsg);

        // 수신 성공
        if (status & IfxMultican_Status_newData)
        {
            //print(GREEN"[CAN 수신]\r\nID: 0x%02X  ---  "RESET, g_rxMsg.id);
            
            uint32 receive_data[2] = {0};

            switch(g_rxMsg.id) {
                case 0x01: // Wakeup 메시지 수신시
                    print(GREEN"[CAN 수신]\r\nID: 0x%02X  ---  "RESET, g_rxMsg.id);
                    print("Wakeup Mode\r\n");
                    break;
                case 0x02: // // Sleep 메시지 수신시
                    print(GREEN"[CAN 수신]\r\nID: 0x%02X  ---  "RESET, g_rxMsg.id);
                    print("Sleep Mode\r\n");    
                    hvac->control = AUTOMATIC_CTL; // 자동 모드로 변경
                    hvac->speed = STOP; // 팬 정지
                    controlFan(hvac->speed);  // 팬 속도 제어 함수 호출
                    enter_sleep_mode();
                    break;
                case 0x17: // hvac 제어 메시지 수신시
                    print(GREEN"[CAN 수신]\r\nID: 0x%02X  ---  "RESET, g_rxMsg.id);
                    receive_data[0] = g_rxMsg.data[0]; // 수신 데이터 1바이트
                    receive_data[1] = g_rxMsg.data[1]; // 수신 데이터 2바이트xx

                    receive_data[0] = swapEndian(receive_data[0]);  // 엔디안 변환
                    receive_data[1] = swapEndian(receive_data[1]);  // 엔디안 변환

                    //print("\r\n0x%08X\r\n", receive_data[0]);
                    //print("0x%08X\r\n", receive_data[1]);

                    if(receive_data[0] == 0x00) { // 수신 데이터가 0x00이면
                        hvac->control = MANUAL_CTL; // 수동 모드로 변경
                    }
                    else if(receive_data[0] == 0x01) { // 수신 데이터가 0x01이면
                        hvac->control = AUTOMATIC_CTL; // 자동 모드로 변경
                        break;  // 자동 모드일 때는 팬 속도 제어를 하지 않음
                    }
                    
                    // 수동 모드에서 팬 속도 제어
                    switch(receive_data[1]) {
                        case 0x00:
                            hvac->speed = STOP; // 팬 정지
                            controlFan(hvac->speed);  // 팬 속도 제어 함수 호출
                            print("Fan Stop\r\n");
                            break;
                        case 0x01:
                            hvac->speed = LOW_SPEED; // 팬 저속
                            controlFan(hvac->speed);  // 팬 속도 제어 함수 호출
                            print("Fan Low Speed\r\n");
                            break;
                        case 0x02:
                            hvac->speed = MID_SPEED; // 팬 중속
                            controlFan(hvac->speed);  // 팬 속도 제어 함수 호출
                            print("Fan Mid Speed\r\n");
                            break;
                        case 0x03:
                            hvac->speed = HIGH_SPEED; // 팬 고속
                            controlFan(hvac->speed);  // 팬 속도 제어 함수 호출
                            print("Fan High Speed\r\n");
                            break;
                        default:
                            print("Unknown Fan Speed: %d\r\n", receive_data[1]);
                            break;
                    }
                    break;  // 큰 switch 문 종료
                default:
                    //print("Unknown ID: 0x%02X\r\n", g_rxMsg.id);
                    break;
            }

        }

        // 수신 Pending Clear
        IfxMultican_MsgObj_clearRxPending(hwObj);
    }
}


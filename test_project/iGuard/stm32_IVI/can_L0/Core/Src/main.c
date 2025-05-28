/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CANSPI.h"
#include "lcd.h"
#include "stdio.h"
#include "MCP2515.h"
#include "DFPLAYER_MINI.h"
#include "string.h"
#include "hw_vcom.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFSIZE 128
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern UART_HandleTypeDef huart2; // USART2
extern UART_HandleTypeDef huart4; // USART4
uCAN_MSG rxMessage;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint8_t can_rx_flag = 1;
volatile uint8_t driving_flag = 1;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	 if (GPIO_Pin == CAN_INT_Pin)  // PB9 INT
    {
				can_rx_flag = 1;
				HAL_ResumeTick();
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_USART4_UART_Init();
  /* USER CODE BEGIN 2 */
	HW_VCOM_Init(&huart2);
	DF_Init(20);
  CANSPI_Initialize();	
	lcd_init();
	
	MCP2515_EnableInterrupts();
	
	DF_Resume();
  Printf("DF_Init finished\r\n");
  HAL_Delay(100);  
	
	Sound_Track(1);
	HAL_Delay(100);
	
  DF_Pause(); // pause
  Printf("DF_Pause \r\n");
  HAL_Delay(1000);
	Printf("now int pin : %d\r\n", HAL_GPIO_ReadPin(CAN_INT_GPIO_Port, CAN_INT_Pin));
	Printf("now driving mode : %d\r\n", driving_flag);
  
  static int lcdCnt = 0;
	static char buf[32];
	static uint8_t musicNumber;
	/* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		// sleeping_mode
		if (driving_flag == 0 && can_rx_flag)
		{
				can_rx_flag = 0; 
				while(CANSPI_messagesInBuffer() > 0)
				{
					if (CANSPI_Receive(&rxMessage))
					{
						// Set song number from Raspberry pi CAN messege !!!!!!!!!!!!
						// STM32 check
						switch (rxMessage.frame.id)
						{
							// wakeup message 
							case 0x1:
								Printf("Wake up Message Received\r\n");
								break;
							// TextLcd message
							case 0x10:
								// store 8 bytes to buf
								Printf("%d th PrintLcd Message Received\r\n", lcdCnt + 1);
								if (lcdCnt < 4)
								{
										buf[lcdCnt * 8 + 0] = rxMessage.frame.data0;
										buf[lcdCnt * 8 + 1] = rxMessage.frame.data1;
										buf[lcdCnt * 8 + 2] = rxMessage.frame.data2;
										buf[lcdCnt * 8 + 3] = rxMessage.frame.data3;
										buf[lcdCnt * 8 + 4] = rxMessage.frame.data4;
										buf[lcdCnt * 8 + 5] = rxMessage.frame.data5;
										buf[lcdCnt * 8 + 6] = rxMessage.frame.data6;
										buf[lcdCnt * 8 + 7] = rxMessage.frame.data7;

										lcdCnt++;
								}

								// print lcd
								if (lcdCnt == 4)
								{
										buf[31] = '\0';  // null-terminate
										Printf("Print Message : %s\r\n", buf);
										lcd_clear();
										lcd_goto(0, 0); 
										lcd_print((char*)buf);  // buf[0~15]

										lcd_goto(1, 0);
										lcd_print((char*)&buf[16]); // buf[16~31]

										lcdCnt = 0;  // clear buf
								}
								break;
							// DFPlayer StartMusic
							case 0x05:
								musicNumber = rxMessage.frame.data0;
								Printf("%d Music Starts\r\n", musicNumber);
								if (musicNumber >= 1 && musicNumber <= 3)
								{
									Sound_Track(musicNumber);
								}
								break;
							// DFPlayer EndMusic
							case 0x06:
								Printf("Music Pauses\r\n");
								DF_Pause();
								break;
							// Resume Music
							case 0x07:
								Printf("Music Resume\r\n");
								DF_Resume();
								break;
							// DFPlayer Volume Up
							case 0x08:
								Printf("Music Volume Up\r\n");
								DF_Volume_Up();
								break;
							// DFPlayer Volume Down
							case 0x09:
								Printf("Music Volume Down\r\n");
								DF_Volume_Down();
								break;
							//  Sleep Message
							case 0x02:
								Printf("Goes to Sleep\r\n");
								HAL_SuspendTick();
								HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
								break;
							default:
								Printf("Default : %d\r\n", rxMessage.frame.id);
								break;
						}
					}		
				}
		}
		// driving mode
		else if (driving_flag && can_rx_flag)
		{
			can_rx_flag = 0;
			while(CANSPI_messagesInBuffer() > 0)
				{
					if (CANSPI_Receive(&rxMessage))
					{
						// Set song number from Raspberry pi CAN messege !!!!!!!!!!!!
						// STM32 check
						switch (rxMessage.frame.id)
						{
							// wakeup message 
							case 0x1:
								Printf("Wake up Message Received\r\n");
								break;
							// TextLcd message
							case 0x10:
								// store 8 bytes to buf
								Printf("%d th PrintLcd Message Received\r\n", lcdCnt + 1);
								if (lcdCnt < 4)
								{
										buf[lcdCnt * 8 + 0] = rxMessage.frame.data0;
										buf[lcdCnt * 8 + 1] = rxMessage.frame.data1;
										buf[lcdCnt * 8 + 2] = rxMessage.frame.data2;
										buf[lcdCnt * 8 + 3] = rxMessage.frame.data3;
										buf[lcdCnt * 8 + 4] = rxMessage.frame.data4;
										buf[lcdCnt * 8 + 5] = rxMessage.frame.data5;
										buf[lcdCnt * 8 + 6] = rxMessage.frame.data6;
										buf[lcdCnt * 8 + 7] = rxMessage.frame.data7;

										lcdCnt++;
								}

								// print lcd
								if (lcdCnt == 4)
								{
										buf[31] = '\0';  // null-terminate
										Printf("Print Message : %s\r\n", buf);
										lcd_clear();
										lcd_goto(0, 0); 
										lcd_print((char*)buf);  // buf[0~15]

										lcd_goto(1, 0);
										lcd_print((char*)&buf[16]); // buf[16~31]

										lcdCnt = 0;  // clear buf
								}
								break;
							// DFPlayer StartMusic
							case 0x05:
								musicNumber = rxMessage.frame.data0;
								Printf("%d Music Starts\r\n", musicNumber);
								if (musicNumber >= 1 && musicNumber <= 3)
								{
									Sound_Track(musicNumber);
								}
								break;
							// DFPlayer EndMusic
							case 0x06:
								Printf("Music Pauses\r\n");
								DF_Pause();
								break;
							// Resume Music
							case 0x07:
								Printf("Music Resume\r\n");
								DF_Resume();
								break;
							// DFPlayer Volume Up
							case 0x08:
								Printf("Music Volume Up\r\n");
								DF_Volume_Up();
								break;
							// DFPlayer Volume Down
							case 0x09:
								Printf("Music Volume Down\r\n");
								DF_Volume_Down();
								break;
							//  Sleep Message
							case 0x02:
								Printf("Goes to Sleep\r\n");
								driving_flag = 0;
								HAL_SuspendTick();
								HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
								break;
							default:
								Printf("Defuault : %d\r\n", rxMessage.frame.id);
								break;
						}
					}		
				}
		}
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_3;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

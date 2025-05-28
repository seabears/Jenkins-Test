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
#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CANSPI.h"
#include "MCP2515.h"
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

volatile uCAN_MSG rxMessage;
extern ADC_HandleTypeDef hadc;
volatile float Vout;

volatile static uint32_t dDist = 0;

volatile uint8_t DistanceReady = 0;
volatile uint16_t no_activity_counter = 0;
volatile uint8_t tim3_flag = 0;

volatile uint8_t driving_mode = 1; // driving mode
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void delay (uint16_t time);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HCSR04_Read(void);
uint8_t FSR_Check (void);
uint8_t Ultrasonic_Check(void);
void check_and_send(void);
void send_sleep_message(void);
void send_wakeup_message(void);
void enter_sleep_mode(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	 if (GPIO_Pin == EXTERNAL_BUTTON_Pin)  // external button
    {
				HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
			  send_sleep_message();
				driving_mode = 0; // car poweroff
    }
}
uint32_t cnt2 = 0;
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
  MX_USART2_Init();
  MX_ADC_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  CANSPI_Initialize();	
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);
  HAL_TIM_Base_Start_IT(&htim3);
	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC3);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HCSR04_Read();
  while (1) {
    if (driving_mode == 0 && tim3_flag) {
      tim3_flag = 0;
      check_and_send();  
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
void delay (uint16_t time)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    while (__HAL_TIM_GET_COUNTER (&htim2) < time);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == htim3.Instance){
    tim3_flag= 1;
  }
}
volatile static float Distance  = 0;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) // 초음파 센서 거리 계산 비동기 로직
{ // INT when detected Ultrasound sig
		volatile static uint32_t IC_Val1 = 0;
		volatile static uint32_t IC_Val2 = 0;
		volatile static uint8_t Is_First_Captured = 0;  // is the first value captured 
		static uint32_t prevDiff = 0;
		volatile static uint32_t currDiff = 0;
		
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)  // if the interrupt source is channel1
    {
        if (Is_First_Captured==0) // if the first value is not captured
        {
            IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3); // read the first value
            Is_First_Captured = 1; 
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else if (Is_First_Captured==1)   // if the first is already captured
        {
            IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);  // read second value
            __HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

            if (IC_Val2 > IC_Val1)
            {
                currDiff = IC_Val2-IC_Val1;
            }
            else if (IC_Val1 > IC_Val2)
            {
                currDiff = (0xffff - IC_Val1) + IC_Val2;
            }
						
						dDist = (prevDiff > currDiff) ? (prevDiff - currDiff) : (currDiff - prevDiff);

						prevDiff = currDiff;
            Distance = ((currDiff * 340)/1000)/2;
						
            DistanceReady = 1;
            Is_First_Captured = 0; 
						
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
            __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC3);
        }
    }
}

void send_sleep_message(void)
{
		uCAN_MSG txMessage;
    txMessage.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
    txMessage.frame.id = 0x2; // sleep message id = 2
    txMessage.frame.dlc = 8;
    txMessage.frame.data0 = 's';
    txMessage.frame.data1 = 'l';
    txMessage.frame.data2 = 'e';
    txMessage.frame.data3 = 'e';
    txMessage.frame.data4 = 'p';
    txMessage.frame.data5 = 0;
    txMessage.frame.data6 = 0;
    txMessage.frame.data7 = 0;

    CANSPI_Transmit(&txMessage);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

void send_wakeup_message(void)
{
		uCAN_MSG txMessage;
    txMessage.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
    txMessage.frame.id = 0x1; // wakeup message id = 1
    txMessage.frame.dlc = 8;
    txMessage.frame.data0 = 'W';
    txMessage.frame.data1 = 'A';
    txMessage.frame.data2 = 'K';
    txMessage.frame.data3 = 'E';
    txMessage.frame.data4 = 0;
    txMessage.frame.data5 = 0;
    txMessage.frame.data6 = 0;
    txMessage.frame.data7 = 0;

    CANSPI_Transmit(&txMessage);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

void enter_sleep_mode(void) 
{
  HAL_SuspendTick(); 
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  HAL_ResumeTick();   
}


void HCSR04_Read(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
	delay(10);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);

	__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC3);
}


uint8_t Ultrasonic_Check(void) {
  DistanceReady = 0;
  HCSR04_Read();
  uint32_t timeout = 0; 
  while (!DistanceReady && timeout < 60) {
		delay(1000);
		timeout++;
  }

  return (dDist > 0x30) ? 1 : 0;
}

uint8_t PIR_Check(void) {
  GPIO_PinState state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);  // PIR 센서 핀
  return (state == GPIO_PIN_SET) ? 1 : 0;
}

uint8_t FSR_Check(void){
	uint32_t adcValue;
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000);
	adcValue = HAL_ADC_GetValue(&hadc); // ADC
	HAL_ADC_Stop(&hadc);
	
	Vout = (adcValue / 4095.0f) * 5.0f;    // VOUT  (5V )
  return (Vout > 1.0f) ? 1 : 0;
}
uint8_t pir;
void check_and_send(void) {
	uint8_t ultrasonic = Ultrasonic_Check();

  uint8_t fsr = FSR_Check();
  pir = PIR_Check();  
  uint8_t result = fsr && (pir || ultrasonic); // Sensor Logic
	
	// result accumulated stores last 3 results
	static uint8_t result_accumulated = 0;
	
	static uint8_t cnt_1s = 0; // cnt_1s counts for 3 second.
	static uint8_t sleep_message_sent = 0; // sleep message sent -> sleeps all other ECUs
	static uint8_t wake_message_sent = 0;
	cnt_1s++;
	// stores result to result_accumulated	
	result_accumulated |= result;
	// no_activity_counter counts for N seconds
	no_activity_counter++;
	
	// if 3 seconds 
	if (cnt_1s == 3)
	{
		if (result_accumulated)
		{
			if(wake_message_sent == 0)
			{ // wake only once
				wake_message_sent = 1;
				send_wakeup_message();
			}
      no_activity_counter = 0;
			result_accumulated = 0;
			sleep_message_sent = 0;
		}
		cnt_1s = 0;
	}

	// if CANT find child in 30seconds -> send sleep message to All other ECUs
  if (no_activity_counter >= 600 && sleep_message_sent == 0) {
		// sleep message send
	  send_sleep_message();
    no_activity_counter = 0;
		sleep_message_sent = 1;
    //enter_sleep_mode();
  }
}

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

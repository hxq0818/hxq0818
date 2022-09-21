/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "max30102.h"
#include "max30102_fir.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */



#define CACHE_NUMS 100//缓存数
#define PPG_DATA_THRESHOLD 100000 	//检测阈值


float ppg_data_cache_RED[CACHE_NUMS]={0};  //缓存区
float ppg_data_cache_IR[CACHE_NUMS]={0};  //缓存区
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_Delay(500);
	max30102_init();
	printf("max30102_init\r\n");
	max30102_fir_init();
	printf("max30102_fir_init\r\n");

	uint32_t cache_counter=0;  //缓存计数器
	uint32_t i = 0;
	uint8_t max30102_flag = 0;
	float max30102_data[2],fir_output[2];
	float max30102_Spo2_data[5];
	int max30102_heart_data[5];
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(max30102_int_flag)			//中断信号产生
			{
				
//				if(cache_counter % 10 == 0)
//						printf("cache_counter：%d \r\n",cache_counter);
				
					max30102_int_flag = 0;
					max30102_fifo_read(max30102_data);		//读取数据
				
				
					ir_max30102_fir(&max30102_data[0],&fir_output[0]);
					red_max30102_fir(&max30102_data[1],&fir_output[1]);  //滤波
					if((max30102_data[0]>PPG_DATA_THRESHOLD)&&(max30102_data[1]>PPG_DATA_THRESHOLD))  //大于阈值，说明传感器有接触
					{		
							ppg_data_cache_IR[cache_counter]=fir_output[0];
							ppg_data_cache_RED[cache_counter]=fir_output[1];
							cache_counter++;
						Hand_Contact = false;
					}
					else				//小于阈值
					{
							Hand_Contact = true;
							cache_counter=0;
					}


					if(cache_counter>=CACHE_NUMS)  //收集满了数据
					{
						printf("Hand_Contact:%d\r\n",Hand_Contact);
						max30102_heart_data[max30102_flag] = max30102_getHeartRate(ppg_data_cache_IR,CACHE_NUMS);
						max30102_Spo2_data[max30102_flag] = max30102_getSpO2(ppg_data_cache_IR,ppg_data_cache_RED,CACHE_NUMS);
						max30102_flag++;
						printf("心率：%d  次/min   ",max30102_getHeartRate(ppg_data_cache_IR,CACHE_NUMS));
						printf("血氧：%.2f  %%\n",max30102_getSpO2(ppg_data_cache_IR,ppg_data_cache_RED,CACHE_NUMS));
						cache_counter=0;
						if(max30102_flag == 5){
								max30102_flag = 0;
								printf("****************\r\n");
								printf("心率：%d  次/min   ",Max30120GeiHeartData(max30102_heart_data));
								printf("血氧：%d  %%\n",Max30120GeiSpo2Data(max30102_Spo2_data));
								printf("****************\r\n");
						}	
					}
			}
  }
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

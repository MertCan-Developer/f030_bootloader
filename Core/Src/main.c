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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bl_cm0.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_SIZE_OF_ARRAY			(uint16_t)262
#define CHUNK_FW_BYTE_SIZE			(uint16_t)256


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
uint8_t DataFrame[MAX_SIZE_OF_ARRAY] = {0};
uint8_t chunk_fw[CHUNK_FW_BYTE_SIZE] = {0};
uint8_t chunks_crc16_reasults[100] = {0};
volatile uint8_t data_ready_flag = 0;
uint8_t command = 0;
uint8_t  crc8_val = 0;
uint16_t crc16_val = 0;
char bl_ack[]  = "BL_ACK\r\n\n";
char bl_nack[] = "BL_NACK\r\n\n";
struct crc_s crc8  = {0};
struct crc_s crc16 = {0};
uint8_t num_of_chunks = 0;
uint8_t chunk_count = 0;
uint8_t chunk_crc = 0;
uint16_t B=0;
uint16_t crc_reg = 0xFFFF;			// Initial value
uint16_t poly	 = 0x1021;			// CRC16 polynom
uint32_t flash_fw_addr = FLASH_SECTOR5_ADDR;
uint8_t erase_count = 0;
uint16_t chunks_total_crc16_val = 0;
uint16_t total_crc16_from_host = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI1_Init(void);
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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_DMA(&huart1, DataFrame, 262);

	crc8.width  	 	= 8;
	crc8.length 	 	= 3;
	crc8.init		 	= 0;
	crc8.reflect_in 	= 0;
	crc8.reflect_out	= 0;

	crc16.length 		= 259;
	crc16.width  	 	= 8;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

//	  crc16_val = crc_calc_crc16(&crc16);


	  if((data_ready_flag == 1) && (DataFrame[1] == BL_IND_CMD))
	  {
		  data_ready_flag = 0;
		  crc8_val = crc_calc_crc8(&crc8);

		  if(crc8_val == DataFrame[3])
		  {
			  HAL_UART_Transmit(&huart1, (uint8_t*)&bl_ack, 9, 1000);
			  HAL_Delay(5);
			  bl_execute_cmd(crc8.cmd[2]);
		  }
		  else
		  {
			  HAL_UART_Transmit(&huart1, (uint8_t*)&bl_nack, 10, 1000);
			  Error_Handler();
		  }
	  }
	  else if((data_ready_flag == 1) && (DataFrame[1] == BL_IND_DATA))
	  {
		  data_ready_flag = 0;
		  crc16_val = crc_calc_crc16(&crc16);

		  if(crc16_val == ((DataFrame[259] << 8) | (DataFrame[260])))
		  {
			  erase_count++;
			  if(erase_count == 1) bl_erase_35kB_mem();
			  bl_write_mem_256_bytes(&crc16.firmware[3], flash_fw_addr);
			  flash_fw_addr += 0x100;
			  chunks_total_crc16_val += crc16_val;
		  }
		  else
		  {
			  HAL_UART_Transmit(&huart1, (uint8_t*)&bl_nack, 10, 1000);
			  Error_Handler();
		  }
	  }
	  else if((data_ready_flag == 1) && (DataFrame[1] == BL_IND_CRC))
	  {
		  if((chunk_count == num_of_chunks) && (chunks_total_crc16_val == total_crc16_from_host))
		  {
			  data_ready_flag = 0;
			  HAL_UART_Transmit(&huart1, (uint8_t*)&bl_ack, 9, 1000);
		  }
		  else
		  {
			  HAL_UART_Transmit(&huart1, (uint8_t*)&bl_nack, 10, 1000);
			  bl_erase_35kB_mem();
			  Error_Handler();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if((huart->Instance == USART1) )
	{
		if((DataFrame[1] == BL_IND_CMD) && (DataFrame[0] == BL_START))
		{
			data_ready_flag = 1;
			memcpy(crc8.cmd, &DataFrame[0], 5);
		}
		else if((DataFrame[1] == BL_IND_DATA) && (DataFrame[0] == BL_START))
		{
			data_ready_flag = 1;
			num_of_chunks = DataFrame[2];	// Number of chunks sent by host device
			memcpy(&crc16.firmware[0], &DataFrame[0], MAX_SIZE_OF_ARRAY);
			chunk_count++;
		}
		else if((DataFrame[1] == BL_IND_CRC) && (DataFrame[0] == BL_START))
		{
			data_ready_flag = 1;
			total_crc16_from_host = (DataFrame[2] << 8) | (DataFrame[3] & 0xFF);
		}
		else
		{
			memset((void*)&DataFrame[0], 0, 262);
		}
	}
//	HAL_UART_Receive_IT(&huart1, DataFrame, 262);
	HAL_UART_Receive_DMA(&huart1, DataFrame, 262);
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

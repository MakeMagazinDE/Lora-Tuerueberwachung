/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SX1278.h"
#include "hal_time.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/// Read analog digital converter values every 10 minutes
#define COUNTS_ADC_READ_INTERVAL          30
/// Standard check interval for the digital input (chicken house door)
#define MEASURING_INTERVAL_MS             20000

/// Temperature sensor calibration value address for 130 °C
#define TEMP130_CAL_ADDR                  ((uint16_t*) ((uint32_t) 0x1FF8007E))
/// Temperature sensor calibration value address for 30 °C
#define TEMP30_CAL_ADDR                   ((uint16_t*) ((uint32_t) 0x1FF8007A))
/// Vdd calibration voltage in unit mV
#define VDD_CALIB                         ((uint16_t) (3000))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
// Hardware connection for the SX1278 (DIO, Reset and Spi)
SX1278_hw_t SX1278_hw;
// Handle to the SX1278 driver
SX1278_t SX1278;

// Send buffer
uint8_t buffer[64];
// Length of a message
int message_length;

// Last computes temperature value
int8_t temperature;
// Last calculates reference voltage value
uint16_t batteryVoltage;

// Read counter for the digital analog converter
uint16_t counterReadAdc = 0xFFFF;

// Flag ad-conversion is complete
uint8_t ADC_ConversionComplete;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//-----------------------------------------------------------------
// Function for low power mode
//-----------------------------------------------------------------
// Choose the low power mode to enter
static void EnterLowPowerMode(void);
// Setup for the low power mode
static void SetupLowPowerMode(bool enableFastWakeup);
// Performs all necessary things after wake up to get a working system
static void ResumeLowPowerMode(void);

//-----------------------------------------------------------------
// Function for analog digital converter
//-----------------------------------------------------------------
// Starts the adc-calibration
static void AdcCalibration(void);
// Gets the analog values via dma process
static void GetAnalogValues(void);
// Starts analog digital converting process if necessary
static void GetAnalogValuesIfNecesssary(void);
// Computes the temperature value
static int32_t ComputeTemperature(uint16_t measure, uint16_t vdda);
// Calculates the current value of the reference voltage (battery voltage) in unit mV
static uint16_t CalcVref3_3V(uint32_t adcValue);

//-----------------------------------------------------------------
// LoRa-Functions
//-----------------------------------------------------------------
// Initializes the SX1278 module
static void InitializeSX1278Module(void);
// Fills the send buffer for a LoRa-packet
static int FillSendBuffer(void);
// Sends out a LoRa-packet
static void SendLoRaPacket(uint8_t *txBuf, uint8_t length);

//-----------------------------------------------------------------
// Other Functions
//-----------------------------------------------------------------
// Gets the current state of the door input pin
static uint8_t GetDoorState(void);
// Enables the door input pin pull down resistor
static void EnableDoorStatePinPullDown(void);
// Disables the door input pin pull down resistor
static void DisableDoorStatePinPullDown(void);
// Shows the led at start for one second
static void ShowLedAtStart(void);
// Generates an 8-Bit-Crc for a stream
uint8_t GenerateCrc(uint8_t *data, size_t len);
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
   MX_RTC_Init();
   MX_SPI1_Init();
   MX_ADC_Init();
   /* USER CODE BEGIN 2 */

   AdcCalibration();

   InitializeSX1278Module();

   ShowLedAtStart();
   /* USER CODE END 2 */

   /* Infinite loop */
   /* USER CODE BEGIN WHILE */
   while (1)
   {
      EnterLowPowerMode();

      EnableDoorStatePinPullDown();

      ResumeLowPowerMode();

      GetAnalogValuesIfNecesssary();

      message_length = FillSendBuffer();

      SendLoRaPacket((uint8_t *) buffer, message_length);

      DisableDoorStatePinPullDown();

      SX1278_sleep(&SX1278);
      /* USER CODE END WHILE */

      /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.OversamplingMode = DISABLE;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.SamplingTime = ADC_SAMPLETIME_39CYCLES_5;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DMAContinuousRequests = ENABLE;
  hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerFrequencyMode = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 0x7c;
  hrtc.Init.SynchPrediv = 0x127;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the WakeUp
  */
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */
}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SX1278_NSS_Pin|LD3_Pin|SX1278_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : DOOR_STATE_Pin */
  GPIO_InitStruct.Pin = DOOR_STATE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(DOOR_STATE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SX1278_NSS_Pin LD3_Pin SX1278_RESET_Pin */
  GPIO_InitStruct.Pin = SX1278_NSS_Pin|LD3_Pin|SX1278_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SX1278_DIO0_Pin */
  GPIO_InitStruct.Pin = SX1278_DIO0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SX1278_DIO0_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

//-----------------------------------------------------------------
// Function for low power mode
//-----------------------------------------------------------------

/**
 * @brief Choose the low power mode to enter
 *
 * If we don't need reading the analog values after wake, fast wake up will be choose to reduce the power consumption
 */
static void EnterLowPowerMode(void)
{
   if (counterReadAdc == 0xFFFF || counterReadAdc == COUNTS_ADC_READ_INTERVAL)
   {
      SetupLowPowerMode(false);
   }
   else
   {
      SetupLowPowerMode(true);
   }
}

/**
 * @brief Setup for the low power mode
 *
 * @param enableFastWakeup: true - Fast wake up is enable (reference voltage not stable directly after wake up),
 *                          false - Fast wake up disabled (reference voltage is stable after wake up)
 */
static void SetupLowPowerMode(bool enableFastWakeup)
{
   // Configure RTC to wake up after 20000ms
   uint32_t _time = (((uint32_t)MEASURING_INTERVAL_MS) * 2314) / 1000;
   HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, _time, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

   // Enable Power Control clock
   __HAL_RCC_PWR_CLK_ENABLE();
   // Enable ultra low power mode
   HAL_PWREx_EnableUltraLowPower();

      if (true == enableFastWakeup)
      HAL_PWREx_EnableFastWakeUp();    // Fast wake-up for ultra low power mode
   else
      HAL_PWREx_DisableFastWakeUp();   // Fast wake-up for ultra low power mode

   // Switch to STOP-Mode
   HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

/**
 * Performs all necessary things after wake up to get a working system
 */
static void ResumeLowPowerMode(void)
{
   // Reinitializes clocks
   SystemClock_Config();
}

//-----------------------------------------------------------------
// Function for analog digital converter
//-----------------------------------------------------------------

/**
 * Gets the analog values via dma process
 */
static void GetAnalogValues(void)
{
   uint16_t regularItems[2];

   ADC_ConversionComplete = 0;

   // start dma process and ...
   if (HAL_ADC_Start_DMA(&hadc, (uint32_t *)regularItems, 2) != HAL_OK)
   {
      Error_Handler();
   }

   // ... wait for the end of DMA transfer ...
   while ( 0 == ADC_ConversionComplete )
   {
   }

   // ... stop the dma
   HAL_ADC_Stop_DMA(&hadc);

   // calculate the values
   batteryVoltage = CalcVref3_3V(regularItems[0]);

   temperature = ComputeTemperature(regularItems[1], batteryVoltage);
}

/**
 * Starts analog digital converting process if necessary
 */
static void GetAnalogValuesIfNecesssary(void)
{
   if (counterReadAdc == 0xFFFF || counterReadAdc == COUNTS_ADC_READ_INTERVAL)
   {
      GetAnalogValues();
      counterReadAdc = 0;
   }
   else
   {
      counterReadAdc++;
   }
}

/**
 * @brief Computes the temperature value
 *
 * @param measure: Raw measuring value
 * @param vdda: Current voltage value of the battery (unit mV)
 *
 * @retval Temperature value in degree celcius
 */
static int32_t ComputeTemperature(uint16_t measure, uint16_t vdda)
{
   int32_t temperatureValue;
   temperatureValue = ((measure * vdda / VDD_CALIB) - (int32_t) *TEMP30_CAL_ADDR );

   temperatureValue = temperatureValue * (int32_t)(130 - 30);
   temperatureValue = temperatureValue / (int32_t)(*TEMP130_CAL_ADDR - *TEMP30_CAL_ADDR);
   temperatureValue = temperatureValue + 30;

   return (temperatureValue);
}

/**
 * @brief Calculates the current value of the reference voltage (battery voltage) in unit mV
 *
 * @param adcValue: Raw measuring value
 *
 * @retval Reference voltage (battery voltage) in unit mV
 */
static uint16_t CalcVref3_3V(uint32_t adcValue)
{
   uint16_t vrefint_cal = *((uint16_t*)0x1FF80078);

   uint16_t vdda = (uint16_t)((3000 * vrefint_cal) / adcValue);

   return vdda;
}

//-----------------------------------------------------------------
// LoRa-Functions
//-----------------------------------------------------------------

/**
 * Initializes the SX1278 module
 */
static void InitializeSX1278Module(void)
{
   // initialize hardware connection
   SX1278_hw.dio0.port = SX1278_DIO0_GPIO_Port;
   SX1278_hw.dio0.pin = SX1278_DIO0_Pin;
   SX1278_hw.nss.port = SX1278_NSS_GPIO_Port;
   SX1278_hw.nss.pin = SX1278_NSS_Pin;
   SX1278_hw.reset.port = SX1278_RESET_GPIO_Port;
   SX1278_hw.reset.pin = SX1278_RESET_Pin;
   SX1278_hw.spi = &hspi1;

   SX1278.hw = &SX1278_hw;

   // Initialize the SX1278 module
   // - Frequency: 433 MHz
   // - Send power: 14 dbm
   // - LoRa rate: 6 (spreading factor)
   // - Bandwidth: 250kHz
   // - Packet length: 10 bytes
   SX1278_begin(&SX1278, SX1278_433MHZ, SX1278_POWER_17DBM, SX1278_LORA_SF_10, SX1278_LORA_BW_250KHZ, 10, SX1278_LORA_IMPLICIT_HEADER_MODE);
}

/**
 * Fills the send buffer for a LoRa-packet
 *
 * @retval Length of the send buffer
 */
static int FillSendBuffer(void)
{
   buffer[0] = GetDoorState();
   buffer[1] = (uint8_t)temperature;
   buffer[2] = (uint8_t)((batteryVoltage >> 8) & 0xFF);
   buffer[3] = (uint8_t)(batteryVoltage & 0xFF);
   buffer[4] = GenerateCrc(buffer, 4);

   return 5;
}

/**
 * Generates an 8-Bit-Crc for a stream
 *
 * @param data: Pointer to the data stream
 * qparam len: Length of the data stream
 *
 * @retval Calculated crc
 */
uint8_t GenerateCrc(uint8_t *data, size_t len)
{
   uint8_t crc = 0xff;
   size_t i, j;

   for (i = 0; i < len; i++)
   {
      crc ^= data[i];

      for (j = 0; j < 8; j++)
      {
         if ((crc & 0x80) != 0)
            crc = (uint8_t)((crc << 1) ^ 0x31);
         else
            crc <<= 1;
      }
   }

   return crc;
}


/**
 * Sends out a LoRa-packet
 *
 * @param txBuf: Buffer with data to send
 * @param length: Length of the send buffer in bytes
 */
static void SendLoRaPacket(uint8_t *txBuf, uint8_t length)
{
   SX1278_LoRaEntryTx(&SX1278, length, 2000);

   SX1278_LoRaTxPacket(&SX1278, txBuf, length, 2000);
}

//-----------------------------------------------------------------
// Other Functions
//-----------------------------------------------------------------

/**
 * @brief Gets the current state of the door input pin
 *
 * @retval Current state of the door input pin (0 - Door closed, 1 - Door open)
 */
static uint8_t GetDoorState(void)
{
   char retVal;

   if (HAL_GPIO_ReadPin(DOOR_STATE_GPIO_Port, DOOR_STATE_Pin) == GPIO_PIN_SET)
   {
      retVal = 0x01;
   }
   else
   {
      retVal = 0x00;
   }

   return retVal;
}

/**
 * Enables the door input pin pull down resistor
 */
void EnableDoorStatePinPullDown(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

   /*Configure GPIO pin : DOOR_STATE_Pin */
   GPIO_InitStruct.Pin = DOOR_STATE_Pin;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_PULLDOWN;
   HAL_GPIO_Init(DOOR_STATE_GPIO_Port, &GPIO_InitStruct);
}

/**
 * Disables the door input pin pull down resistor
 */
static void DisableDoorStatePinPullDown(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

   /*Configure GPIO pin : DOOR_STATE_Pin */
   GPIO_InitStruct.Pin = DOOR_STATE_Pin;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   HAL_GPIO_Init(DOOR_STATE_GPIO_Port, &GPIO_InitStruct);
}

/**
 * Starts the adc-calibration
 */
static void AdcCalibration(void)
{
   if (HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED) !=  HAL_OK)
   {
      Error_Handler();
   }
}

/**
 * Shows the led at start for one second
 */
static void ShowLedAtStart(void)
{
   HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);

   HAL_Delay(1000);

   HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
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

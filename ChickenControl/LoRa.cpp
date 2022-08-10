//-----------------------------------------------------------------------------
//  LoRa.cpp
/// @file
///
/// @brief Contains all function for the LORA task
///
/// $Date:$
/// $Revision:$
/// $LastChangedBy:$
///
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------
#include "arduino.h"
#include "GpioDefs.h"
#include "LoRa.h"

#include <SPI.h>
#include "SX1278.h"
#include "ChickenControlData.h"
#include "PlaySound.h"
#include "TimeLoc.h"

//-----------------------------------------------------------------------------
// Private definitions, macros and constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// External references
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global members of the module
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototypes of private functions
//-----------------------------------------------------------------------------
void HAL_GPIO_WritePin (uint16_t GPIO_Pin, GPIO_PinState PinState);

HAL_StatusTypeDef HAL_SPI_Transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout);

HAL_SPI_StateTypeDef HAL_SPI_GetState(void);

HAL_StatusTypeDef HAL_SPI_TransmitReceive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);

void DelayMethod(uint32_t delayms);

GPIO_PinState HAL_GPIO_ReadPin(uint16_t GPIO_Pin);

void IRAM_ATTR isr_sx1278_dio0();

uint8_t GenerateCrc(uint8_t *data, size_t len);

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------

static const int spiClk = 1000000; // 1 MHz

//uninitalised pointers to SPI objects
SPIClass * vspi = NULL;
SPIClass * hspi = NULL;

static SemaphoreHandle_t packetReceivedSema;

void vspiCommand();
void hspiCommand();
void InitTaskLoRaScan(void);

void InitSpi() 
{
   //initialise two instances of the SPIClass attached to HSPI respectively
   hspi = new SPIClass(HSPI);
  
   hspi->begin();

   pinMode(HSPI_SS, OUTPUT);
   pinMode(GPIO16, OUTPUT);
   
   pinMode(GPIO25, INPUT);
   attachInterrupt(GPIO25, isr_sx1278_dio0, RISING);

   SX1278_hw_SetHalCallbacks(HAL_GPIO_WritePin, HAL_SPI_Transmit, HAL_SPI_GetState, HAL_SPI_TransmitReceive, HAL_GPIO_ReadPin, DelayMethod);
}

int ret;
SX1278_hw_t SX1278_hw;
SX1278_t SX1278;

char buffer[256];

int message;
int message_length;

void TaskLoRaScan(void *pvParameters)
{
   uint8_t reg;
   uint8_t count = 0;
   int16_t rssi;
   int8_t temperature;
   uint16_t batteryVoltage;
   BaseType_t rc;
   int8_t snr;
   uint8_t crc;

   InitTaskLoRaScan();

   SX1278.packetLength = 5;

   Serial.println("LoRa initialized!");

   for (;;)
   {
      // wait until next packet is received ...
      rc = xSemaphoreTake(packetReceivedSema, portMAX_DELAY);
      assert(rc == pdPASS);

      printf("\r\n");
      printLocalTime();

      ret = SX1278_LoRaRxPacket(&SX1278);
        
      if (ret > 0)
      {
         count = 0;

         printf("Received: %d\r\n", ret);

         SX1278_read(&SX1278, (uint8_t *) buffer, ret);

         if (ret == 5)
         {
            printf("Buffer 4: 0x%02x\r\n", buffer[4]);

            crc = GenerateCrc((uint8_t *)buffer, 4);

            if (crc == buffer[4])
            {
               SetChickenHouseDoorState(buffer[0]);
               temperature = buffer[1];
               batteryVoltage = ((uint16_t)buffer[2]) << 8;
               batteryVoltage |= buffer[3]; 

               printf("Content (%d): 0x%02x, %i °C, %i mV\r\n", ret, buffer[0], temperature, batteryVoltage );

               rssi = SX1278_RSSI_LoRaPacket(&SX1278);

               printf("Rssi: %i dbm\r\n", rssi );

               snr = SX1278_SNR_LoRaPacket(&SX1278);

               printf("Snr: %i db\r\n", snr );

               SetChickenHouseData(temperature, batteryVoltage, rssi);

               if (!GetChickenHouseConnected())
               {
                  SetChickenHouseConnected(true);

                  QueueInSoundToPlay(SOUND_ROOSTER);
               }
            }
            else
            {
               printf("Packet crc wrong (crc in packet = 0x%02x, calculated crc = 0x%02x)!\r\n", buffer[4], crc);
               printf("Full packet content:\r\n");
               printf("0x00: 0x%02x\r\n", buffer[0]);
               printf("0x01: 0x%02x\r\n", buffer[1]);
               printf("0x02: 0x%02x\r\n", buffer[2]);
               printf("0x03: 0x%02x\r\n", buffer[3]);
               printf("0x04: 0x%02x\r\n", buffer[4]);
            }
         }
         else
         {
            printf("Packet lenght is wrong (received %i bytes)!\r\n", ret);
         }
      }
      else
      {
         printf("Payload crc error occurred\r\n");
      }
   }
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

void InitTaskLoRaScan(void)
{
   uint8_t buf[3];

   packetReceivedSema = xSemaphoreCreateBinary();

   InitSpi();

   //initialize LoRa module
   SX1278_hw.dio0.port = NULL;
   SX1278_hw.dio0.pin = GPIO25;
   SX1278_hw.nss.port = NULL;
   SX1278_hw.nss.pin = HSPI_SS;
   SX1278_hw.reset.port = NULL;
   SX1278_hw.reset.pin = GPIO16;
   SX1278_hw.spi = NULL;

   SX1278.hw = &SX1278_hw;

   SX1278_hw_Reset(SX1278.hw);

   printf("Configuring LoRa module\r\n");
   SX1278_begin(&SX1278, SX1278_433MHZ, SX1278_POWER_14DBM, SX1278_LORA_SF_10, SX1278_LORA_BW_250KHZ, 10, SX1278_LORA_IMPLICIT_HEADER_MODE);
   printf("Done configuring LoRaModule\r\n");

   printf("Current SX1278 mode: 0x%02x\r\n", SX1278_Mode(&SX1278));

   printf("Registers after SX1278_begin()\r\n");
   printf("------------------------------\r\n");

   printf("Register LR_RegOpMode(0x%02x):         0x%02x\r\n", LR_RegOpMode, SX1278_SPIRead(&SX1278, LR_RegOpMode));

	SX1278_SPIBurstRead(&SX1278, LR_RegFrMsb, buf, 3);

   printf("Register LR_RegFrMsb(0x%02x):          0x%02x, 0x%02x, 0x%02x\r\n", LR_RegFrMsb, buf[0], buf[1], buf[2]);
   printf("Register LR_RegPaConfig(0x%02x):       0x%02x\r\n", LR_RegPaConfig, SX1278_SPIRead(&SX1278, LR_RegPaConfig));
   printf("Register LR_RegOcp(0x%02x):            0x%02x\r\n", LR_RegOcp, SX1278_SPIRead(&SX1278, LR_RegOcp));
   printf("Register LR_RegLna(0x%02x):            0x%02x\r\n", LR_RegLna, SX1278_SPIRead(&SX1278, LR_RegLna));
   printf("Register LR_RegModemConfig1(0x%02x):   0x%02x\r\n", LR_RegModemConfig1, SX1278_SPIRead(&SX1278, LR_RegModemConfig1));
   printf("Register LR_RegModemConfig2(0x%02x):   0x%02x\r\n", LR_RegModemConfig2, SX1278_SPIRead(&SX1278, LR_RegModemConfig2));
   printf("Register LR_RegSymbTimeoutLsb(0x%02x): 0x%02x\r\n", LR_RegSymbTimeoutLsb, SX1278_SPIRead(&SX1278, LR_RegSymbTimeoutLsb));
   printf("Register LR_RegPreambleMsb(0x%02x):    0x%02x\r\n", LR_RegPreambleMsb, SX1278_SPIRead(&SX1278, LR_RegPreambleMsb));
   printf("Register LR_RegPreambleLsb(0x%02x):    0x%02x\r\n", LR_RegPreambleLsb, SX1278_SPIRead(&SX1278, LR_RegPreambleLsb));
   printf("Register REG_LR_DIOMAPPING1(0x%02x):   0x%02x\r\n", REG_LR_DIOMAPPING1, SX1278_SPIRead(&SX1278, REG_LR_DIOMAPPING1));
   printf("Register REG_LR_DIOMAPPING2(0x%02x):   0x%02x\r\n", REG_LR_DIOMAPPING2, SX1278_SPIRead(&SX1278, REG_LR_DIOMAPPING2));
   printf("Register LR_RegBWOptimize1(0x%02x):    0x%02x\r\n", LR_RegHighBWOptimize1, SX1278_SPIRead(&SX1278, LR_RegHighBWOptimize1));
   printf("Register LR_RegBWOptimize2(0x%02x):    0x%02x\r\n", LR_RegHighBWOptimize2, SX1278_SPIRead(&SX1278, LR_RegHighBWOptimize2));
   printf("Register LR_RegIfFreq1(0x%02x):        0x%02x\r\n", LR_RegIfFreq1, SX1278_SPIRead(&SX1278, LR_RegIfFreq1));
   printf("Register LR_RegIfFreq2(0x%02x):        0x%02x\r\n", LR_RegIfFreq2, SX1278_SPIRead(&SX1278, LR_RegIfFreq2));
   printf("Register LR_RegDetectOptimize(0x%02x): 0x%02x\r\n", LR_RegDetectOptimize, SX1278_SPIRead(&SX1278, LR_RegDetectOptimize));

   ret = SX1278_LoRaEntryRx(&SX1278, 5, 2000);

   printf("State SX1278_LoRaEntryRx(): 0x%02x\r\n", ret);

   printf("Registers after SX1278_LoRaEntryRx()\r\n");
   printf("------------------------------------\r\n");

   printf("Register REG_LR_PADAC(0x%02x):         0x%02x\r\n", REG_LR_PADAC, SX1278_SPIRead(&SX1278, REG_LR_PADAC));
   printf("Register LR_RegHopPeriod(0x%02x):      0x%02x\r\n", LR_RegHopPeriod, SX1278_SPIRead(&SX1278, LR_RegHopPeriod));
   printf("Register REG_LR_DIOMAPPING1(0x%02x):   0x%02x\r\n", REG_LR_DIOMAPPING1, SX1278_SPIRead(&SX1278, REG_LR_DIOMAPPING1));
   printf("Register REG_LR_DIOMAPPING2(0x%02x):   0x%02x\r\n", REG_LR_DIOMAPPING2, SX1278_SPIRead(&SX1278, REG_LR_DIOMAPPING2));
   printf("Register LR_RegIrqFlagsMask(0x%02x):   0x%02x\r\n", LR_RegIrqFlagsMask, SX1278_SPIRead(&SX1278, LR_RegIrqFlagsMask));
   printf("Register LR_RegPayloadLength(0x%02x):  0x%02x\r\n", LR_RegPayloadLength, SX1278_SPIRead(&SX1278, LR_RegPayloadLength));
   printf("Register LR_RegFifoRxBaseAddr(0x%02x): 0x%02x\r\n", LR_RegFifoRxBaseAddr, SX1278_SPIRead(&SX1278, LR_RegFifoRxBaseAddr));
   printf("Register LR_RegFifoAddrPtr(0x%02x):    0x%02x\r\n", LR_RegFifoAddrPtr, SX1278_SPIRead(&SX1278, LR_RegFifoAddrPtr));
   printf("Register LR_RegOpMode(0x%02x):         0x%02x\r\n", LR_RegOpMode, SX1278_SPIRead(&SX1278, LR_RegOpMode));
   printf("Register LR_RegModemStat(0x%02x):      0x%02x\r\n", LR_RegModemStat, SX1278_SPIRead(&SX1278, LR_RegModemStat));
}

void HAL_GPIO_WritePin (uint16_t GPIO_Pin, GPIO_PinState PinState)
{
   if (PinState == GPIO_PIN_SET)
      digitalWrite(GPIO_Pin, HIGH);
   else
      digitalWrite(GPIO_Pin, LOW);
}

void Wiring_spi_dma_transfercomplete(void);

HAL_StatusTypeDef HAL_SPI_Transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
   hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
//  digitalWrite(HSPI_SS, LOW);
   hspi->transfer(pData, Size);
//  digitalWrite(HSPI_SS, HIGH);
   hspi->endTransaction();

   return HAL_OK;
}

HAL_SPI_StateTypeDef HAL_SPI_GetState(void)
{
   return HAL_SPI_STATE_READY;
}

HAL_StatusTypeDef HAL_SPI_TransmitReceive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
   hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
//  digitalWrite(HSPI_SS, LOW);
   hspi->transferBytes (pTxData, pRxData, Size);
//  digitalWrite(HSPI_SS, HIGH);
   hspi->endTransaction();
   return HAL_OK;
}

GPIO_PinState HAL_GPIO_ReadPin(uint16_t GPIO_Pin)
{
   if (digitalRead(GPIO_Pin) == HIGH)
      return GPIO_PIN_SET;
   else
      return GPIO_PIN_RESET;
}

void DelayMethod(uint32_t delayms)
{
   vTaskDelay(delayms);
}

void Wiring_spi_dma_transfercomplete(void)
{
   printf("Wiring_spi_dma_transfercomplete\r\n");
}

void IRAM_ATTR isr_sx1278_dio0()
{
   BaseType_t xHigherPriorityTaskWoken;

   xHigherPriorityTaskWoken = pdFALSE;

   xSemaphoreGiveFromISR(packetReceivedSema, &xHigherPriorityTaskWoken);

   if( xHigherPriorityTaskWoken )
   {
      portYIELD_FROM_ISR();
   }
}



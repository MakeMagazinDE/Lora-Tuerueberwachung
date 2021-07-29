/**
 * Author Wojciech Domski <Wojciech.Domski@gmail.com>
 * www: www.Domski.pl
 *
 * work based on DORJI.COM sample code and
 * https://github.com/realspinner/SX1278_LoRa
 */

#include "SX1278.h"
#include <string.h>

#include "stm32l0xx_hal.h"


//////////////////////////////////
// logic
//////////////////////////////////

void SX1278_hw_init(SX1278_hw_t * hw)
{
   SX1278_hw_SetNSS(hw, 1);
   HAL_GPIO_WritePin(hw->reset.port, hw->reset.pin, GPIO_PIN_SET);
}

void SX1278_hw_SetNSS(SX1278_hw_t * hw, int value)
{
   HAL_GPIO_WritePin(hw->nss.port, hw->nss.pin, (value == 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void SX1278_hw_Reset(SX1278_hw_t * hw)
{
   SX1278_hw_SetNSS(hw, 1);
   HAL_GPIO_WritePin(hw->reset.port, hw->reset.pin, GPIO_PIN_RESET);

   SX1278_hw_DelayMs(1);

   HAL_GPIO_WritePin(hw->reset.port, hw->reset.pin, GPIO_PIN_SET);

   SX1278_hw_DelayMs(100);
}

void SX1278_hw_SPICommand(SX1278_hw_t * hw, uint8_t cmd)
{
   SX1278_hw_SetNSS(hw, 0);
   HAL_SPI_Transmit(hw->spi, &cmd, 1, 1000);
   while (HAL_SPI_GetState(hw->spi) != HAL_SPI_STATE_READY);
}

uint8_t SX1278_hw_SPIReadByte(SX1278_hw_t * hw)
{
   uint8_t txByte = 0x00;
   uint8_t rxByte = 0x00;

   SX1278_hw_SetNSS(hw, 0);
   HAL_SPI_TransmitReceive(hw->spi, &txByte, &rxByte, 1, 1000);
   while (HAL_SPI_GetState(hw->spi) != HAL_SPI_STATE_READY);

   return rxByte;
}

void SX1278_hw_DelayMs(uint32_t msec)
{
   HAL_Delay(msec);
}

int SX1278_hw_GetDIO0(SX1278_hw_t * hw)
{
   return (HAL_GPIO_ReadPin(hw->dio0.port, hw->dio0.pin) == GPIO_PIN_SET);
}

//////////////////////////////////
// logic
//////////////////////////////////

uint8_t SX1278_SPIRead(SX1278_t * module, uint8_t addr)
{
   uint8_t tmp;
   SX1278_hw_SPICommand(module->hw, addr);
   tmp = SX1278_hw_SPIReadByte(module->hw);
   SX1278_hw_SetNSS(module->hw, 1);
   return tmp;
}

void SX1278_SPIWrite(SX1278_t * module, uint8_t addr, uint8_t cmd) 
{
   SX1278_hw_SetNSS(module->hw, 0);
   SX1278_hw_SPICommand(module->hw, addr | 0x80);
   SX1278_hw_SPICommand(module->hw, cmd);
   SX1278_hw_SetNSS(module->hw, 1);
}

void SX1278_SPIBurstRead(SX1278_t * module, uint8_t addr, uint8_t* rxBuf, uint8_t length)
{
   uint8_t i;
	
   if (length <= 1)
   {
      return;
   }
   else
   {
      SX1278_hw_SetNSS(module->hw, 0);
      SX1278_hw_SPICommand(module->hw, addr);
		
      for (i = 0; i < length; i++)
      {
          *(rxBuf + i) = SX1278_hw_SPIReadByte(module->hw);
      }
      
      SX1278_hw_SetNSS(module->hw, 1);
   }
}

void SX1278_SPIBurstWrite(SX1278_t * module, uint8_t addr, uint8_t* txBuf, uint8_t length)
{
   unsigned char i;
	
   if (length <= 1)
   {
      return;
   }
   else 
   {
      SX1278_hw_SetNSS(module->hw, 0);
      SX1278_hw_SPICommand(module->hw, addr | 0x80);
		
      for (i = 0; i < length; i++)
      {
         SX1278_hw_SPICommand(module->hw, *(txBuf + i));
      }
		
      SX1278_hw_SetNSS(module->hw, 1);
   }
}

void SX1278_defaultConfig(SX1278_t * module)
{
   SX1278_config(module, module->frequency, module->power, module->LoRa_Rate, module->LoRa_BW, module->headerMode);
}

void SX1278_config(SX1278_t * module, uint8_t frequency, uint8_t power, uint8_t LoRa_Rate, uint8_t LoRa_BW, uint8_t headerMode)
{
   //Change modem mode Must in Sleep mode ...
   SX1278_sleep(module);

   // ... and wait a little bit
   SX1278_hw_DelayMs(15);

   SX1278_entryLoRa(module);
   //SX1278_SPIWrite(module, 0x5904); //?? Change digital regulator form 1.6V to 1.47V: see errata note

   // RF carrier frequency (register 0x06-0x08)
   SX1278_SPIBurstWrite(module, LR_RegFrMsb, (uint8_t*) SX1278_Frequency[frequency], 3);

   // Output power (register 0x09)
   SX1278_SPIWrite(module, LR_RegPaConfig, SX1278_Power[power]);

   // switch on 'High Power +20 dBm Operation' if selected (register 0x4D)
   if (power == SX1278_POWER_20DBM)
   {
      SX1278_SPIWrite(module, REG_LR_PADAC, 0x87);
   }
   else
   {
      SX1278_SPIWrite(module, REG_LR_PADAC, 0x84);
   }

   // Disable overload current protection (register 0x0B)
   SX1278_SPIWrite(module, LR_RegOcp, 0x0B);

   // Boost on, 150% LNA current, Maximum gain (register 0x0C)
   SX1278_SPIWrite(module, LR_RegLna, 0x23);

   if (SX1278_SpreadFactor[LoRa_Rate] == 6)
   {
      uint8_t tmp;

      // register RegModemConfig1 (0x1D) Implicit Header mode, Error Coding rate 4/5(0x01) and selected bandwidth
      SX1278_SPIWrite(module, LR_RegModemConfig1, ((SX1278_LoRaBandwidth[LoRa_BW] << 4) + (SX1278_CR << 1) + 0x01));

      // register RegModemConfig2 (0x1E) Spreading factor 6, CRC enable, Symbol timeout msb 0x03
      SX1278_SPIWrite(module, LR_RegModemConfig2, ((SX1278_SpreadFactor[LoRa_Rate] << 4) + (SX1278_CRC << 2) + 0x03));

      // set register RegDetectOptimize (0x31) to value 0b101
      tmp = SX1278_SPIRead(module, LR_RegDetectOptimize);

      tmp &= 0xF8;
      tmp |= 0x05;

      SX1278_SPIWrite(module, LR_RegDetectOptimize, tmp);

      // set register RegDetectionThreshold to value 0x0C
      SX1278_SPIWrite(module, LR_RegDetectionThreshold, 0x0C);
   }
   else
   {
      if (module->headerMode == SX1278_LORA_IMPLICIT_HEADER_MODE)
      {
         // register RegModemConfig1 (0x1D) Implicit Header mode, Error Coding rate 4/5(0x01) and selected bandwidth
         SX1278_SPIWrite(module, LR_RegModemConfig1,
               ((SX1278_LoRaBandwidth[LoRa_BW] << 4) + (SX1278_CR << 1) + 0x01));
      }
      else
      {
         // register RegModemConfig1 (0x1D) Explicit Header mode, Error Coding rate 4/5(0x01) and selected bandwidth
         SX1278_SPIWrite(module,	LR_RegModemConfig1,
               ((SX1278_LoRaBandwidth[LoRa_BW] << 4) + (SX1278_CR << 1) + 0x00));
      }

      // register RegModemConfig2 (0x1E) Spreading factor as selected, CRC enable, Symbol timeout msb 0x03
      SX1278_SPIWrite(module, LR_RegModemConfig2,
            ((SX1278_SpreadFactor[LoRa_Rate] << 4) + (SX1278_CRC << 2) + 0x03));
   }

   // Sensitivity optimization for bandwidth 500kHz (see SX127x Errata Note)
   if (SX1278_LORA_BW_500KHZ == LoRa_BW)
   {
      SX1278_SPIWrite(module, LR_RegHighBWOptimize1, 0x02);
      SX1278_SPIWrite(module, LR_RegHighBWOptimize2, 0x7F);
   }
   else
   {
      SX1278_SPIWrite(module, LR_RegHighBWOptimize1, 0x03);

      // value at address 0x3a (register LR_RegHighBWOptimize2) will be automatically selected by the chip
   }

   // register RegSymbTimeoutLsb (0x1F) Symbol timeout = 0x3FF (Max)
   SX1278_SPIWrite(module, LR_RegSymbTimeoutLsb, 0xFF);
   // register RegPreambleMsb (0x20) Preamble length msb = 0
   SX1278_SPIWrite(module, LR_RegPreambleMsb, 0x00);
   // register LR_RegPreambleLsb (0x21) Preamble length lsb = 6 -> Preamble length = 6 byte
   SX1278_SPIWrite(module, LR_RegPreambleLsb, 6);
   // register REG_LR_DIOMAPPING2 (0x41) Enable PreambleDetect interrupt
   SX1278_SPIWrite(module, REG_LR_DIOMAPPING2, 0x01);

   // change the default sync word to 0x42 (0x34 is reserved for LoRaWAN)
   SX1278_SPIWrite(module, LR_RegSyncWord, 0x42);

   module->readBytes = 0;

   // Entry standby mode
   SX1278_standby(module);
}

void SX1278_standby(SX1278_t * module)
{
   SX1278_SPIWrite(module, LR_RegOpMode, 0x09);
   module->status = STANDBY;
}

void SX1278_sleep(SX1278_t * module)
{
   SX1278_SPIWrite(module, LR_RegOpMode, 0x08);
   module->status = SLEEP;
}

void SX1278_entryLoRa(SX1278_t * module)
{
   SX1278_SPIWrite(module, LR_RegOpMode, 0x88);
}

uint8_t SX1278_Mode(SX1278_t * module)
{
   return SX1278_SPIRead(module, LR_RegOpMode);
}

void SX1278_clearLoRaIrq(SX1278_t * module)
{
   SX1278_SPIWrite(module, LR_RegIrqFlags, 0xFF);
}

int SX1278_LoRaEntryRx(SX1278_t * module, uint8_t length, uint32_t timeout)
{
   uint8_t addr;

   module->packetLength = length;

   SX1278_defaultConfig(module);		//Setting base parameter
   SX1278_SPIWrite(module, REG_LR_PADAC, 0x84);	//Normal and RX
   SX1278_SPIWrite(module, LR_RegHopPeriod, 0xFF);	//No FHSS
   SX1278_SPIWrite(module, REG_LR_DIOMAPPING1, 0x01);//DIO=00,DIO1=00,DIO2=00, DIO3=01
   SX1278_SPIWrite(module, LR_RegIrqFlagsMask, 0x3F);//Open RxDone interrupt & Timeout
   SX1278_clearLoRaIrq(module);
   SX1278_SPIWrite(module, LR_RegPayloadLength, length);//Payload Length 21byte(this register must difine when the data long of one byte in SF is 6)
   addr = SX1278_SPIRead(module, LR_RegFifoRxBaseAddr); //Read RxBaseAddr
   SX1278_SPIWrite(module, LR_RegFifoAddrPtr, addr); //RxBaseAddr->FiFoAddrPtr
   SX1278_SPIWrite(module, LR_RegOpMode, 0x8d);	//Mode//Low Frequency Mode
   //SX1278_SPIWrite(module, LR_RegOpMode,0x05);	//Continuous Rx Mode //High Frequency Mode
   module->readBytes = 0;

   while (1)
   {
      if ((SX1278_SPIRead(module, LR_RegModemStat) & 0x04) == 0x04)
      {	//Rx-on going RegModemStat
         module->status = RX;
         return 1;
      }
		
      if (--timeout == 0)
      {
         SX1278_hw_Reset(module->hw);
         SX1278_defaultConfig(module);
         return 0;
      }
	
      SX1278_hw_DelayMs(1);
   }
}

uint8_t SX1278_LoRaRxPacket(SX1278_t * module)
{
   unsigned char addr;
   unsigned char packet_size;

   if (SX1278_hw_GetDIO0(module->hw))
   {
      memset(module->rxBuffer, 0x00, SX1278_MAX_PACKET);

      addr = SX1278_SPIRead(module, LR_RegFifoRxCurrentaddr); //last packet addr
      SX1278_SPIWrite(module, LR_RegFifoAddrPtr, addr); //RxBaseAddr -> FiFoAddrPtr

      if (module->LoRa_Rate == SX1278_LORA_SF_6)
      { //When SpreadFactor is six,will used Implicit Header mode(Excluding internal packet length)
         packet_size = module->packetLength;
      } 
      else
      {
         packet_size = SX1278_SPIRead(module, LR_RegRxNbBytes); //Number for received bytes
      }

      SX1278_SPIBurstRead(module, 0x00, module->rxBuffer, packet_size);
      module->readBytes = packet_size;
      SX1278_clearLoRaIrq(module);
   }

   return module->readBytes;
}

int SX1278_LoRaEntryTx(SX1278_t * module, uint8_t length, uint32_t timeout)
{
   uint8_t addr;
   uint8_t temp;

   module->packetLength = length;

   // Set default configuration
   SX1278_defaultConfig(module);

   // Symbol periods between frequency hops, 0 = disabled (register 0x24)
   SX1278_SPIWrite(module, LR_RegHopPeriod, 0x00);
   // Configuration of DIO0-3 (register 0x40), DIO0=01 (RX-CrcOk), DIO1=00, DIO2=00, DIO3=01 (TX-TxReady)
   SX1278_SPIWrite(module, REG_LR_DIOMAPPING1, 0x41);
   // Clear all LoRa interrupt requests (register 0x12)
   SX1278_clearLoRaIrq(module);
   // Enable the TxDone-interrupt (register 0x11)
   SX1278_SPIWrite(module, LR_RegIrqFlagsMask, 0xF7);
   // Setting the payload length (register 0x22)
   SX1278_SPIWrite(module, LR_RegPayloadLength, length);
   // Read the write base address in FIFO data buffer (register 0x0E)
   addr = SX1278_SPIRead(module, LR_RegFifoTxBaseAddr);
   // Write the SPI interface address pointer in FIFO data buffer
   SX1278_SPIWrite(module, LR_RegFifoAddrPtr, addr);

   while (1)
   {
      temp = SX1278_SPIRead(module, LR_RegPayloadLength);

      if (temp == length)
      {
         module->status = TX;
         return 1;
      }

      if (--timeout == 0)
      {
         SX1278_hw_Reset(module->hw);
         SX1278_defaultConfig(module);
         return 0;
      }
   }
}

int SX1278_LoRaTxPacket(SX1278_t * module, uint8_t* txBuffer, uint8_t length, uint32_t timeout)
{
   // Write tx data to the FIFO
   SX1278_SPIBurstWrite(module, LR_RegFifo, txBuffer, length);
   // Switch to Transmit mode, Low frequency mode, LoRa mode
   SX1278_SPIWrite(module, LR_RegOpMode, 0x8b);

   while (1)
   {
      if (SX1278_hw_GetDIO0(module->hw))
      {
         // Read and write the irq flags (only for clearing)
         SX1278_SPIRead(module, LR_RegIrqFlags);
         SX1278_clearLoRaIrq(module);
         // Entry Standby mode
         SX1278_standby(module);
         return 1;
      }

      if (--timeout == 0)
      {
         SX1278_hw_Reset(module->hw);
         SX1278_defaultConfig(module);
         return 0;
      }

      SX1278_hw_DelayMs(1);
   }
}

void SX1278_begin(SX1278_t * module, uint8_t frequency, uint8_t power, uint8_t LoRa_Rate, uint8_t LoRa_BW, uint8_t packetLength, uint8_t headerMode)
{
   SX1278_hw_init(module->hw);
   module->frequency = frequency;
   module->power = power;
   module->LoRa_Rate = LoRa_Rate;
   module->LoRa_BW = LoRa_BW;
   module->packetLength = packetLength;
   module->headerMode = headerMode;
   SX1278_defaultConfig(module);
}

int SX1278_transmit(SX1278_t * module, uint8_t* txBuf, uint8_t length, uint32_t timeout)
{
   if (SX1278_LoRaEntryTx(module, length, timeout))
   {
      return SX1278_LoRaTxPacket(module, txBuf, length, timeout);
   }
	
   return 0;
}

int SX1278_receive(SX1278_t * module, uint8_t length, uint32_t timeout)
{
   return SX1278_LoRaEntryRx(module, length, timeout);
}

uint8_t SX1278_available(SX1278_t * module)
{
   return SX1278_LoRaRxPacket(module);
}

uint8_t SX1278_read(SX1278_t * module, uint8_t* rxBuf, uint8_t length)
{
   if (length != module->readBytes)
      length = module->readBytes;
	
   memcpy(rxBuf, module->rxBuffer, length);
	
   rxBuf[length] = '\0';
   module->readBytes = 0;
	
   return length;
}

uint8_t SX1278_RSSI_LoRa(SX1278_t * module)
{
   uint32_t temp = 10;
   temp = SX1278_SPIRead(module, LR_RegRssiValue); //Read RegRssiValue, Rssi value
   temp = temp + 127 - 137; //127:Max RSSI, 137:RSSI offset
   return (uint8_t) temp;
}

uint8_t SX1278_RSSI(SX1278_t * module)
{
   uint8_t temp = 0xff;
   temp = SX1278_SPIRead(module, 0x11);
   temp = 127 - (temp >> 1);	//127:Max RSSI
   return temp;
}

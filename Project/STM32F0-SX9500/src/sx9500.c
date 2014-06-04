/******************************************************************************
 * Project        : STM32F0+SX9500
 * File           : sx9500.c
 * Copyright      : 2014 Yosun Singapore Pte Ltd
 ******************************************************************************
  Change History:

    Version 1.0 - May 2014
    > Initial revision

******************************************************************************/
#include "sx9500.h"
#include "stm32f0xx_tim.h"

/**
  * @brief  This function initializes TIM1 timer.
  * @param  None
  * @retval None
  */
static void InitializeTimer1(void)
{
	TIM_TimeBaseInitTypeDef timerInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; //create NVIC structure
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  
	/* use internal clock */
	/* 48000*10000/48MHz == 10 secconds  */
	timerInitStructure.TIM_Prescaler = 48000;
  timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timerInitStructure.TIM_Period = 10000;
  timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  timerInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &timerInitStructure);
	
	// Clear update interrupt bit
  TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
  // Enable update interrupt
  TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_UP_TRG_COM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  //TIM_Cmd(TIM1, ENABLE);
}
	
/**
  * @brief  This function initializes peripherals connect to SX9500.
  * @param  None
  * @retval None
  */
void STM32F0_Setup(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  I2C_InitTypeDef  I2C_InitStructure;
  
  RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);

  //(#) Enable peripheral clock using RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, ENABLE)
  //    function for I2C1 or I2C2.
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  
  //(#) Enable SDA, SCL  and SMBA (when used) GPIO clocks using 
  //    RCC_AHBPeriphClockCmd() function. 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  
	// Enable PA0 and make it as interrupt input pin
  /* Enable GPIOA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	/* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure PA0 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Connect EXTI0 Line to PA0 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  // Enable PB8 and make it active high
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIOB->BSRR = GPIO_BSRR_BS_8;
  
  //(#) Peripherals alternate function: 
  //    (++) Connect the pin to the desired peripherals' Alternate 
  //         Function (AF) using GPIO_PinAFConfig() function.
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_1);

  //    (++) Configure the desired pin in alternate function by:
  //         GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;

  //    (++) Select the type, OpenDrain and speed via  
  //         GPIO_PuPd, GPIO_OType and GPIO_Speed members
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  //    (++) Call GPIO_Init() function.
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //(#) Program the Mode, Timing , Own address, Ack and Acknowledged Address 
  //    using the I2C_Init() function.
  I2C_StructInit(&I2C_InitStructure);
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_OwnAddress1 = 0;
  I2C_InitStructure.I2C_Timing = 0x10420F13;
  I2C_Init(I2C1, &I2C_InitStructure);
  
  //(#) Optionally you can enable/configure the following parameters without
  //    re-initialization (i.e there is no need to call again I2C_Init() function):
  //    (++) Enable the acknowledge feature using I2C_AcknowledgeConfig() function.
  //    (++) Enable the dual addressing mode using I2C_DualAddressCmd() function.
  //    (++) Enable the general call using the I2C_GeneralCallCmd() function.
  //    (++) Enable the clock stretching using I2C_StretchClockCmd() function.
  //    (++) Enable the PEC Calculation using I2C_CalculatePEC() function.
  //    (++) For SMBus Mode:
  //         (+++) Enable the SMBusAlert pin using I2C_SMBusAlertCmd() function.

  //(#) Enable the NVIC and the corresponding interrupt using the function
  //    I2C_ITConfig() if you need to use interrupt mode.
	//I2C_ITConfig(I2C1, (I2C_IT_RXI | I2C_IT_TXI | I2C_IT_TCI | I2C_IT_ADDRI | I2C_IT_NACKI | I2C_IT_STOPI), ENABLE);
  
  //(#) When using the DMA mode 
  //   (++) Configure the DMA using DMA_Init() function.
  //   (++) Active the needed channel Request using I2C_DMACmd() function.
  
  //(#) Enable the I2C using the I2C_Cmd() function.
  I2C_Cmd(I2C1, ENABLE);

  //(#) Enable the DMA using the DMA_Cmd() function when using DMA mode in the 
  //    transfers. 
	
	InitializeTimer1();
}

/**
  * @brief  This function initializes SX9500.
  * @param  None
  * @retval None
  */
void SX9500_Setup(void)
{
	uint8_t tmpReg;
	
	/* Enables the detection irq & the release irq */
	tmpReg = SX9500_RegRead(SX9500_REG_IRQ_Enable);
	tmpReg |= (IRQ_Enable_TCHIRQEN | IRQ_Enable_RLSIRQEN);
	SX9500_RegWrite(SX9500_REG_IRQ_Enable, tmpReg);
	/* Enables all four sensors CS0~CS3 and set scan period as 30ms */
	SX9500_RegWrite(SX9500_REG_CPS_CTRL0, CPS_CTRL0_CPS_PERIOD_30ms | CPS_CTRL0_CS0_EN | CPS_CTRL0_CS1_EN | CPS_CTRL0_CS2_EN | CPS_CTRL0_CS3_EN);
	/* Turn on CG bias/shield */
	/* Capacitance Range & Resolution as small */
	SX9500_RegWrite(SX9500_REG_CPS_CTRL1, CPS_CTRL1_CPS_SH_ON | CPS_CTRL1_CPS_CINR_Small);
	/* Set Digital gain factor as 8 */
	/* Sampling frequency 167KHz */
	/* Resolution Control as finest */
	SX9500_RegWrite(SX9500_REG_CPS_CTRL2, CPS_CTRL2_CPS_Digital_GAIN_8 | CPS_CTRL2_CPS_FS_167KHz | CPS_CTRL2_CPS_RES_7);
	/* Disables doze mode */
	/* Raw filter coefficient as low */
	SX9500_RegWrite(SX9500_REG_CPS_CTRL3, CPS_CTRL3_CPS_DOZEDIS | CPS_CTRL3_CPS_RAWFILT_LOW);
	/* Average pos/neg threshold */
	SX9500_RegWrite(SX9500_REG_CPS_CTRL4, 0x30);
	/* Average pos/neg debouncer: off */
	/* Average negative filter coefficient : lowest */
	/* Average positive filter coefficient : Highest (Max. Filter) */
	SX9500_RegWrite(SX9500_REG_CPS_CTRL5, CPS_CTRL5_CPS_AVGPOSFILT_7 | CPS_CTRL5_CPS_AVGNEGFILT_1);
	/* the touch/prox detection threshold for all sensors */
	SX9500_RegWrite(SX9500_REG_CPS_CTRL6, 0x04);
	/* Enables automatic compensation */
	/* compensate all channels when triggered */
	/* Detection hysteresis as 32 */
	/* Close debouncer: off */
	/* Far debouncer: off */
	SX9500_RegWrite(SX9500_REG_CPS_CTRL7, CPS_CTRL7_CPS_CMPTRG_ALL);
	/* Stuck at timeout timer : off */
	/* Periodic compensation: off */
	SX9500_RegWrite(SX9500_REG_CPS_CTRL8, 0x00);
}

/**
  * @brief  This function reads data from a register of the SX9500 EVK.
  * @param  reg: register to be read
  * @retval 8-bit register contents
  */
uint8_t SX9500_RegRead(uint8_t reg)
{
  uint8_t tmp = 0xFF;
  
//I2C1->CR2 = (uint32_t)(LIS302DL_ADDR) | (uint32_t)(1 << 16) | I2C_CR2_START ;
  I2C_TransferHandling(I2C1, SX9500_ADDR, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  while(!(I2C1->ISR & I2C_ISR_TXIS)){;}
		
//I2C1->TXDR = (uint8_t)(addr);
  I2C_SendData(I2C1, reg);
  while(!(I2C1->ISR & I2C_ISR_TC)){;}
    
//I2C1->CR2 = (uint32_t)(LIS302DL_ADDR) | (uint32_t)(1 << 16) | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START;
  I2C_TransferHandling(I2C1, SX9500_ADDR, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  while(!(I2C1->ISR & I2C_ISR_RXNE)){;}
    
//tmp = (uint8_t)I2C1->RXDR;
  tmp = I2C_ReceiveData(I2C1);
    
  while(!(I2C1->ISR & I2C_ISR_STOPF)){;}
  I2C1->ICR = I2C_ICR_STOPCF;
  
  return(tmp);
}

/**
  * @brief  This function writes data to a register of the SX9500 EVK.
  * @param  reg: register to be written
  *         data: data to be written to the selected register
  * @retval None
  */
void SX9500_RegWrite(uint8_t reg, uint8_t data)
{
//I2C1->CR2 |= (uint32_t)(LIS302DL_ADDR) | (uint32_t)(1 << 16) | I2C_CR2_RELOAD | I2C_CR2_START ;
  I2C_TransferHandling(I2C1, SX9500_ADDR, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);
  while(!(I2C1->ISR & I2C_ISR_TXIS)){;}
  
//I2C1->TXDR = (uint8_t)(addr);
  I2C_SendData(I2C1, reg);
  while(!(I2C1->ISR & I2C_ISR_TCR)){;}
    
//I2C1->CR2 |= I2C_CR2_AUTOEND;
  I2C_AutoEndCmd(I2C1, ENABLE);
  while(!(I2C1->ISR & I2C_ISR_TXIS)){;}
    
//I2C1->TXDR = data;
  I2C_SendData(I2C1, data);
  
//I2C1->CR2 &= ~I2C_CR2_RELOAD;
  I2C_ReloadCmd(I2C1, DISABLE);
    
  while(!(I2C1->ISR & I2C_ISR_STOPF)){;}
  I2C1->ICR = I2C_ICR_STOPCF;
}

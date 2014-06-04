/******************************************************************************
 * Project        : STM32F0+SX9500
 * File           : task.c
 * Copyright      : 2014 Yosun Singapore Pte Ltd
 ******************************************************************************
  Change History:

    Version 1.0 - May 2014
    > Initial revision

******************************************************************************/
#include "sx9500.h"
#include "stm32f0_discovery.h"
#include "stm32f0xx_tim.h"
#include "task.h"

/* 15th bit is keylock, which means the flag is used */
/* bit 7 is for Reset event of SX9500 */
/* bit 6 is for Sensor detected a touch/proximity of SX9500 */
/* bit 5 is for Sensor detected a release condition */
/* bit 4 is for Compensation complete of SX9500 */
/* bit 3 is for Conversion cycle complete of SX9500 */
/* bit 2-1 reserve */
/* bit 0 is for Reports status of SX9500 */
static uint16_t EXTI0Flag = 0x0000;
static uint8_t keyState = 0x00;
static uint8_t CS3Flag = 0x00;

#define EXTI0Flag_LOCK	EXTI0Flag |= 0x8000;
#define EXTI0Flag_UNLOCK	EXTI0Flag &= 0x7FFF;
#define IS_EXTI0Flag_LOCK	((EXTI0Flag & 0x8000) == 0x8000)

#define SET_CS3_DETECTED	keyState |= 0x80;
#define SET_CS3_RELEASED	keyState |= 0x40;
#define SET_CS2_DETECTED	keyState |= 0x20;
#define SET_CS2_RELEASED	keyState |= 0x10;
#define SET_CS1_DETECTED	keyState |= 0x08;
#define SET_CS1_RELEASED	keyState |= 0x04;
#define SET_CS0_DETECTED	keyState |= 0x02;
#define SET_CS0_RELEASED	keyState |= 0x01;
#define CLR_CS3_DETECTED	keyState &= ~0x80;
#define CLR_CS3_RELEASED	keyState &= ~0x40;
#define CLR_CS2_DETECTED	keyState &= ~0x20;
#define CLR_CS2_RELEASED	keyState &= ~0x10;
#define CLR_CS1_DETECTED	keyState &= ~0x08;
#define CLR_CS1_RELEASED	keyState &= ~0x04;
#define CLR_CS0_DETECTED	keyState &= ~0x02;
#define CLR_CS0_RELEASED	keyState &= ~0x01;
#define CLR_CS3	keyState &= ~0xC0;
#define CLR_CS2	keyState &= ~0x30;
#define CLR_CS1	keyState &= ~0x0C;
#define CLR_CS0	keyState &= ~0x03;

#define IS_CS3_DETECTED	(((keyState & 0x80) == 0x80))
#define IS_CS2_DETECTED	(((keyState & 0x20) == 0x20))
#define IS_CS1_DETECTED	(((keyState & 0x08) == 0x08))
#define IS_CS0_DETECTED	(((keyState & 0x02) == 0x02))

#define IS_CS3_RELEASED	(((keyState & 0x40) == 0x40))
#define IS_CS2_RELEASED	(((keyState & 0x10) == 0x10))
#define IS_CS1_RELEASED	(((keyState & 0x04) == 0x04))
#define IS_CS0_RELEASED	(((keyState & 0x01) == 0x01))

#define IS_CS3_PRESSED	(((keyState & 0xC0) == 0xC0) || (IS_CS3_DETECTED))
#define IS_CS2_PRESSED	(((keyState & 0x30) == 0x30) || (IS_CS2_DETECTED))
#define IS_CS1_PRESSED	(((keyState & 0x0C) == 0x0C) || (IS_CS1_DETECTED))
#define IS_CS0_PRESSED	(((keyState & 0x03) == 0x0C) || (IS_CS0_DETECTED))

/**
  * @brief  This function initializes task.
  * @param  None
  * @retval None
  */
void init_task(void)
{
	/* reset it as default state */
	EXTI0Flag = 0x0000;
	
	keyState = 0x00;
	
	CS3Flag = 0x00;
}	

/**
  * @brief  This function creates task.
  * @param  event will be assigned to bit of static variable EXTI0Flag  
	* @retval 0 - success; -1 - failure
  */
int8_t create_task(uint16_t event)
{
	if(!IS_EXTI0Flag_LOCK)
	{
		EXTI0Flag_LOCK
		
		EXTI0Flag |= event;
		
		EXTI0Flag_UNLOCK
		
		return 0;
	}
	else
	{
		return -1;
	}
}

/**
  * @brief  This function deletes task.
  * @param  event will be assigned to bit of static variable EXTI0Flag  
	* @retval 0 - success; -1 - failure
  */
int8_t delete_task(uint16_t event)
{
	if(!IS_EXTI0Flag_LOCK)
	{
		EXTI0Flag_LOCK
		
		EXTI0Flag &= ~event;
		
		EXTI0Flag_UNLOCK
		
		return 0;
	}
	else
	{
		return -1;
	}
}

/**
  * @brief  This function executes task.
  * @param  none  
	* @retval 0 - success; -1 - failure
  */
int8_t exec_task(void)
{
	if(!IS_EXTI0Flag_LOCK)
	{
		if((EXTI0Flag & IRQStat_RESETIRQ) == IRQStat_RESETIRQ)
		{
			EXTI0Flag_LOCK
			
			SX9500_Setup();
			
			EXTI0Flag &= ~IRQStat_RESETIRQ;
			
			EXTI0Flag_UNLOCK
		}
		else if((EXTI0Flag & IRQStat_TCHIRQ) == IRQStat_TCHIRQ)
		{
			uint8_t reg_data = 0x00;
			
			EXTI0Flag_LOCK
			
			reg_data = SX9500_RegRead(SX9500_REG_TchCmpStat);
			
			if((CS3Flag == 0x00) && ((reg_data & TchCmpStat_TCHSTAT3) == TchCmpStat_TCHSTAT3))
			{
				/* touch/proximity has been detected on CS3 */
				
				SET_CS3_DETECTED
				
				CLR_CS3_RELEASED
				
				STM_EVAL_LEDOn(LED3);
				
				STM_EVAL_LEDOn(LED4);
				
				TIM_Cmd(TIM1, ENABLE);
				
				lock_CS3();
			}
			
			if((reg_data & TchCmpStat_TCHSTAT2) == TchCmpStat_TCHSTAT2)
			{
				/* touch/proximity has been detected on CS2 */
				
				SET_CS2_DETECTED
				
				CLR_CS2_RELEASED
			}
			
			if((reg_data & TchCmpStat_TCHSTAT1) == TchCmpStat_TCHSTAT1)
			{
				/* touch/proximity has been detected on CS1 */
				SET_CS1_DETECTED
				
				CLR_CS1_RELEASED
			}
			
			if((reg_data & TchCmpStat_TCHSTAT0) == TchCmpStat_TCHSTAT0)
			{
				/* touch/proximity has been detected on CS0 */
				SET_CS0_DETECTED
				
				CLR_CS0_RELEASED
			}
			
			EXTI0Flag &= ~IRQStat_TCHIRQ;
			
			EXTI0Flag_UNLOCK
		}
		else if((EXTI0Flag & IRQStat_RLSIRQ) == IRQStat_RLSIRQ)
		{
			EXTI0Flag_LOCK
			
			if(IS_CS3_DETECTED)
			{
				
				SET_CS3_RELEASED
				
				CLR_CS3
			}
			
			if(IS_CS2_DETECTED)
			{
				SET_CS2_RELEASED
				
				STM_EVAL_LEDOff(LED3);
				
				STM_EVAL_LEDOff(LED4);
				
				CLR_CS2
			}
			
			if(IS_CS1_DETECTED)
			{
				SET_CS1_RELEASED
				
				STM_EVAL_LEDToggle(LED3);
				
				CLR_CS1
			}
			
			if(IS_CS0_DETECTED)
			{
				SET_CS0_RELEASED
				
				STM_EVAL_LEDToggle(LED4);
				
				CLR_CS0
			}
			
			EXTI0Flag &= ~IRQStat_RLSIRQ;
			
			EXTI0Flag_UNLOCK
		}
		else if((EXTI0Flag & IRQStat_COMPDONE) == IRQStat_COMPDONE)
		{
			EXTI0Flag_LOCK
			
			EXTI0Flag &= ~IRQStat_COMPDONE;
			
			EXTI0Flag_UNLOCK
		}
		else if((EXTI0Flag & IRQStat_CONVIRQ) == IRQStat_CONVIRQ)
		{
			EXTI0Flag_LOCK
			
			EXTI0Flag &= ~IRQStat_CONVIRQ;
			
			EXTI0Flag_UNLOCK
		}
		else if((EXTI0Flag & IRQStat_TXENSTAT) == IRQStat_TXENSTAT)
		{
			EXTI0Flag_LOCK
			
			EXTI0Flag &= ~IRQStat_TXENSTAT;
			
			EXTI0Flag_UNLOCK
		}
		else
		{
		}
		
		return 0;
	}
	else
	{
		return -1;
	}
}

/**
  * @brief  This function locks CS3 sensor of SX9500.
  * @param  none  
	* @retval none
  */
void lock_CS3(void)
{
	CS3Flag |= 0x01;
}

/**
  * @brief  This function unlocks CS3 sensor of SX9500.
  * @param  none  
	* @retval none
  */
void unlock_CS3(void)
{
	CS3Flag &= (~0x01);
}

/**
  * @brief  This function checks if CS2 is pressed.
  * @param  none  
	* @retval 1 - pressed; 0 - released
  */
int8_t is_CS2Pressed(void)
{
	if(IS_CS2_PRESSED)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
  * @brief  This function checks if CS1 is pressed.
  * @param  none  
	* @retval 1 - pressed; 0 - released
  */
int8_t is_CS1Pressed(void)
{
	if(IS_CS1_PRESSED)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
  * @brief  This function checks if CS0 is pressed.
  * @param  none  
	* @retval 1 - pressed; 0 - released
  */
int8_t is_CS0Pressed(void)
{
	if(IS_CS0_PRESSED)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


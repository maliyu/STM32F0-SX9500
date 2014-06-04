/******************************************************************************
 * Project        : STM32F0+SX9500
 * File           : task.h
 * Copyright      : 2014 Yosun Singapore Pte Ltd
 ******************************************************************************
  Change History:

    Version 1.0 - May 28, 2014
    > Initial revision
******************************************************************************/
#ifndef _TASK_H_
#define _TASK_H_
#include "stm32f0xx.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
typedef enum {NO_EVENT=0, PROXIMITY_SENSE=1, PROXIMITY_RELEASE=2, TOUCH_DETECT=3, TOUCH_RELEASE=4} EventName;


// Registers


// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void init_task(void);
int8_t create_task(uint16_t event);
int8_t delete_task(uint16_t event);
int8_t exec_task(void);
void lock_CS3(void);
void unlock_CS3(void);
int8_t is_CS2Pressed(void);
int8_t is_CS1Pressed(void);
int8_t is_CS0Pressed(void);

#endif /* _TASK_H_ */

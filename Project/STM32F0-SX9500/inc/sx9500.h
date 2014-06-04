/******************************************************************************
 * Project        : STM32F0+SX9500
 * File           : sx9500.h
 * Copyright      : 2014 Yosun Singapore Pte Ltd
 ******************************************************************************
  Change History:

    Version 1.0 - May 28, 2014
    > Initial revision

******************************************************************************
   Hardware:
                       VDD Semtech SX9500 EVK
												|	 +-------------+
                        |--+SVDD         |
   ------------+        +--+VDD          |
               |   GND|----+GND          |
   I2C1_SCL/PB6+-----------+SCL					 |
   I2C1_SDA/PB7+-----------+SDA					 |
            PA0+-----------+NIRQ		     |
            PB8+-----------+NRST         |
               |           |             |
   ------------+       		 |		         |
													 |		         |
                           |             |
                           +-------------+

******************************************************************************/
#ifndef _SX9500_H_
#define _SX9500_H_

#include "stm32f0xx.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define SX9500_ADDR          ((uint16_t)(0x2B))
#define IRQStat_RESETIRQ		 ((uint8_t)0x80)	/* Reset event occurred */
#define IRQStat_TCHIRQ			 ((uint8_t)0x40)	/* Sensor detected a touch/proximity */
#define IRQStat_RLSIRQ			 ((uint8_t)0x20)	/* Sensor detected a release condition */
#define IRQStat_COMPDONE		 ((uint8_t)0x10)	/* Compensation complete. Writing a one in this bit trigs a compensation on all channels */
#define IRQStat_CONVIRQ			 ((uint8_t)0x08)	/* Conversion cycle complete */
#define IRQStat_TXENSTAT		 ((uint8_t)0x01)	/* Reports TXEN pad status */
#define TchCmpStat_TCHSTAT3	 ((uint8_t)0x80)	/* Determines if touch/proximity has been detected on CS3 */
#define TchCmpStat_TCHSTAT2	 ((uint8_t)0x40)	/* Determines if touch/proximity has been detected on CS2 */
#define TchCmpStat_TCHSTAT1	 ((uint8_t)0x20)	/* Determines if touch/proximity has been detected on CS1 */
#define TchCmpStat_TCHSTAT0	 ((uint8_t)0x10)	/* Determines if touch/proximity has been detected on CS0 */
#define TchCmpStat_COMPSTAT_CS3	 ((uint8_t)0x08)	/* Specifies capacitive sensor CS3 has a compensation pending */
#define TchCmpStat_COMPSTAT_CS2	 ((uint8_t)0x04)	/* Specifies capacitive sensor CS2 has a compensation pending */
#define TchCmpStat_COMPSTAT_CS1	 ((uint8_t)0x02)	/* Specifies capacitive sensor CS1 has a compensation pending */
#define TchCmpStat_COMPSTAT_CS0	 ((uint8_t)0x01)	/* Specifies capacitive sensor CS0 has a compensation pending */
#define IRQ_Enable_TCHIRQEN		((uint8_t)0x40)		/* Enables the detection irq */
#define IRQ_Enable_RLSIRQEN		((uint8_t)0x20)		/* Enables the release irq */
#define IRQ_Enable_COMPDONEIRQEN		((uint8_t)0x10)		/* Enables the compensation irq */
#define IRQ_Enable_CONVIRQEN		((uint8_t)0x08)		/* Enables the conversion irq */
#define CPS_CTRL0_CPS_PERIOD_30ms	((uint8_t)0x0<<4)	/* Scan period is 30ms */
#define CPS_CTRL0_CPS_PERIOD_60ms	((uint8_t)0x1<<4)	/* Scan period is 60ms */
#define CPS_CTRL0_CPS_PERIOD_90ms	((uint8_t)0x2<<4)	/* Scan period is 90ms */
#define CPS_CTRL0_CPS_PERIOD_120ms	((uint8_t)0x3<<4)	/* Scan period is 120ms */
#define CPS_CTRL0_CPS_PERIOD_150ms	((uint8_t)0x4<<4)	/* Scan period is 150ms */
#define CPS_CTRL0_CPS_PERIOD_200ms	((uint8_t)0x5<<4)	/* Scan period is 200ms */
#define CPS_CTRL0_CPS_PERIOD_300ms	((uint8_t)0x6<<4)	/* Scan period is 300ms */
#define CPS_CTRL0_CPS_PERIOD_400ms	((uint8_t)0x7<<4)	/* Scan period is 400ms */
#define CPS_CTRL0_CS3_EN	((uint8_t)0x08)	/* Enables CS3 */
#define CPS_CTRL0_CS2_EN	((uint8_t)0x04)	/* Enables CS2 */
#define CPS_CTRL0_CS1_EN	((uint8_t)0x02)	/* Enables CS1 */
#define CPS_CTRL0_CS0_EN	((uint8_t)0x01)	/* Enables CS0 */
#define CPS_CTRL1_CPS_SH_ON	((uint8_t)0x01<<6)	/* CG bias/shield usage is On(def.) */
#define CPS_CTRL1_CPS_SH_OFF	((uint8_t)0x00<<6)	/* CG bias/shield usage is Off, CG high-Z (off) */
#define CPS_CTRL1_CPS_CINR_Large	((uint8_t)0x00) /* Capacitance Range & Resolution: */
#define CPS_CTRL1_CPS_CINR_Medium_Large	((uint8_t)0x01) /* Capacitance Range & Resolution: */
#define CPS_CTRL1_CPS_CINR_Medium_Small	((uint8_t)0x02) /* Capacitance Range & Resolution: */
#define CPS_CTRL1_CPS_CINR_Small	((uint8_t)0x03) /* Capacitance Range & Resolution: */
#define CPS_CTRL2_CPS_Digital_GAIN_1	((uint8_t)0x00<<5)	/* Digital gain == 1 */
#define CPS_CTRL2_CPS_Digital_GAIN_2	((uint8_t)0x01<<5)	/* Digital gain == 2 */
#define CPS_CTRL2_CPS_Digital_GAIN_4	((uint8_t)0x02<<5)	/* Digital gain == 4 */
#define CPS_CTRL2_CPS_Digital_GAIN_8	((uint8_t)0x03<<5)	/* Digital gain == 8 */
#define CPS_CTRL2_CPS_FS_83KHz	((uint8_t)0x00<<3)	/* Sampling frequency == 83KHz */
#define CPS_CTRL2_CPS_FS_125KHz	((uint8_t)0x01<<3)	/* Sampling frequency == 125KHz */
#define CPS_CTRL2_CPS_FS_167KHz	((uint8_t)0x02<<3)	/* Sampling frequency == 167KHz */
#define CPS_CTRL2_CPS_RES_0	((uint8_t)0x00)	/* Resolution Control - Coarsest */
#define CPS_CTRL2_CPS_RES_1	((uint8_t)0x01)	/* Resolution Control -  Very Coarse */
#define CPS_CTRL2_CPS_RES_2	((uint8_t)0x02)	/* Resolution Control -  Coarse */
#define CPS_CTRL2_CPS_RES_3	((uint8_t)0x03)	/* Resolution Control -  Medium Coarse */
#define CPS_CTRL2_CPS_RES_4	((uint8_t)0x04)	/* Resolution Control -  Medium */
#define CPS_CTRL2_CPS_RES_5	((uint8_t)0x05)	/* Resolution Control -  Fine */
#define CPS_CTRL2_CPS_RES_6	((uint8_t)0x06)	/* Resolution Control -  Very Fine */
#define CPS_CTRL2_CPS_RES_7	((uint8_t)0x07)	/* Resolution Control -  Finest */
#define CPS_CTRL3_CPS_DOZEEN	((uint8_t)0x01<<6)	/* Enables doze mode */
#define CPS_CTRL3_CPS_DOZEDIS	((uint8_t)0x00<<6)	/* Disable doze mode */
#define CPS_CTRL3_CPS_DOZEPERIOD_2	((uint8_t)0x00<<4)	/* When doze is enabled, the cap sensing period moves from CPS_PERIOD to CPS_PERIOD * : 00: 2*CPS_PERIOD */
#define CPS_CTRL3_CPS_DOZEPERIOD_4	((uint8_t)0x01<<4)	/* When doze is enabled, the cap sensing period moves from CPS_PERIOD to CPS_PERIOD * : 00: 4*CPS_PERIOD */
#define CPS_CTRL3_CPS_DOZEPERIOD_8	((uint8_t)0x02<<4)	/* When doze is enabled, the cap sensing period moves from CPS_PERIOD to CPS_PERIOD * : 00: 8*CPS_PERIOD */
#define CPS_CTRL3_CPS_DOZEPERIOD_16	((uint8_t)0x03<<4)	/* When doze is enabled, the cap sensing period moves from CPS_PERIOD to CPS_PERIOD * : 00: 16*CPS_PERIOD */
#define CPS_CTRL3_CPS_RAWFILT_OFF	((uint8_t)0x00)	/* Raw filter coefficient: Off */
#define CPS_CTRL3_CPS_RAWFILT_LOW	((uint8_t)0x01)	/* Raw filter coefficient: Low */
#define CPS_CTRL3_CPS_RAWFILT_MEDIUM	((uint8_t)0x02)	/* Raw filter coefficient: Medium */
#define CPS_CTRL3_CPS_RAWFILT_HIGH	((uint8_t)0x03)	/* Raw filter coefficient: High (Max Filtering) */
#define CPS_CTRL5_CPS_AVGDEB_0	((uint8_t)0x00<<6)	/* Average pos/neg debouncer: 00: off */
#define CPS_CTRL5_CPS_AVGDEB_2	((uint8_t)0x01<<6)	/* Average pos/neg debouncer: 01: 2 samples */
#define CPS_CTRL5_CPS_AVGDEB_4	((uint8_t)0x02<<6)	/* Average pos/neg debouncer: 02: 4 samples */
#define CPS_CTRL5_CPS_AVGDEB_8	((uint8_t)0x03<<6)	/* Average pos/neg debouncer: 03: 8 samples */
#define CPS_CTRL5_CPS_AVGNEGFILT_0	((uint8_t)0x00<<3)	/* Average negative filter coefficient : 000: off */
#define CPS_CTRL5_CPS_AVGNEGFILT_1	((uint8_t)0x01<<3)	/* Average negative filter coefficient : 001: Lowest */
#define CPS_CTRL5_CPS_AVGNEGFILT_2	((uint8_t)0x02<<3)	/* Average negative filter coefficient : 002: */
#define CPS_CTRL5_CPS_AVGNEGFILT_3	((uint8_t)0x03<<3)	/* Average negative filter coefficient : 003: */
#define CPS_CTRL5_CPS_AVGNEGFILT_4	((uint8_t)0x04<<3)	/* Average negative filter coefficient : 004: */
#define CPS_CTRL5_CPS_AVGNEGFILT_5	((uint8_t)0x05<<3)	/* Average negative filter coefficient : 005: */
#define CPS_CTRL5_CPS_AVGNEGFILT_6	((uint8_t)0x06<<3)	/* Average negative filter coefficient : 006: */
#define CPS_CTRL5_CPS_AVGNEGFILT_7	((uint8_t)0x07<<3)	/* Average negative filter coefficient : 007: Highest (Max. Filter) */
#define CPS_CTRL5_CPS_AVGPOSFILT_0	((uint8_t)0x00)	/* Average positive filter coefficient : 000: off */
#define CPS_CTRL5_CPS_AVGPOSFILT_1	((uint8_t)0x01)	/* Average positive filter coefficient : 001: Lowest */
#define CPS_CTRL5_CPS_AVGPOSFILT_2	((uint8_t)0x02)	/* Average positive filter coefficient : 002: */
#define CPS_CTRL5_CPS_AVGPOSFILT_3	((uint8_t)0x03)	/* Average positive filter coefficient : 003: */
#define CPS_CTRL5_CPS_AVGPOSFILT_4	((uint8_t)0x04)	/* Average positive filter coefficient : 004: */
#define CPS_CTRL5_CPS_AVGPOSFILT_5	((uint8_t)0x05)	/* Average positive filter coefficient : 005: */
#define CPS_CTRL5_CPS_AVGPOSFILT_6	((uint8_t)0x06)	/* Average positive filter coefficient : 006: */
#define CPS_CTRL5_CPS_AVGPOSFILT_7	((uint8_t)0x07)	/* Average positive filter coefficient : 007: Highest (Max. Filter) */
#define CPS_CTRL7_CPS_CMPAUTOOFF	((uint8_t)0x01<<7)	/* Disables the automatic compensation trigged by average */
#define CPS_CTRL7_CPS_CMPAUTOON	((uint8_t)0x00<<7)	/* Enables the automatic compensation trigged by average */
#define CPS_CTRL7_CPS_CMPTRG_INDEPEND	((uint8_t)0x00<<6)	/* compensate channels independently */
#define CPS_CTRL7_CPS_CMPTRG_ALL	((uint8_t)0x01<<6)	/* compensate all channels when triggered */
#define CPS_CTRL7_CPS_HYST_32	((uint8_t)0x00<<4)	/* Detection hysteresis 00: 32 */
#define CPS_CTRL7_CPS_HYST_64	((uint8_t)0x01<<4)	/* Detection hysteresis 01: 64 */
#define CPS_CTRL7_CPS_HYST_128	((uint8_t)0x02<<4)	/* Detection hysteresis 02: 128 */
#define CPS_CTRL7_CPS_HYST_256	((uint8_t)0x03<<4)	/* Detection hysteresis 03: 256 */
#define CPS_CTRL7_CPS_CLSDEB_0	((uint8_t)0x00<<2)	/* Close debouncer: off */
#define CPS_CTRL7_CPS_CLSDEB_2	((uint8_t)0x01<<2)	/* Close debouncer: 2 samples */
#define CPS_CTRL7_CPS_CLSDEB_4	((uint8_t)0x02<<2)	/* Close debouncer: 4 samples */
#define CPS_CTRL7_CPS_CLSDEB_8	((uint8_t)0x03<<2)	/* Close debouncer: 8 samples */
#define CPS_CTRL7_CPS_FARDEB_0	((uint8_t)0x00)	/* Far debouncer: off */
#define CPS_CTRL7_CPS_FARDEB_2	((uint8_t)0x01)	/* Far debouncer: 2 samples */
#define CPS_CTRL7_CPS_FARDEB_4	((uint8_t)0x02)	/* Far debouncer: 4 samples */
#define CPS_CTRL7_CPS_FARDEB_8	((uint8_t)0x03)	/* Far debouncer: 8 samples */
#define CPS_CTRL8_CPS_STUCK_OFF	((uint8_t)0x00<<4)	/* Stuck at timeout timer: off */
#define CPS_CTRL8_CPS_CMPPRD_OFF	((uint8_t)0x00)	/* Periodic compensation off */


// Registers
#define SX9500_REG_IRQStat						((uint8_t)(0x00))
#define SX9500_REG_TchCmpStat					((uint8_t)(0x01))
#define SX9500_REG_IRQ_Enable					((uint8_t)(0x03))
#define SX9500_REG_CPS_CTRL0					((uint8_t)(0x06))
#define SX9500_REG_CPS_CTRL1					((uint8_t)(0x07))
#define SX9500_REG_CPS_CTRL2					((uint8_t)(0x08))
#define SX9500_REG_CPS_CTRL3					((uint8_t)(0x09))
#define SX9500_REG_CPS_CTRL4					((uint8_t)(0x0A))
#define SX9500_REG_CPS_CTRL5					((uint8_t)(0x0B))
#define SX9500_REG_CPS_CTRL6					((uint8_t)(0x0C))
#define SX9500_REG_CPS_CTRL7					((uint8_t)(0x0D))
#define SX9500_REG_CPS_CTRL8					((uint8_t)(0x0E))
#define SX9500_REG_CPS_CPSRD					((uint8_t)(0x20))
#define SX9500_REG_CPS_UseMSB					((uint8_t)(0x21))
#define SX9500_REG_CPS_UseLSB					((uint8_t)(0x22))
#define SX9500_REG_CPS_AvgMSB					((uint8_t)(0x23))
#define SX9500_REG_CPS_AvgLSB					((uint8_t)(0x24))
#define SX9500_REG_CPS_DiffMSB				((uint8_t)(0x25))
#define SX9500_REG_CPS_DiffLSB				((uint8_t)(0x26))
#define SX9500_REG_CPS_OffMSB					(uint8_t)(0x27))
#define SX9500_REG_CPS_OffLSB					((uint8_t)(0x28))
#define SX9500_REG_CPS_I2CSoftReset		((uint8_t)(0x7F))

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void STM32F0_Setup(void);
void SX9500_Setup(void);
uint8_t SX9500_RegRead(uint8_t reg);
void SX9500_RegWrite(uint8_t reg, uint8_t data);

#endif /* _SX9500_H_ */

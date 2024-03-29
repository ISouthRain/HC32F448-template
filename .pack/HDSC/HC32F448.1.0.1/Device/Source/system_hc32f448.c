/**
 *******************************************************************************
 * @file  system_hc32f448.c
 * @brief This file provides two functions and two global variables to be called
 *        from user application.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2023-05-31       CDT             First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022-2023, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "system_hc32f448.h"
#include "hc32f448.h"

/**
 * @addtogroup CMSIS
 * @{
 */

/**
 * @addtogroup HC32F448_System
 * @{
 */

/*******************************************************************************
 * Global pre-processor symbols/macros ('define')
 ******************************************************************************/
/**
 * @defgroup HC32F448_System_Local_Macros HC32F448 System Local Macros
 * @{
 */
#define HRC_16MHz_VALUE                 (16000000UL)  /*!< Internal high speed RC freq. */
#define HRC_20MHz_VALUE                 (20000000UL)  /*!< Internal high speed RC freq. */
/* HRC select */
#define HRC_FREQ_MON()                  (*((volatile uint32_t *)(0x40010684UL)))

/* Vector Table base offset field */
#ifndef VECT_TAB_OFFSET
#define VECT_TAB_OFFSET                 (0x0UL)     /*!< This value must be a multiple of 0x400. */
#endif
/* Compiler Macros */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#ifndef __NO_INIT
#define __NO_INIT                       __attribute__((section(".bss.noinit")))
#endif /* __NO_INIT */
#elif defined ( __GNUC__ ) && !defined (__CC_ARM) /*!< GNU Compiler */
#ifndef __NO_INIT
#define __NO_INIT                       __attribute__((section(".noinit")))
#endif /* __NO_INIT */
#elif defined (__ICCARM__)              /*!< IAR Compiler */
#ifndef __NO_INIT
#define __NO_INIT                       __no_init
#endif /* __NO_INIT */
#elif defined (__CC_ARM)                /*!< ARM Compiler */
#ifndef __NO_INIT
#define __NO_INIT                       __attribute__((section(".bss.noinit"), zero_init))
#endif /* __NO_INIT */
#endif
/**
 * @}
 */

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/**
 * @addtogroup HC32F448_System_Global_Variable
 * @{
 */

/*!< System clock frequency (Core clock) */
__NO_INIT uint32_t SystemCoreClock;
/*!< High speed RC frequency (HCR clock) */
__NO_INIT uint32_t HRC_VALUE;

/**
 * @}
 */

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @addtogroup HC32F448_System_Global_Functions
 * @{
 */

/**
 * @brief  Setup the microcontroller system. Initialize the System and update
 *         the SystemCoreClock variable.
 * @param  None
 * @retval None
 */
void SystemInit(void)
{
    /* FPU settings */
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 20) | (3UL << 22)); /* set CP10 and CP11 Full Access */
#endif
    SystemCoreClockUpdate();
    /* Configure the Vector Table relocation */
    SCB->VTOR = VECT_TAB_OFFSET;    /* Vector Table Relocation */
}

/**
 * @brief  Update SystemCoreClock variable according to Clock Register Values.
 * @param  None
 * @retval None
 */
void SystemCoreClockUpdate(void)
{
    uint8_t u8SysClkSrc;
    uint32_t plln;
    uint32_t pllp;
    uint32_t pllm;
    uint32_t u32PllSrcFreq;

    /* Select proper HRC_VALUE according to ICG1.HRCFREQSEL bit */
    if (1UL == (HRC_FREQ_MON() & 1UL)) {
        HRC_VALUE = HRC_16MHz_VALUE;
    } else {
        HRC_VALUE = HRC_20MHz_VALUE;
    }
    u8SysClkSrc = CM_CMU->CKSWR & CMU_CKSWR_CKSW;
    switch (u8SysClkSrc) {
        case 0x00U: /* use internal high speed RC */
            SystemCoreClock = HRC_VALUE;
            break;
        case 0x01U: /* use internal middle speed RC */
            SystemCoreClock = MRC_VALUE;
            break;
        case 0x02U: /* use internal low speed RC */
            SystemCoreClock = LRC_VALUE;
            break;
        case 0x03U: /* use external high speed OSC */
            SystemCoreClock = XTAL_VALUE;
            break;
        case 0x04U: /* use external low speed OSC */
            SystemCoreClock = XTAL32_VALUE;
            break;
        case 0x05U:  /* use PLLH */
            /* PLLCLK = ((pllsrc / pllm) * plln) / pllp */
            plln = (CM_CMU->PLLHCFGR & CMU_PLLHCFGR_PLLHN) >> CMU_PLLHCFGR_PLLHN_POS;
            pllp = (CM_CMU->PLLHCFGR & CMU_PLLHCFGR_PLLHP) >> CMU_PLLHCFGR_PLLHP_POS;
            pllm = (CM_CMU->PLLHCFGR & CMU_PLLHCFGR_PLLHM) >> CMU_PLLHCFGR_PLLHM_POS;
            if (0UL == (CM_CMU->PLLHCFGR & CMU_PLLHCFGR_PLLSRC)) {    /* use external high speed OSC as PLL source */
                u32PllSrcFreq = XTAL_VALUE;
            } else {                                    /* use internal high RC as PLL source */
                u32PllSrcFreq = HRC_VALUE;
            }
            SystemCoreClock = u32PllSrcFreq / (pllm + 1UL) * (plln + 1UL) / (pllp + 1UL);
            break;
        default:
            break;
    }
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

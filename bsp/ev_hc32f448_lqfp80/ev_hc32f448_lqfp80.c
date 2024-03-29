/**
 *******************************************************************************
 * @file  ev_hc32f448_lqfp80.c
 * @brief This file provides firmware functions for EV_HC32F448_LQFP80 BSP
 @verbatim
   Change Logs:
   Date             Author          Notes
   2023-05-31       CDT             First version
   2023-12-15       CDT             Add API BSP_XTAL32_Init()
                                    Optimize function BSP_I2C_Init()
                                    Update EXCLK clock frequency: 100MHz -> 50MHZ in function BSP_CLK_Init()
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
#include "ev_hc32f448_lqfp80.h"

/**
 * @defgroup BSP BSP
 * @{
 */

/**
 * @defgroup EV_HC32F448_LQFP80 HC32F448_LQFP80_EVB
 * @{
 */

#if (BSP_EV_HC32F448_LQFP80 == BSP_EV_HC32F4XX)

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/**
 * @defgroup BSP_Local_Types BSP Local Types
 * @{
 */
typedef struct {
    uint8_t port;
    uint16_t pin;
    uint16_t func;
} BSP_Port_Pin;

typedef struct {
    uint8_t      port;
    uint16_t     pin;
    uint32_t     ch;
    IRQn_Type    irq;
} BSP_KeyIn_Config;
/**
 * @}
 */

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
#if (DDL_ON == LL_KEYSCAN_ENABLE)
/**
 * @addtogroup BSP_Local_Functions
 * @{
 */
static void BSP_KEY_ROW_Init(void);
static void BSP_KEY_COL_Init(void);
/**
 * @}
 */

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
/**
* @defgroup BSP_Local_Variables BSP Local Variables
* @{
*/
static const BSP_Port_Pin BSP_KEYOUT_PORT_PIN[BSP_KEY_COL_NUM] = {
    {BSP_KEYOUT3_PORT, BSP_KEYOUT3_PIN, BSP_KEYOUT3_FUNC},
    {BSP_KEYOUT7_PORT, BSP_KEYOUT7_PIN, BSP_KEYOUT7_FUNC},
};

static const BSP_KeyIn_Config BSP_KEYIN_PORT_PIN[BSP_KEY_ROW_NUM + BSP_KEY_INDEPENDENT_NUM] = {
    {BSP_KEYIN0_PORT, BSP_KEYIN0_PIN, BSP_KEY_ROW0_EXTINT, BSP_KEY_ROW0_IRQn},
    {BSP_KEYIN1_PORT, BSP_KEYIN1_PIN, BSP_KEY_ROW1_EXTINT, BSP_KEY_ROW1_IRQn},
    {BSP_KEY_KEY5_PORT, BSP_KEY_KEY5_PIN, BSP_KEY_KEY5_EXTINT, BSP_KEY_KEY5_IRQn},
};

static uint32_t m_u32GlobalKey = 0UL;
/**
 * @}
 */
#endif
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @defgroup EV_HC32F448_LQFP80_Global_Functions BSP Global Functions
 * @{
 */

#if (LL_I2C_ENABLE == DDL_ON)
/**
 * @brief  BSP I2C initialize
 * @param  [in] I2Cx                Pointer to I2C instance register base.
 *                                  This parameter can be a value of the following:
 *         @arg CM_I2Cx:            I2C instance register base.
 * @retval int32_t:
 *            - LL_OK:              Configure success
 *            - LL_ERR_INVD_PARAM:  Invalid parameter
 */
int32_t BSP_I2C_Init(CM_I2C_TypeDef *I2Cx)
{
    int32_t i32Ret;
    float32_t fErr;
    stc_i2c_init_t stcI2cInit;
    uint32_t I2cSrcClk;
    uint32_t I2cClkDiv;
    uint32_t I2cClkDivReg;

    I2cSrcClk = I2C_SRC_CLK;
    I2cClkDiv = I2cSrcClk / BSP_I2C_BAUDRATE / I2C_WIDTH_MAX_IMME;
    for (I2cClkDivReg = I2C_CLK_DIV1; I2cClkDivReg <= I2C_CLK_DIV128; I2cClkDivReg++) {
        if (I2cClkDiv < (1UL << I2cClkDivReg)) {
            break;
        }
    }

    (void)I2C_DeInit(I2Cx);
    (void)I2C_StructInit(&stcI2cInit);
    stcI2cInit.u32Baudrate = BSP_I2C_BAUDRATE;
    stcI2cInit.u32SclTime  = (uint32_t)((uint64_t)180UL * ((uint64_t)I2cSrcClk / ((uint64_t)1UL << I2cClkDivReg)) / (uint64_t)1000000000UL);  /* SCL time is about 180nS in EVB board */
    stcI2cInit.u32ClockDiv = I2cClkDivReg;
    i32Ret = I2C_Init(I2Cx, &stcI2cInit, &fErr);

    if (LL_OK == i32Ret) {
        I2C_BusWaitCmd(I2Cx, ENABLE);
        I2C_Cmd(I2Cx, ENABLE);
    }

    return i32Ret;
}

/**
 * @brief  BSP I2C De-initialize
 * @param  [in] I2Cx                Pointer to I2C instance register base.
 *                                  This parameter can be a value of the following:
 *         @arg CM_I2Cx:            I2C instance register base.
 * @retval None
 */
void BSP_I2C_DeInit(CM_I2C_TypeDef *I2Cx)
{
    (void)I2C_DeInit(I2Cx);
}

/**
 * @brief  BSP I2C write.
 * @param  [in] I2Cx                Pointer to I2C instance register base.
 *                                  This parameter can be a value of the following:
 *         @arg CM_I2Cx:            I2C instance register base.
 * @param  [in] u16DevAddr:         Device address.
 * @param  [in] pu8Reg:             Pointer to the register address or memory address.
 * @param  [in] u8RegLen:           Length of register address or memory address.
 * @param  [in] pu8Buf:             The pointer to the buffer contains the data to be write.
 * @param  [in] u32Len:             Buffer size in byte.
 * @retval int32_t:
 *            - LL_OK:              Success
 *            - LL_ERR:             Receive NACK
 *            - LL_ERR_TIMEOUT:     Timeout
 *            - LL_ERR_INVD_PARAM:  pu8Buf is NULL
 */
int32_t BSP_I2C_Write(CM_I2C_TypeDef *I2Cx, uint16_t u16DevAddr, const uint8_t *pu8Reg, uint8_t u8RegLen, const uint8_t *pu8Buf, uint32_t u32Len)
{
    int32_t i32Ret;

    I2C_SWResetCmd(I2Cx, ENABLE);
    I2C_SWResetCmd(I2Cx, DISABLE);
    i32Ret = I2C_Start(I2Cx, BSP_I2C_TIMEOUT);
    if (LL_OK == i32Ret) {
        i32Ret = I2C_TransAddr(I2Cx, u16DevAddr, I2C_DIR_TX, BSP_I2C_TIMEOUT);

        if (LL_OK == i32Ret) {
            i32Ret = I2C_TransData(I2Cx, pu8Reg, u8RegLen, BSP_I2C_TIMEOUT);
            if (LL_OK == i32Ret) {
                i32Ret = I2C_TransData(I2Cx, pu8Buf, u32Len, BSP_I2C_TIMEOUT);
            }
        }
    }
    (void)I2C_Stop(I2Cx, BSP_I2C_TIMEOUT);
    return i32Ret;
}

/**
 * @brief  BSP I2C read.
 * @param  [in] I2Cx                Pointer to I2C instance register base.
 *                                  This parameter can be a value of the following:
 *         @arg CM_I2Cx:            I2C instance register base.
 * @param  [in] u16DevAddr:         Device address.
 * @param  [in] pu8Reg:             Pointer to the register address or memory address.
 * @param  [in] u8RegLen:           Length of register address or memory address.
 * @param  [in] pu8Buf:             The pointer to the buffer contains the data to be read.
 * @param  [in] u32Len:             Buffer size in byte.
 * @retval int32_t:
 *            - LL_OK:              Success
 *            - LL_ERR:             Receive NACK
 *            - LL_ERR_TIMEOUT:     Timeout
 *            - LL_ERR_INVD_PARAM:  pu8Buf is NULL
 */
int32_t BSP_I2C_Read(CM_I2C_TypeDef *I2Cx, uint16_t u16DevAddr, const uint8_t *pu8Reg, uint8_t u8RegLen, uint8_t *pu8Buf, uint32_t u32Len)
{
    int32_t i32Ret;
    I2C_SWResetCmd(I2Cx, ENABLE);
    I2C_SWResetCmd(I2Cx, DISABLE);
    i32Ret = I2C_Start(I2Cx, BSP_I2C_TIMEOUT);
    if (LL_OK == i32Ret) {
        i32Ret = I2C_TransAddr(I2Cx, u16DevAddr, I2C_DIR_TX, BSP_I2C_TIMEOUT);

        if (LL_OK == i32Ret) {
            i32Ret = I2C_TransData(I2Cx, pu8Reg, u8RegLen, BSP_I2C_TIMEOUT);
            if (LL_OK == i32Ret) {
                i32Ret = I2C_Restart(I2Cx, BSP_I2C_TIMEOUT);
                if (LL_OK == i32Ret) {
                    if (1UL == u32Len) {
                        I2C_AckConfig(I2Cx, I2C_NACK);
                    }

                    i32Ret = I2C_TransAddr(I2Cx, u16DevAddr, I2C_DIR_RX, BSP_I2C_TIMEOUT);
                    if (LL_OK == i32Ret) {
                        i32Ret = I2C_MasterReceiveDataAndStop(I2Cx, pu8Buf, u32Len, BSP_I2C_TIMEOUT);
                    }
                    I2C_AckConfig(I2Cx, I2C_ACK);
                }
            }
        }
    }

    if (LL_OK != i32Ret) {
        (void)I2C_Stop(I2Cx, BSP_I2C_TIMEOUT);
    }

    return i32Ret;
}

/**
 * @brief  BSP 24CXX status get.
 * @param  [in] I2Cx                Pointer to I2C instance register base.
 *                                  This parameter can be a value of the following:
 *         @arg CM_I2Cx:            I2C instance register base.
 * @param  [in] u16DevAddr:         Device address.
 * @retval int32_t:
 *            - LL_OK:              Idle
 *            - LL_ERR:             Receive NACK
 *            - LL_ERR_TIMEOUT:     Timeout
 *            - LL_ERR_INVD_PARAM:  pu8Buf is NULL
 */
int32_t BSP_I2C_GetDevStatus(CM_I2C_TypeDef *I2Cx, uint16_t u16DevAddr)
{
    int32_t i32Ret;

    i32Ret = I2C_Start(I2Cx, BSP_I2C_TIMEOUT);
    if (LL_OK == i32Ret) {
        i32Ret = I2C_TransAddr(I2Cx, u16DevAddr, I2C_DIR_TX, BSP_I2C_TIMEOUT);

        if (LL_OK == i32Ret) {
            if (SET == I2C_GetStatus(I2Cx, I2C_FLAG_ACKR)) {
                i32Ret = LL_ERR;
            }
        }
    }
    (void)I2C_Stop(I2Cx, BSP_I2C_TIMEOUT);
    return i32Ret;
}
#endif /* LL_I2C_ENABLE */

/**
 * @brief  BSP clock initialize.
 *         SET board system clock to PLLH@200MHz
 *         Flash: 3 wait
 *         SRAM_H: 0 wait
 *         SRAM_B: 1 wait
 *         PCLK0: 200MHz
 *         PCLK1: 100MHz
 *         PCLK2: 50MHz
 *         PCLK3: 50MHz
 *         PCLK4: 100MHz
 *         EXCLK: 50MHz
 *         HCLK:  200MHz
 * @param  None
 * @retval None
 */
__WEAKDEF void BSP_CLK_Init(void)
{
    stc_clock_xtal_init_t stcXtalInit;
    stc_clock_pll_init_t stcPLLHInit;

    /* PCLK0, HCLK Max 200MHz */
    /* PCLK1, PCLK4 Max 100MHz */
    /* PCLK2, EXCLK Max 60MHz */
    /* PCLK3 Max 50MHz */
    CLK_SetClockDiv(CLK_BUS_CLK_ALL,
                    (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV4 |
                     CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV4 |
                     CLK_HCLK_DIV1));

    GPIO_AnalogCmd(BSP_XTAL_PORT, BSP_XTAL_IN_PIN | BSP_XTAL_OUT_PIN, ENABLE);
    (void)CLK_XtalStructInit(&stcXtalInit);
    /* Config Xtal and enable Xtal */
    stcXtalInit.u8Mode   = CLK_XTAL_MD_OSC;
    stcXtalInit.u8Drv    = CLK_XTAL_DRV_ULOW;
    stcXtalInit.u8State  = CLK_XTAL_ON;
    stcXtalInit.u8StableTime = CLK_XTAL_STB_2MS;
    (void)CLK_XtalInit(&stcXtalInit);

    (void)CLK_PLLStructInit(&stcPLLHInit);
    /* VCO = (8/1)*100 = 800MHz*/
    stcPLLHInit.u8PLLState = CLK_PLL_ON;
    stcPLLHInit.PLLCFGR = 0UL;
    stcPLLHInit.PLLCFGR_f.PLLM = 1UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLN = 100UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLP = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLQ = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLR = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLSRC = CLK_PLL_SRC_XTAL;
    (void)CLK_PLLInit(&stcPLLHInit);

    /* 3 cycles for 150 ~ 200MHz */
    (void)EFM_SetWaitCycle(EFM_WAIT_CYCLE3);
    /* 3 cycles for 150 ~ 200MHz */
    GPIO_SetReadWaitCycle(GPIO_RD_WAIT3);
    CLK_SetSysClockSrc(CLK_SYSCLK_SRC_PLL);
}

/**
 * @brief  BSP Xtal32 initialize.
 * @param  None
 * @retval int32_t:
 *         - LL_OK: XTAL32 enable successfully
 *         - LL_ERR_TIMEOUT: XTAL32 enable timeout.
 */
__WEAKDEF int32_t BSP_XTAL32_Init(void)
{
    stc_clock_xtal32_init_t stcXtal32Init;
    stc_fcm_init_t stcFcmInit;
    uint32_t u32TimeOut = 0UL;
    uint32_t u32Time = HCLK_VALUE / 5UL;

    if (CLK_XTAL32_ON == READ_REG8(CM_CMU->XTAL32CR)) {
        /* Disable xtal32 */
        (void)CLK_Xtal32Cmd(DISABLE);
        /* Wait 5 * xtal32 cycle */
        DDL_DelayUS(160U);
    }

    /* Xtal32 config */
    (void)CLK_Xtal32StructInit(&stcXtal32Init);
    stcXtal32Init.u8State  = CLK_XTAL32_ON;
    stcXtal32Init.u8Drv    = CLK_XTAL32_DRV_MID;
    stcXtal32Init.u8Filter = CLK_XTAL32_FILTER_ALL_MD;
    GPIO_AnalogCmd(BSP_XTAL32_PORT, BSP_XTAL32_IN_PIN | BSP_XTAL32_OUT_PIN, ENABLE);
    (void)CLK_Xtal32Init(&stcXtal32Init);

    /* FCM config */
    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_FCM, ENABLE);
    (void)FCM_StructInit(&stcFcmInit);
    stcFcmInit.u32RefClock       = FCM_REF_CLK_MRC;
    stcFcmInit.u32RefClockDiv    = FCM_REF_CLK_DIV8192;
    stcFcmInit.u32RefClockEdge   = FCM_REF_CLK_RISING;
    stcFcmInit.u32TargetClock    = FCM_TARGET_CLK_XTAL32;
    stcFcmInit.u32TargetClockDiv = FCM_TARGET_CLK_DIV1;
    stcFcmInit.u16LowerLimit     = (uint16_t)((XTAL32_VALUE / (MRC_VALUE / 8192U)) * 96UL / 100UL);
    stcFcmInit.u16UpperLimit     = (uint16_t)((XTAL32_VALUE / (MRC_VALUE / 8192U)) * 104UL / 100UL);
    (void)FCM_Init(&stcFcmInit);
    /* Enable FCM, to ensure xtal32 stable */
    FCM_Cmd(ENABLE);
    for (;;) {
        if (SET == FCM_GetStatus(FCM_FLAG_END)) {
            FCM_ClearStatus(FCM_FLAG_END);
            if ((SET == FCM_GetStatus(FCM_FLAG_ERR)) || (SET == FCM_GetStatus(FCM_FLAG_OVF))) {
                FCM_ClearStatus(FCM_FLAG_ERR | FCM_FLAG_OVF);
            } else {
                (void)FCM_DeInit();
                FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_FCM, DISABLE);
                return LL_OK;
            }
        }
        u32TimeOut++;
        if (u32TimeOut > u32Time) {
            (void)FCM_DeInit();
            FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_FCM, DISABLE);
            return LL_ERR_TIMEOUT;
        }
    }
}

#if (LL_PRINT_ENABLE == DDL_ON)
/**
 * @brief  BSP printf device, clock and port pre-initialize.
 * @param  [in] vpDevice                Pointer to print device
 * @param  [in] u32Baudrate             Print device communication baudrate
 * @retval int32_t:
 *           - LL_OK:                   Initialize successfully.
 *           - LL_ERR:                  Initialize unsuccessfully.
 *           - LL_ERR_INVD_PARAM:       The u32Baudrate value is 0.
 */
int32_t BSP_PRINTF_Preinit(void *vpDevice, uint32_t u32Baudrate)
{
    uint32_t i;
    float32_t f32Error;
    const uint32_t au32Div[] = {USART_CLK_DIV1,   USART_CLK_DIV4,   USART_CLK_DIV16,  USART_CLK_DIV64,
                                USART_CLK_DIV128, USART_CLK_DIV256, USART_CLK_DIV512, USART_CLK_DIV1024
                               };
    stc_usart_uart_init_t stcUartInit;
    int32_t i32Ret = LL_ERR_INVD_PARAM;

    (void)vpDevice;

    if (0UL != u32Baudrate) {
        /* Set TX port function */
        GPIO_SetFunc(BSP_PRINTF_PORT, BSP_PRINTF_PIN, BSP_PRINTF_PORT_FUNC);

        /* Enable clock  */
        FCG_Fcg3PeriphClockCmd(BSP_PRINTF_DEVICE_FCG, ENABLE);

        /* Configure UART */
        (void)USART_UART_StructInit(&stcUartInit);
        stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
        (void)USART_UART_Init(BSP_PRINTF_DEVICE, &stcUartInit, NULL);

        for (i = 0UL; i < ARRAY_SZ(au32Div); i++) {
            USART_SetClockDiv(BSP_PRINTF_DEVICE, au32Div[i]);
            i32Ret = USART_SetBaudrate(BSP_PRINTF_DEVICE, u32Baudrate, &f32Error);
            if ((LL_OK == i32Ret) && \
                ((-BSP_PRINTF_BAUDRATE_ERR_MAX <= f32Error) && (f32Error <= BSP_PRINTF_BAUDRATE_ERR_MAX))) {
                USART_FuncCmd(BSP_PRINTF_DEVICE, USART_TX, ENABLE);
                break;
            } else {
                i32Ret = LL_ERR;
            }
        }
    }

    return i32Ret;
}
#endif

/**
 * @brief  BSP key initialize
 * @param  None
 * @retval None
 */
void BSP_KEY_Init(void)
{
    uint8_t i;

    BSP_KEY_ROW_Init();
    BSP_KEY_COL_Init();
    /* Clear all KEYIN interrupt flag before enable */
    for (i = 0U; i < BSP_KEY_ROW_NUM + BSP_KEY_INDEPENDENT_NUM; i++) {
        EXTINT_ClearExtIntStatus(BSP_KEYIN_PORT_PIN[i].ch);
    }
    KEYSCAN_Cmd(ENABLE);
}

/**
 * @brief  Get BSP key status
 * @param  [in] u32Key chose one macro from below
 *   @arg  BSP_KEY_1
 *   @arg  BSP_KEY_2
 *   @arg  BSP_KEY_3
 *   @arg  BSP_KEY_4
 *   @arg  BSP_KEY_5
 * @retval An @ref en_flag_status_t enumeration type value.
 *            - SET, Key pressed.
 *            - RESET, Key released.
 */
en_flag_status_t BSP_KEY_GetStatus(uint32_t u32Key)
{
    en_flag_status_t enStatus = RESET;
    if (0UL != (m_u32GlobalKey & u32Key)) {
        enStatus = SET;
        m_u32GlobalKey &= ~u32Key;
    } else {
    }
    return enStatus;
}

/**
 * @}
 */

/**
 * @defgroup BSP_Local_Functions BSP Local Functions
 * @{
 */

#if (DDL_ON == BSP_INT_KEY_ENABLE)
/**
 * @brief  EXTINT Ch.5 as BSP Key 5 callback function
 * @param  None
 * @retval None
 */
void EXTINT06_SWINT22_Handler(void)
{
    m_u32GlobalKey |= BSP_KEY_5;
    EXTINT_ClearExtIntStatus(BSP_KEY_KEY5_EXTINT);
    BSP_KEY_KEY5_IrqCallback();

    __DSB();  /* Arm Errata 838869 */
}

/**
 * @brief  User callback function for BSP KEY5.
 * @param  None
 * @retval None
 */
__WEAKDEF void BSP_KEY_KEY5_IrqCallback(void)
{
    /* This function should be implemented by the user application. */
}
#endif  /* BSP_EV_HC32F448_LQFP80 */

/**
 * @brief  EXTINT Ch.0 as BSP Key row 0 callback function
 * @param  None
 * @retval None
 */
void EXTINT00_SWINT16_Handler(void)
{
    uint8_t u8Idx = (uint8_t)KEYSCAN_GetKeyoutIdx();
    if (SET == EXTINT_GetExtIntStatus(BSP_KEYIN_PORT_PIN[0].ch)) {
        for (;;) {
            if (PIN_RESET == GPIO_ReadInputPins(BSP_KEYIN_PORT_PIN[0].port, BSP_KEYIN_PORT_PIN[0].pin)) {
                m_u32GlobalKey |= (0x01UL) << u8Idx;
            } else {
                /* clear int request flag  after KEY released */
                EXTINT_ClearExtIntStatus(BSP_KEYIN_PORT_PIN[0].ch);
                break;
            }
        }
    }

    __DSB();  /* Arm Errata 838869 */
}

/**
 * @brief  EXTINT Ch.1 as BSP Key row 1 callback function
 * @param  None
 * @retval None
 */
void EXTINT01_SWINT17_Handler(void)
{
    uint8_t u8Idx = (uint8_t)KEYSCAN_GetKeyoutIdx();
    if (SET == EXTINT_GetExtIntStatus(BSP_KEYIN_PORT_PIN[1].ch)) {
        for (;;) {
            if (PIN_RESET == GPIO_ReadInputPins(BSP_KEYIN_PORT_PIN[1].port, BSP_KEYIN_PORT_PIN[1].pin)) {
                m_u32GlobalKey |= (0x100UL) << u8Idx;
            } else {
                /* clear int request flag after KEY released */
                EXTINT_ClearExtIntStatus(BSP_KEYIN_PORT_PIN[1].ch);
                break;
            }
        }
    }

    __DSB();  /* Arm Errata 838869 */
}

/**
 * @brief  BSP key row initialize
 * @param  None
 * @retval None
 */
static void BSP_KEY_ROW_Init(void)
{
    uint8_t i;
    stc_extint_init_t stcExtIntInit;
    stc_gpio_init_t stcGpioInit;

    /* configuration structure initialization */
    (void)GPIO_StructInit(&stcGpioInit);

    /* GPIO config */
    stcGpioInit.u16ExtInt = PIN_EXTINT_ON;
    stcGpioInit.u16PullUp = PIN_PU_ON;
    for (i = 0U; i < BSP_KEY_ROW_NUM + BSP_KEY_INDEPENDENT_NUM; i++) {
        (void)GPIO_Init(BSP_KEYIN_PORT_PIN[i].port, BSP_KEYIN_PORT_PIN[i].pin, &stcGpioInit);
    }

    /* Extint config */
    (void)EXTINT_StructInit(&stcExtIntInit);
    stcExtIntInit.u32Edge = EXTINT_TRIG_FALLING;
    stcExtIntInit.u32FilterB = NMI_EXTINT_FILTER_B_ON;
    stcExtIntInit.u32FilterBClock = NMI_EXTINT_FCLK_B_4US;
    for (i = 0U; i < BSP_KEY_ROW_NUM + BSP_KEY_INDEPENDENT_NUM; i++) {
        (void)EXTINT_Init(BSP_KEYIN_PORT_PIN[i].ch, &stcExtIntInit);
    }

    /* IRQ sign-in */
    for (i = 0U; i < BSP_KEY_ROW_NUM + BSP_KEY_INDEPENDENT_NUM; i++) {
        /* NVIC config */
        NVIC_ClearPendingIRQ(BSP_KEYIN_PORT_PIN[i].irq);
        NVIC_SetPriority(BSP_KEYIN_PORT_PIN[i].irq, DDL_IRQ_PRIO_DEFAULT);
        NVIC_EnableIRQ(BSP_KEYIN_PORT_PIN[i].irq);
    }
}

/**
 * @brief  BSP key column initialize
 * @param  None
 * @retval None
 */
static void BSP_KEY_COL_Init(void)
{
    uint8_t i;
    stc_gpio_init_t stcGpioInit;
    stc_keyscan_init_t stcKeyscanInit;

    /* configuration structure initialization */
    (void)GPIO_StructInit(&stcGpioInit);

    /* Set corresponding pins to KEYSCAN function */
    for (i = 0U; i < BSP_KEY_COL_NUM; i++) {
        (void)GPIO_Init(BSP_KEYOUT_PORT_PIN[i].port, BSP_KEYOUT_PORT_PIN[i].pin, &stcGpioInit);
        GPIO_SetFunc(BSP_KEYOUT_PORT_PIN[i].port, BSP_KEYOUT_PORT_PIN[i].pin, BSP_KEYOUT_PORT_PIN[i].func);
    }
    /* enable KEYSCAN module source clock */
    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_KEY, ENABLE);
    /* ENABLE LRC for scan clock */
    (void)CLK_LrcCmd(ENABLE);

    /* KEYSCAN config */
    (void)KEYSCAN_StructInit(&stcKeyscanInit);
    stcKeyscanInit.u32HizCycle = KEYSCAN_HIZ_CYCLE_4;
    stcKeyscanInit.u32LowCycle = KEYSCAN_LOW_CYCLE_512;
    stcKeyscanInit.u32KeyClock = KEYSCAN_CLK_LRC;
    stcKeyscanInit.u32KeyOut   = BSP_KEYOUT_SELECT;
    stcKeyscanInit.u32KeyIn    = BSP_KEYIN_SELECT;
    (void)KEYSCAN_Init(&stcKeyscanInit);
}

/**
 * @}
 */
#endif /* BSP_EV_HC32F448_LQFP80 */

/**
 * @}
 */

/**
 * @}
 */

/******************************************************************************
 * EOF (not truncated)
 *****************************************************************************/

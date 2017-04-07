/**
  ******************************************************************************
  * @file    DTQ_RP551_F.h
  * @author  MCD Application Team
  * @version V6.0.0
  * @date    16-December-2014
  * @brief   This file contains definitions for DTQ_RP551_F's LEDs, 
  *          push-buttons and COM ports hardware resources.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup DTQ_RP551_F
  * @{
  */ 

/** @addtogroup DTQ_RP551_F_COMMON
  * @{
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DTQ_RP551_F_H
#define __DTQ_RP551_F_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#ifdef HAL_I2C_MODULE_ENABLED
#include "DTQ_RP551_F_io.h"
#endif /* HAL_I2C_MODULE_ENABLED */

/** @defgroup DTQ_RP551_F_Exported_Types Exported Types
  * @{
  */

/**
 * @brief LED Types Definition
 */
typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  LED_RED    = LED1,
  LED_BLUE   = LED2
} Led_TypeDef;


/**
 * @brief COM Types Definition
 */
typedef enum 
{
  COM1 = 0,
  COM2 = 1
} COM_TypeDef;
/**
  * @}
  */ 

/** @defgroup DTQ_RP551_F_Exported_Constants Exported Constants
  * @{
  */ 

/** 
  * @brief  Define for DTQ_RP551_F board  
  */ 
#if !defined (USE_DTQ_RP551_F)
 #define USE_DTQ_RP551_F
#endif
  
/** @addtogroup DTQ_RP551_F_LED
  * @{
  */
#define LEDn                             4

#define LED1_PIN                         GPIO_PIN_7             /* PD.07*/
#define LED1_GPIO_PORT                   GPIOD
#define LED1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOD_CLK_DISABLE()
  
#define LED2_PIN                         GPIO_PIN_13            /* PD.13*/
#define LED2_GPIO_PORT                   GPIOD
#define LED2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOD_CLK_DISABLE()


#define LEDx_GPIO_CLK_ENABLE(__LED__)    do { if ((__LED__) == LED1) LED1_GPIO_CLK_ENABLE(); else \
                                              if ((__LED__) == LED2) LED2_GPIO_CLK_ENABLE(); } while(0)

#define LEDx_GPIO_CLK_DISABLE(__LED__)   (((__LED__) == LED1) ? LED1_GPIO_CLK_DISABLE() :\
                                          ((__LED__) == LED2) ? LED2_GPIO_CLK_DISABLE() : 0 )

/**
  * @}
  */
  


/**
  * @}
  */ 

/** @addtogroup DTQ_RP551_F_COM
  * @{
  */
#define COMn                             1

/**
 * @brief Definition for COM port1, connected to USART2
 */ 
#define DTQ_RP551_COM1                        USART1
#define DTQ_RP551_COM1_CLK_ENABLE()           __HAL_RCC_USART2_CLK_ENABLE()
#define DTQ_RP551_COM1_CLK_DISABLE()          __HAL_RCC_USART2_CLK_DISABLE()

#define AFIOCOM1_CLK_ENABLE()                 __HAL_RCC_AFIO_CLK_ENABLE()
#define AFIOCOM1_CLK_DISABLE()                __HAL_RCC_AFIO_CLK_DISABLE()

#define DTQ_RP551_COM1_TX_PIN                 GPIO_PIN_5             /* PD.05*/
#define DTQ_RP551_COM1_TX_GPIO_PORT           GPIOD
#define DTQ_RP551_COM1_TX_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()
#define DTQ_RP551_COM1_TX_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOD_CLK_DISABLE()

#define DTQ_RP551_COM1_RX_PIN                 GPIO_PIN_6             /* PD.06*/
#define DTQ_RP551_COM1_RX_GPIO_PORT           GPIOD
#define DTQ_RP551_COM1_RX_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()
#define DTQ_RP551_COM1_RX_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOD_CLK_DISABLE()

#define DTQ_RP551_COM1_IRQn                   USART2_IRQn

#define COMx_CLK_ENABLE(__INDEX__)              do { if((__INDEX__) == COM1) DTQ_RP551_COM1_CLK_ENABLE();} while(0)
#define COMx_CLK_DISABLE(__INDEX__)             (((__INDEX__) == COM1) ? DTQ_RP551_COM1_CLK_DISABLE() : 0)

#define AFIOCOMx_CLK_ENABLE(__INDEX__)          do { if((__INDEX__) == COM1) AFIOCOM1_CLK_ENABLE();} while(0)
#define AFIOCOMx_CLK_DISABLE(__INDEX__)         (((__INDEX__) == COM1) ? AFIOCOM1_CLK_DISABLE() : 0)

#define AFIOCOMx_REMAP(__INDEX__)               (((__INDEX__) == COM1) ? (AFIO->MAPR |= (AFIO_MAPR_USART2_REMAP)) : 0)

#define COMx_TX_GPIO_CLK_ENABLE(__INDEX__)      do { if((__INDEX__) == COM1) DTQ_RP551_COM1_TX_GPIO_CLK_ENABLE();} while(0)
#define COMx_TX_GPIO_CLK_DISABLE(__INDEX__)     (((__INDEX__) == COM1) ? DTQ_RP551_COM1_TX_GPIO_CLK_DISABLE() : 0)

#define COMx_RX_GPIO_CLK_ENABLE(__INDEX__)      do { if((__INDEX__) == COM1) DTQ_RP551_COM1_RX_GPIO_CLK_ENABLE();} while(0)
#define COMx_RX_GPIO_CLK_DISABLE(__INDEX__)     (((__INDEX__) == COM1) ? DTQ_RP551_COM1_RX_GPIO_CLK_DISABLE() : 0)

/**
  * @}
  */ 

/** @addtogroup DTQ_RP551_F_BUS
  * @{
  */

/** 
  * @brief  IO Expander Interrupt line on EXTI  
  */ 
#define IOE_IT_PIN                       GPIO_PIN_14
#define IOE_IT_GPIO_PORT                 GPIOB
#define IOE_IT_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define IOE_IT_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()
#define IOE_IT_EXTI_IRQn                 EXTI15_10_IRQn       
#define IOE_IT_EXTI_IRQHANDLER           EXTI15_10_IRQHandler

/* Exported constant IO ------------------------------------------------------*/
#define IO1_I2C_ADDRESS                       0x82
#define IO2_I2C_ADDRESS                       0x88
#define TS_I2C_ADDRESS                        0x82

/*The Slave ADdress (SAD) associated to the LIS302DL is 001110xb. SDO pad can be used 
to modify less significant bit of the device address. If SDO pad is connected to voltage 
supply LSb is ‘1’ (address 0011101b) else if SDO pad is connected to ground LSb value is 
‘0’ (address 0011100b).*/
#define L1S302DL_I2C_ADDRESS                  0x38


/*##################### ACCELEROMETER ##########################*/
/* Read/Write command */
#define READWRITE_CMD                     ((uint8_t)0x80) 
/* Multiple byte read/write command */ 
#define MULTIPLEBYTE_CMD                  ((uint8_t)0x40)

/*##################### I2Cx ###################################*/
/* User can use this section to tailor I2Cx instance used and associated 
   resources */
/* Definition for I2Cx Pins */
#define DTQ_RP551_I2Cx_SCL_PIN                       GPIO_PIN_6        /* PB.06*/
#define DTQ_RP551_I2Cx_SCL_GPIO_PORT                 GPIOB
#define DTQ_RP551_I2Cx_SDA_PIN                       GPIO_PIN_7        /* PB.07*/
#define DTQ_RP551_I2Cx_SDA_GPIO_PORT                 GPIOB

/* Definition for I2Cx clock resources */
#define DTQ_RP551_I2Cx                               I2C1
#define DTQ_RP551_I2Cx_CLK_ENABLE()                  __HAL_RCC_I2C1_CLK_ENABLE()
#define DTQ_RP551_I2Cx_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define DTQ_RP551_I2Cx_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE() 

#define DTQ_RP551_I2Cx_FORCE_RESET()                 __HAL_RCC_I2C1_FORCE_RESET()
#define DTQ_RP551_I2Cx_RELEASE_RESET()               __HAL_RCC_I2C1_RELEASE_RESET()
    
/* Definition for I2Cx's NVIC */
#define DTQ_RP551_I2Cx_EV_IRQn                       I2C1_EV_IRQn
#define DTQ_RP551_I2Cx_EV_IRQHandler                 I2C1_EV_IRQHandler
#define DTQ_RP551_I2Cx_ER_IRQn                       I2C1_ER_IRQn
#define DTQ_RP551_I2Cx_ER_IRQHandler                 I2C1_ER_IRQHandler

/* I2C clock speed configuration (in Hz) */
#ifndef BSP_I2C_SPEED
 #define BSP_I2C_SPEED                            400000
#endif /* I2C_SPEED */


/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define DTQ_RP551_I2Cx_TIMEOUT_MAX                   3000

/*##################### SPI3 ###################################*/
#define DTQ_RP551_SPIx                               SPI3
#define DTQ_RP551_SPIx_CLK_ENABLE()                  __HAL_RCC_SPI3_CLK_ENABLE()

#define DTQ_RP551_SPIx_SCK_GPIO_PORT                 GPIOC             /* PC.10*/
#define DTQ_RP551_SPIx_SCK_PIN                       GPIO_PIN_10
#define DTQ_RP551_SPIx_SCK_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define DTQ_RP551_SPIx_SCK_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOC_CLK_DISABLE()

#define DTQ_RP551_SPIx_MISO_MOSI_GPIO_PORT           GPIOC
#define DTQ_RP551_SPIx_MISO_MOSI_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOC_CLK_ENABLE()
#define DTQ_RP551_SPIx_MISO_MOSI_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOC_CLK_DISABLE()
#define DTQ_RP551_SPIx_MISO_PIN                      GPIO_PIN_11       /* PC.11*/
#define DTQ_RP551_SPIx_MOSI_PIN                      GPIO_PIN_12       /* PC.12*/
/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define DTQ_RP551_SPIx_TIMEOUT_MAX                   1000

/**
  * @}
  */ 


/**
  * @}
  */ 



/** @addtogroup DTQ_RP551_F_Exported_Functions
  * @{
  */ 
uint32_t                BSP_GetVersion(void);
void                    BSP_LED_Init(Led_TypeDef Led);
void                    BSP_LED_On(Led_TypeDef Led);
void                    BSP_LED_Off(Led_TypeDef Led);
void                    BSP_LED_Toggle(Led_TypeDef Led);
#ifdef HAL_UART_MODULE_ENABLED
void                    BSP_COM_Init(COM_TypeDef COM, UART_HandleTypeDef* huart);
#endif /* HAL_UART_MODULE_ENABLED */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif
  
#endif /* __DTQ_RP551_F_H */

/**
  * @}
  */
  
/**
  * @}
  */
  
/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

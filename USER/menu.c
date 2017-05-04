/**
  ******************************************************************************
  * @file    IAP_Main/Src/menu.c 
  * @author  MCD Application Team
  * @version 1.0.0
  * @date    8-April-2015
  * @brief   This file provides the software which contains the main menu routine.
  *          The main menu gives the options of:
  *             - downloading a new binary file, 
  *             - uploading internal flash memory,
  *             - executing the binary file already loaded 
  *             - configuring the write protection of the Flash sectors where the 
  *               user loads his binary file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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

/** @addtogroup STM32F1xx_IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "common.h"
#include "flash_if.h"
#include "menu.h"
#include "ymodem.h"
#include "stdio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
pFunction JumpToApplication;
uint32_t JumpAddress;
uint32_t FlashProtection = 0;
uint8_t aFileName[FILE_NAME_LENGTH];

/* Private function prototypes -----------------------------------------------*/
void SerialDownload(void);
void SerialUpload(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Download a file via serial port
  * @param  None
  * @retval None
  */
void SerialDownload(void)
{
  uint8_t number[11] = {0};
  uint32_t size = 0;
  COM_StatusTypeDef result;

  printf("Waiting for the file to be sent ... (press 'a' to abort)\r\n");
  result = Ymodem_Receive( &size );
	HAL_Delay( 300 );
  if (result == COM_OK)
  {
		printf(" Programming Completed Successfully!\r\n");
		printf("-------------------------------------\r\n");
		printf(" Name:%s\r\n",aFileName);
		Int2Str(number, size);
		printf(" Size:%s Bytes\r\n",number);
		printf("-------------------------------------\r\n");
  }
	
  else if (result == COM_LIMIT)
  {
    printf("The image size is higher than the allowed space memory!\r\n");
  }
  else if (result == COM_DATA)
  {
    printf("Verification failed!\r\n");
  }
  else if (result == COM_ABORT)
  {
    printf("Aborted by user.\r\n");
  }
  else
  {
    printf("Failed to receive the file!\r\n");
  }
}

/**
  * @brief  Upload a file via serial port.
  * @param  None
  * @retval None
  */
void SerialUpload(void)
{
  uint8_t status = 0;

  printf("Select Receive File\r\n");

  HAL_UART_Receive(&UartHandle, &status, 1, RX_TIMEOUT);
  if ( status == CRC16)
  {
    /* Transmit the flash image through ymodem protocol */
    status = Ymodem_Transmit((uint8_t*)APPLICATION_ADDRESS, (const uint8_t*)"UploadedFlashImage.bin", USER_FLASH_SIZE);

    if (status != 0)
    {
      printf("Error Occurred while Transmitting File\r\n");
    }
    else
    {
      printf("File uploaded successfully \r\n");
    }
  }
}

/**
  * @brief  Display the Main Menu on HyperTerminal
  * @param  None
  * @retval None
  */
void Main_Menu(void)
{
  uint8_t key = 0;

  /* Test if any sector of Flash memory where user application will be loaded is write protected */
  FlashProtection = FLASH_If_GetWriteProtectionStatus();
	printf("FlashProtection:%u \r\n",FlashProtection);
  while (1)
  {

    /* Clean the input path */
    __HAL_UART_FLUSH_DRREGISTER(&UartHandle);
	
    /* Receive key */
    HAL_UART_Receive(&UartHandle, &key, 1, RX_TIMEOUT);
    switch (key)
    {
    case '1' :
      /* Download user application in the Flash */
      SerialDownload();
		      printf("Start program execution......\r\n\n");
      /* execute the new program */
      JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
      /* Jump to user application */
      JumpToApplication = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
      JumpToApplication();
      break;
    case '2' :
      break;
    case '3' :
      break;
    case '4' :
      if (FlashProtection != FLASHIF_PROTECTION_NONE)
      {
        /* Disable the write protection */
        if (FLASH_If_WriteProtectionConfig(FLASHIF_WRP_DISABLE) == FLASHIF_OK)
        {
          printf("Write Protection disabled...\r\n");
          printf("System will now restart...\r\n");
          /* Launch the option byte loading */
          HAL_FLASH_OB_Launch();
        }
        else
        {
          printf("Error: Flash write un-protection failed...\r\n");
        }
      }
      else
      {
        if (FLASH_If_WriteProtectionConfig(FLASHIF_WRP_ENABLE) == FLASHIF_OK)
        {
          printf("Write Protection enabled...\r\n");
          printf("System will now restart...\r\n");
          /* Launch the option byte loading */
          HAL_FLASH_OB_Launch();
        }
        else
        {
          printf("Error: Flash write protection failed...\r\n");
        }
      }
      break;
	default:
	break;
    }
  }
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

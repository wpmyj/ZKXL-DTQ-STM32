/**
  ******************************************************************************
  * @file   	rfsim_MCUConf.h
  * @author  	Tim.Xia
  * @version 	V2.0.0
  * @date   	31-5-2013
  * @brief    	
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_NEW_H__
#define __FLASH_NEW_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_flash.h"

/* Exported typedef ----------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/

/* LED typedef ---------------------------------------------------------------*/
/* LED typedef ---------------------------------------------------------------*/

/* Exported defines ----------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/*
* \stm32f106RBT6 flash is 128k, addr is 0x0800_0000 to 0x0801_ffff
*  make the last page to save user data
*/
#define BANK_WRITE_START_ADDR  (u_long)(0x0801E400)
#define BANK_WRITE_END_ADDR    (u_long)(0x0801E400 + 7 * 0x400)

#define u_int  unsigned int
#define u_long unsigned int
#define u_char unsigned char
#define u_short unsigned short

/* Exported functions ------------------------------------------------------- */
/* Exported functions ------------------------------------------------------- */
int eeReadHalfWord(u_long dwiAddr);
int eeReadWord(u_long dwiAddr);
int eeCheckBlockFlash(u_long dwiAddr, u_short wiSize);
int eeWriteHalfWord(u_long dwAddr, u_short wiData);
int eeWriteWord(u_long dwiAddr, u_long dwiData);
int eeWriteMultiHalfWord(u_long dwiAddr, const u_short *pciSrc, u_short wiDataLen);
int eeWriteMultiWord(u_long dwiAddr, const u_long *pciSrc, u_short wiDataLen);
int eeReadMultiHalfWord(u_short *piDestAddr, u_long dwiSrcAddr, u_short wiDataLen);
int eeReadMultiWord(u_long *piDestAddr, u_long dwiSrcAddr, u_short wiDataLen);

#endif


/*******************END OF FILE***********************************************/










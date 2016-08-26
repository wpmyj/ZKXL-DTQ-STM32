#ifndef _HEADER_FLASHRW_H_
#define _HEADER_FLASHRW_H_

#include "stm32f10x_flash.h"
#include "string.h"

#define Flash_DefaultVal (0xff)
#define PageSize			(0x400)
#define InPageOffset	(PageSize-0x01)
#define PageNo				(~InPageOffset)

#define STM32_FLASH_BASE 0X08000000

#define ProgrammableFlash_Start				((0x0801E400))
#define PROGRAM_FLASH_SHUAIJIAN				(ProgrammableFlash_Start + PageSize)
#define PROGRAM_FLASH_TFLASH_PARA			(ProgrammableFlash_Start + PageSize + PageSize)
#define PROGRAM_IAP_FLAG							(ProgrammableFlash_Start + PageSize + PageSize + PageSize)
#define ProgrammableFlash_MCU_ID			((0x0801F800))
#define ProgrammableFlash_MAC					((0x0801FC00))
#define ProgrammableFlash_ID					((0x0801FD00))
#define ProgrammableFlash_HW					((0x0801FE00))
#define ProgrammableFlash_End					((0x0807FFFF))
#define IS_ProgrammableFlash(_add_)		((((uint32_t)(_add_)) >= ProgrammableFlash_Start) && (((uint32_t)(_add_)) < ProgrammableFlash_End))

#define HRF_PARA_AREA_SRART						(ProgrammableFlash_Start)
#define HRF_PARA_AREA_END							(ProgrammableFlash_Start + 7 * PageSize)

u16 STMFLASH_ReadHalfWord(u32 faddr);
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);
void Test_Write(u32 WriteAddr,u16 WriteData);

#endif










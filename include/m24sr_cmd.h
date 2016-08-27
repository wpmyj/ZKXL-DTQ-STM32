/** @file m24sr_cmd.h
 *
 * @author 
 *
 * @compiler This program has been tested with Keil C51 V7.50.
 *
 * @copyright
 * Copyright (c) 2012 ZKXL. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic Semiconductor. The use,
 * copying, transfer or disclosure of such information is prohibited except by express written
 * agreement with Nordic Semiconductor.
 * @endcopyright
 *
 * $Date: 		2014.07.19
 * $Revision: 	V1000
 *
 */
 
#ifndef	_M24SR_CMD_H_
#define _M24SR_CMD_H_

#include <stdint.h>

/* special M24SR command ----------------------------------------------------------------------*/	 
#define M24SR_OPENSESSION									(0x26)
#define M24SR_KILLSESSION									(0x52)

/*-------------------------- File Identifier ---------------------------------*/	 
#define SYSTEM_FILE_ID										(0xE101)	 
#define CC_FILE_ID											(0xE103)
#define NDEF_FILE_ID										(0x0001)	

#define M24SR_READ_MAX_NBBYTE								(0xF6) 
#define M24SR_WRITE_MAX_NBBYTE								(0xF6)
#define BUF_LEN												(0xF6)

#define UB_STATUS_OFFSET									(4)
#define LB_STATUS_OFFSET  									(3)

/*  Length	----------------------------------------------------------------------------------*/
#define M24SR_STATUS_NBBYTE									(2)
#define M24SR_CRC_NBBYTE									(2)
#define M24SR_STATUSRESPONSE_NBBYTE							(5)
#define M24SR_DESELECTREQUEST_NBBYTE						(3)
#define M24SR_DESELECTRESPONSE_NBBYTE						(3)
#define M24SR_WATINGTIMEEXTRESPONSE_NBBYTE					(4)
#define M24SR_PASSWORD_NBBYTE								(0x10)


/* APDU Command: class list -------------------------------------------*/
#define C_APDU_CLA_DEFAULT	  								(0x00)
#define C_APDU_CLA_ST										(0xA2)

/*------------------------ Data Area Management Commands ---------------------*/
#define C_APDU_SELECT_FILE     								(0xA4)
#define C_APDU_GET_RESPONCE    								(0xC0)
#define C_APDU_STATUS          								(0xF2)
#define C_APDU_UPDATE_BINARY   								(0xD6)
#define C_APDU_INTERRUPT       								(0xD6)
#define C_APDU_READ_BINARY     								(0xB0)
#define C_APDU_WRITE_BINARY    								(0xD0)
#define C_APDU_UPDATE_RECORD   								(0xDC)
#define C_APDU_READ_RECORD     								(0xB2)
#define C_APDU_VERIFY          								(0x20)

/*  Command structure	------------------------------------------------------------------------*/
#define M24SR_CMDSTRUCT_SELECTAPPLICATION					(0x00FF)
#define M24SR_CMDSTRUCT_SELECTCCFILE						(0x007F)
#define M24SR_CMDSTRUCT_SELECTNDEFFILE						(0x007F)
#define M24SR_CMDSTRUCT_READBINARY							(0x009F)
#define M24SR_CMDSTRUCT_UPDATEBINARY						(0x007F)
#define M24SR_CMDSTRUCT_VERIFYBINARYWOPWD					(0x003F)
#define M24SR_CMDSTRUCT_VERIFYBINARYWITHPWD					(0x007F)
#define M24SR_CMDSTRUCT_CHANGEREFDATA						(0x007F)
#define M24SR_CMDSTRUCT_ENABLEVERIFREQ						(0x001F)
#define M24SR_CMDSTRUCT_DISABLEVERIFREQ						(0x001F)
#define M24SR_CMDSTRUCT_SENDINTERRUPT						(0x003F)
#define M24SR_CMDSTRUCT_GPOSTATE							(0x007F)

/*  Command structure Mask -------------------------------------------------------------------*/
#define M24SR_PCB_NEEDED									(0x0001)		/* PCB byte present or not */
#define M24SR_CLA_NEEDED									(0x0002) 		/* CLA byte present or not */
#define M24SR_INS_NEEDED									(0x0004) 		/* Operation code present or not*/ 
#define M24SR_P1_NEEDED										(0x0008)		/* Selection Mode  present or not*/
#define M24SR_P2_NEEDED										(0x0010)		/* Selection Option present or not*/
#define M24SR_LC_NEEDED										(0x0020)		/* Data field length byte present or not */
#define M24SR_DATA_NEEDED									(0x0040)		/* Data present or not */
#define M24SR_LE_NEEDED										(0x0080)		/* Expected length present or not */
#define M24SR_CRC_NEEDED									(0x0100)		/* 2 CRC bytes present	or not */	                                          
#define M24SR_DID_NEEDED									(0x08)			/* DID byte present or not */

/*-------------------------- Password Management -----------------------------*/
#define READ_PWD											(0x0001)
#define WRITE_PWD											(0x0002)
#define I2C_PWD												(0x0003)

#define M24SR_ERROR_CRC										(0x0012)

/* APDU-Header command structure ---------------------------------------------*/
typedef struct
{
  uint8_t CLA;  /* Command class */
  uint8_t INS;  /* Operation code */
  uint8_t P1;   /* Selection Mode */
  uint8_t P2;   /* Selection Option */
} C_APDU_Header;

/* APDU-Body command structure -----------------------------------------------*/
typedef struct 
{
  uint8_t LC;         						/* Data field length */	
  uint8_t pData[BUF_LEN];  					/* Command parameters */ 
  uint8_t LE;          						/* Expected length of data to be returned */
} C_APDU_Body;
/* APDU Command structure ----------------------------------------------------*/
typedef struct
{
  C_APDU_Header Header;
  C_APDU_Body Body;
} C_APDU;

/* GPO state structure -------------------------------------------------------*/
typedef enum{
	HIGH_IMPEDANCE = 0,
	SESSION_OPENED,
	WIP,
	I2C_ANSWER_READY,
	INTERRUPT,
	STATE_CONTROL
}M24SR_GPO_MGMT;

#define GETMSB(val) 					( (uint8_t) ((val & 0xFF00 )>>8) ) 
#define GETLSB(val) 					( (uint8_t) (val & 0x00FF )) 
#define TOGGLE(val) 					((val != 0x00)? 0x00 : 0x01)

void M24SR_InitStructure ( void );
void M24SR_SelectApplication  ( uint8_t *len );
void M24SR_SelectCCfile (  uint8_t *len  );
void M24SR_SelectSystemfile (  uint8_t *len  );
void M24SR_SelectNDEFfile ( uint16_t NDEFfileId, uint8_t *len );
void M24SR_ReadBinary ( uint16_t Offset ,uint8_t NbByteToRead, uint8_t *len );
void M24SR_UpdateBinary ( uint16_t Offset , uint8_t NbByteToWrite , uint8_t *pDataToWrite, uint8_t *len );
void M24SR_SendInterrupt (  uint8_t *len   );
void M24SR_FWTExtension( uint8_t FWTbyte, uint8_t *len  );
uint16_t M24SR_ComputeCrc(uint8_t *Data, uint8_t Length);



#endif	//_M24SR_CMD_H_

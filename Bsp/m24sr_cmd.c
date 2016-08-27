/** @file m24sr_cmd.c
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

#include "string.h"
#include "m24sr_cmd.h"

C_APDU 							Command;
uint8_t 					 	DataBuffer[BUF_LEN] = {0x00};
uint8_t 						uM24SRbuffer[BUF_LEN] = {0x00};

uint16_t M24SR_IsCorrectCRC16Residue (uint8_t *DataIn, uint8_t Length);
void M24SR_BuildIBlockCommand ( uint16_t CommandStructure, C_APDU Command, uint8_t *NbByte , uint8_t *pCommand);

/*********************************************************************************
* 功	能：void M24SR_InitStructure ( void )
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
void M24SR_InitStructure ( void )
{
	uint8_t i = 0;
	/* build the command */
	Command.Header.CLA = 0x00;
	Command.Header.INS = 0x00;
	/* copy the offset */
	Command.Header.P1 = 0x00 ;
	Command.Header.P2 = 0x00 ;
	/* copy the number of byte of the data field */
	Command.Body.LC = 0x00 ;
	/* copy the number of byte to read */
	Command.Body.LE = 0x00 ;
	for(i=0; i<BUF_LEN; i++)
		Command.Body.pData[i] = DataBuffer[i]; 
	
// 	/* initializes the response structure*/
// 	Response.pData = DataBuffer; 
// 	Response.SW1 = 0x00;
// 	Response.SW2 = 0x00;
}


/*********************************************************************************
* 功	能：void M24SR_SelectApplication ( uint8_t *len )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：选择应用
*********************************************************************************/
extern uint8_t g_cardType[];
void M24SR_SelectApplication ( uint8_t *len )
{
	uint8_t		i = 0;
	uint8_t		NbByte;
	uint8_t 	*pBuffer = uM24SRbuffer;
	uint8_t		uLc = 0x07,
				pCMD_Data[] = {0xD2,0x76,0x00,0x00,0x85,0x01,0x01},
				uLe = 0x00;
	uint16_t	uP1P2 =0x0400;
				
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_SELECT_FILE;
	/* copy the offset */
	Command.Header.P1 = GETMSB	(uP1P2 ) ;
	Command.Header.P2 = GETLSB	(uP1P2 ) ;
	/* copy the number of byte of the data field */
	Command.Body.LC = uLc ;
	/* copy the data */
	for(i=0; i<uLc; i++)
		Command.Body.pData[i] = pCMD_Data[i];
				
	/* copy the number of byte to read */
	Command.Body.LE = uLe ;
	/* build the I_Block command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SELECTAPPLICATION,  Command, &NbByte , pBuffer);
	*len = NbByte;
}

/*********************************************************************************
* 功	能：void M24SR_SelectCCfile (  uint8_t *len  )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：选择CC文件
*********************************************************************************/
void M24SR_SelectCCfile (  uint8_t *len  )
{
	uint8_t 	*pBuffer = uM24SRbuffer, NbByte;
	uint8_t		uLc = 0x02;
	uint16_t	uP1P2 =0x000C,
				uNbFileId =CC_FILE_ID;
				
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_SELECT_FILE;
	/* copy the offset */
	Command.Header.P1 = GETMSB	(uP1P2 ) ;
	Command.Header.P2 = GETLSB	(uP1P2 ) ;
	/* copy the number of byte of the data field */
	Command.Body.LC = uLc ;
	/* copy the File Id */
	Command.Body.pData[0] = GETMSB	(uNbFileId ) ;
	Command.Body.pData[1] = GETLSB	(uNbFileId ) ;
	/* build the I_Block command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SELECTCCFILE,  Command, &NbByte , pBuffer);
	*len = NbByte;
	
}

/*********************************************************************************
* 功	能：void M24SR_SelectSystemfile ( uint8_t *len )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：选择system文件
*********************************************************************************/
void M24SR_SelectSystemfile ( uint8_t *len )
{
	uint8_t 	*pBuffer = uM24SRbuffer, NbByte;
	uint8_t		uLc = 0x02;
	uint16_t	uP1P2 =0x000C,
				uNbFileId = SYSTEM_FILE_ID;
				
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_SELECT_FILE;
	/* copy the offset */
	Command.Header.P1 = GETMSB	(uP1P2 ) ;
	Command.Header.P2 = GETLSB	(uP1P2 ) ;
	/* copy the number of byte of the data field */
	Command.Body.LC = uLc ;
	/* copy the File Id */
	Command.Body.pData[0] = GETMSB	(uNbFileId ) ;
	Command.Body.pData[1] = GETLSB	(uNbFileId ) ;
	/* build the I_Block command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SELECTCCFILE,  Command, &NbByte , pBuffer);
	*len = NbByte;

}

/*********************************************************************************
* 功	能：void M24SR_SelectNDEFfile ( uint16_t NDEFfileId, uint8_t *len )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：选择NDEF文件
*********************************************************************************/
void M24SR_SelectNDEFfile ( uint16_t NDEFfileId, uint8_t *len )
{
	uint8_t 	*pBuffer = uM24SRbuffer, NbByte;
	uint8_t		uLc = 0x02;
	uint16_t	uP1P2 =0x000C;
				
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_SELECT_FILE;
	/* copy the offset */
	Command.Header.P1 = GETMSB	(uP1P2 ) ;
	Command.Header.P2 = GETLSB	(uP1P2 ) ;
	/* copy the number of byte of the data field */
	Command.Body.LC = uLc ;
	/* copy the File Id */
	Command.Body.pData[0] = GETMSB	(NDEFfileId ) ;
	Command.Body.pData[1] = GETLSB	(NDEFfileId ) ;
	/* build the I_Block command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SELECTNDEFFILE,  Command, &NbByte , pBuffer);
	*len = NbByte;
	
}

/*********************************************************************************
* 功	能：void M24SR_ReadBinary ( uint16_t Offset ,uint8_t NbByteToRead, uint8_t *len )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：NULL
*********************************************************************************/
void M24SR_ReadBinary ( uint16_t Offset ,uint8_t NbByteToRead , uint8_t *len )
{
	uint8_t 	*pBuffer = uM24SRbuffer ;
	uint8_t		NbByte;
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_READ_BINARY;
	/* copy the offset */
	Command.Header.P1 = GETMSB	(Offset ) ;
	Command.Header.P2 = GETLSB	(Offset ) ;
	/* copy the number of byte to read */
	Command.Body.LE = NbByteToRead ;
	/* build the I_Block command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_READBINARY,  Command, &NbByte , pBuffer);
	*len = NbByte;
	
}

/*********************************************************************************
* 功	能：void M24SR_UpdateBinary ( uint16_t Offset , uint8_t NbByteToWrite , uint8_t *pDataToWrite, uint8_t *len )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：NULL
*********************************************************************************/
void M24SR_UpdateBinary ( uint16_t Offset , uint8_t NbByteToWrite , uint8_t *pDataToWrite, uint8_t *len )
{
	uint8_t 	*pBuffer = uM24SRbuffer ;
	uint8_t		NbByte;
	uint8_t		i = 0;
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_UPDATE_BINARY;
	/* copy the offset */
	Command.Header.P1 = GETMSB	(Offset ) ;
	Command.Header.P2 = GETLSB	(Offset ) ;
	/* copy the number of byte of the data field */
	Command.Body.LC = NbByteToWrite ;
	/* copy the File Id */
	for(i=0; i<NbByteToWrite; i++)
		Command.Body.pData[i] = pDataToWrite[i];
//	memcpy(Command.Body.pData ,pDataToWrite, NbByteToWrite );
	/* build the I_Block command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_UPDATEBINARY,  Command, &NbByte , pBuffer);
	*len = NbByte;

}

/*********************************************************************************
* 功	能：void M24SR_SendInterrupt ( uint8_t *len  )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：NULL
*********************************************************************************/
void M24SR_SendInterrupt ( uint8_t *len  )
{
	uint8_t 	*pBuffer = uM24SRbuffer;
	uint16_t	uP1P2 =0x001E; 
	uint8_t		NbByte;
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_ST;
	Command.Header.INS = C_APDU_INTERRUPT;
	/* copy the Password Id */
	Command.Header.P1 = GETMSB	(uP1P2 ) ;
	Command.Header.P2 = GETLSB	(uP1P2 ) ;
	Command.Body.LC = 0x00 ;
	/* build the I_Block command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SENDINTERRUPT,  Command, &NbByte , pBuffer);
	*len = NbByte;
}

/*********************************************************************************
* 功	能：void M24SR_FWTExtension( uint8_t FWTbyte, uint8_t *len  )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：NULL
*********************************************************************************/
void M24SR_FWTExtension( uint8_t FWTbyte, uint8_t *len  )
{
	uint8_t 	*pBuffer = uM24SRbuffer;
	uint16_t	NthByte = 0,
					uCRC16;
	
	pBuffer[NthByte++] = 0xF2 ;	
	pBuffer[NthByte++] = FWTbyte ;
	/* compute the CRC */
	uCRC16 = M24SR_ComputeCrc (pBuffer,0x02);
	/* append the CRC16 */
	pBuffer [NthByte++] = GETLSB(uCRC16 ) ;
	pBuffer [NthByte++] = GETMSB(uCRC16 ) ;	
	*len = NthByte;
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

/*********************************************************************************
* 功	能：uint16_t M24SR_UpdateCrc (uint8_t ch, uint16_t *lpwCrc)
* 输    入: NULL
* 返	回：执行状态
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_UpdateCrc (uint8_t ch, uint16_t *lpwCrc)
{
	 ch = (ch^(uint8_t)((*lpwCrc) & 0x00FF));
	 ch = (ch^(ch<<4));
	 *lpwCrc = (*lpwCrc >> 8)^((uint16_t)ch << 8)^((uint16_t)ch<<3)^((uint16_t)ch>>4);
	
	 return(*lpwCrc);
}

/*********************************************************************************
* 功	能：uint16_t M24SR_ComputeCrc(uint8_t *Data, uint8_t Length)
* 输    入: NULL
* 返	回：执行状态
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_ComputeCrc(uint8_t *Data, uint8_t Length)
{
	uint8_t chBlock;
	uint16_t wCrc;

	wCrc = 0x6363; // ITU-V.41

	do 
	{
		chBlock = *Data++;
		M24SR_UpdateCrc(chBlock, &wCrc);
	} while (--Length);

	return wCrc ;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_IsCorrectCRC16Residue (uc8 *DataIn,uc8 Length)
* 输    入: NULL
* 返	回：Status (SW1&SW2)  	:   CRC16 residue is correct	
			M24SR_ERROR_CRC  	: 	CRC16 residue is false
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_IsCorrectCRC16Residue (uint8_t *DataIn, uint8_t Length)
{
	uint16_t ResCRC=0;

	/* check the CRC16 Residue */
	if (Length !=0)
		ResCRC= M24SR_ComputeCrc (DataIn, Length);
	
	if ( ResCRC == 0x0000)
	{
		/* Good CRC, but error status from M24SR */
			return( ((DataIn[Length-UB_STATUS_OFFSET]<<8) & 0xFF00) | (DataIn[Length-LB_STATUS_OFFSET] & 0x00FF) ); 
	}
	else
	{
		ResCRC=0;
		ResCRC= M24SR_ComputeCrc (DataIn, 5);
		if ( ResCRC != 0x0000)
		{
			/* Bad CRC */
			return M24SR_ERROR_CRC;
		}
		else
		{
			/* Good CRC, but error status from M24SR */
			return( ((DataIn[1]<<8) & 0xFF00) | (DataIn[2] & 0x00FF) ); 
		}
	}	
}

/*********************************************************************************
* 功	能：组装M24SR的操作命令
* 输    入: CommandStructure = 制定命令中包含的特定结构
*			Command = CAPDU命令
* 返	回：NbByte = 命令长度值
*			pCommand = 最终生成的命令
* 备	注：NULL
*********************************************************************************/
extern uint8_t flag_App_or_Ctr;
void M24SR_BuildIBlockCommand ( uint16_t CommandStructure, C_APDU Command, uint8_t *NbByte , uint8_t *pCommand)
{
	uint8_t			i = 0;
	uint16_t		uCRC16; 
	static uint8_t 	BlockNumber = 0x01;
	uint8_t 		uDIDbyte = 0x00;
	
	(*NbByte) = 0;
	
	/* add the PCD byte */
	if ((CommandStructure & M24SR_PCB_NEEDED) !=0)
	{
		/* toggle the block number */
		BlockNumber = TOGGLE ( BlockNumber );
		/* Add the I block byte */
		pCommand[(*NbByte)++] = 0x02 | BlockNumber;
	}
	
	/* add the DID byte */
	if ((BlockNumber & M24SR_DID_NEEDED) !=0)
	{
		/* Add the I block byte */
		pCommand[(*NbByte)++] = uDIDbyte; 
	}
	
	/* add the Class byte */
	if ((CommandStructure & M24SR_CLA_NEEDED) !=0)
	{
		pCommand[(*NbByte)++] = Command.Header.CLA ;
	}
	/* add the instruction byte byte */
	if ( (CommandStructure & M24SR_INS_NEEDED) !=0)
	{
		pCommand[(*NbByte)++] = Command.Header.INS ;
	}
	/* add the Selection Mode byte */
	if ((CommandStructure & M24SR_P1_NEEDED) !=0)
	{
		pCommand[(*NbByte)++] = Command.Header.P1 ;
	}
	/* add the Selection Mode byte */
	if ((CommandStructure & M24SR_P2_NEEDED) !=0)
	{
		pCommand[(*NbByte)++] = Command.Header.P2 ;
	}
	/* add Data field lengthbyte */
	if ((CommandStructure & M24SR_LC_NEEDED) !=0)
	{
		pCommand[(*NbByte)++] = Command.Body.LC ;
	}
	/* add Data field  */
	if ((CommandStructure & M24SR_DATA_NEEDED) !=0)
	{
		for(i=0; i<Command.Body.LC; i++)
			pCommand[(*NbByte)++] = Command.Body.pData[i];
	}
	/* add Le field  */
	if ((CommandStructure & M24SR_LE_NEEDED) !=0)
	{
		pCommand[(*NbByte)++] = Command.Body.LE ;
	}
	/* add CRC field  */
	if ((CommandStructure & M24SR_CRC_NEEDED) !=0)
	{
		uCRC16 = M24SR_ComputeCrc (pCommand,(uint8_t) (*NbByte));
		/* append the CRC16 */
		pCommand [(*NbByte)++] = GETLSB	(uCRC16 ) ;
		pCommand [(*NbByte)++] = GETMSB	(uCRC16 ) ;	
	}
}


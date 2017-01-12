/** @file m24sr_cmd.c
 *
 * @author 
 *
 * @compiler This program has been tested with Keil MDK V4.73.
 *
 * @copyright
 * Copyright (c) 2016 ZKXL. All Rights Reserved.
 *
 * $Date: 		2016.05.19
 * $Revision: 	V1000
 *
 */
 
#include "m24sr_cmd.h"


//#define	ENABLE_I2C_DEBUGLOG
#ifdef	ENABLE_I2C_DEBUGLOG
	#define	I2C_DebugLog			app_trace_log
#else
	#define	I2C_DebugLog(...)
#endif //ENABLE_I2C_DEBUGLOG

C_APDU 							Command;
uint8_t 					 	DataBuffer[BUF_LEN] = {0x00};
uint8_t 						uM24SRbuffer[BUF_LEN] = {0x00};

static uint16_t M24SR_FWTExtension ( uint8_t FWTbyte );
uint16_t M24SR_ComputeCrc(uint8_t *Data, uint8_t Length);
uint16_t M24SR_IsCorrectCRC16Residue (uint8_t *DataIn, uint8_t Length);
void M24SR_BuildIBlockCommand ( uint16_t CommandStructure, C_APDU Command, uint16_t *NbByte , uint8_t *pCommand);
static int8_t IsSBlock (uint8_t *pBuffer);

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
* 功	能：uint16_t M24SR_GetSession ( void )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED 	: the function is succesful.
			Status (SW1&SW2) 		: if operation does not complete.
* 备	注：打开IIC会话
*********************************************************************************/
uint16_t M24SR_GetSession ( void )
{
	uint8_t Buffer = M24SR_OPENSESSION;
	uint16_t status;

	status = M24SR_SendI2Ccommand (&Buffer, 0x01);
	if( status == M24SR_STATUS_SUCCESS)
	{
		I2C_DebugLog("[M24SR_GetSession]: status = %02X\r\n", status);
		return M24SR_ACTION_COMPLETED;
	}
	else
	{
		I2C_DebugLog("[M24SR_GetSession]: status = %02X\r\n", status);
		return status;
	}
}

/*********************************************************************************
* 功	能：uint16_t M24SR_KillSession ( void )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED 	: the function is succesful.
			Status (SW1&SW2) 		: if operation does not complete.
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_KillSession ( void )
{
	uint8_t Buffer = M24SR_KILLSESSION;
	uint16_t status;

	status = M24SR_SendI2Ccommand (&Buffer, 0x01);
	if( status == M24SR_STATUS_SUCCESS)
	{
		I2C_DebugLog("[M24SR_KillSession]: M24SR_KillSession Success\r\n");
		return M24SR_ACTION_COMPLETED;
	}
	else
	{
		I2C_DebugLog("[M24SR_KillSession]: status = %02X\r\n", status);
		return status;
	}
}

/*********************************************************************************
* 功	能：uint16_t M24SR_SelectApplication ( void )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：选择应用
*********************************************************************************/
uint16_t M24SR_SelectApplication ( void )
{
	uint8_t		i = 0;
	uint8_t 	*pBuffer = uM24SRbuffer, NbByteToRead = M24SR_STATUSRESPONSE_NBBYTE;
	uint8_t		uLc = 0x07,
				pCMD_Data[] = {0xD2,0x76,0x00,0x00,0x85,0x01,0x01},
				uLe = 0x00;
	uint16_t	status ; 
	uint16_t	uP1P2 =0x0400,
				NbByte;
	
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
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( pBuffer, NbByte ));
	/* check if the response is ok */ 
	errchk(M24SR_PollI2C ( )); 
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cbuffer ( pBuffer, NbByteToRead ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer, NbByteToRead); 
	
	I2C_DebugLog("[M24SR_SelectApplication]: status = %02X\r\n", status);
	return status;
	
Error:
	I2C_DebugLog("[M24SR_SelectApplication]: status = %02X\r\n", status);
	return M24SR_ERROR_I2CTIMEOUT;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_SelectCCfile ( void )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：选择CC文件
*********************************************************************************/
uint16_t M24SR_SelectCCfile ( void )
{
	uint8_t 	*pBuffer = uM24SRbuffer ,
				NbByteToRead = M24SR_STATUSRESPONSE_NBBYTE;
	uint8_t		uLc = 0x02;
	uint16_t	status ; 
	uint16_t	uP1P2 =0x000C,
				uNbFileId =CC_FILE_ID,
				NbByte;
	
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
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( pBuffer, NbByte));
	/* check if the response is ok */ 
	errchk(M24SR_PollI2C ( )); 
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cbuffer ( pBuffer, NbByteToRead ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer, NbByteToRead);

	I2C_DebugLog("[M24SR_SelectCCfile]: status = %02X\r\n", status);
	return status; 
	
Error:
	I2C_DebugLog("[M24SR_SelectCCfile]: status = %02X\r\n", status);
	return M24SR_ERROR_I2CTIMEOUT;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_SelectSystemfile ( void )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：选择system文件
*********************************************************************************/
uint16_t M24SR_SelectSystemfile ( void )
{
	uint8_t 	*pBuffer = uM24SRbuffer ,
				NbByteToRead = M24SR_STATUSRESPONSE_NBBYTE;
	uint8_t		uLc = 0x02;
	uint16_t	status ; 
	uint16_t	uP1P2 =0x000C,
				uNbFileId = SYSTEM_FILE_ID,
				NbByte;
	
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
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( pBuffer, NbByte));
	/* check if the response is ok */ 
	errchk(M24SR_PollI2C ( )); 
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cbuffer ( pBuffer, NbByteToRead ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer, NbByteToRead); 
	
	I2C_DebugLog("[M24SR_SelectSystemfile]: status = %02X\r\n", status);
	return status; 
	
Error:
	I2C_DebugLog("[M24SR_SelectSystemfile]: status = %02X\r\n", status);
	return M24SR_ERROR_I2CTIMEOUT;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_SelectNDEFfile ( uint16_t NDEFfileId )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：选择NDEF文件
*********************************************************************************/
uint16_t M24SR_SelectNDEFfile ( uint16_t NDEFfileId )
{
	uint8_t 	*pBuffer = uM24SRbuffer ,
				NbByteToRead = M24SR_STATUSRESPONSE_NBBYTE;
	uint8_t		uLc = 0x02;
	uint16_t	status ; 
	uint16_t	uP1P2 =0x000C,
				NbByte;
	
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
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( pBuffer, NbByte));
	/* check if the response is ok */ 
	errchk(M24SR_PollI2C ( )); 
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cbuffer ( pBuffer, NbByteToRead ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer, NbByteToRead);

	I2C_DebugLog("[M24SR_SelectNDEFfile]: status = %02X\r\n", status);
	return status; 
	
Error:
	I2C_DebugLog("[M24SR_SelectNDEFfile]: status = %02X\r\n", status);
	return M24SR_ERROR_I2CTIMEOUT;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_ReadBinary ( uint16_t Offset ,uint8_t NbByteToRead , uint8_t *pBufferRead )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_ReadBinary ( uint16_t Offset ,uint8_t NbByteToRead , uint8_t *pBufferRead )
{
	uint8_t 	*pBuffer = uM24SRbuffer ;
	uint16_t	status ; 
	uint16_t	NbByte;
	
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

	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( pBuffer, NbByte));
	/* check if the response is ok */ 
	errchk(M24SR_PollI2C ( )); 
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cbuffer ( pBuffer, NbByteToRead + M24SR_STATUSRESPONSE_NBBYTE ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer, NbByteToRead + M24SR_STATUSRESPONSE_NBBYTE ); 
	/* retrieve the data without SW1 & SW2 as provided as return value of the function */
//	for(i=0; i<NbByteToRead; i++)
//		pBufferRead[i] = pBuffer[i];
	memcpy(pBufferRead ,&pBuffer[1], NbByteToRead);
	
	I2C_DebugLog("[M24SR_ReadBinary]: status = %02X\r\n", status);
	return status;
	
Error:
	I2C_DebugLog("[M24SR_ReadBinary]: status = %02X\r\n", status);
	return M24SR_ERROR_I2CTIMEOUT;
	
}

/*********************************************************************************
* 功	能：uint16_t M24SR_UpdateBinary ( uint16_t Offset , uint8_t NbByteToWrite , uint8_t *pDataToWrite )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_UpdateBinary ( uint16_t Offset , uint8_t NbByteToWrite , uint8_t *pDataToWrite )
{
	uint8_t 	*pBuffer = uM24SRbuffer ;
	uint16_t	status ; 
	uint16_t	NbByte;
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

	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( pBuffer, NbByte));
	/* check if the response is ok */ 
	errchk(M24SR_PollI2C ( )); 
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cbuffer ( pBuffer, M24SR_STATUSRESPONSE_NBBYTE ));
	
	/* if the response is a Watiting frame extenstion request */ 
	if (IsSBlock (pBuffer) == M24SR_STATUS_SUCCESS)
	{
		/*check the CRC */ 
		if (M24SR_IsCorrectCRC16Residue (pBuffer , M24SR_WATINGTIMEEXTRESPONSE_NBBYTE) != M24SR_ERROR_CRC)
		{
			/* send the FrameExension response*/ 
			status = M24SR_FWTExtension (  pBuffer [M24SR_OFFSET_PCB+1] );
		}
	}
	else
	{	
		status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
	}
	
	I2C_DebugLog("[M24SR_UpdateBinary]: status = %02X\r\n", status);
	return status;
	
Error:
	I2C_DebugLog("[M24SR_UpdateBinary]: status = %02X\r\n", status);
	return M24SR_ERROR_I2CTIMEOUT;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_Verify ( uint16_t uPwdId , uint8_t NbPwdByte , uint8_t *pPwd )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_Verify ( uint16_t uPwdId , uint8_t NbPwdByte , uint8_t *pPwd )
{
	uint8_t 	*pBuffer = uM24SRbuffer ;
	uint16_t	status = 0x0000 ; 
	uint16_t	NbByte;
	uint8_t 	i = 0;

	/*check the parameters */
	if (uPwdId > 0x0003)
	{	
		return M24SR_ERROR_PARAMETER;
	}
	if ( (NbPwdByte != 0x00) && (NbPwdByte != 0x10))
	{	
		return M24SR_ERROR_PARAMETER;
	}

	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_VERIFY;
	/* copy the Password Id */
	Command.Header.P1 = GETMSB	(uPwdId ) ;
	Command.Header.P2 = GETLSB	(uPwdId ) ;
	/* copy the number of byte of the data field */
	Command.Body.LC = NbPwdByte ;

	if (NbPwdByte == 0x10) 
	{
		/* copy the password */
		for(i=0; i<NbPwdByte; i++)
			Command.Body.pData[i] = pPwd[i];
//		memcpy(Command.Body.pData, pPwd, NbPwdByte);
		/* build the I_Block command */
		M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_VERIFYBINARYWITHPWD,  Command, &NbByte , pBuffer);
	}
	else 
	{
		/* build the I_Block command */
		M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_VERIFYBINARYWOPWD,  Command, &NbByte , pBuffer);
	}
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( pBuffer, NbByte));
	/* check if the response is ok */ 
	errchk(M24SR_PollI2C ( )); 
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cbuffer ( pBuffer, M24SR_STATUSRESPONSE_NBBYTE ));

	status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE ); 
	
	I2C_DebugLog("[M24SR_Verify]: status = %02X\r\n", status);
	return status;
	
Error:
	I2C_DebugLog("[M24SR_Verify]: status = %02X\r\n", status);
	return M24SR_ERROR_I2CTIMEOUT;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_SendInterrupt ( void  )
* 输    入: NULL
* 返	回：M24SR_ACTION_COMPLETED : the function is succesful. 
			M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_SendInterrupt ( void  )
{
	uint8_t 	*pBuffer = uM24SRbuffer;
	uint16_t	uP1P2 =0x001E;
	uint16_t	status ; 
	uint16_t	NbByte;
	
	status = M24SR_ManageI2CGPO( INTERRUPT);
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_ST;
	Command.Header.INS = C_APDU_INTERRUPT;
	/* copy the Password Id */
	Command.Header.P1 = GETMSB	(uP1P2 ) ;
	Command.Header.P2 = GETLSB	(uP1P2 ) ;
	Command.Body.LC = 0x00 ;
	/* build the I_Block command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SENDINTERRUPT,  Command, &NbByte , pBuffer);

	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( pBuffer, NbByte));
	/* check if the response is ok */ 
	errchk(M24SR_PollI2C ( )); 
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cbuffer ( pBuffer, M24SR_STATUSRESPONSE_NBBYTE ));

	status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE ); 
	
	I2C_DebugLog("[M24SR_SendInterrupt]: status = %02X\r\n", status);
	return status;
	
Error:
	I2C_DebugLog("[M24SR_SendInterrupt]: status = %02X\r\n", status);
	return M24SR_ERROR_I2CTIMEOUT;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_Deselect ( void )
* 输    入: NULL
* 返	回：执行状态
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_Deselect ( void )
{
	uint8_t 	pBuffer[] = {0xC2,0xE0,0xB4} ;
	int8_t		status ; 
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( pBuffer , M24SR_DESELECTREQUEST_NBBYTE));
	/* check if the response is ok */ 
	errchk(M24SR_PollI2C ( )); 
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cbuffer ( pBuffer , M24SR_DESELECTREQUEST_NBBYTE));

	I2C_DebugLog("[M24SR_Deselect]: M24SR_ACTION_COMPLETED\r\n");
	return M24SR_ACTION_COMPLETED;

Error:
	I2C_DebugLog("[M24SR_Deselect]: M24SR_ACTION_COMPLETED\r\n");
	return M24SR_ERROR_I2CTIMEOUT;
}

/*********************************************************************************
* 功	能：static uint16_t M24SR_FWTExtension ( uint8_t FWTbyte )
* 输    入: NULL
* 返	回：执行状态
* 备	注：NULL
*********************************************************************************/
static uint16_t M24SR_FWTExtension ( uint8_t FWTbyte )
{
	uint8_t 	pBuffer[M24SR_STATUSRESPONSE_NBBYTE];
	uint16_t	status ; 
	uint16_t	NthByte = 0,
				uCRC16;

	/* create the response */
	pBuffer[NthByte++] = 0xF2 ;	
	pBuffer[NthByte++] = FWTbyte ;
	/* compute the CRC */
	uCRC16 = M24SR_ComputeCrc (pBuffer,0x02);
	/* append the CRC16 */
	pBuffer [NthByte++] = GETLSB	(uCRC16 ) ;
	pBuffer [NthByte++] = GETMSB	(uCRC16 ) ;	
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand (  pBuffer, NthByte ));
	errchk(M24SR_PollI2C ( )); 
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cbuffer ( pBuffer , M24SR_STATUSRESPONSE_NBBYTE ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
	
	I2C_DebugLog("[M24SR_FWTExtension]: status = %02X\r\n", status);
	return status;
	
Error:
	I2C_DebugLog("[M24SR_FWTExtension]: status = %02X\r\n", status);
	return M24SR_ERROR_I2CTIMEOUT;
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
* 功	能：void M24SR_BuildIBlockCommand ( uint16_t CommandStructure, C_APDU Command, uint16_t *NbByte , uint8_t *pCommand)
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
void M24SR_BuildIBlockCommand ( uint16_t CommandStructure, C_APDU Command, uint16_t *NbByte , uint8_t *pCommand)
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
		pCommand[(*NbByte)++] = 0x02 |  BlockNumber; 
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

/*********************************************************************************
* 功	能：static int8_t IsSBlock (uc8 *pBuffer)
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
static int8_t IsSBlock (uint8_t *pBuffer)
{

	if ((pBuffer[M24SR_OFFSET_PCB] & M24SR_MASK_BLOCK) == M24SR_MASK_SBLOCK)
	{
		return M24SR_STATUS_SUCCESS;
	}
	else 
	{	
		return M24SR_ERROR_DEFAULT;
	}
	
}


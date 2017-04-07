/** @file nfc.c
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
#include "nfc.h"



//#define ENABLE_M24SR_DEBUGLOG
#ifdef	ENABLE_M24SR_DEBUGLOG
	#define	M24SR_DebugLog			app_trace_log
#else
	#define	M24SR_DebugLog(...)
#endif //ENABLE_M24SR_DEBUGLOG

sCCFileInfo 				CCFileStruct;
uint16_t 					NDEF_FileID = 0xDEAD;
uint32_t 					NDEFSessionOpenID = NDEF_SESSION_CLOSED;


void NFC_init(void)
{
	uint8_t xor = 0;
	TT4_Init();												//NFC初始化，获取CC File，设置GPO，获取UID
	TT4_ReadNDEF(NDEF_DataRead);							//收到中断信号后被激活，读出参数
	M24SR_Deselect();	
	
	xor = XOR_Cal(NDEF_DataRead+1,26);
		
	//13.56M数据校验出错后，显示信号强度作为标志
	if(NDEF_DataRead[27] != xor)
	{
		LCD->signal(3);
	}
}




/*********************************************************************************
* 功	能：void M24SR_Init( void )
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
void M24SR_Init( void )
{		
	I2C_Init();
	M24SR_InitStructure();
														
//#ifdef I2C_GPO_SYNCHRO_ALLOWED
	M24SR_KillSession();
	M24SR_ManageI2CGPO(I2C_ANSWER_READY);
	M24SR_Deselect ();
//#endif /* I2C_GPO_SYNCHRO_ALLOWED */
	
	M24SR_DebugLog("[M24SR_Init]: M24SR_Init Ok\r\n");
}

/*********************************************************************************
* 功	能：uint16_t M24SR_SendI2Ccommand(uint8_t *buf, uint8_t len)
* 输    入: buf		写入数据的buf
			len		写入数据的长度
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_SendI2Ccommand(uint8_t *buf, uint8_t len)
{
	uint16_t M24SR_Timeout = M24SR_I2C_TIMEOUT;
	uint8_t i = 0;
	
	I2C_Start();
	Write_Byte( M24SR_I2C_WRITE_ADDR );
	while( !Check_Ack() )
	{
		if (M24SR_Timeout-- == 0) goto Error;
	}
	for(i=0; i<len; i++)
	{
		Write_Byte(buf[i]);
		M24SR_Timeout = M24SR_I2C_TIMEOUT;
		while( !Check_Ack() )
		{
			if (M24SR_Timeout-- == 0) goto Error;
		}
	}
	I2C_Stop();
	return M24SR_STATUS_SUCCESS;

Error:
	I2C_Stop();
	return M24SR_ERROR_I2CTIMEOUT;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_ReceiveI2Cbuffer ( uint8_t *buf, uint8_t len )
* 输    入: buf		读出数据的buf
			len		读出数据的长度
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_ReceiveI2Cbuffer( uint8_t *buf, uint8_t len )
{
	uint16_t M24SR_Timeout = M24SR_I2C_TIMEOUT/2;
	uint8_t i = 0;
	
	I2C_Start();
	Write_Byte( M24SR_I2C_READ_ADDR );
	while( !Check_Ack() )
	{
		if (M24SR_Timeout-- == 0) goto Error;
	}
	for(i=0; i<len; i++)
	{
		buf[i] = ReadByte();
		Send_Ack();
	}
	I2C_Stop();
	return M24SR_STATUS_SUCCESS;

Error:
	I2C_Stop();
	return M24SR_ERROR_I2CTIMEOUT;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_PollI2C ( void )
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_PollI2C ( void )
{
	uint16_t	NbAtempt = 0x0FFF;
	uint8_t		status = 0;
	
	/* In the loop avoid to send START just after STOP */
	/* At 400kHz see issue where start condition was sent but not the 7bits addr... */ 
	nrf_delay_us(5);
	do
	{  
		I2C_Start();
		Write_Byte( M24SR_I2C_WRITE_ADDR );
		status = Check_Ack();
		I2C_Stop();
	}while( !status && (--NbAtempt > 0));
	
	if(NbAtempt > 0)
	{
		M24SR_DebugLog("[M24SR_PollI2C]: ACK\r\n");
		return M24SR_STATUS_SUCCESS;
	}
	else
		goto Error;

Error:
	M24SR_DebugLog("[M24SR_PollI2C]: NACK\r\n");
	return M24SR_ERROR_I2CTIMEOUT;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_ManageI2CGPO( uint8_t GPO_I2Cconfig)
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_ManageI2CGPO( uint8_t GPO_I2Cconfig)
{
	uint16_t status;
	uint8_t GPO_config;
	uint8_t DefaultPassword[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
	if( GPO_I2Cconfig > STATE_CONTROL)
	{	
		return M24SR_ERROR_PARAMETER;
	}
	
//	/* we must not be in interrupt mode for I2C synchro as we will change GPO purpose */
//	M24SR_SetI2CSynchroMode(M24SR_WAITINGTIME_POLLING);
//	
	M24SR_SelectApplication();
	M24SR_SelectSystemfile();
	M24SR_ReadBinary ( 0x0004 , 0x01 , &GPO_config );

	/* Update only GPO purpose for I2C */	
	GPO_config = (GPO_config & 0xF0) | GPO_I2Cconfig;
	M24SR_SelectSystemfile();
	M24SR_Verify( I2C_PWD ,0x10 , DefaultPassword );
	status = M24SR_UpdateBinary ( 0x0004 ,0x01, &(GPO_config) );
	
//	/* if we have set interrupt mode for I2C synchro we can enable interrupt mode */
//	if ( GPO_I2Cconfig == I2C_ANSWER_READY)
//		M24SR_SetI2CSynchroMode(M24SR_WAITINGTIME_GPO);
//	
	M24SR_DebugLog("[M24SR_ManageI2CGPO]: status = %02X\r\n", status);
	return status;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_ManageRFGPO( uint8_t GPO_RFconfig)
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_ManageRFGPO( uint8_t GPO_RFconfig)
{
	uint16_t status;
	uint8_t GPO_config;
	uint8_t DefaultPassword[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
	if( GPO_RFconfig > STATE_CONTROL)
	{	
		return M24SR_ERROR_PARAMETER;
	}
	
	M24SR_SelectApplication();
	M24SR_SelectSystemfile();
	M24SR_ReadBinary ( 0x0004 , 0x01 , &GPO_config );

  /* Update only GPO purpose for I2C */	
	GPO_config = (GPO_config & 0x0F) | (GPO_RFconfig<<4);
	M24SR_SelectSystemfile();
	M24SR_Verify( I2C_PWD ,0x10 ,DefaultPassword );
	status = M24SR_UpdateBinary ( 0x0004 ,0x01, &(GPO_config) );
	
	M24SR_DebugLog("[M24SR_ManageRFGPO]: status = %02X\r\n", status);
	return status;
}

/*********************************************************************************
* 功	能：void M24SR_GetUID( uint8_t* UIDBuf)
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
void M24SR_GetUID( uint8_t* UIDBuf)
{
	M24SR_SelectApplication();
	M24SR_SelectSystemfile();
	M24SR_ReadBinary ( 0x0008, 7, UIDBuf );
	
	M24SR_DebugLog("[M24SR_GetUID]: %02X%02X%02X%02X%02X%02X%02X\r\n", UIDBuf[0], UIDBuf[1], UIDBuf[2], UIDBuf[3], UIDBuf[4], UIDBuf[5], UIDBuf[6]);
}

/*********************************************************************************
* 功	能：uint16_t M24SR_ManageIICProtect( uint8_t GPO_IICconfig)
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_ManageIICProtect( uint8_t IICconfig)
{
	uint16_t status;
	uint8_t DefaultPassword[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
	if( (IICconfig != 0x00) && (IICconfig != 0x01) )
	{	
		return M24SR_ERROR_PARAMETER;
	}
	
	M24SR_SelectApplication();
	M24SR_SelectSystemfile();
	M24SR_Verify( I2C_PWD ,0x10 ,DefaultPassword );
	status = M24SR_UpdateBinary ( 0x0002 ,0x01, &(IICconfig) );
	
	M24SR_DebugLog("[M24SR_ManageIICProtect]: status = %02X\r\n", status);
	return status;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_ReadData ( uint16_t Offset , uint16_t DataSize , uint8_t* pData)
* 输    入: NULL
* 返	回：执行状态
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_ReadData ( uint16_t Offset , uint16_t DataSize , uint8_t* pData)
{
	uint16_t status;
	
	if( DataSize > M24SR_READ_MAX_NBBYTE)
	{	
		do
		{
			status = M24SR_ReadBinary ( Offset, M24SR_READ_MAX_NBBYTE , pData);
			Offset += M24SR_READ_MAX_NBBYTE;
			pData += M24SR_READ_MAX_NBBYTE;
			DataSize -= M24SR_READ_MAX_NBBYTE;
		}while( DataSize > M24SR_READ_MAX_NBBYTE && status == M24SR_ACTION_COMPLETED);
		if( status == M24SR_ACTION_COMPLETED && DataSize)
			status = M24SR_ReadBinary ( Offset, (uint8_t)(DataSize) , pData);
	}
	else
		status = M24SR_ReadBinary ( Offset, (uint8_t)(DataSize) , pData);
	
	M24SR_DebugLog("[M24SR_ReadData]: status = %02X\r\n", status);
	return status;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_WriteData ( uint16_t Offset , uint16_t DataSize , uint8_t* pData)
* 输    入: NULL
* 返	回：执行状态
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_WriteData ( uint16_t Offset , uint16_t DataSize , uint8_t* pData)
{
	uint16_t status;
	
	if( DataSize > M24SR_WRITE_MAX_NBBYTE)
	{	
		do
		{
			status = M24SR_UpdateBinary ( Offset, M24SR_WRITE_MAX_NBBYTE , pData);
			Offset += M24SR_WRITE_MAX_NBBYTE;
			pData += M24SR_WRITE_MAX_NBBYTE;
			DataSize -= M24SR_WRITE_MAX_NBBYTE;
		}while( DataSize > M24SR_WRITE_MAX_NBBYTE && status == M24SR_ACTION_COMPLETED);
		if( status == M24SR_ACTION_COMPLETED && DataSize)
			status = M24SR_UpdateBinary ( Offset, (uint8_t)(DataSize) , pData);
	}
	else
		status = M24SR_UpdateBinary ( Offset, (uint8_t)(DataSize) , pData);
	
	M24SR_DebugLog("[M24SR_WriteData]: status = %02X\r\n", status);
	return status;
}

/*********************************************************************************
* 功	能：uint16_t M24SR_OpenNDEFSession ( uint16_t NDEF_fileID, uint16_t Priority )
* 输    入: NDEF_fileID : NDEF identification to select NDEF in M24SR
			Priority: 2 options: check if M24SR available to open session (no RF session on going)
															Kill RF session and open I2C sesssion.
* 返	回：执行状态
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_OpenNDEFSession ( uint16_t NDEF_fileID, uint16_t Priority )
{
	uint16_t status = ERROR;
	uint16_t timeout = 1000;
	
	if(NDEFSessionOpenID == NDEF_SESSION_CLOSED)
	{
		if( Priority == TAKE_SESSION)
		{
			status = M24SR_KillSession();
		}
		else
		{
			status = M24SR_GetSession();
			while( status != M24SR_ACTION_COMPLETED && timeout)
			{
				nrf_delay_ms(1);
				status = M24SR_GetSession();
				timeout--;
			}
		}
		if (status != M24SR_ACTION_COMPLETED)
		{
			M24SR_DebugLog("[M24SR_OpenNDEFSession]: status = %02X\r\n", status);
			return ERROR;
		}
	
		/*===================================*/
		/* Select the NFC type 4 application */ 
		/*===================================*/
		status_chk( M24SR_SelectApplication() );
		
		/*====================*/
		/* select NDEF file   */
		/*====================*/
		status_chk( M24SR_SelectNDEFfile(NDEF_fileID) );
	
		NDEFSessionOpenID = (uint32_t)(NDEF_fileID);

		M24SR_DebugLog("[M24SR_OpenNDEFSession]: status = %02X\r\n", status);
		return SUCCESS;
	}
	else if(NDEFSessionOpenID == NDEF_fileID)
	{
		M24SR_DebugLog("[M24SR_OpenNDEFSession]: status = %02X\r\n", status);
		/* Session already Open not an issue caller can perform access in NDEF file */
		return SUCCESS;
	}
		
Error:
	M24SR_DebugLog("[M24SR_OpenNDEFSession]: status = %02X\r\n", status);
	return ERROR;	
}

/*********************************************************************************
* 功	能：uint16_t M24SR_CloseNDEFSession ( uint16_t NDEF_fileID )
* 输    入: NULL
* 返	回：执行状态
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_CloseNDEFSession ( uint16_t NDEF_fileID )
{
	uint16_t status = ERROR;
	
	if(NDEFSessionOpenID == (uint32_t)(NDEF_fileID))
	{
		status_chk( M24SR_Deselect () );
		NDEFSessionOpenID = NDEF_SESSION_CLOSED;
	
		M24SR_DebugLog("[M24SR_CloseNDEFSession]: status = %02X\r\n", status);
		return SUCCESS;
	}
	else if(NDEFSessionOpenID == NDEF_SESSION_CLOSED)
	{
		M24SR_DebugLog("[M24SR_CloseNDEFSession]: status = %02X\r\n", status);
		/* Not an error as session is already closed */
		return SUCCESS;
	}
		
Error:
	M24SR_DebugLog("[M24SR_CloseNDEFSession]: status = %02X\r\n", status);
	return ERROR;	
}

/*********************************************************************************
* 功	能：uint16_t NDEF_ReadNDEF( uint8_t* pNDEF)
* 输    入: pNDEF : pointer on the buffer to store NDEF data
* 返	回：SUCCESS : NDEF file data retrieve and store in the buffer
			ERROR : not able to read NDEF
* 备	注：This fonction read the NDEF content of the TAG
*********************************************************************************/
uint16_t NDEF_ReadNDEF( uint8_t* pNDEF)
{
	uint16_t status = ERROR;
	uint16_t NDEF_Size = 0;
	
	status = M24SR_ReadData( 0 , 2 , pNDEF);
	
	if( status == M24SR_ACTION_COMPLETED)
	{
		NDEF_Size = (uint16_t)(*pNDEF) << 8;
		NDEF_Size |= (uint16_t)(*(pNDEF+1));
		
		status = M24SR_ReadData( 0 , NDEF_Size+2 , pNDEF);
	}
	
	if( status == M24SR_ACTION_COMPLETED)
	{
		M24SR_DebugLog("[NDEF_ReadNDEF]: status = %02X\r\n", status);
		return SUCCESS;
	}
	else
	{
		M24SR_DebugLog("[NDEF_ReadNDEF]: status = %02X\r\n", status);
		return ERROR;
	}
}

/*********************************************************************************
* 功	能：uint16_t NDEF_WriteNDEF( uint8_t *pNDEF)
* 输    入: pNDEF : pointer on the buffer containing the NDEF data
* 返	回：SUCCESS : NDEF file data written in the tag
			ERROR : not able to store NDEF in tag
* 备	注：This fonction write the NDEF in the TAG
*********************************************************************************/
uint16_t NDEF_WriteNDEF( uint8_t *pNDEF)
{
	uint16_t status = ERROR;
	uint16_t NDEF_Size = 0;
	
	NDEF_Size = (uint16_t)(*pNDEF) << 8;
	NDEF_Size |= (uint16_t)(*(pNDEF+1));

	status = M24SR_WriteData( 0 , NDEF_Size+2 , pNDEF);
	
	if( status == M24SR_ACTION_COMPLETED)
	{
		M24SR_DebugLog("[NDEF_WriteNDEF]: status = %02X\r\n", status);
		return SUCCESS;
	}
	else
	{
		M24SR_DebugLog("[NDEF_WriteNDEF]: status = %02X\r\n", status);
		return ERROR;
	}
}

/*********************************************************************************
* 功	能：uint16_t M24SR_Initialization ( uint8_t* CCBuffer, uint8_t size )
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
uint16_t M24SR_Initialization ( uint8_t* CCBuffer, uint8_t size )
{
	uint16_t status = ERROR;
	uint16_t timeout = 1000; 													/* wait 1sec */
	
	M24SR_Init();																/* Perform HW initialization */		
	status = M24SR_KillSession();												/* Read CC file */
	while( status != M24SR_ACTION_COMPLETED && timeout)
	{
		nrf_delay_ms(1);
		status = M24SR_KillSession();
		timeout--;
	}
	if (status != M24SR_ACTION_COMPLETED)
		return ERROR;
	/*===================================*/
	/* Select the NFC type 4 application */ 
	/*===================================*/
	status_chk( M24SR_SelectApplication() );
		
	/*==================*/		
	/* select a CC file */ 
	/*==================*/
	status_chk (M24SR_SelectCCfile() );
			
	/* read the first 15 bytes of the CC file */
	if( M24SR_ReadData ( 0x0000 , size , CCBuffer )== M24SR_ACTION_COMPLETED)
	{			
		NDEF_FileID = (uint16_t) ((CCBuffer[0x09]<<8) | CCBuffer[0x0A]);
		status_chk( M24SR_Deselect () );
		M24SR_DebugLog("[M24SR_Initialization]: status = %02X\r\n", status);
		return SUCCESS;
	}
	else
		status_chk( M24SR_Deselect () );
		
Error:
	M24SR_DebugLog("[M24SR_Initialization]: status = %02X\r\n", status);
	return ERROR;
}

/*********************************************************************************
* 功	能：uint16_t TT4_Init (void)
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
uint16_t TT4_Init (void)
{
	uint16_t status = ERROR;
	uint8_t CCBuffer[15];
	sCCFileInfo *pCCFile;
	
	pCCFile = &CCFileStruct;

	status = M24SR_Initialization( CCBuffer, sizeof(CCBuffer));					//获取CCfile的内容
	
	M24SR_GetSession();															//开始会话
	M24SR_ManageI2CGPO( HIGH_IMPEDANCE );										//将IIC控制GPO中断关闭				
	M24SR_ManageRFGPO( INTERRUPT );												//使用RF发送中断信号
	M24SR_ManageIICProtect( IIC_NO_PROTECT );									//去掉IIC保护
	M24SR_GetUID( &UID[0]);														//从系统文件获取UID
	
	pCCFile->NumberCCByte 		= 	(uint16_t)CCBuffer[0x00]<<8 | CCBuffer[0x01];
	pCCFile->Version 			= 	CCBuffer[0x02];
	pCCFile->MaxReadByte		= 	(uint16_t)CCBuffer[0x03]<<8 | CCBuffer[0x04];
	pCCFile->MaxWriteByte 		= 	(uint16_t)CCBuffer[0x05]<<8 | CCBuffer[0x06];
	pCCFile->TField 			= 	CCBuffer[0x07];
	pCCFile->LField 			= 	CCBuffer[0x08];
	pCCFile->FileID 			= 	(uint16_t)CCBuffer[0x09]<<8 | CCBuffer[0x0A];
	pCCFile->NDEFFileMaxSize 	= 	(uint16_t)CCBuffer[0x0B]<<8 | CCBuffer[0x0C];
	pCCFile->ReadAccess 		= 	CCBuffer[0x0D];
	pCCFile->WriteAccess 		= 	CCBuffer[0x0E];	
	
	M24SR_DebugLog("[TT4_Init]: CCFile Data:\r\n");
	M24SR_DebugLog("NumberCCByte: 	%02X\r\n", pCCFile->NumberCCByte);
	M24SR_DebugLog("Version: 	%02X\r\n", pCCFile->Version);
	M24SR_DebugLog("MaxReadByte: 	%02X\r\n", pCCFile->MaxReadByte);
	M24SR_DebugLog("MaxWriteByte: 	%02X\r\n", pCCFile->MaxWriteByte);
	M24SR_DebugLog("TField: 	%02X\r\n", pCCFile->TField);
	M24SR_DebugLog("LField: 	%02X\r\n", pCCFile->LField);
	M24SR_DebugLog("FileID: 	%02X\r\n", pCCFile->FileID);
	M24SR_DebugLog("NDEFFileMaxSize:%02X\r\n", pCCFile->NDEFFileMaxSize);
	M24SR_DebugLog("ReadAccess: 	%02X\r\n", pCCFile->ReadAccess);
	M24SR_DebugLog("WriteAccess: 	%02X\r\n", pCCFile->WriteAccess);
	
	if(SUCCESS == status)
	{
		M24SR_DebugLog("[TT4_Init]: TT4_Init Success\r\n");
	}
	else
	{
		M24SR_DebugLog("[TT4_Init]: TT4_Init Error! status = %02X\r\n", status);
	}
	return status;
}	

/*********************************************************************************
* 功	能：uint16_t TT4_Init (void)
* 输    入: pNDEF : pointer on buffer to fill with NDEF data
* 返	回：SUCCESS : NDEF file data have been retrieve
			ERROR : Not able to get NDEF file data
* 备	注：This fonction read NDEF file
*********************************************************************************/
uint16_t TT4_ReadNDEF(uint8_t *pNDEF)
{
	uint16_t status = ERROR;
	sCCFileInfo *pCCFile;
	
	pCCFile = &CCFileStruct;
	
	if(M24SR_OpenNDEFSession(pCCFile->FileID, ASK_FOR_SESSION) == SUCCESS)
	{
		status = NDEF_ReadNDEF(pNDEF);
		M24SR_CloseNDEFSession(pCCFile->FileID);
	}
	
	M24SR_DebugLog("[TT4_ReadNDEF]: status = %02X\r\n", status);
	return status;
}

/*********************************************************************************
* 功	能：uint16_t TT4_WriteNDEF(uint8_t *pNDEF)
* 输    入: pNDEF : pointer on buffer that contain the NDEF data
* 返	回：SUCCESS : NDEF file data have been stored
			ERROR : Not able to store NDEF file
* 备	注：This fonction write NDEF file
*********************************************************************************/
uint16_t TT4_WriteNDEF(uint8_t *pNDEF)
{
	uint16_t status = ERROR;
	sCCFileInfo *pCCFile;
	
	pCCFile = &CCFileStruct;
	
	if(M24SR_OpenNDEFSession(pCCFile->FileID, ASK_FOR_SESSION) == SUCCESS)
	{
		status = NDEF_WriteNDEF( pNDEF);
		M24SR_CloseNDEFSession(pCCFile->FileID);
	}
	
	M24SR_DebugLog("[TT4_WriteNDEF]: status = %02X\r\n", status);
	return status;
}








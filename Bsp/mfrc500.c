/**
  ******************************************************************************
  * @file   	mfrc500.c
  * @author  	Sam.wu
  * @version 	V1.0.0.0
  * @date   	2016.12.29
  * @brief   	hal function for nrf moulde
  ******************************************************************************
  */
 
#include "main.h"
#include "mfrc500.h" 

/*********************************************************************************
* 功	能：void ComSelectApplication(void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful.
* 备	注：
*********************************************************************************/
uint8_t SelectApplication (void)
{
	uint8_t len = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	status = PcdRATS(respon, &len);
	
	if( (MI_OK == status) && (respon[0] == 0x05) && (respon[1] == 0x78) && (respon[4] == 0x02) )
	{
	}
	else
	{
		return 0x11;
	}
	
	memset(respon, 0, BUF_LEN);
	len = 0;
	status = PcdPPS(respon, &len);
	if( (MI_OK == status) && (respon[0] == 0xD0) )
	{
	}
	else
	{
		return 0x11;
	}
	
	memset(respon, 0, BUF_LEN);
	len = 0;
	status = PcdSelectApplication(respon, &len);
	if( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) )
	{
		return MI_OK;
	}
	else
	{
		return 0x11;
	}
}

/*********************************************************************************
* 功	能：uint8_t ForceReadCCfile (void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful.
* 备	注：
*********************************************************************************/
uint8_t ForceReadCCfile (void)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead = 0;
	uint8_t status = 0;

	memset(respon, 0, BUF_LEN);
	status = PcdSelectCCfile(respon, &len);
	if( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) )
	{
	}
	else
	{
		return (1);
	}

	memset(respon, 0, BUF_LEN);
	len = 0;
	status = PcdReadCCfileLength(respon, &len);
	if( MI_OK == status )
	{
		NDEF_DataRead = (uint16_t)( (respon[1] << 8) | (respon[2]) );
	}
	else
	{
		return (1);
	}
	
	memset(respon, 0, BUF_LEN);
	status = PcdReadCCfile(0x0000, NDEF_DataRead, respon, &len);
	if( MI_OK == status )
	{
		return MI_OK;
	}
	else
	{
		return (1);
	}
}

/*********************************************************************************
* 功	能：void ComReadNDEFfile (void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful.
* 备	注：
*********************************************************************************/
uint8_t ReadNDEFfile (uint8_t *NDEFfile_Data, uint16_t *NDEFfile_len)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead_Len = 0;
	uint8_t status = 0;
	status = ForceReadCCfile();
	//printf("ForceReadCCfile status = %x \r\n",status);
	if( MI_OK != status )
	{
		return 0x11;
	}
	
	memset(respon, 0, BUF_LEN);
	len = 0;
	status = PcdSelectNDEFfile(respon, &len);
	//printf("PcdSelectNDEFfile status = %x respon = %02x %02x %02x %02x\r\n",\
    status,respon[0],respon[1],respon[2],respon[3]);
	if( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) )
	{
	}
	else
	{
		return 0x11;
	}
	
	memset(respon, 0, BUF_LEN);
	len = 0;
	//MRC500_DEBUG_START("PcdReadNDEFfileLength \r\n");
	status = PcdReadNDEFfileLength(respon, &len);
	//MRC500_DEBUG_END();
	//printf("PcdReadNDEFfileLength status = %x respon = %02x %02x %02x %02x %02x\r\n",\
    status,respon[0],respon[1],respon[2],respon[3],respon[4]);
	if( MI_OK == status )
	{
		NDEF_DataRead_Len = ((uint16_t)respon[1] << 8) | respon[2] ;
		//printf("NDEF_DataRead_Len = %d\r\n",NDEF_DataRead_Len);
	}
	else
	{
		return 0x11;
	}
	NDEF_DataRead_Len = 28;
	memset(respon, 0, BUF_LEN);
	len = 0;
	//MRC500_DEBUG_START("ReadNDEFfile \r\n");
	status = PcdReadNDEFfile(0x0000, NDEF_DataRead_Len + 2, respon, &len);
	//MRC500_DEBUG_END();
	//printf("PcdReadNDEFfile status = %x respon = %02x %02x %02x %02x\r\n",\
    status,respon[0],respon[1],respon[2],respon[3]);
	if( MI_OK == status )
	{
		memcpy(NDEFfile_Data, &respon[1], NDEF_DataRead_Len + 2);
		*NDEFfile_len = NDEF_DataRead_Len + 2;
	}
	else
	{
		return 0x11;
	}
	return 0x00;
}

/******************************************************************************
  Function:WriteNDEFfile1
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
uint8_t WriteNDEFfile1( uint8_t *pDataToWrite )
{
	uint8_t len = 0;
	uint8_t EraseLen[2] = {0x00, 0x00};
	uint8_t status = 0;
	uint8_t i = 0;

	/* len = 2byte */
	uint16_t NbByteToWrite = ((uint16_t)*pDataToWrite << 8) | *(pDataToWrite + 1);
	
	status = ForceReadCCfile();
	if(MI_OK != status)
		return 11;
	
	/* Select NDEFfile  */
	memset(respon, 0, BUF_LEN);
	status = PcdSelectNDEFfile(respon, &len);
	if((MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00))
	{
	}
	else
	{
		return 11;
	}
	
	/* Erase NDEFfile Len  */
	status = PcdWriteNDEFfile(0x0000, 0x02, EraseLen, respon, &len);
	if( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) )
	{
	}
	else
	{
		return 11;
	}
	
	#define NbByteToWrite_LEN 2
//printf("NbByteToWrite = %d\r\n",NbByteToWrite);
	for( i = 0; i<NbByteToWrite;  )
	{
		uint8_t write_len = NbByteToWrite_LEN;

		if( (i+NbByteToWrite_LEN) > NbByteToWrite )
				write_len = NbByteToWrite-i;
//	printf("write data: %02x %02x ",*(pDataToWrite+i),*(pDataToWrite+i+1));
		status = PcdWriteNDEFfile(i, write_len, pDataToWrite+i, respon, &len);
//		printf("index = %02d PcdWriteNDEFfile2 status = %x respon = %02x %02x %02x %02x\r\n",
//			i, status,respon[0],respon[1],respon[2],respon[3]);
		if( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) )
		{
			i = i+write_len;
		}
		else
		{
			return 11;
		}
	}
	
	/* Update NDEFfile Len  */
	status = PcdWriteNDEFfile(0x0000, 0x02, pDataToWrite, respon, &len);
	if( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) )
	{
	}
	else
	{
		return 11;
	}
	return MI_OK;
}
/*********************************************************************************
* 功	能：void ComSendInterrupt (void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful.
* 备	注：
*********************************************************************************/
uint8_t SendInterrupt (void)
{
	uint8_t len = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	status = ForceReadCCfile();
	
	if(MI_OK != status) 
	{
		return 0x11;
	}
	
	status = PcdSelectSystemfile(respon, &len);
	if(( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) )) 
	{
	}
	else
	{
		return 0x11;
	}
	
	memset(respon, 0, BUF_LEN);
	len = 0;
	status = PcdReadSystemfile(0x0004, 0x01, respon, &len);
	if( (MI_OK == status) && ((respon[1] & 0x40 ) == 0x40) )
	{
	}
	else
	{
		return 0x11;
	}
	
	memset(respon, 0, BUF_LEN);
	len = 0;
	status = PcdSendInterrupt(respon, &len);
	if( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) 
	{
		return MI_OK;
	}
	else
	{
		return 0x11;
	}
}

/*********************************************************************************
* 功	能：void ComDeselect (void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful.
* 备	注：
*********************************************************************************/
uint8_t Deselect (void)
{
	uint8_t len = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	status = PcdDeselect(respon, &len);
	
	if( (MI_OK == status) && (respon[0] == 0xC2) && (respon[1] == 0xE0) && (respon[2] == 0xB4) )
	{
		return MI_OK;
	}
	else
	{
		return 0x11;
	}
}
/**************************************END OF FILE****************************/

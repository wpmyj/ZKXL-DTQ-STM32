/**
  ******************************************************************************
  * @file    drv_I2C_M24SR.c
  * @author  MMY Application Team
  * @version V4.5.0
  * @date    08-May-2013
  * @brief   This file provides a set of functions needed to manage the I2C of
	*				   the M24SR device.
 ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MMY-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "drv_I2C_M24SR.h"

//static uint8_t						uSynchroMode = M24SR_WAITINGTIME_POLLING;
//static uint8_t						uSynchroMode = M24SR_WAITINGTIME_TIMEOUT;



/* 模拟I2C驱动函数 ------------------------------------------------------------------*/
void M24SR_I2CInit(void)
{
	nrf_gpio_cfg_output(I2C_RF_EN);
	nrf_gpio_cfg_output(I2C_SCL);
	nrf_gpio_cfg_output(I2C_SDA);
	//nrf_gpio_cfg_sense_input(I2C_INT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
	
	nrf_gpio_pin_clear(I2C_RF_EN);
	nrf_gpio_pin_set(I2C_SCL);
	nrf_gpio_pin_set(I2C_SDA);
}

//
void M24SR_I2CStart(void)
{
	I2C_SDA_HIGH();
	I2C_DELAY();
	I2C_SCL_HIGH();
	I2C_DELAY();
	I2C_SDA_LOW();
	I2C_DELAY();
	I2C_DELAY();
}

void M24SR_I2CStop(void)
{
	I2C_SDA_LOW();
	I2C_DELAY();
	I2C_SCL_HIGH();
	I2C_DELAY();
	I2C_SDA_HIGH();
	I2C_DELAY();
	I2C_DELAY();
}

void M24SR_SendAck(void)
{
	I2C_SCL_LOW();
	I2C_DELAY();
	I2C_SDA_LOW();
	I2C_DELAY();
	I2C_SCL_HIGH();
	I2C_DELAY();
	I2C_SCL_LOW();
}

uint8_t M24SR_CheckAck(void)
{
	uint8_t tempbit;

	I2C_SCL_LOW();
	I2C_SDA_HIGH();
	I2C_DELAY();
	nrf_gpio_cfg_input(I2C_SDA, NRF_GPIO_PIN_PULLUP);							//将SDA设置为输入
	I2C_DELAY();
	tempbit = nrf_gpio_pin_read(I2C_SDA);
	I2C_SCL_HIGH();
	I2C_DELAY();
	I2C_SCL_LOW();
	nrf_gpio_cfg_output(I2C_SDA);												//将SDA设置为输出
	if(tempbit==1)
	{
		return 0; 																//返回noACK
	}
	else
	{
		return 1; 																//返回ACK
	}
}

void M24SR_WriteByte(uint8_t dat)
{
	uint8_t bitCnt = 0 ;
	
	for(bitCnt=0; bitCnt<8; bitCnt++)
	{
		I2C_SCL_LOW();
		I2C_DELAY();
		if((dat & 0x80) == 0x80) 				//if the MSb is 1
		{
			I2C_SDA_HIGH();
		}
		else
		{
			I2C_SDA_LOW();
		}
		I2C_DELAY();
		I2C_SCL_HIGH();
		I2C_DELAY();
		dat<<=1 ;
	}
}

uint8_t M24SR_ReadByte(void)
{
	uint8_t tempbit = 1 ;
	uint8_t temp = 0 ;
	uint8_t bitCnt ;
	
	I2C_SDA_HIGH(); 															//释放SDA，开始接收数据
	I2C_DELAY();
	nrf_gpio_cfg_input(I2C_SDA, NRF_GPIO_PIN_PULLUP);							//将SDA设置为输入
	
	for(bitCnt=0; bitCnt<8; bitCnt++)
	{
		I2C_SCL_LOW(); 
		I2C_DELAY();
		I2C_DELAY();
		I2C_SCL_HIGH();		
		tempbit = nrf_gpio_pin_read(I2C_SDA);
		if(tempbit)
			temp |= 0x01 ;
		else
			temp &= 0xfe ;
		I2C_DELAY();
		if(bitCnt<7)
		temp <<= 1 ;
	}
	
	I2C_SCL_LOW();
	nrf_gpio_cfg_output(I2C_SDA);												//将SDA设置为输出
	return (temp) ;
}





/**			指令之后， I2C 主机可执行轮询流程，以确定何时有响应
  * @brief  This functions polls the I2C interface，
  * @retval M24SR_STATUS_SUCCESS : the function is succesful
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured. 
	* @retval M24SR_ERROR_NACK : the M24SR doesn't acknowledge
	* @retval M24SR_ERROR_NBATEMPT  
  */
int8_t M24SR_PollI2C ( void )
{
	uint16_t	NbAtempt = 0x0FFF;
	uint8_t		status = 0;
	
	/* In the loop avoid to send START just after STOP */
	/* At 400kHz see issue where start condition was sent but not the 7bits addr... */ 
	nrf_delay_us(5);
	do
	{  
		M24SR_I2CStart();
		M24SR_WriteByte( M24SR_I2C_WRITE_ADDR );
		status = M24SR_CheckAck();
		M24SR_I2CStop();
	}while( !status && (--NbAtempt > 0));
	
	if(NbAtempt > 0)
	{
		return M24SR_STATUS_SUCCESS;
	}
	else
		goto Error;

Error:
	return M24SR_ERROR_I2CTIMEOUT;
}

/**
  * @brief  This function generates an I2C Token release
  * @retval M24SR_STATUS_SUCCESS : the function is succesful
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured. 
  */
int8_t M24SR_I2CTokenRelease ( void )
{
	return 0;
}

/**
  * @brief  This functions sends the command buffer 
	* @param  NbByte : Number of byte to send
  * @param  pBuffer : pointer to the buffer to send to the M24SR
  * @retval M24SR_STATUS_SUCCESS : the function is succesful
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured. 
  */
int8_t M24SR_SendI2Ccommand ( uc8 NbByte , uc8 *pBuffer )
{
	
	uint16_t M24SR_Timeout = M24SR_I2C_TIMEOUT;
	uint8_t i = 0;
	
	M24SR_I2CStart();
	M24SR_WriteByte( M24SR_I2C_WRITE_ADDR );
	while( !M24SR_CheckAck() )
	{
		if (M24SR_Timeout-- == 0) goto Error;
	}
	for(i=0; i<NbByte; i++)
	{
		M24SR_WriteByte(pBuffer[i]);
		M24SR_Timeout = M24SR_I2C_TIMEOUT;
		while( !M24SR_CheckAck() )
		{
			if (M24SR_Timeout-- == 0) goto Error;
		}
	}
	M24SR_I2CStop();
	return M24SR_STATUS_SUCCESS;

Error:
		M24SR_I2CStop();
	return M24SR_ERROR_I2CTIMEOUT;
}


/**
  * @brief  This functions reads a response of the M24SR device
	* @param  NbByte : Number of byte to read (shall be >= 5)
  * @param  pBuffer : Pointer to the buffer to send to the M24SR
  * @retval M24SR_STATUS_SUCCESS : The function is succesful
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured. 
  */
int8_t M24SR_ReceiveI2Cresponse ( uint8_t NbByte , uint8_t *pBuffer )
{
	uint16_t M24SR_Timeout = M24SR_I2C_TIMEOUT/2;
	uint8_t i = 0;
	
	M24SR_I2CStart();
	M24SR_WriteByte( M24SR_I2C_READ_ADDR );
	while( !M24SR_CheckAck() )
	{
		if (M24SR_Timeout-- == 0) goto Error;
	}
	for(i=0; i<NbByte; i++)
	{
		pBuffer[i] = M24SR_ReadByte();
		M24SR_SendAck();
	}
	M24SR_I2CStop();
	return M24SR_STATUS_SUCCESS;

Error:
	M24SR_I2CStop();
	return M24SR_ERROR_I2CTIMEOUT;
}



/**
  * @brief  This function reads a number of byte through the I2C interface
	* @param  NbByte : Number of byte to read
  * @param  pBuffer : pointer to the buffer to send to the M24SR
  * @retval M24SR_STATUS_SUCCESS : the function is succesful
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured. 
  */
int8_t M24SR_ReceiveI2Cbuffer ( uint8_t NbByte , uint8_t *pBuffer )
{
	uint16_t M24SR_Timeout = M24SR_I2C_TIMEOUT/2;
	uint8_t i = 0;
	
	M24SR_I2CStart();
	M24SR_WriteByte( M24SR_I2C_READ_ADDR );
	while( !M24SR_CheckAck() )
	{
		if (M24SR_Timeout-- == 0) goto Error;
	}
	for(i=0; i<NbByte; i++)
	{
		pBuffer[i] = M24SR_ReadByte();
		M24SR_SendAck();
	}
	M24SR_I2CStop();
	return M24SR_STATUS_SUCCESS;

Error:
	M24SR_I2CStop();
	return M24SR_ERROR_I2CTIMEOUT;
}

/**
  * @brief  This functions returns M24SR_STATUS_SUCCESS when a response is ready
  * @retval M24SR_STATUS_SUCCESS : a response of the M24LR is ready
	* @retval M24SR_ERROR_DEFAULT : the response of the M24LR is not ready
  */
int8_t M24SR_IsAnswerReady ( void )
{
	return 0;
}

/**
  * @brief  This function enable or disable RF communication
	* @param	OnOffChoice: GPO configuration to set
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  */
void M24SR_RFConfig_Hard( uc8 OnOffChoice)
{
	;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/


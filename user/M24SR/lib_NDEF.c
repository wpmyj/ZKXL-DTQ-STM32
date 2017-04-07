/**
  ******************************************************************************
  * @file    lib_NDEF.c
  * @author  MMY Application Team
  * @version V1.0.1
  * @date    06-December-2016
  * @brief   This file help to manage NDEF file, to parse and identify them.
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
#include "lib_NDEF.h"

/** @addtogroup NFC_libraries
 * 	@{
 *	@brief  <b>This is the library used to manage the content of the TAG (data)
 *          But also the specific feature of the tag, for instance
 *          password, gpo... </b>
 */
 
 
/**
  * @brief  This fonction read the NDEF content of the TAG
	* @param	pNDEF : pointer on the buffer to store NDEF data
  * @retval SUCCESS : NDEF file data retrieve and store in the buffer
	* @retval ERROR : not able to read NDEF
  */
uint16_t NDEF_ReadNDEF( uint8_t* pNDEF)
{
	uint16_t status = ERROR;
	uint16_t NDEF_Size = 0;
	
	status = M24SR_ReadData( 0 , 2 , pNDEF);
	
	if( status == M24SR_ACTION_COMPLETED)
	{
		NDEF_Size = (uint16_t) (*pNDEF << 8);
		NDEF_Size = NDEF_Size | (uint16_t) (*++pNDEF );
		
		status = M24SR_ReadData( 0 , NDEF_Size+2 , --pNDEF);
	}
	
	if( status == M24SR_ACTION_COMPLETED)
		return SUCCESS;
	else
		return ERROR;
	
}

/**
  * @brief  This fonction write the NDEF in the TAG
	* @param	pNDEF : pointer on the buffer containing the NDEF data
  * @retval SUCCESS : NDEF file data written in the tag
	* @retval ERROR : not able to store NDEF in tag
  */
uint16_t NDEF_WriteNDEF( uint8_t *pNDEF)
{
	uint16_t status = ERROR;
	uint16_t NDEF_Size = 0;
	
	NDEF_Size = (uint16_t) (*pNDEF << 8);
	NDEF_Size = NDEF_Size | (uint16_t) (*++pNDEF );

	status = M24SR_WriteData( 0 , NDEF_Size+2 , --pNDEF);
	
	if( status == M24SR_ACTION_COMPLETED)
		return SUCCESS;
	else
		return ERROR;
	
}

/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/


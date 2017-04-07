/**
  ******************************************************************************
  * @file    lib_NDEF.h
  * @author  MMY Application Team
  * @version V1.0.1
  * @date    06-December-2016
  * @brief   This file help to manage NDEF file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LIB_NDEF_H
#define __LIB_NDEF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "lib_M24SR.h"	 
	 
#ifndef errorchk
#define errorchk(fCall) if (status = (fCall), status != M24SR_ACTION_COMPLETED) \
	{goto Error;} else
#endif
	

uint16_t NDEF_ReadNDEF( uint8_t *pNDEF);
uint16_t NDEF_WriteNDEF( uint8_t *pNDEF);

#ifdef __cplusplus
}
#endif
#endif /* __LIB_NDEF_H */


/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/

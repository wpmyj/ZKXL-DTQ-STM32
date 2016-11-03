/**
  ******************************************************************************
  * @file   	rc500_handle_layer.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	all variables called by other functions
  ******************************************************************************
  */
  
#ifndef _RC500_HANDLE_LAYER_H
#define _RC500_HANDLE_LAYER_H

#include <stdint.h> 
#include <stdbool.h>

void ComPcdAntenna(void);
uint8_t ComRequestA(void);
uint8_t ComAnticoll(void);
uint8_t ComAnticol2(void);
void ComSelect1(void);
void ComSelect2(void);
void ComHlta(void);
void ComAuthentication(void);
void ComM1Read(void);
void ComM1Write(void);
void ComM1Initval(void);
void ComM1Readval(void);
void ComM1Decrement(void);
void ComM1Increment(void);
void AnswerErr(int faultcode); 
void AnswerOk(uint8_t *answerdata,unsigned int answernum);
void AnswerCommandOk(void);
void DoubleModeCtrRC500(void);
void LedGreen(void);
void LedRed(void);
void ComM1BakValue(void);
void ComUL_PcdAnticoll(void);
void ComUL_PcdWrite(void);
void ComTypeARst(void);
void ComTCLCOS(void);
void ComCL_Deselect(void);
void ComSelectApplication(void);
void ComSendInterrupt (void);
uint8_t ForceReadCCfile (void);
void ComReadCCfile (void);
void ComReadSystemfile (void);
void ComReadNDEFfile (void);
void ComWriteNDEFfile (uint16_t NbByteToWrite, uint8_t *pDataToWrite);
void ComDeselect (void);

uint8_t FindICCard(void);
uint8_t SelectApplication(void);
uint8_t SendInterrupt(void);
uint8_t ReadCCfile(uint8_t *CCfileData, uint8_t *CCfileLen);
uint8_t ReadSystemfile(uint8_t *Systemfile_Data, uint8_t *Systemfile_len);
uint8_t ReadNDEFfile(uint8_t *NDEFfile_Data, uint16_t *NDEFfile_len);
uint8_t WriteNDEFfile(uint8_t *pDataToWrite);
uint8_t Deselect(void);
void write_RF_config(uint8_t upos,uint8_t ndef_xor);

#endif //_RC500_HANDLE_LAYER_H



/**
  ******************************************************************************
  * @file   	mfrc500.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	include all other header files
  ******************************************************************************
  */

#ifndef _MFRC500_H_
#define _MFRC500_H_

#include <stdint.h>
#include "main.h"
/*****************************************************************************
*函数原型
*****************************************************************************/
uint8_t ReadRC(uint8_t Address);
void 	WriteRC(uint16_t Address,uint16_t value);
void 	PcdAntennaOn(void);                                        	  	 	 /*开启RC500天线发射*/
void 	PcdAntennaOff(void);                                       	  	 	 /*关闭RC500天线发射*/
uint8_t PcdReset(void);                                            	  		 /*复位并初始化RC500*/

uint8_t PcdRequest(uint8_t req_code,uint8_t *pTagType);               		 /*寻卡*/
uint8_t PcdAnticoll(uint8_t antiFlag, uint8_t *snr);                         /*防冲撞*/
uint8_t PcdSelect(uint8_t *snr);                                      		 /*选定一张卡*/
uint8_t PcdSelect1(uint8_t *snr, uint8_t *res, uint8_t *len);				 /*选定一张卡*/
uint8_t PcdSelect2(uint8_t *snr, uint8_t *res, uint8_t *len);				 /*选定一张卡*/
uint8_t PcdHalt(void);                                             	  		 /*卡休眠*/
uint8_t ChangeCodeKey(uint8_t *uncoded,uint8_t *coded);               		 /*转换密钥格式*/
uint8_t PcdAuthKey(uint8_t *coded);                                   		 /*传送密钥*/
uint8_t PcdAuthState(uint8_t auth_mode,uint8_t block,uint8_t *snr);   		 /*验证密钥*/
uint8_t PcdRead(uint8_t addr,uint8_t *readdata);                      		 /*读块*/
uint8_t PcdWrite(uint8_t addr,uint8_t *writedata);                    		 /*写块*/
uint8_t PcdValue(uint8_t dd_mode,uint8_t addr,uint8_t *value);        		 /*增（减）钱包值*/
uint8_t PcdRestore(uint8_t addr);                                     		 /*调钱包值*/
uint8_t PcdTransfer(uint8_t addr);                                    		 /*备份钱包值*/

uint8_t PcdReadE2(uint32_t startaddr,uint8_t length,uint8_t *readdata);		 /*读RC500-EEPROM数据*/
uint8_t PcdWriteE2(uint32_t startaddr,uint8_t length,uint8_t *writedata);	 /*写数据到RC500-EEPROM*/
uint8_t PcdConfigRestore(void);                                    	  		 /*恢复RC500出厂设置*/
uint8_t PcdLoadConfig(uint32_t startaddr);                             		 /*调RC500-EEPROM中自设定的设置*/
uint8_t UL_PcdAnticoll(uint8_t *pSnr);
uint8_t UL_PcdWrite(uint8_t addr,uint8_t *pData) ;
uint8_t MifareProRst(uint8_t param,uint8_t *pLen,uint8_t *pData)  ;
uint8_t ISO14443CLCom(uint8_t CID, uint8_t *pCommand, uint8_t ComLg, uint8_t *pAnswer, uint8_t *pAnsLg)  ;
uint8_t CL_Deselect(uint8_t CID) ;
//extern uint8_t ComM1Initval(u8 block);									 /*初始化钱包*/

//新增ms24sr命令
uint8_t PcdRATS(uint8_t *res, uint8_t *len);
uint8_t PcdPPS(uint8_t *res, uint8_t *len);				   
uint8_t PcdSelectApplication(uint8_t *res, uint8_t *len);
				   
uint8_t PcdSelectCCfile(uint8_t *res, uint8_t *len);
uint8_t PcdReadCCfileLength(uint8_t *res, uint8_t *len);
uint8_t PcdReadCCfile(uint16_t Offset , uint8_t NbByteToRead, uint8_t *res, uint8_t *len);
				   
uint8_t PcdSelectNDEFfile(uint8_t *res, uint8_t *len);
uint8_t PcdReadNDEFfileLength(uint8_t *res, uint8_t *len);
uint8_t PcdReadNDEFfile(uint16_t Offset , uint8_t NbByteToRead, uint8_t *res, uint8_t *len);
uint8_t PcdWriteNDEFfile(uint16_t Offset , uint8_t NbByteToWrite , uint8_t *pDataToWrite, uint8_t *res, uint8_t *len);
				   
uint8_t PcdSelectSystemfile(uint8_t *res, uint8_t *len);
uint8_t PcdReadSystemfileLength(uint8_t *res, uint8_t *len);
uint8_t PcdReadSystemfile(uint16_t Offset , uint8_t NbByteToRead, uint8_t *res, uint8_t *len);
				   
uint8_t PcdSendInterrupt (uint8_t *res, uint8_t *len);	
uint8_t PcdDeselect (uint8_t *res, uint8_t *len);
uint8_t PcdFWTExtension (uint8_t FWTbyte, uint8_t *res, uint8_t *len);

/*****************************************************************************
*MFIRE卡命令字
*****************************************************************************/
#define PICC_REQIDL           				0x26                              /*寻天线区内未进入休眠状态的卡*/
#define PICC_REQALL           				0x52                              /*寻天线区内全部卡*/
#define PICC_ANTICOLL1        				0x93                              /*防冲撞*/
#define PICC_ANTICOLL2        				0x95               				  //防冲撞
#define PICC_AUTHENT1A        				0x60                              /*验证A密钥*/
#define PICC_AUTHENT1B        				0x61                              /*验证B密钥*/
#define PICC_READ             				0x30                              /*读块*/
#define PICC_WRITE            				0xA0                              /*写块*/
#define PICC_DECREMENT        				0xC0                              /*扣款*/
#define PICC_INCREMENT        				0xC1                              /*充值*/
#define PICC_RESTORE          				0xC2                              /*调钱包到缓冲区*/
#define PICC_TRANSFER         				0xB0                              /*保存缓冲区中数据*/
#define PICC_HALT             				0x50                              /*休眠*/
#define PICC_RESET            				0xE0               				  //复位
/*****************************************************************************
*RC500命令字
*****************************************************************************/
#define PCD_IDLE              				0x00                              /*取消当前命令*/
#define PCD_WRITEE2           				0x01                              /*写EEPROM*/
#define PCD_READE2            				0x03                              /*读EEPROM */
#define PCD_LOADCONFIG        				0x07                              /*调EEPROM中保存的RC500设置*/
#define PCD_LOADKEYE2         				0x0B                              /*将EEPROM中保存的密钥调入缓存*/
#define PCD_AUTHENT1          				0x0C                              /*验证密钥第一步*/
#define PCD_AUTHENT2          				0x14                              /*验证密钥第二步*/
#define PCD_RECEIVE           				0x16                              /*接收数据*/
#define PCD_LOADKEY           				0x19                              /*传送密钥*/
#define PCD_TRANSMIT          				0x1A                              /*发送数据*/
#define PCD_TRANSCEIVE        				0x1E                              /*发送并接收数据*/
#define PCD_RESETPHASE        				0x3F                              /*复位*/
#define PCD_CALCCRC           				0x12                              /*CRC计算*/
/*****************************************************************************
*RC500 FIFO长度定义
*****************************************************************************/
#define DEF_FIFO_LENGTH       				64                                /*FIFO size=64byte*/
/*****************************************************************************
*RC500寄存器定义
*****************************************************************************/
#define RegPage               				0x00                              //!< Page Select Register
#define RegCommand            				0x01                              //!< Command Register
#define RegFIFOData           				0x02                              //!< FiFo Register
#define RegPrimaryStatus      				0x03                              //!< Modem State/IRQ/ERR/LoHiAlert Reg
#define RegFIFOLength         				0x04                              //!< Buffer length Register
#define RegSecondaryStatus    				0x05                              //!< diverse status flags
#define RegInterruptEn        				0x06                              //!< IRQ enable Register
#define RegInterruptRq        				0x07                              //!< IRQ bits Register

#define RegControl            				0x09                              //!< processor control
#define RegErrorFlag          				0x0A                              /*!< error flags showing the error status of the last command executed */
#define RegCollPos            				0x0B                              /*!< bit position of the first bit collision detected on the RF-interface */
#define RegTimerValue         				0x0C                              //!< preload value of the timer
#define RegCRCResultLSB       				0x0D                              //!< LSB of the CRC Coprocessor register
#define RegCRCResultMSB       				0x0E                              //!< MSB of the CRC Coprocessor register
#define RegBitFraming         				0x0F                              //!< Adjustments for bit oriented frames

#define RegTxControl          				0x11                              //!< controls the logical behaviour of the antenna driver pins TX1 and TX2
#define RegCwConductance      				0x12                              /*!< selects the conductance of the antenna driver pins TX1 and TX2 */
#define RFU13                 				0x13                              //!< RFU
#define RegCoderControl       				0x14                              //!< selects coder rate
#define RegModWidth           				0x15                              /*!< selects the width of the modulation pulse */
#define RFU16                 				0x16                              //!< RFU
#define RFU17                 				0x17                              //!< RFU

#define RegRxControl1         				0x19                              //!< controls receiver behaviour
#define RegDecoderControl     				0x1A                              //!< controls decoder behaviour
#define RegBitPhase           				0x1B                              /*!< selets the bit phase between transmitter and receiver clock */
#define RegRxThreshold        				0x1C                              //!< selects thresholds for the bit decoder
#define RFU1D                 				0x1D                              //!< RFU
#define RegRxControl2         				0x1E                              /*!< controls decoder behaviour and defines the input source for the receiver */
#define RegClockQControl      				0x1F                              /*!< controls clock generation for the 90?phase shifted Q-channel clock */

#define RegRxWait             				0x21                              /*!< selects the time interval after transmission, before receiver starts */
#define RegChannelRedundancy  				0x22                              /*!< selects the kind and mode of checking the data integrity on the RF-channel */
#define RegCRCPresetLSB       				0x23                              /*!< LSB of the pre-set value for the CRC register */
#define RegCRCPresetMSB       				0x24                              /*!< MSB of the pre-set value for the CRC register */
#define RFU25                 				0x25                              //!< RFU
#define RegMfOutSelect        				0x26                              /*!< selects internal signal applied to pin MfOut */
#define RFU27                 				0x27                              //!< RFU

#define RegFIFOLevel          				0x29                              /*!< Defines level for FIFO over- and underflow warning */
#define RegTimerClock         				0x2A                              //!< selects the divider for the timer clock
#define RegTimerControl       				0x2B                              /*!< selects start and stop conditions for the timer */
#define RegTimerReload        				0x2C                              //!< defines the pre-set value for the timer
#define RegIRqPinConfig       				0x2D                              //!< configures the output stage of pin IRq
#define RFU2E                 				0x2E                              //!< RFU
#define RFU2F                 				0x2F                              //!< RFU

#define RFU31                 				0x31                              //!< RFU
#define RFU32                 				0x32                              //!< RFU
#define RFU33                 				0x33                              //!< RFU
#define RFU34                 				0x34                              //!< RFU
#define RFU35                 				0x35                              //!< RFU
#define RFU36                 				0x36                              //!< RFU
#define RFU37                 				0x37                              //!< RFU

#define RFU39                 				0x39                              //!< RFU
#define RegTestAnaSelect      				0x3A                              //!< selects analog test mode
#define RFU3B                 				0x3B                              //!< RFU
#define RFU3C                 				0x3C                              //!< RFU
#define RegTestDigiSelect     				0x3D                              //!< selects digital test mode
#define RFU3E                 				0x3E                              //!< RFU
#define RegTestDigiAccess     				0x3F

/*****************************************************************************
*和RC500通讯时返回的错误代码
*****************************************************************************/
#define MI_OK                 				(0)								//
#define MI_CHK_OK             				(0)			
#define MI_NOTAGERR           				(1)
#define MI_CHK_FAILED         				(1)
#define MI_CRCERR             				(2)
#define MI_CHK_COMPERR        				(2)
#define MI_EMPTY              				(3)
#define MI_AUTHERR            				(4)
#define MI_PARITYERR          				(5)
#define MI_CODEERR            				(6)
#define MI_SERNRERR           				(8)
#define MI_KEYERR             				(9)
#define MI_NOTAUTHERR         				(10)
#define MI_BITCOUNTERR        				(11)
#define MI_BYTECOUNTERR       				(12)
#define MI_IDLE               				(13)
#define MI_TRANSERR           				(14)
#define MI_WRITEERR           				(15)
#define MI_INCRERR            				(16)
#define MI_DECRERR            				(17)
#define MI_READERR            				(18)
#define MI_OVFLERR            				(19)
#define MI_POLLING            				(20)
#define MI_FRAMINGERR         				(21)
#define MI_ACCESSERR          				(22)
#define MI_UNKNOWN_COMMAND    				(23)
#define MI_COLLERR            				(24)
#define MI_RESETERR           				(25)
#define MI_INITERR            				(25)
#define MI_INTERFACEERR       				(26)
#define MI_ACCESSTIMEOUT      				(27)
#define MI_NOBITWISEANTICOLL  				(28)
#define MI_QUIT               				(30)
#define MI_RECBUF_OVERFLOW    				(50)
#define MI_SENDBYTENR         				(51)
#define MI_SENDBUF_OVERFLOW   				(53)
#define MI_BAUDRATE_NOT_SUPPORTED       	(54)
#define MI_SAME_BAUDRATE_REQUIRED       	(55)
#define MI_WRONG_PARAMETER_VALUE        	(60)
#define MI_BREAK                        	(99)
#define MI_NY_IMPLEMENTED               	(100)
#define MI_NO_MFRC                      	(101)
#define MI_MFRC_NOTAUTH                 	(102)
#define MI_WRONG_DES_MODE               	(103)
#define MI_HOST_AUTH_FAILED             	(104)
#define MI_WRONG_LOAD_MODE              	(106)
#define MI_WRONG_DESKEY                 	(107)
#define MI_MKLOAD_FAILED                	(108)
#define MI_FIFOERR                      	(109)
#define MI_WRONG_ADDR                   	(110)
#define MI_DESKEYLOAD_FAILED            	(111)
#define MI_WRONG_SEL_CNT                	(114)
#define MI_WRONG_TEST_MODE              	(117)
#define MI_TEST_FAILED                  	(118)
#define MI_TOC_ERROR                    	(119)
#define MI_COMM_ABORT                   	(120)
#define MI_INVALID_BASE                 	(121)
#define MI_MFRC_RESET                   	(122)
#define MI_WRONG_VALUE                  	(123)
#define MI_VALERR                       	(124)
#define MI_COM_ERR                      	(125)

#endif /*_MFRC500_H_*/






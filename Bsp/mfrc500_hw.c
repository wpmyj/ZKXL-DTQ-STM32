/**
  ******************************************************************************
  * @file   	mfrc500_hw.c
  * @author  	Sam.wu
  * @version 	V1.0.0.0
  * @date   	2016.12.29
  * @brief   	hal function for nrf moulde
  ******************************************************************************
  */
#include "main.h"
#include "mfrc500.h"

#ifdef 	ENABLE_MF1702NL_DEBUG_LOG

typedef struct
{
	uint8_t addr;
	uint8_t WR_OR_RD;
	uint8_t wr_value;
	uint8_t rd_value;
}Rc500RegDebug_Typedef;


Rc500RegDebug_Typedef RegData[150];
uint8_t RegCount = 0;
uint8_t StartFlg = 0;
uint8_t PrintfFlg = 0;

#endif

void MRC500_DEBUG_START(uint8_t *str)
{
#ifdef 	ENABLE_MF1702NL_DEBUG_LOG
	DEBUG_MF1702NL_LOG("%s",str);
	if(PrintfFlg == 0)
	{
		StartFlg = 1;
		RegCount = 0;
	}
#endif
}

void MRC500_DEBUG_END(void)
{
#ifdef 	ENABLE_MF1702NL_DEBUG_LOG
	StartFlg = 0;
	PrintfFlg = 1;
	if(PrintfFlg == 1)
	{
		uint8_t index = 0;
		for( index = 0; index<RegCount; index++ )
		{
			DEBUG_MF1702NL_LOG("index = %4d Addr = %4x ,OP = %d WR = %4x ,RD = %4x \r\n",
			index,RegData[index].addr,
			RegData[index].WR_OR_RD,RegData[index].wr_value,RegData[index].rd_value);
		}
		PrintfFlg = 0;
	}
#endif
}

/*******************************************************************************
  * @brief  初始化mfrc500接口寄存器
  * @param  None
  * @retval 读出的值
  * @note 	None		  
*******************************************************************************/
void mfrc500_gpio_init(void)
{
#ifdef ZL_RP551_MAIN_E
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	GPIO_InitStructure.GPIO_Pin = MFRC500_PD_Pin|MFRC500_ALE_Pin|MFRC500_CS_Pin|MFRC500_IRQ_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC500_CTL_PORT, &GPIO_InitStructure);
	GPIO_WriteBit(MFRC500_CTL_PORT, MFRC500_PD_Pin, Bit_RESET);
	GPIO_WriteBit(MFRC500_CTL_PORT, MFRC500_ALE_Pin, Bit_RESET);
	GPIO_WriteBit(MFRC500_CTL_PORT, MFRC500_CS_Pin, Bit_SET);
	GPIO_WriteBit(MFRC500_CTL_PORT, MFRC500_IRQ_Pin, Bit_RESET);

	GPIO_InitStructure.GPIO_Pin = MFRC500_WR_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC500_WR_PORT, &GPIO_InitStructure);
	GPIO_WriteBit(MFRC500_WR_PORT, MFRC500_WR_Pin, Bit_SET);

	GPIO_InitStructure.GPIO_Pin = MFRC500_RD_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC500_RD_PORT, &GPIO_InitStructure);
	GPIO_WriteBit(MFRC500_RD_PORT, MFRC500_RD_Pin, Bit_SET);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10|\
																GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_13|\
																GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC500_DATA_Port, &GPIO_InitStructure);
	GPIO_WriteBit(MFRC500_DATA_Port, GPIO_InitStructure.GPIO_Pin, Bit_RESET);
#endif

#if defined (ZL_RP551_MAIN_F) && defined (ZL_RP551_MAIN_H)
GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC , ENABLE);

  /* Configure MFRC500 PIN */
	GPIO_InitStructure.GPIO_Pin = MFRC500_PD_Pin | MFRC500_ALE_Pin | MFRC500_CS_Pin | MFRC500_IRQ_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC500_CTL_PORT, &GPIO_InitStructure);
	GPIO_WriteBit(MFRC500_CTL_PORT, MFRC500_PD_Pin, Bit_RESET);
	GPIO_WriteBit(MFRC500_CTL_PORT, MFRC500_ALE_Pin, Bit_RESET);
	GPIO_WriteBit(MFRC500_CTL_PORT, MFRC500_CS_Pin, Bit_SET);
	GPIO_WriteBit(MFRC500_CTL_PORT, MFRC500_IRQ_Pin, Bit_RESET);

	GPIO_InitStructure.GPIO_Pin = MFRC500_WR_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC500_WR_PORT, &GPIO_InitStructure);
	GPIO_WriteBit(MFRC500_WR_PORT, MFRC500_WR_Pin, Bit_SET);

	GPIO_InitStructure.GPIO_Pin = MFRC500_RD_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MFRC500_RD_PORT, &GPIO_InitStructure);
	GPIO_WriteBit(MFRC500_RD_PORT, MFRC500_RD_Pin, Bit_SET);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_WriteBit(GPIOB, GPIO_InitStructure.GPIO_Pin, Bit_RESET);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11 |GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_WriteBit(GPIOC, GPIO_InitStructure.GPIO_Pin, Bit_RESET);
#endif
}
/*******************************************************************************
  * @brief  读RC500寄存器
  * @param  Address=寄存器地址
  * @retval 读出的值
  * @note 	None		  
*******************************************************************************/
uint8_t ReadRC(uint8_t Address)
{
	u8 value;

	ALE(Bit_SET);  		 //ALE = 1
	CS(Bit_SET);		   //CS = 1
  RD(Bit_SET);		   //RD = 1
	WR(Bit_SET);		   //WR = 1
  WR_DATA( Address );//Address = PC[10:12] + PB[8:12]
	ALE(Bit_SET);		   //ALE = 1	
	Delay10us(2);	
	Delay3us();
	Delay3us();
	ALE(Bit_RESET);		 //ALE = 0
	MFRC500_DATA_IN();	 
	
	CS(Bit_RESET);		 //CS = 0
	RD(Bit_RESET);		 //RD = 0
	Delay3us();
	value = RD_DATA(); //value = PC[10:12] + PB[8:12]
	Delay3us();
	RD(Bit_SET);		   //RD = 1	
	CS(Bit_SET);		   //CS = 1	
	Delay10us(1);
	MFRC500_DATA_OUT();
#ifdef 	ENABLE_MF1702NL_DEBUG_LOG
	if(StartFlg == 1)
	{
		RegData[RegCount].addr = Address;
		RegData[RegCount].WR_OR_RD = 1;
		RegData[RegCount].rd_value = value;
		RegData[RegCount].wr_value = 0;
		RegCount++;
	}
#endif
	return value;
}
/*******************************************************************************
  * @brief  写RC500寄存器
  * @param  Address=寄存器地址
		      	value=要写入的值
  * @retval None
  * @note 	None		  
*******************************************************************************/
void WriteRC(uint16_t Address,uint16_t value)
{
	ALE(Bit_SET);  			//ALE = 1
	WR_DATA( Address ); //PC[10:12] + PB[8:12]
	Delay10us(2);
	ALE(Bit_RESET);			//ALE = 0
	Delay10us(2);
	CS(Bit_RESET);			//CS = 0
	WR_DATA( value );   //PC[10:12] + PB[8:12]
	WR(Bit_RESET);      //WR = 0
	Delay10us(2);
	WR(Bit_SET);		    //WR = 1
	CS(Bit_SET);		    //CS = 1
#ifdef 	ENABLE_MF1702NL_DEBUG_LOG
	if(StartFlg == 1)
	{
		RegData[RegCount].addr = Address;
		RegData[RegCount].WR_OR_RD = 0;
		RegData[RegCount].wr_value = value;
		RegData[RegCount].rd_value = 0;
		RegCount++;
	}
#endif
}

/*******************************************************************************
  * @brief  调试RC500寄存器
  * @param  Address=寄存器地址
			      value=要写入的值
  * @retval None
  * @note 	None		  
*******************************************************************************/
void DEBUGRC(uint16_t Address,uint16_t value)
{
	uint8_t read_value;
	WriteRC(Address,value);
	read_value = ReadRC(Address);
	printf("Addr = %4x ,WR = %4x ,RD = %4x \r\n",Address,value,read_value);
}

/*******************************************************************************
  * @brief  置RC500寄存器位
  * @param  reg=寄存器地址
  * @param  mask=置位值
  * @retval None
  * @note 	None	  
*******************************************************************************/
void SetBitMask(uint8_t reg,uint8_t mask)
{
   uint8_t tmp=0x0;
   tmp=ReadRC(reg);
   WriteRC(reg,tmp|mask);
}

/*******************************************************************************
  * @brief  清RC500寄存器位
  * @param  reg=寄存器地址
  * @param  mask=清位值
  * @retval None
  * @note 	None	  
*******************************************************************************/
void ClearBitMask(uint8_t reg,uint8_t mask)
{
   uint8_t tmp=0x0;
   tmp = ReadRC(reg);
   WriteRC(reg,tmp & ~mask);
}

/*******************************************************************************
  * @brief  初始化RC500
  * @param  None
  * @retval status=错误状态
  * @note 	注意:RC500上电后应延时500ms才能可靠初始化  
*******************************************************************************/
uint8_t mfrc500_init(void)
{
	uint8_t status=MI_OK;
	uint16_t  i=0x2000;

  /* 初始化GPIO */
	mfrc500_gpio_init();

	PD(Bit_RESET);
	Delay10us(2500);
	PD(Bit_SET);
	Delay10us(1000);
	PD(Bit_RESET);
	Delay10us(300);
//MRC500_DEBUG_START("mfrc500_init\r\n");
	/* 等待接口检测完成 */
	while ((ReadRC(RegCommand) & 0x3F) && i--)
	{
	}

	if(!i)
	{
			status = MI_NOTAGERR;
	}

	if (status == MI_OK)
	{
		/* 设置寄存器地址有效范围：bit3~bit5 */
		WriteRC(RegPage,0x80);
		if (ReadRC(RegCommand) != 0x00)
		{                           
				status = MI_NOTAGERR;
		}
		else
		{
			/* 设置寄存器地址有效范围：bit0~bit7 */
			WriteRC(RegPage,0x00);
		}
	}

	if(status==MI_OK)
	{
		WriteRC(RegClockQControl,0x00);
		Delay10us(20);
		WriteRC(RegClockQControl,0x40);
		Delay10us(20);
		ClearBitMask(RegClockQControl,0x40);
		WriteRC(RegBitPhase,0xad);
		WriteRC(RegRxThreshold,0xff);
		WriteRC(RegRxControl2,0x41);
		WriteRC(RegFIFOLevel,0x1a);
		WriteRC(RegTimerControl,0x02);
		WriteRC(RegIRqPinConfig,0x02);
		WriteRC(RegTxControl,0x5b);		   //开天线
		WriteRC(RegCwConductance,0x3f) ;
		PcdAntennaOff();
	}
//MRC500_DEBUG_END();
//MRC500_DEBUG_START("mfrc500_init check\r\n");
//{
//	uint8_t i = 0;
//	for(i= 0x10; i<0x30;i++)
//		ReadRC(i);
//}
//MRC500_DEBUG_END();
	return status;
}


/*******************************************************************************
  * @brief  设置RC500定时
  * @param  tmolength=设置值
  * @retval None
  * @note 	None	  
*******************************************************************************/
void PcdSetTmo(uint8_t tmoLength)
{
   switch(tmoLength)
   {  
      case 1:                             // short timeout (1,0 ms)
         WriteRC(RegTimerClock,0x07);     // TAutoRestart=0,TPrescale=128
         WriteRC(RegTimerReload,0x6a);    // TReloadVal = 'h6a =106(dec)
         break;
      case 2:                             // medium timeout (1,5 ms)
         WriteRC(RegTimerClock,0x07);     // TAutoRestart=0,TPrescale=128
         WriteRC(RegTimerReload,0xa0);    // TReloadVal = 'ha0 =160(dec)
         break;
      case 3:                              // long timeout (6 ms)
         WriteRC(RegTimerClock,0x09);      // TAutoRestart=0,TPrescale=4*128
         WriteRC(RegTimerReload,0xa0);     // TReloadVal = 'ha0 =160(dec)
         break;
      case 4:                              // long timeout (9.6 ms)
         WriteRC(RegTimerClock,0x09);      // TAutoRestart=0,TPrescale=4*128
         WriteRC(RegTimerReload,0xff);     // TReloadVal = 'ff =255(dec)
         break;
      default:                             // short timeout (1,0 ms)   time：1ms = (2^7 *106)/13560000
         WriteRC(RegTimerClock,0x07);      // TAutoRestart=0,TPrescale=128
         WriteRC(RegTimerReload,tmoLength);// TReloadVal = tmoLength
         break;
   }
   WriteRC(RegTimerControl,0x06);
}

/*******************************************************************************
  * @brief  和RC500通讯
  * @param  pi->MfCommand=RC500命令字
  * @param  pi->MfLength=发送的数据长度
  *	@param	pi->MfData[]=发送数据
  * @retval pi->MfLength=接收的数据长度(位)
  *	@retval	pi->MfData[]=接收到的数据
  * @retval len = 接收的数据长度（字节）
  * @retval status=错误状态
  * @note 	None	  
*******************************************************************************/
uint8_t PcdComTransceive(void *p)
{  
	struct TranSciveBuffer{	uint8_t MfCommand;
							uint8_t MfLength;
							uint8_t MfData[BUF_LEN];};
	struct TranSciveBuffer *pi = (struct TranSciveBuffer *)p;
	uint8_t recebyte=0;
	uint8_t status;
	uint8_t irqEn=0x00;
	uint8_t waitFor=0x00;
	uint8_t lastBits;
	uint8_t n;
	uint32_t i;

	switch(pi->MfCommand)
	{
      case PCD_IDLE:
         irqEn = 0x00;
         waitFor = 0x00;
         break;
      case PCD_WRITEE2:
         irqEn = 0x11;
         waitFor = 0x10;
         break;
      case PCD_READE2:
         irqEn = 0x07;
         waitFor = 0x04;
         recebyte=1;
         break;
      case PCD_LOADCONFIG:
      case PCD_LOADKEYE2:
      case PCD_AUTHENT1:
         irqEn = 0x05;
         waitFor = 0x04;
         break;
      case PCD_CALCCRC:
         irqEn = 0x11;
         waitFor = 0x10;
         break;
      case PCD_AUTHENT2:
         irqEn = 0x04;
         waitFor = 0x04;
         break;
      case PCD_RECEIVE:
         irqEn = 0x06;
         waitFor = 0x04;
         recebyte=1;
         break;
      case PCD_LOADKEY:
         irqEn = 0x05;
         waitFor = 0x04;
         break;
      case PCD_TRANSMIT:
         irqEn = 0x05;
         waitFor = 0x04;
         break;
      case PCD_TRANSCEIVE:
         irqEn = 0x3D;
         waitFor = 0x04;
         recebyte=1;
         break;
      default:
         pi->MfCommand=MI_UNKNOWN_COMMAND;
         break;
   }
   if(pi->MfCommand!=MI_UNKNOWN_COMMAND)
   {
      WriteRC(RegPage,0x00);
      WriteRC(RegInterruptEn,0x7F);
      WriteRC(RegInterruptRq,0x7F);
      WriteRC(RegCommand,PCD_IDLE);
      SetBitMask(RegControl,0x01);										//Flush FIFO
      WriteRC(RegInterruptEn,irqEn|0x80);
      for(i=0;i<pi->MfLength;i++)
      {
         WriteRC(RegFIFOData,pi->MfData[i]);
      }
      WriteRC(RegCommand,pi->MfCommand);
      i=0x2000;
      do
      {
         n=ReadRC(RegInterruptRq);
         i--;
      }
      while((i!=0)&&!(n&irqEn&0x20)&&!(n&waitFor));
      status=MI_COM_ERR;
      if((i!=0)&&!(n&irqEn&0x20))
      {
			 if(!(ReadRC(RegErrorFlag)&0x17))
			 {
				status=MI_OK;
				if(recebyte)
				{
					n=ReadRC(RegFIFOLength);
					lastBits=ReadRC(RegSecondaryStatus)&0x07;
					if(lastBits)
					{
						 pi->MfLength=(n-1)*8+lastBits;
					}
					else
					{
						 pi->MfLength=n*8;
					}
	
					if(n==0)
					{
						 n=1;
					}
					for(i=0;i<n;i++)
					{
								pi->MfData[i]=ReadRC(RegFIFOData);
					}
					memcpy(p, pi, sizeof(struct TranSciveBuffer));
				}
			 }
      }
      else if(n&irqEn&0x20)
      {
        status=MI_NOTAGERR;
      }
      else
      {
        status=MI_COM_ERR;
      }
      WriteRC(RegInterruptEn,0x7F);
      WriteRC(RegInterruptRq,0x7F);
   }
   return status;
}
/*******************************************************************************
  * @brief  关闭RC500天线发射和RC500通讯
  * @param  None
  * @retval None
  * @note 	None	  
*******************************************************************************/
void PcdAntennaOff(void)
{
    ClearBitMask(RegTxControl,0x03);
}

/*******************************************************************************
  * @brief  启动RC500天线发射
  * @param  None
  * @retval None
  * @note 	每次启动或关闭天险发射之间应至少有1ms的间隔,
  *         执行完初始化RC500函数后天线已启动和RC500通讯	  
*******************************************************************************/
void PcdAntennaOn(void)
{
    SetBitMask(RegTxControl,0x03);
}

/**************************************END OF FILE****************************/

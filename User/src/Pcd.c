
//*************************************
//*************************************
/* Copyright (c) 2011 ShenZhen zhongkexunlian All Rights Reserved.
 *  
 * $vision: 1.0 $
 */ 
//author:Simon_huang
//date:2011.07
//*************************************
//*************************************
#include "RC500.h"
//#include "DoubleModeCtrRC500.h"
#include "stm32f10x_it.h"
#include "hal_uart.h"
#include "main.h"
#include "mcu_config.h"

uint8_t test_tianxian0;
extern uint8_t g_cardType[];
bool g_bIblock ;

/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//       pTagType[OUT]//：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(uchar req_code,uchar *pTagType)
{
   char status;
   uchar ret_len = 0;
   struct TranSciveBuffer {uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[1];
                                }MfComData;
   struct TranSciveBuffer *pi;

   pi=&MfComData;
   PcdSetTmo(106);
   WriteRC(RegChannelRedundancy,0x03);
   ClearBitMask(RegControl,0x08);
   WriteRC(RegBitFraming,0x07);
   MfComData.MfCommand=PCD_TRANSCEIVE;
   MfComData.MfLength=1;
   MfComData.MfData[0]=req_code;
   status=PcdComTransceive(pi, &ret_len);
 
  if ((status == MI_OK) && (MfComData.MfLength == 0x10))
   {    
       *pTagType     = MfComData.MfData[0];
       *(pTagType+1) = MfComData.MfData[1];
   }
   else
   {   status = MI_BITCOUNTERR;   }
   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: Snr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdAnticoll(uchar antiFlag, uchar *snr)
{
    uchar i;
	uchar ret_len = 0;
    uchar snr_check=0;
    char status=MI_OK;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[10];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegDecoderControl,0x28);
    ClearBitMask(RegControl,0x08);
    WriteRC(RegChannelRedundancy,0x03);

    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
	if(PICC_ANTICOLL1 == antiFlag)
		MfComData.MfData[0]=PICC_ANTICOLL1;
	else if(PICC_ANTICOLL2 == antiFlag)
		MfComData.MfData[0]=PICC_ANTICOLL2;
    MfComData.MfData[1]=0x20;
    status=PcdComTransceive(pi, &ret_len);
    if(!status)
    {
    	 for(i=0;i<4;i++)
         {
             snr_check^=MfComData.MfData[i];
         }
         if(snr_check!=MfComData.MfData[i])
         {
             status=MI_SERNRERR;
         }
         else
         {
             for(i=0;i<4;i++)
             {
             	*(snr+i)=MfComData.MfData[i];
             }
         }

    }
    ClearBitMask(RegDecoderControl,0x20);
    return status;
}

////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect1(uchar *snr, uchar *res, uchar *len)
{
    uchar i;
    char status;
    uchar snr_check=0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[7];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x0F);
    ClearBitMask(RegControl,0x08);

    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=7;
    MfComData.MfData[0]=PICC_ANTICOLL1;
    MfComData.MfData[1]=0x70;
    for(i=0;i<4;i++)
    {
    	snr_check^=*(snr+i);
    	MfComData.MfData[i+2]=*(snr+i);
    }
    MfComData.MfData[6]=snr_check;
    status=PcdComTransceive(pi, len);
	
    if(status==MI_OK)
    {   
		memcpy(res, pi->MfData, *len); 		
		if(MfComData.MfLength!=0x8)
        {
         status = MI_BITCOUNTERR;
        }
   }
   return status;
}

////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect2(uchar *snr, uchar *res, uchar *len)
{
    uchar i;
    char status;
    uchar snr_check=0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[7];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x0F);
    ClearBitMask(RegControl,0x08);

    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=7;
    MfComData.MfData[0]=PICC_ANTICOLL2;
    MfComData.MfData[1]=0x70;
    for(i=0;i<4;i++)
    {
    	snr_check^=*(snr+i);
    	MfComData.MfData[i+2]=*(snr+i);
    }
    MfComData.MfData[6]=snr_check;
    status=PcdComTransceive(pi, len);

    if(status==MI_OK)
    {  
		memcpy(res, pi->MfData, *len); 
		if(MfComData.MfLength!=0x8)
        {
         status = MI_BITCOUNTERR;
        }
   }
   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：将Mifare_One卡密钥转换为RC500接收格式
//参数说明：uncoded[IN]:6字节未转换的密钥
//          coded[OUT]:12字节转换后的密钥
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char ChangeCodeKey(uchar *uncoded,uchar *coded)
{
   uchar cnt=0;
   uchar ln=0;
   uchar hn=0;

   for(cnt=0;cnt<6;cnt++)
   {
      ln=uncoded[cnt]&0x0F;
      hn=uncoded[cnt]>>4;
      coded[cnt*2+1]=(~ln<<4)|ln;
      coded[cnt*2]=(~hn<<4)|hn;
   }
   return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//功    能：将已转换格式后的密钥送到RC500的FIFO中
//参数说明：pKey[IN]:密钥
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAuthKey(uchar *keys)
{
    char status;
    uchar i;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[12];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;
    PcdSetTmo(106);
    MfComData.MfCommand=PCD_LOADKEY;
    MfComData.MfLength=12;
    for(i=0;i<12;i++)
    {
        MfComData.MfData[i]=*(keys+i);
    }
    status=PcdComTransceive(pi, &ret_len);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdAuthState(uchar auth_mode,uchar block,uchar *snr)
{
    char status=MI_OK;
    uchar i;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[6];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    WriteRC(RegChannelRedundancy,0x07);
    if(status==MI_OK)
    {
        PcdSetTmo(106);
        MfComData.MfCommand=PCD_AUTHENT1;
        MfComData.MfLength=6;
        MfComData.MfData[0]=auth_mode;
        MfComData.MfData[1]=block;
        for(i=0;i<4;i++)
        {
	      MfComData.MfData[i+2]=*(snr+i);
        }
        if((status=PcdComTransceive(pi, &ret_len))==MI_OK)
        {
            if (ReadRC(RegSecondaryStatus)&0x07) 
            {
                status = MI_BITCOUNTERR;
            }
            else
            {
                MfComData.MfCommand=PCD_AUTHENT2;
                 MfComData.MfLength=0;
                if((status=PcdComTransceive(pi, &ret_len))==MI_OK)
                {
                    if(ReadRC(RegControl)&0x08)
                        status=MI_OK;
                    else
                        status=MI_AUTHERR;
                }
             }
         }
   }
   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(uchar addr,uchar *readdata)
{
    char status;
    uchar i;
	uchar ret_len = 0;
    struct TranSciveBuffer
	{
		uchar MfCommand;
        uchar MfLength;
        uchar MfData[16];
    }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(4);
    WriteRC(RegChannelRedundancy,0x0F);
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
    MfComData.MfData[0]=PICC_READ;
    MfComData.MfData[1]=addr;

    status=PcdComTransceive(pi, &ret_len);
    if(status==MI_OK)
    {
        if(MfComData.MfLength!=0x80)
        {
            status = MI_BITCOUNTERR;
        }
        else
        {
            for(i=0;i<16;i++)
            {
                *(readdata+i)=MfComData.MfData[i];
            }
        }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////   
extern char PcdWrite(uchar addr,uchar *writedata)
{
    char status;
    uchar i;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[16];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(4);
//  WriteRC(RegChannelRedundancy,0x0F); 
    WriteRC(RegChannelRedundancy,0x07);
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
    MfComData.MfData[0]=PICC_WRITE;
    MfComData.MfData[1]=addr;

    status=PcdComTransceive(pi, &ret_len);
    if(status!=MI_NOTAGERR)
    {
        if(MfComData.MfLength!=4)
        {
           status=MI_BITCOUNTERR;
        }
        else
        {
           MfComData.MfData[0]&=0x0f;
           switch(MfComData.MfData[0])
           {
              case 0x00:
                 status=MI_NOTAUTHERR;
                 break;
              case 0x0a:
                 status=MI_OK;
                 break;
              default:
                 status=MI_CODEERR;
                 break;
           }
        }
     }
     if(status==MI_OK)
     {
        PcdSetTmo(3);
        MfComData.MfCommand=PCD_TRANSCEIVE;
        MfComData.MfLength=16;
        for(i=0;i<16;i++)
        {
            MfComData.MfData[i]=*(writedata+i);
        }
        status=PcdComTransceive(pi, &ret_len);
        if(status!=MI_NOTAGERR)
        {
            MfComData.MfData[0]&=0x0f;
            switch(MfComData.MfData[0])
            {
               case 0x00:
                  status=MI_WRITEERR;
                  break;
               case 0x0a:
                  status=MI_OK;
                  break;
               default:
                  status=MI_CODEERR;
                  break;
           }
        }
     }
  return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////       
char PcdValue(uchar dd_mode,uchar addr,uchar *value)
{
    char status;
    uchar i;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[4];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    PcdSetTmo(106);
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
    MfComData.MfData[0]=dd_mode;
    MfComData.MfData[1]=addr;

    status=PcdComTransceive(pi, &ret_len);
    if(status!=MI_NOTAGERR)
    {
        if(MfComData.MfLength!=4)
        {
           status=MI_BITCOUNTERR;
        }
        else
        {
           MfComData.MfData[0]&=0x0f;
           switch(MfComData.MfData[0])
           {
              case 0x00:
                 status=MI_NOTAUTHERR;
                 break;
              case 0x0a:
                 status=MI_OK;
                 break;
              case 0x01:
                 status=MI_VALERR;
                 break;
              default:
                 status=MI_CODEERR;
                 break;
           }
        }
     }
     if(status==MI_OK)
     {
        PcdSetTmo(4);
        MfComData.MfCommand=PCD_TRANSCEIVE;
        MfComData.MfLength=4;
        pi=&MfComData;
        for(i=0;i<4;i++)
        {
            MfComData.MfData[i]=*(value+i);
        }
        status=PcdComTransceive(pi, &ret_len);
        if(status==MI_OK||status==MI_NOTAGERR)
        {
           if(MfComData.MfLength!=4)
           {
              status = MI_BITCOUNTERR;
           }
           else
           {
              status=MI_OK;
            }
         }
         else
         {
              status=MI_COM_ERR;
          }
     }
     if(status==MI_OK)
     {

        MfComData.MfCommand=PCD_TRANSCEIVE;
        MfComData.MfLength=2;
        MfComData.MfData[0]=PICC_TRANSFER;
        MfComData.MfData[1]=addr;
        status=PcdComTransceive(pi, &ret_len);
        if(status!=MI_NOTAGERR)
        {
            if(MfComData.MfLength!=4)
            {
               status=MI_BITCOUNTERR;
            }
            else
            {
               MfComData.MfData[0]&=0x0f;
               switch(MfComData.MfData[0])
               {
                  case 0x00:
                     status=MI_NOTAUTHERR;
                     break;
                  case 0x0a:
                     status=MI_OK;
                     break;
                  case 0x01:
                     status=MI_VALERR;
                     break;
                  default:
                     status=MI_CODEERR;
                     break;
               }
            }
        }
     }
   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：关天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
    ClearBitMask(RegTxControl,0x03);
}

/////////////////////////////////////////////////////////////////////
//功    能：开天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn()
{
    SetBitMask(RegTxControl,0x03);
}

/*****************************************************************************
*          复位并初始化RC500
*注意:RC500上电后应延时500ms才能可靠初始化
******************************************************************************/
char PcdReset()
{
   char status=MI_OK;
   u16  i=0x2000;
    
   PD(Bit_RESET);
   Delay10us(2500);
   PD(Bit_SET);
   Delay10us(1000);
   PD(Bit_RESET);
   Delay10us(300);

   while((ReadRC(RegCommand) & 0x3F) && i--);
    if(!i)
    {
        status = MI_NOTAGERR;
//		USARTpos_SENDstr("TimeOut");
//   		USARTpos_SENDbyte('\n');
    }
    if (status == MI_OK)
    {
        WriteRC(RegPage,0x80);
        if (ReadRC(RegCommand) != 0x00)
        {                           
            status = MI_NOTAGERR;
//			USARTpos_SENDstr("MI_ERR2");
//   		  	USARTpos_SENDbyte('\n');
        }
		else
		{	
			WriteRC(RegPage,0x00);
		}
    }
  
   if(status==MI_OK)
   {
     WriteRC(RegClockQControl,0x0);
     WriteRC(RegClockQControl,0x40);
     Delay10us(10);
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
	 test_tianxian0++;
   }
   return status;
}

void ClearBitMask(uchar reg,uchar mask)
{
   char tmp=0x0;
   tmp = ReadRC(reg);
   WriteRC(reg,tmp & ~mask);
}

struct TranSciveBuffer
	{
		uchar MfCommand;
        uchar MfLength;
        uchar MfData[BUF_LEN];
    };

//char PcdComTransceive(struct TranSciveBuffer *pi)
char PcdComTransceive(void *p, uchar *len)
{  
   struct TranSciveBuffer *pi = (struct TranSciveBuffer *)p;
   uchar recebyte=0;
   char status;
   uchar irqEn=0x00;
   uchar waitFor=0x00;
   uchar lastBits;
   uchar n;
   uint i;
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
				memcpy(p, pi, sizeof(struct TranSciveBuffer));		//返回数据
				*len = n;											//返回数据的长度
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

char PcdHalt(void)
{
    char status=MI_OK;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[2];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    PcdSetTmo(106);
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
    MfComData.MfData[0]=PICC_HALT;
    MfComData.MfData[1]=0;

    status=PcdComTransceive(pi, &ret_len);
    if (status)
    {
        if(status==MI_NOTAGERR||status==MI_ACCESSTIMEOUT)
        status = MI_OK;
    }
    WriteRC(RegCommand,PCD_IDLE);
    return status;
}

/////////////////////////////////////////////////////////////////////
//M I F A R E   U L T R A L I G H T   A N T I C O L L I S I O N 
//A N D    S E L E C T
//parameter: pSnr[OUT]:7Byte card SNR
/////////////////////////////////////////////////////////////////////
char UL_PcdAnticoll(unsigned char *pSnr)
{
   
//    char status ;
//    unsigned char i, snr_check = 0;
//    unsigned int  unLen;
//    unsigned char idata ucComBuf[FSD];

	char status;
    char i;
	uchar ret_len = 0;
	uchar snr_check=0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[7];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    WriteRC(RegDecoderControl,0x28);
    ClearBitMask(RegControl,0x08);
    WriteRC(RegChannelRedundancy,0x03);
    PcdSetTmo(4);

    MfComData.MfCommand=PCD_TRANSCEIVE;
	MfComData.MfLength = 2;
    MfComData.MfData[0] = PICC_ANTICOLL1;
    MfComData.MfData[1] = 0x20;

    //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,2,ucComBuf,&unLen);
	status=PcdComTransceive(pi, &ret_len);
    
    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = MfComData.MfData[i];
             snr_check ^= MfComData.MfData[i];
         }
         if (snr_check != MfComData.MfData[i])
         {   status = MI_COM_ERR;    }
         else
         {   memcpy(pSnr, &MfComData.MfData[1], 3);   }
    }
    
    ClearBitMask(RegDecoderControl,0x20);

    snr_check = 0x88;
    WriteRC(RegChannelRedundancy,0x0F);
    ClearBitMask(RegControl,0x08);
    PcdSetTmo(4);

	MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength = 7;
    MfComData.MfData[0] = PICC_ANTICOLL1;
    MfComData.MfData[1] = 0x70;
    MfComData.MfData[2] = 0x88;
    for (i=0; i<3; i++)
    {
    	snr_check ^= *(pSnr+i);
    	MfComData.MfData[i+3] = *(pSnr+i);
    }
    MfComData.MfData[6] = snr_check;
    
    //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,7,ucComBuf,&unLen);
	status=PcdComTransceive(pi, &ret_len);
    if (status == MI_OK)
    {    
    	if (MfComData.MfLength != 0x08)
        {   status = MI_COM_ERR;     }
    }
    
    if (status == MI_OK)
    {
       snr_check = 0;
       WriteRC(RegDecoderControl,0x28);          
       ClearBitMask(RegControl,0x08);
       WriteRC(RegChannelRedundancy,0x03);
       PcdSetTmo(4);

       MfComData.MfCommand=PCD_TRANSCEIVE;
	   MfComData.MfLength = 2;
       MfComData.MfData[0] = PICC_ANTICOLL2;
       MfComData.MfData[1] = 0x20;
   
       //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,2,ucComBuf,&unLen);
	   status=PcdComTransceive(pi, &ret_len);
       if (!status)
       {
    	  for (i=0; i<4; i++)
          {  snr_check ^= MfComData.MfData[i];   }
          if (snr_check != MfComData.MfData[i])
          {  status = MI_COM_ERR;   }
          else
          {   memcpy(pSnr+3, &MfComData.MfData[0], 4);    }
       }
       ClearBitMask(RegDecoderControl,0x20);
    
       snr_check = 0;
       WriteRC(RegChannelRedundancy,0x0F);
       ClearBitMask(RegControl,0x08);
       PcdSetTmo(4);

       MfComData.MfCommand=PCD_TRANSCEIVE;
	   MfComData.MfLength = 7;
       MfComData.MfData[0] = PICC_ANTICOLL2;
       MfComData.MfData[1] = 0x70;
       for (i=0; i<4; i++)
       {
    	   snr_check ^= *(pSnr+3+i);
    	   MfComData.MfData[i+2] = *(pSnr+3+i);
       }
       MfComData.MfData[6] = snr_check;
       
       //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,7,ucComBuf,&unLen);
	   status=PcdComTransceive(pi, &ret_len);
       if (status == MI_OK)
       {    
       	   if (MfComData.MfLength != 0x08)
           {   status = MI_COM_ERR;   }
       }   	
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//M I F A R E   U L T R A L I G H T   W R I T E
//parameter: addr[IN]:page addr
//           pData[IN]:4byte data
/////////////////////////////////////////////////////////////////////
char UL_PcdWrite(unsigned char addr,unsigned char *pData)
{
//    char status;
//    unsigned int  unLen;
//    unsigned char idata ucComBuf[FSD]; 
    char status;
//   char i;
	uchar ret_len = 0;
//	uchar snr_check=0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[6];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    PcdSetTmo(4);
    WriteRC(RegChannelRedundancy,0x07); 

    MfComData.MfCommand = PCD_TRANSCEIVE;
	MfComData.MfLength = 3;
    MfComData.MfData[0] = 0xA2;
    MfComData.MfData[1] = addr;
    memcpy(&MfComData.MfData[2], pData, 4);
	status=PcdComTransceive(pi, &ret_len);
    //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,6,ucComBuf,&unLen);
    
    if ((status != MI_OK) || (MfComData.MfLength != 4) || ((MfComData.MfData[0] & 0x0F) != 0x0A))
    {   status = MI_COM_ERR;   }
    
    PcdSetTmo(4);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：复位Mifare_Pro(X)
//参数说明：param[IN]:FSDI+CID见ISO14443-4
//          pLen[OUT]:复位信息字节长度
//          pData[OUT]:复位信息
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char MifareProRst(unsigned char param,unsigned char *pLen,unsigned char *pData)
{
//    char status;
//    unsigned int  unLen;
//    unsigned char idata ucComBuf[FSD];
    char status;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[2];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;
    
    PcdSetTmo(15);
  
	MfComData.MfCommand = PCD_TRANSCEIVE;
	MfComData.MfLength = 2;
    MfComData.MfData[0] = PICC_RESET;
    MfComData.MfData[1] = param;
    g_bIblock = 0;

    //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,2,ucComBuf,&unLen);
	status=PcdComTransceive(pi, &ret_len);
    
    if ((*pLen = MfComData.MfLength/8) <=1 )
    {   memcpy(pData, MfComData.MfData, *pLen);   }    
    else
    {   status = MI_COM_ERR;   }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：向T=CL协议的卡发送COS指令
//参数说明：CID[IN]:     高半字节指明是否包含CID,低半字节＝CID
//          pCommand[IN]:COS指令
//          ComLg[IN]:   COS指令长度
//          pAnswer[OUT]:卡片应答数据
//          pAnsLg[OUT]: 应答数据长度
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char ISO14443CLCom(unsigned char CID, 
                   unsigned char *pCommand, 
                   unsigned char ComLg,
                   unsigned char *pAnswer,
                   unsigned char *pAnsLg)
{
//    char status;
//    unsigned int  unLen;
//    unsigned char idata ucComBuf[FSD];
//    
    unsigned char i,CidLg,PCB_I,PCB_R,a,b,c,d,e;
//    unsigned char idata sendlgok,sendlgnow,sendlgsum,recelgnow,recelgsum;
    char status;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[3];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;
    
//    recelgnow = 0x00;
//    recelgsum = 0x00;
//    sendlgnow = 0x00;
//    sendlgok  = 0x00;
//    sendlgsum = ComLg;
    a = 0x00;
	b = 0x00;
	c = 0x00;
	d = 0x00;
	e = ComLg;
    PCB_R  = 0xA2;
    
    PcdSetTmo(11);

    if (CID & 0xF0)
    {    CidLg = 1;    }
    else
    {    CidLg = 0;    }
    
    //if (e > FSD-1-CidLg)
	if (e > 3-1-CidLg)
    {
       // a  = FSD-1-CidLg;
	    a  = 3-1-CidLg;
        e -= c;
        PCB_I = 0x12 ^ g_bIblock;
    }    
    else
    {   
        c  = e;
        PCB_I = 0x02 ^ g_bIblock;
    }
    
    g_bIblock = !g_bIblock;

	MfComData.MfCommand = PCD_TRANSCEIVE;
	MfComData.MfLength	= c+CidLg+1	;
    MfComData.MfData[0] = (CidLg<<3) | PCB_I; 
    if (CidLg)
    {   MfComData.MfData[1] = CID & 0x0F;    }
    for (i=0; i<CidLg; i++)
    {  MfComData.MfData[i+1] = 0;  } 
    memcpy(&MfComData.MfData[CidLg+1], pCommand, c);   
    
    d += c;
    
   // status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,sendlgnow+CidLg+1,ucComBuf,&unLen);
   status=PcdComTransceive(pi, &ret_len);
//////////////////////////////////////////////
    while (status == MI_OK)
    {
       // if (MfComData.MfLength/8-1-CidLg > FSD)
	    if (MfComData.MfLength/8-1-CidLg > 3)
        {   
            status = MI_COM_ERR;
            break;
        }
        if ((MfComData.MfData[0] & 0xF0) == 0x00) 
        //命令通讯结束
        {
            a = MfComData.MfLength/8 - 1 - CidLg;
            memcpy(pAnswer+b, &MfComData.MfData[CidLg+1], a);    
            b += a;
            *pAnsLg = b;
            break;
        }  
        if ((MfComData.MfData[0] & 0xF0) == 0xA0)
        //发送后续数据
        {
            for(i=0; i<CidLg; i++)
            {   MfComData.MfData[i+1] = 0;  }               
            PCB_I ^= 1;
           // if(e > FSD-1-CidLg)
		    if(e > 3-1-CidLg)
            {  
               // c  = FSD-1-CidLg; 
			    c  = 3-1-CidLg;
                e -= c; 
                PCB_I |= 0x10^ g_bIblock;
                g_bIblock = !g_bIblock;
            }    
            else
            {
            	c = e;
                PCB_I &= 0xEF;
                PCB_I |= g_bIblock;
                g_bIblock = !g_bIblock;
            }
			MfComData.MfCommand = PCD_TRANSCEIVE;
			MfComData.MfLength  = c+CidLg+1	;
            MfComData.MfData[0] = 0x02 | (CID<<3) | PCB_I;
            memcpy(&MfComData.MfData[CidLg+1], pCommand+d, c); 
            d += c;

//            status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,sendlgnow+CidLg+1,ucComBuf,&unLen);
			status=PcdComTransceive(pi, &ret_len);
            continue;
        }
        if ((MfComData.MfData[0] & 0xF0) == 0x10)
        //接收后续数据
        {
            a = MfComData.MfLength/8 - 1 - CidLg;
            memcpy(pCommand+b, &MfComData.MfData[CidLg+1], a);  
            b += a;
                          
            if(MfComData.MfData[0] & 0x01) 
            {  PCB_R &= 0xFE;  }
            else 
            {  PCB_R |= 0x01;  }
			MfComData.MfCommand = PCD_TRANSCEIVE;
			MfComData.MfLength  = CidLg+1	;
            MfComData.MfData[0] = PCB_R | (CID<<3) & 0xFB;
            for(i=0; i<CidLg; i++)
            {  MfComData.MfData[i+1]=0;  } 
            
//            status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,CidLg+1,ucComBuf,&unLen);
            status=PcdComTransceive(pi, &ret_len); 
            continue;    
        }
        status = MI_COM_ERR;
        break;
    }     
    return status;
}

//////////////////////////////////////////////////////////////////////
//ISO14443-4 DESELECT
//////////////////////////////////////////////////////////////////////
char CL_Deselect(unsigned char CID)
{
//    char status;
//    unsigned int  unLen;
//    unsigned char idata ucComBuf[FSD];
    char status;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[2];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;
    
    PcdSetTmo(4);
	MfComData.MfCommand = PCD_TRANSCEIVE;
	MfComData.MfLength  = 2;
    MfComData.MfData[0] = 0xCA;
    MfComData.MfData[1] = CID;
    
    //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,2,ucComBuf,&unLen);
    status=PcdComTransceive(pi, &ret_len);
    return status;
}

void PcdSetTmo(uchar tmoLength)
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
         WriteRC(RegTimerClock,0x0a);      // TAutoRestart=0,TPrescale=4*128
         WriteRC(RegTimerReload,0xff);     // TReloadVal = 'ff =255(dec)
         break;
      default:                             // short timeout (1,0 ms)
         WriteRC(RegTimerClock,0x07);      // TAutoRestart=0,TPrescale=128
         WriteRC(RegTimerReload,tmoLength);// TReloadVal = tmoLength
         break;
   }
   WriteRC(RegTimerControl,0x06);
}

uchar ReadRC(uchar Address)
{
	u8 value;
	u16 temp;
	
	ALE(Bit_RESET);		   //ALE =0
	
	CS(Bit_SET);		   //CS = 1

    RD(Bit_SET);		   //RD = 1

	WR(Bit_SET);		   //WR = 1

	temp = GPIO_ReadOutputData(MFRC500_DATA_Port) & 0xff; //取低8位
	WR_DATA((Address << 8)|temp);		   //PB[8:15] = DATA

	ALE(Bit_SET);		   //ALE = 1
	
//	delay_us(30);
	Delay10us(3);
	

	ALE(Bit_RESET);		   //ALE = 0

	MFRC500_DATA_Port_config_input();	 

	CS(Bit_RESET);		   //CS = 0

	RD(Bit_RESET);		   //RD = 0

	Delay3us();

	value = RD_DATA();	   //value = PB[8:15] 
	
	Delay3us();


	RD(Bit_SET);		   //RD = 1
	
	CS(Bit_SET);		   //CS = 1
	

	MFRC500_DATA_Port_config_output();
	
	return value;
}

void SetBitMask(uchar reg,uchar mask)
{
   char tmp=0x0;
   tmp=ReadRC(reg);
   WriteRC(reg,tmp|mask);
}

void WriteRC(u16 Address,u16 value)
{
  	u16 temp;
	ALE(Bit_SET);  			//ALE = 1
	temp = GPIO_ReadOutputData(MFRC500_DATA_Port) & 0xff;
	WR_DATA((Address << 8)|temp); 		//PB[8:15] = DATA 
	Delay10us(2);
	ALE(Bit_RESET);			//ALE = 0
	Delay10us(2);
	CS(Bit_RESET);			//CS = 0
	temp = GPIO_ReadOutputData(MFRC500_DATA_Port) & 0xff;
	WR_DATA((value << 8)|temp);   		//PB[8:15] = DATA
	WR(Bit_RESET);      	//WR = 0
	Delay10us(2);
	WR(Bit_SET);		   //WR = 1
	CS(Bit_SET);		   //CS = 1

//   	ALE(Bit_RESET);	       //ALE = 0
//	CS(Bit_SET);		   //CS = 1
//	RD(Bit_SET);		   //RD = 1
//	WR(Bit_SET);		   //WR = 1
//	temp = GPIO_ReadOutputData(MFRC500_DATA_Port) & 0xff;
//	WR_DATA((Address << 8)|temp); 		//PB[8:15] = DATA  
//    Delay3us();
//	Delay3us();
//	ALE(Bit_SET);  			//ALE = 1
//	Delay10us(3);
//	ALE(Bit_RESET);			//ALE = 0
//	Delay3us();
//	CS(Bit_RESET);			//CS = 0
//    Delay3us();
//		 	
//	temp = GPIO_ReadOutputData(MFRC500_DATA_Port) & 0xff;
//	WR_DATA((value << 8)|temp);   		//PB[8:15] = DATA	  	
//    WR(Bit_RESET);      	//WR = 0
//	Delay3us();
//	WR(Bit_SET);			//WR = 1
//	CS(Bit_SET);			//CS = 1
}

char PcdWriteE2(uint startaddr,uchar length,uchar *writedata)
{
    char status;
    char i;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[19];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    MfComData.MfCommand=PCD_WRITEE2;
    MfComData.MfLength=length+2;
    MfComData.MfData[0]=startaddr&0xff;
    MfComData.MfData[1]=(startaddr>>8)&0xff;
    for(i=0;i<length;i++)
    {
        MfComData.MfData[i+2]=*(writedata+i);
    }
    status=PcdComTransceive(pi, &ret_len);
    return status;
}

char PcdLoadConfig(uint startaddr)
{
    char status;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[2];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    MfComData.MfCommand=PCD_LOADCONFIG;
    MfComData.MfLength=2;
    MfComData.MfData[0]=startaddr&0xff;
    MfComData.MfData[1]=(startaddr>>8)&0xff;
    status=PcdComTransceive(pi, &ret_len);
    return status;
}

char PcdConfigRestore()
{
    uchar rc500config[32]=
    {
	 0x00,0x58,0x3f,0x3f,0x19,0x13,0x00,0x00,0x00,0x73,0x08,0xad,0xff,0x00,0x41,0x00,
     0x00,0x06,0x03,0x63,0x63,0x00,0x00,0x00,0x00,0x08,0x07,0x06,0x0a,0x02,0x00,0x00
	};
    char status;
    uchar i;
    uchar congfigdata[16];
    uchar *pa;
    pa=&congfigdata[0];
    for(i=0;i<16;i++)
    {
    	*(pa+i)=rc500config[i];
    }
    status=PcdWriteE2(0x0010,0x10,pa);
    if(status==MI_OK)
    {
        for(i=0;i<16;i++)
        {
    	    *(pa+i)=rc500config[i+16];
        }
    }
    status=PcdWriteE2(0x0020,0x10,pa);
    return status;
}

char PcdLoadKeyE2(uint startaddr)
{
    char status;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[2];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    MfComData.MfCommand=PCD_LOADKEYE2;
    MfComData.MfLength=2;
    MfComData.MfData[0]=startaddr&0xff;
    MfComData.MfData[1]=(startaddr>>8)&0xff;
    status=PcdComTransceive(pi, &ret_len);
    return status;
}

extern char PcdReadE2(uint startaddr,uchar length,uchar *readdata)
{
    char status;
    uchar i;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[16];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    MfComData.MfCommand=PCD_READE2;
    MfComData.MfLength=3;
    MfComData.MfData[0]=startaddr&0xff;
    MfComData.MfData[1]=(startaddr>>8)&0xff;
    MfComData.MfData[2]=length;
    status=PcdComTransceive(pi, &ret_len);

    if(status==MI_OK)
    {
        for(i=0;i<length;i++)
        {
            *(readdata+i)=MfComData.MfData[i];
        }
    }
    return status;
}

char PcdRestore(uchar addr)
{
    char status;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[4];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    PcdSetTmo(4);
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
    MfComData.MfData[0]=PICC_RESTORE;
    MfComData.MfData[1]=addr;

    status=PcdComTransceive(pi, &ret_len);
    if(status!=MI_NOTAGERR)
    {
        if(MfComData.MfLength!=4)
        {
           status=MI_BITCOUNTERR;
        }
        else
        {
           MfComData.MfData[0]&=0x0f;
           switch(MfComData.MfData[0])
           {
              case 0x00:
                 status=MI_NOTAUTHERR;
                 break;
              case 0x0a:
                 status=MI_OK;
                 break;
              case 0x01:
                 status=MI_VALERR;
                 break;
              default:
                 status=MI_CODEERR;
                 break;
           }
        }
     }
     if(status==MI_OK)
     {
        PcdSetTmo(4);
        MfComData.MfCommand=PCD_TRANSCEIVE;
        MfComData.MfLength=4;
        MfComData.MfData[0]=0;
        MfComData.MfData[1]=0;
        MfComData.MfData[2]=0;
        MfComData.MfData[3]=0;
        status=PcdComTransceive(pi, &ret_len);
        if(status==MI_NOTAGERR)
        {
            status=MI_OK;
        }
     }
     return status;
}

char PcdTransfer(uchar addr)
{
    char status;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[2];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    PcdSetTmo(4);
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
    MfComData.MfData[0]=PICC_TRANSFER;
    MfComData.MfData[1]=addr;

    status=PcdComTransceive(pi, &ret_len);
    if (status!=MI_NOTAGERR)
    {
        if(MfComData.MfLength!=4)
        {
           status=MI_BITCOUNTERR;
        }
        else
        {
           MfComData.MfData[0]&=0x0f;
           switch(MfComData.MfData[0])
           {
              case 0x00:
                 status=MI_NOTAUTHERR;
                 break;
              case 0x0a:
                 status=MI_OK;
                 break;
              case 0x01:
                 status=MI_VALERR;
                 break;
              default:
                 status=MI_CODEERR;
                 break;
           }
        }
     }
   return status;
}

/*
 ********************************************************************
 *    配置 MFRC500 DATA Port 为输入模式
 ********************************************************************
 */
void MFRC500_DATA_Port_config_input()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = MFRC500_DATA_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(MFRC500_DATA_Port, &GPIO_InitStructure);
}


/*
 ********************************************************************
 *    配置 MFRC500 DATA Port 为输出模式
 ********************************************************************
 */
void MFRC500_DATA_Port_config_output()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = MFRC500_DATA_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(MFRC500_DATA_Port, &GPIO_InitStructure);
}


#if 0

/*
 ********************************************************************
 *    新增命令
 ********************************************************************
 */
extern C_APDU 							Command;
extern uint8_t 							uM24SRbuffer[];
/*********************************************************************************
* 功	能：char PcdRATS(uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdRATS(uchar *res, uchar *len)
{
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[15];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x03);
    ClearBitMask(RegControl,0x08);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= 4;
    MfComData.MfData[0] = 0xE0;
	MfComData.MfData[1] = 0x40;
	MfComData.MfData[2] = GETLSB( M24SR_ComputeCrc(MfComData.MfData, 2) );							
    MfComData.MfData[3] = GETMSB( M24SR_ComputeCrc(MfComData.MfData, 2) );
								
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}

   return status;
}

/*********************************************************************************
* 功	能：char PcdPPS(uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdPPS(uchar *res, uchar *len)
{
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[15];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x03);
    ClearBitMask(RegControl,0x08);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= 5;
    MfComData.MfData[0] = 0xD0;
	MfComData.MfData[1] = 0x11;
	MfComData.MfData[2] = 0x00;
	MfComData.MfData[3] = GETLSB( M24SR_ComputeCrc(MfComData.MfData, 3) );							
    MfComData.MfData[4] = GETMSB( M24SR_ComputeCrc(MfComData.MfData, 3) );
   
    status=PcdComTransceive(pi,len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	} 

   return status;
}

/*********************************************************************************
* 功	能：char PcdSelectApplication(uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdSelectApplication(uchar *res, uchar *len)
{
    char status;
	uchar ret_len = 0;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_SelectApplication(&ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
    if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}

   return status;
}

/*********************************************************************************
* 功	能：char PcdSelectCCfile(uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdSelectCCfile(uchar *res, uchar *len)
{
	uchar ret_len = 0;
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_SelectCCfile(&ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
    if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*********************************************************************************
* 功	能：char PcdReadCCfileLength(uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdReadCCfileLength(uchar *res, uchar *len)
{
	uchar ret_len = 0;
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_ReadBinary(0x0000, 0x02, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*********************************************************************************
* 功	能：char PcdReadCCfile(uint16_t Offset , uint8_t NbByteToRead, uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdReadCCfile(uint16_t Offset , uint8_t NbByteToRead, uchar *res, uchar *len)
{
	uchar ret_len = 0;
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_ReadBinary(Offset, NbByteToRead, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*********************************************************************************
* 功	能：char PcdSelectNDEFfile(uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdSelectNDEFfile(uchar *res, uchar *len)
{
	uchar ret_len = 0;
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_SelectNDEFfile(0x0001, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*********************************************************************************
* 功	能：char PcdReadNDEFfileLength(uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdReadNDEFfileLength(uchar *res, uchar *len)
{
	uchar ret_len = 0;
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_ReadBinary(0x0000, 0x02, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*********************************************************************************
* 功	能：char PcdReadNDEFfile(uint16_t Offset , uint8_t NbByteToRead, uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdReadNDEFfile(uint16_t Offset , uint8_t NbByteToRead, uchar *res, uchar *len)
{
	uchar ret_len = 0;
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[BUF_LEN];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_ReadBinary(Offset, NbByteToRead, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*********************************************************************************
* 功	能：char PcdWriteNDEFfile(uint16_t Offset , uint8_t NbByteToWrite , uint8_t *pDataToWrite, uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdWriteNDEFfile(uint16_t Offset , uint8_t NbByteToWrite , uint8_t *pDataToWrite, uchar *res, uchar *len)
{
	uchar ret_len = 0;
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[BUF_LEN];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(4);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_UpdateBinary(Offset, NbByteToWrite, pDataToWrite, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
	memcpy(res, pi->MfData, *len);
   return status;
}

/*********************************************************************************
* 功	能：char PcdSelectSystemfile(uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdSelectSystemfile(uchar *res, uchar *len)
{
	uchar ret_len = 0;
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_SelectSystemfile(&ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*********************************************************************************
* 功	能：char PcdReadSystemfileLength(uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdReadSystemfileLength(uchar *res, uchar *len)
{
	uchar ret_len = 0;
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_ReadBinary(0x0000, 0x02, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*********************************************************************************
* 功	能：char PcdReadSystemfile(uint16_t Offset , uint8_t NbByteToRead, uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdReadSystemfile(uint16_t Offset , uint8_t NbByteToRead, uchar *res, uchar *len)
{
	uchar ret_len = 0;
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_ReadBinary(Offset, NbByteToRead, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*********************************************************************************
* 功	能：char PcdSendInterrupt(uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdSendInterrupt (uchar *res, uchar *len)
{
	uchar ret_len = 0;
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_SendInterrupt(&ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*********************************************************************************
* 功	能：char PcdDeselect(uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdDeselect (uchar *res, uchar *len)
{
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[15];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x03);
    ClearBitMask(RegControl,0x08);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= 3;
    MfComData.MfData[0] = 0xC2;			
	MfComData.MfData[1] = 0xE0;
	MfComData.MfData[2] = 0xB4;
   
    status=PcdComTransceive(pi,len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	} 

   return status;
}

/*********************************************************************************
* 功	能：char PcdFWTExtension (uint8_t FWTbyte, uchar *res, uchar *len)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
char PcdFWTExtension (uint8_t FWTbyte, uchar *res, uchar *len)
{
    uchar ret_len = 0;
    char status;
    struct TranSciveBuffer{uchar MfCommand;
                                 uchar MfLength;
                                 uchar MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(4);
    WriteRC(RegChannelRedundancy,0x03);
    ClearBitMask(RegControl,0x08);

	M24SR_FWTExtension(FWTbyte, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

#endif






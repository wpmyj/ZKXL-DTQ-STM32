/**
  ******************************************************************************
  * @file   	rc500_handle_layer.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#define RC500_DATIQI_FUNCHTION	(1)
static void sendtoRC500(void);
void ComPcdFindCard(void);
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
void RC500_FindCard(void);

uint8_t FindICCard(void);
uint8_t SelectApplication(void);
uint8_t SendInterrupt(void);
uint8_t ReadCCfile(uint8_t *CCfileData, uint8_t *CCfileLen);
uint8_t ReadSystemfile(uint8_t *Systemfile_Data, uint8_t *Systemfile_len);
uint8_t ReadNDEFfile(uint8_t *NDEFfile_Data, uint16_t *NDEFfile_len);
uint8_t WriteNDEFfile(uint8_t *pDataToWrite);
uint8_t Deselect(void);

void rc500_handle_layer(void)
{
	
	sendtoRC500();

}
void write_RF_config(void)
{
//	uint8_t NDEF_Data[BUF_LEN] = {0};
//	uint16_t NDEF_Len;
	
	if( FindICCard() == MI_OK)		//寻卡成功
	{
		DebugLog("[FindICCard]:UID is ");
		app_debuglog_dump(g_cSNR, 8);

		if(SelectApplication() == MI_OK)		//选择应用
		{
			DebugLog("[WriteNDEFfile]:nrf_parameter is ");
//			app_debuglog_dump((uint8_t *)&answer_setting,sizeof(answer_setting));
//			if(WriteNDEFfile((uint8_t *)&answer_setting) == MI_OK)		//写入NDEF文件
//			{
//				time_for_buzzer_on = 10;
//				time_for_buzzer_off = 300;
//				if(ReadNDEFfile(NDEF_Data, &NDEF_Len) == MI_OK)			//读出验证
//				{
//					DebugLog("[ReadNDEFfile]:NDEF_Data is ");
//					app_debuglog_dump(NDEF_Data, NDEF_Len);
//				}
//			}
			Deselect();	//去除选择
			PcdHalt();
		}
	}
}

static void sendtoRC500(void)
{
	uint8_t  temp_cmd;

	uint8_t len = 0;
	uint16_t tmp_read_len = 0;
		
	if(App_to_CtrRC500Req)
	{ 
		temp_cmd = Buf_AppToCtrRC500[3];

		switch(temp_cmd)  
		{
			case 0x35:
				ComRequestA();
			break;
			case 0x36:		// 天线打开 or 关闭
				ComPcdAntenna();
			break;             
			case 0x37:		// 13.56M寻卡操作
				ComPcdFindCard();
			break;  
			case 0x38:		// A卡防冲撞命令
				ComAnticoll();
			break;        
			case 0x39:		// A卡锁定
				ComSelect1();
			break;
			case 0x30:		// A卡防冲撞命令
				ComAnticol2();
			break;
			case 0x31:		// A卡锁定命令
				ComSelect2();
			break;
			case 0x29:		// A卡休眠命令
				ComHlta();
			break;
			case 0x4A:		// A卡验证密钥
				ComAuthentication();
			break; 
			case 0x4B:		// 读M1卡
				ComM1Read(); 
			break;
			case 0x4C:		// 写M1卡
				ComM1Write();
			break;
			case 0x4D:		// 初始化钱包
				ComM1Initval();
			break; 
			case 0x4E:		// 读钱包
				ComM1Readval();
			break;
			case 0x4F:		// 扣款
				ComM1Decrement();
			break;
			case 0x50:		// 充值
				ComM1Increment();
			break;
			case 0x51:		// 备份钱包
				ComM1BakValue();
			break;
			case 0x52:		// UltraLght卡防冲撞
				ComUL_PcdAnticoll();
			break;      
			case 0x53:		// 写UltraLight卡
				ComUL_PcdWrite();
			break;  
			case 0x54:		// Mifare_Pro(X)卡高级复位
				ComTypeARst();
			break; 
			case 0x55:		// T=CL卡COS命令
				ComTCLCOS();
			break; 
			case 0x56:		// T=CL卡退出激活状态
				ComCL_Deselect();
			break; 
			case 0x57:		// 选择应用
				ComSelectApplication();
			break;
			case 0x58:		// 发送中断
				ComSendInterrupt();
			break;
			case 0x59:		// 读NDEF文件
				ComReadNDEFfile();			
			break;
			case 0x5a:		// 写NDEF文件
				ComWriteNDEFfile( (((uint16_t)Buf_AppToCtrRC500[4] << 8) | Buf_AppToCtrRC500[5]), &Buf_AppToCtrRC500[6]);			
			break;
			case 0x5b:		// 读 CC 文件
				ComReadCCfile();
			break;
			case 0x5c:		// 读系统文件
				ComReadSystemfile();
			break;
			case 0x5d:		// 去除选择
				ComDeselect();
			break;
					 
////////////////////////////////////   NFC相关命令  ////////////////////////////////////////////////					 
			case 0x70:		// Request for Answer To Select, Type A
				memset(g_cardType, 0, BUF_LEN);
				PcdRATS(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x71:		// Protocol and Parameter Selection
				memset(g_cardType, 0, BUF_LEN);
				PcdPPS(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x72:		// 选择M24SR应用
				memset(g_cardType, 0, BUF_LEN);
				PcdSelectApplication(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;
			break;	

			case 0x73:		// 选择CC文件
				memset(g_cardType, 0, BUF_LEN);
				PcdSelectCCfile(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x74:		// 读取CC文件长度
				memset(g_cardType, 0, BUF_LEN);
				PcdReadCCfileLength(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x75:		// 读取指定长度CC文件
				memset(g_cardType, 0, BUF_LEN);
				PcdReadCCfile(0x0002, 0x0d, respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x76:		// 选择NDEF文件
				memset(g_cardType, 0, BUF_LEN);
				PcdSelectNDEFfile(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x77:		// 读取NDEF文件长度
				memset(g_cardType, 0, BUF_LEN);
				PcdReadNDEFfileLength(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x78:		// 读取指定长度NDEF文件
				memset(g_cardType, 0, BUF_LEN);
				PcdReadNDEFfileLength(respon, &len);
				tmp_read_len = ((uint16_t)respon[1] << 8) | (respon[2]);
				PcdReadNDEFfile(0x0000, tmp_read_len + 2, respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x79:		// 选择系统文件
				memset(g_cardType, 0, BUF_LEN);
				PcdSelectSystemfile(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x7a:		// 读取系统文件长度
				memset(g_cardType, 0, BUF_LEN);
				PcdReadSystemfileLength(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;
	
			case 0x7b:		// 读取指定长度系统文件
				memset(g_cardType, 0, BUF_LEN);
				PcdReadSystemfile(0x0000, 10, respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x7c:		//  读卡器发送中断
				memset(g_cardType, 0, BUF_LEN);
				PcdSendInterrupt(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;					 

			case 0x7d:		// 写数据到NDEF文件
				memset(g_cardType, 0, BUF_LEN);
				PcdWriteNDEFfile(0x0000, 0x02, NDEF_DataWrite, respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;
			
			case 0x7e:		//去除选择
				memset(g_cardType, 0, BUF_LEN);
				PcdDeselect(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;
////////////////////////////////////   NFC相关命令  ////////////////////////////////////////////////					 
			default:
				AnswerErr( 11 );         
			break;
		}
		App_to_CtrRC500Req = false;
	}
}

/////////////////////////////////////////////////////////////////////
//写参数到答题器
/////////////////////////////////////////////////////////////////////
/*
void write_RF_config(void)
{
	uint8_t len;
	uint8_t temp[15] = {0};
	uint16_t NDEF_DataRead_Len;
	uint8_t Step = 1;
	do{
		switch(Step)
		{
			case 0x01:	// 打开天线
				PcdAntennaOn();												//打开13.56M天线
				Step = 0x02;
				break;
			case 0x02:	//发送reqA指令
				memset(g_cardType, 0, 40);
				if(PcdRequest(PICC_REQIDL,g_cardType) == MI_OK)				//请求A卡，返回卡类型，不同类型卡对应不同的UID长度
				{	
					if( (g_cardType[0] & 0x40) == 0x40)
					{	uid_len = 8;	}
					else
					{	uid_len = 4;	}
					Step = 0x03;
				}
				break;
			case 0x03:	//防碰撞1
				if(PcdAnticoll(PICC_ANTICOLL1, g_cSNR) == MI_OK )
					Step = 0x04;
				else
					Step = 0x02;
			break;
			case 0x04:	//选卡1	
				memset(respon, 0, BUF_LEN);
				if (MI_OK == PcdSelect1(g_cSNR, respon, &len))				
				{
					if((uid_len == 8) && ((respon[0] & 0x04) == 0x04))
						Step = 0x05;
					else
						Step = 0x07;
				}
				else
					Step = 0x02;
			break;
			case 0x05:	//防碰撞2
				memset(respon, 0, BUF_LEN);
				if(MI_OK == PcdAnticoll(PICC_ANTICOLL2, &g_cSNR[4]))		
				{
					Step = 0x06;
				}
				else
					Step = 0x02;		
			break;
			case 0x06:	//选卡2
				if((MI_OK == PcdSelect2(&g_cSNR[4], respon, &len))&&((respon[0] & 0x20) == 0x20))
				{
					Step = 0x08;
				}
				else
					Step = 0x02;		
			break;
			case 0x07:	//4位卡号输出
				DebugLog("[RC500_FindCard]:UID is ");
				sprintf((char *)temp, "%010u [正码]", (g_cSNR[0] << 24) | (g_cSNR[1] << 16) | (g_cSNR[2] << 8) | (g_cSNR[3] << 0));
				DebugLog("%s or ", temp);
				sprintf((char *)temp, "%010u [反码]", (g_cSNR[3] << 24) | (g_cSNR[2] << 16) | (g_cSNR[1] << 8) | (g_cSNR[0] << 0));
				DebugLog("%s\r\n", temp);	
				time_for_buzzer_switch = 10;
				Step = 0x09;
			break;
			case 0x08:	//8位卡号输出
				DebugLog("[RC500_FindCard]:UID is ");
				app_debuglog_dump_no_space(g_cSNR, 8);
				time_for_buzzer_switch = 10;
				Step = 0x09;
			break;
			case 0x09:	//卡休眠
				PcdHalt();												//使读到卡号的卡进入休眠
				Step = 0x00;
			break;
			
			case 0x10:	//设置最大可接收数据为48byte
				memset(respon, 0, BUF_LEN);
				if((MI_OK == PcdRATS(respon, &len)) && (respon[0] == 0x05) && (respon[1] == 0x78) && (respon[4] == 0x02))
					Step = 0x11;
				else
					Step = 0x02;	
			break;
			case 0x11:	//设置传输速率
				memset(respon, 0, BUF_LEN);
				if((MI_OK == PcdPPS(respon, &len)) && (respon[0] == 0xD0))
					Step = 0x12;
				else
					Step = 0x02;	
			break;
			case 0x12:	//选择系统应用
				memset(respon, 0, BUF_LEN);
				if((MI_OK == PcdSelectApplication(respon, &len)) && (respon[1] == 0x90) && (respon[2] == 0x00))
					Step = 0x08;
				else
					Step = 0x02;	
			break;
			
			case 0x20:	//强制读取CC file
				if(ForceReadCCfile() == MI_OK)
					Step = 0x21;
				else
					Step = 0x02;
			break;
			case 0x21:	//选择NDEF文件
				memset(respon, 0, BUF_LEN);
				if((MI_OK == PcdSelectNDEFfile(respon, &len)) && (respon[1] == 0x90) && (respon[2] == 0x00)) 
					Step = 0x22;
				else
					Step = 0x02;
			break;
			case 0x22:	//读取NDEF文件长度及内容
				memset(respon, 0, BUF_LEN);
				if(PcdReadNDEFfileLength(respon, &len) == MI_OK)
				{
					NDEF_DataRead_Len = ((uint16_t)respon[1] << 8) | respon[2] ;
					memset(respon, 0, BUF_LEN);
					if(PcdReadNDEFfile(0x0000, NDEF_DataRead_Len + 2, respon, &len) == MI_OK)
						Step = 0x23;
					else
						Step = 0x02;
				}
				else
					Step = 0x02;
			break;
			case 24:
				memset(respon, 0, BUF_LEN);
				if( status = PcdWriteNDEFfile(0x0000, 0x02, EraseLen, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
					Step = 0x22;
				else
					Step = 0x02;	
			
			case 0x23:	//写NDEF文件
				ComWriteNDEFfile( (((uint16_t)Buf_AppToCtrRC500[4] << 8) | Buf_AppToCtrRC500[5]), &Buf_AppToCtrRC500[6]);
			break;
			case 0x30:	//去除选择
				ComDeselect();
			break;
			case 0xFF:
				PcdAntennaOff();											//关闭13.56M天线
				Step = 0x00;
			break;
			default	:
				Step = 0x00;
		}
	}while(step);
}

*/

/////////////////////////////////////////////////////////////////////
//响应应用层的寻卡命令
/////////////////////////////////////////////////////////////////////
void ComPcdFindCard(void)
{
	uint8_t len,findIC_flow = 0x01;
	
	while(findIC_flow)
	{
		switch(findIC_flow)
		{
			case 0x01:	// 打开天线
				PcdAntennaOn();												//打开13.56M天线
				findIC_flow = 0x02;
				break;
			
			case 0x02:	//发送reqA指令
				memset(g_cardType, 0, 40);
				if(PcdRequest(PICC_REQIDL,g_cardType) == MI_OK)				//请求A卡，返回卡类型，不同类型卡对应不同的UID长度
				{	
					if( (g_cardType[0] & 0x40) == 0x40)
					{	uid_len = 8;	}
					else
					{	uid_len = 4;	}
					findIC_flow = 0x03;
				}
				else
					findIC_flow = 0x00;
				break;
				
			case 0x03:	//防碰撞1
				if(PcdAnticoll(PICC_ANTICOLL1, g_cSNR) == MI_OK )
					findIC_flow = 0x04;
				else
					findIC_flow = 0x02;
				break;
				
			case 0x04:	//选卡1	
				memset(respon, 0, BUF_LEN);
				if (MI_OK == PcdSelect1(g_cSNR, respon, &len))				
				{
					if((uid_len == 8) && ((respon[0] & 0x04) == 0x04))
						findIC_flow = 0x05;
					else if(uid_len == 4)
						findIC_flow = 0x07;
				}
				else
					findIC_flow = 0x02;
				break;
				
			case 0x05:	//防碰撞2
				memset(respon, 0, BUF_LEN);
				if(MI_OK == PcdAnticoll(PICC_ANTICOLL2, &g_cSNR[4]))		
				{
					findIC_flow = 0x06;
				}
				else
					findIC_flow = 0x02;		
				break;
				
			case 0x06:	//选卡2
				if((MI_OK == PcdSelect2(&g_cSNR[4], respon, &len))&&((respon[0] & 0x20) == 0x20))
				{
					findIC_flow = 0x07;
				}
				else
					findIC_flow = 0x02;		
				break;
				
			case 0x07:	//寻卡成功
				flag_App_or_Ctr = 0x09;							//寻卡成功
				if(flag_upload_uid_once)						//如果置位单次上传卡号标志，则让卡休眠，卡号只上传一次
				{
					flag_upload_uid_once = false;
					findIC_flow = 0x08;
				}
				else
					findIC_flow = 0x00;				
				break;
				
			case 0x08:	//卡休眠
				PcdHalt();												//使读到卡号的卡进入休眠
				findIC_flow = 0x00;
				break;
			
			default	:
				findIC_flow = 0x00;
		}
	}
}
/////////////////////////////////////////////////////////////////////
//响应上位机发送的天线命令
/////////////////////////////////////////////////////////////////////
void ComPcdAntenna(void)
{
    if (!Buf_AppToCtrRC500[4])
    {   PcdAntennaOff();   }
    else
    {   PcdAntennaOn();    }
    AnswerCommandOk();  
}

/////////////////////////////////////////////////////////////////////
//响应上位机发送的寻A卡命令
/////////////////////////////////////////////////////////////////////
uint8_t ComRequestA(void)
{
    uint8_t atq[2];
    uint8_t status;
	
	memset(g_cardType, 0, 40);
	status = PcdRequest(Buf_AppToCtrRC500[4],atq);
	g_cardType[0] = atq[0];
	g_cardType[1] = atq[1];
	if( (g_cardType[0] & 0x40) == 0x40)
	{
		uid_len = 8;
	}
	else
	{
		uid_len = 4;
	}
	
//	memcpy(&g_cSNR[8], g_cardType, 2);
	
	return (status); 	
}  

/////////////////////////////////////////////////////////////////////
//响应上位机发送的A卡防冲撞命令
/////////////////////////////////////////////////////////////////////
uint8_t ComAnticoll(void)
{ 
	uint8_t status;
	status = PcdAnticoll(PICC_ANTICOLL1, g_cSNR);
//	memcpy(&g_cardType[2], g_cSNR, 4);
    return(status) ;	
}

/////////////////////////////////////////////////////////////////////
//响应上位机发送的A卡防冲撞命令
/////////////////////////////////////////////////////////////////////
uint8_t ComAnticol2(void)
{ 
	uint8_t status;
	status = PcdAnticoll(PICC_ANTICOLL2, &g_cSNR[4]);
//	memset(g_cardType, 0, 20);
//	memcpy(&g_cardType[6], &g_cSNR[4], 4);
    return(status) ;	
}

/////////////////////////////////////////////////////////////////////
//响应上位机发送的A卡锁定命令
/////////////////////////////////////////////////////////////////////
void ComSelect1(void)
{
	uint8_t len = 0;
    if (MI_OK == PcdSelect1(g_cSNR, respon, &len))
    {    
//		memcpy(&g_cSNR[10], respon, 1);
//		memcpy(&g_cardType[10], respon, 1);
        AnswerOk(respon, len);   
    }
    else
    {    
		AnswerErr(11);   
	}    	
}  

/////////////////////////////////////////////////////////////////////
//响应上位机发送的A卡锁定命令
/////////////////////////////////////////////////////////////////////
void ComSelect2(void)
{
	uint8_t len = 0;
    if (MI_OK == PcdSelect2(&g_cSNR[4], respon, &len))
    {    
//		memcpy(&g_cSNR[15], respon, 1);
//		memcpy(&g_cardType[15], respon, 1);
        AnswerOk(respon, len);   
    }
    else
    {    
		AnswerErr(11);     	
	}
}  


/////////////////////////////////////////////////////////////////////
//响应上位机发送的A卡休眠命令
/////////////////////////////////////////////////////////////////////
void ComHlta(void)
{
    if (MI_OK == PcdHalt())
    {   
		AnswerCommandOk();  
    }
    else
    {    AnswerErr(11);   }    	
}

/////////////////////////////////////////////////////////////////////
//响应上位机发送的A卡验证密钥命令
//Buf_AppToCtrRC500[4]:密码验证方式
//Buf_AppToCtrRC500[5]:要验证的块号
//Buf_AppToCtrRC500[6]6字节密钥
/////////////////////////////////////////////////////////////////////
void ComAuthentication(void)
{	
    uint8_t status;
    if (MI_OK == ChangeCodeKey(&Buf_AppToCtrRC500[6],&Buf_AppToCtrRC500[12]))                       //转换密钥格式
    {    
    	if (MI_OK == PcdAuthKey(&Buf_AppToCtrRC500[12]))                                     //传送密钥到RC500FIFO
        {
             status = PcdAuthState(Buf_AppToCtrRC500[4], Buf_AppToCtrRC500[5], g_cSNR);     //验证密钥
        }
    }
    if (status == MI_OK)
    {   
	   AnswerCommandOk();  
    }
    else
    {    
	    AnswerErr(11); 
    }
}    

/////////////////////////////////////////////////////////////////////
//响应上位机读M1卡命令
/////////////////////////////////////////////////////////////////////
void ComM1Read(void)
{
    if (MI_OK == PcdRead(Buf_AppToCtrRC500[4], &Buf_CtrRC500return[0]))
    {	 AnswerOk(&Buf_CtrRC500return[0], 16);  }
    else
    {    AnswerErr(11);            }
}

/////////////////////////////////////////////////////////////////////
//响应上位机写M1卡命令
//Buf_AppToCtrRC500[4]:要写入数据的块号
//Buf_AppToCtrRC500[5]:连续16字节要写入的数据
/////////////////////////////////////////////////////////////////////
void ComM1Write(void)
{
    if (MI_OK == PcdWrite(Buf_AppToCtrRC500[4], &Buf_AppToCtrRC500[5]))
    {    AnswerCommandOk();    }
    else
    {    AnswerErr(11);   }
}

/////////////////////////////////////////////////////////////////////
//响应上位机初始化钱包命令
//g_cReceBuf[4]:块号
//g_cReceBuf[5]:4字节初始值，低字节在前
//钱包格式：4字节钱包值＋4字节钱包值取反＋4字节钱包值＋1字节块号＋1字节块号取反＋1字节块号＋1字节块号取反
/////////////////////////////////////////////////////////////////////
void ComM1Initval(void)
{
//    Buf_AppToCtrRC500[5]=0;
//    Buf_AppToCtrRC500[6]=0;
//    Buf_AppToCtrRC500[7]=0;
//    Buf_AppToCtrRC500[8]=0;

    Buf_AppToCtrRC500[9]=~Buf_AppToCtrRC500[5];
    Buf_AppToCtrRC500[10]=~Buf_AppToCtrRC500[6];
    Buf_AppToCtrRC500[11]=~Buf_AppToCtrRC500[7];
    Buf_AppToCtrRC500[12]=~Buf_AppToCtrRC500[8];
    
    Buf_AppToCtrRC500[13]=Buf_AppToCtrRC500[5];
    Buf_AppToCtrRC500[14]=Buf_AppToCtrRC500[6];
    Buf_AppToCtrRC500[15]=Buf_AppToCtrRC500[7];
    Buf_AppToCtrRC500[16]=Buf_AppToCtrRC500[8];
    
    Buf_AppToCtrRC500[17]=Buf_AppToCtrRC500[4];
    Buf_AppToCtrRC500[18]=~Buf_AppToCtrRC500[4];
    Buf_AppToCtrRC500[19]=Buf_AppToCtrRC500[4];
    Buf_AppToCtrRC500[20]=~Buf_AppToCtrRC500[4];
    
    if (MI_OK == PcdWrite(Buf_AppToCtrRC500[4], &Buf_AppToCtrRC500[5]))
    {    AnswerCommandOk();    }
    else
    {    AnswerErr(11);   }
}

/////////////////////////////////////////////////////////////////////
//响应上位机读钱包命令
/////////////////////////////////////////////////////////////////////
void ComM1Readval(void)
{
    if (MI_OK == PcdRead(Buf_AppToCtrRC500[4], &Buf_CtrRC500return[0]))
    {	 AnswerOk(&Buf_CtrRC500return[0], 4);   }
    else
    {    AnswerErr(11);         }
}

/////////////////////////////////////////////////////////////////////
//响应上位机扣款命令
//Buf_AppToCtrRC500[4]:块号
//Buf_AppToCtrRC500f[5]:4字节扣款值，低字节在前
/////////////////////////////////////////////////////////////////////
void ComM1Decrement()
{
    if (MI_OK == PcdValue(PICC_DECREMENT, Buf_AppToCtrRC500[4], &Buf_AppToCtrRC500[5]))
    {    AnswerCommandOk(); 
	}
    else
    {    AnswerErr(11);   }
}

/////////////////////////////////////////////////////////////////////
//响应上位机充值命令
//Buf_AppToCtrRC500[4]:块号
//Buf_AppToCtrRC500[5]:4字节充值金额，低字节在前
/////////////////////////////////////////////////////////////////////
void ComM1Increment(void)
{
    if (MI_OK == PcdValue(PICC_INCREMENT, Buf_AppToCtrRC500[4], &Buf_AppToCtrRC500[5]))
    {    AnswerCommandOk(); 
	}
    else
    {    AnswerErr(11);   }
}

/////////////////////////////////////////////////////////////////////
//响应上位机M1卡备份钱包命令
//Buf_AppToCtrRC500[4]):源地址块号
//Buf_AppToCtrRC500[5]):目标地址块号，必须和源地址在同一扇区内
/////////////////////////////////////////////////////////////////////
void ComM1BakValue()
{
	uint8_t status;
	if(MI_OK == PcdRestore(Buf_AppToCtrRC500[4]))
	{
		status = PcdTransfer(Buf_AppToCtrRC500[5])	;
	}
	if(status == MI_OK)
	{
		AnswerCommandOk();
	}
	else
	{
		AnswerErr(11); 
	}
}

/////////////////////////////////////////////////////////////////////
//响应上位机UltraLght卡防冲撞命令
/////////////////////////////////////////////////////////////////////
void ComUL_PcdAnticoll()
{
    if (MI_OK == UL_PcdAnticoll(&Buf_AppToCtrRC500[0]))
    {   AnswerOk(&Buf_AppToCtrRC500[0], 7);  }
    else
    {   AnswerErr(11);    }    
}

/////////////////////////////////////////////////////////////////////
//响应上位机写UltraLight卡命令
//Buf_AppToCtrRC500[4]:要写入数据的页号
//Buf_AppToCtrRC500[5]:连续4字节要写入的数据
/////////////////////////////////////////////////////////////////////
void ComUL_PcdWrite()
{
    if (MI_OK == UL_PcdWrite(Buf_AppToCtrRC500[4], &Buf_AppToCtrRC500[5]))
    {   AnswerCommandOk();   }
    else
    {   AnswerErr(11);  }    
}

/////////////////////////////////////////////////////////////////////
//响应上位机Mifare_Pro(X)卡高级复位命令
/////////////////////////////////////////////////////////////////////
void ComTypeARst()
{
    uint8_t status;
	uint8_t *response = NULL;
	uint8_t len = 0;

    status = PcdRequest(Buf_AppToCtrRC500[4], &Buf_AppToCtrRC500[0]);
    
    if (status == MI_OK)
    {   status =  PcdAnticoll(PICC_ANTICOLL1, &Buf_AppToCtrRC500[1]);   }
    
    if (status == MI_OK)
    {   status =  PcdSelect1(&Buf_AppToCtrRC500[1], response, &len);   }
    
    if (status == MI_OK)    	
    {   status =  MifareProRst(0x40, &Buf_AppToCtrRC500[0], &Buf_AppToCtrRC500[5]);    }
    
    if (status == MI_OK)
    {	
        Buf_AppToCtrRC500[0] += 4;
        g_cCid     = 0;//(g_cReceBuf[9] & 0x03) << 2;
        AnswerOk(&Buf_AppToCtrRC500[1], Buf_AppToCtrRC500[0]);
    }
    else
    {   AnswerErr(11);    }
}   

/////////////////////////////////////////////////////////////////////
//响应上位机T=CL卡COS命令
/////////////////////////////////////////////////////////////////////
void ComTCLCOS()
{
    uint8_t ucLg;
	ucLg =  Buf_AppToCtrRC500[1] - 3;
    if (MI_OK == ISO14443CLCom(g_cCid, &Buf_AppToCtrRC500[4], ucLg, &Buf_AppToCtrRC500[1], &Buf_AppToCtrRC500[0]))
    {	AnswerOk(&Buf_AppToCtrRC500[1], Buf_AppToCtrRC500[0]);   }
    else
    {   AnswerErr(11);    }
} 

/////////////////////////////////////////////////////////////////////
//响应上位机T=CL卡退出激活状态命令
/////////////////////////////////////////////////////////////////////
void ComCL_Deselect()
{
    if (MI_OK == CL_Deselect(0))
    {    AnswerCommandOk();    }
    else
    {    AnswerErr(11);   }
}
/////////////////////////////////////////////////////////////////////
//正确执行完上位机指令，无返回数据
/////////////////////////////////////////////////////////////////////
void AnswerCommandOk(void)
{
	if(flag_App_or_Ctr == 0)
	{
		Buf_CtrRC500ToApp[0] = 0x00;
		Buf_CtrRC500ToApp[1] = 0x00;
		Buf_CtrRC500ToApp[2] = 0x04;
		Buf_CtrRC500ToApp[3] = Buf_AppToCtrRC500[3];
		Buf_CtrRC500ToApp[4] = 0x00;	 //成功标志位
		Buf_CtrRC500ToApp[5] = XOR_Cal(&Buf_CtrRC500ToApp[0],5);
		flag_App_or_Ctr = 0x05;	
		Length_CtrRC500ToApp = 6;	
	}	
}

/////////////////////////////////////////////////////////////////////
//正确执行完上位机指令，有返回数据
//input：answerdata = 应答数据
//       answerlen  = 数据长度
/////////////////////////////////////////////////////////////////////
void AnswerOk(uint8_t *answerdata, unsigned int answerlen)
{
	if(flag_App_or_Ctr == 0)
	{
		Buf_CtrRC500ToApp[0] = 0x00;
		Buf_CtrRC500ToApp[1] = 0x00;
		Buf_CtrRC500ToApp[2] = answerlen + 4;
		Buf_CtrRC500ToApp[3] = Buf_AppToCtrRC500[3];
		Buf_CtrRC500ToApp[4] = 0x00;	 //成功标志位
		memcpy(&Buf_CtrRC500ToApp[5], answerdata, answerlen);
		Buf_CtrRC500ToApp[Buf_CtrRC500ToApp[2]+1]= XOR_Cal(&Buf_CtrRC500ToApp[0],answerlen+5);
		flag_App_or_Ctr = 0x05;  
		Length_CtrRC500ToApp = answerlen + 6; 	
//		memcpy(&Buf_CtrRC500return[0],answerdata,answernum);	
	}
}

/////////////////////////////////////////////////////////////////////
//上位机指令执行出错
/////////////////////////////////////////////////////////////////////
void AnswerErr(int faultcode)
{
	if(flag_App_or_Ctr == 0)
	{
		Buf_CtrRC500ToApp[0] = 0x00;
		Buf_CtrRC500ToApp[1] = 0x00;
		Buf_CtrRC500ToApp[2] = 0x04;
		Buf_CtrRC500ToApp[3] = Buf_AppToCtrRC500[3];
		Buf_CtrRC500ToApp[4] = faultcode  ;	 //错误代码
		Buf_CtrRC500ToApp[5] = XOR_Cal(&Buf_CtrRC500ToApp[0],5);
		flag_App_or_Ctr = 0x05;	
		Length_CtrRC500ToApp = 6; 		
	}
}


/*
 *********************************************************************************
 *    新增命令
 *********************************************************************************
 */
/*********************************************************************************
* 功	能：void ComSelectApplication(void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
void ComSelectApplication (void)
{
	uint8_t len = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	if( status = PcdRATS(respon, &len), ( (MI_OK == status) && (respon[0] == 0x05) && (respon[1] == 0x78) && (respon[4] == 0x02) ) )
	{   
		memset(respon, 0, BUF_LEN);
		if( status = PcdPPS(respon, &len), ( (MI_OK == status) && (respon[0] == 0xD0) ) )
		{
			memset(respon, 0, BUF_LEN);
			if( status = PcdSelectApplication(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
			{	
				AnswerOk(&respon[1], len - 1);	
			}
			else
			{	AnswerErr(11); 		}  
		}
		else
		{	AnswerErr(11); 	}   
	}
	else
	{    AnswerErr(11);   }
}

/*********************************************************************************
* 功	能：void ComSendInterrupt (void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
void ComSendInterrupt (void)
{
	uint8_t len = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		if( status = PcdSelectSystemfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{   
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadSystemfile(0x0004, 0x01, respon, &len), ( (MI_OK == status) && ((respon[1] & 0x40 ) == 0x40) ) )
			{
				memset(respon, 0, BUF_LEN);
				if( status = PcdSendInterrupt(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
				{
					AnswerOk(&respon[1], len - 1);
				}
				else
				{
					AnswerErr(11); 
				}  
			}
			else
			{
				AnswerErr(11); 
			}
		}
		else
		{    
			AnswerErr(11);     	
		}
	}
	else
	{    
		AnswerErr(11);     	
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
	if( status = PcdSelectCCfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
	{   
		memset(respon, 0, BUF_LEN);
		if( status = PcdReadCCfileLength(respon, &len), (MI_OK == status) )
		{
			NDEF_DataRead = (uint16_t)( (respon[1] << 8) | (respon[2]) );
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadCCfile(0x0000, NDEF_DataRead, respon, &len), (MI_OK == status) )
			{
				return (MI_OK); 
			}
			else
			{
				return (1); 
			} 
		}
		else
		{
			return (1); 
		} 
	}
	else
	{    
		return (1);      	
	}
}

/*********************************************************************************
* 功	能：void ComReadCCfile (void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
void ComReadCCfile (void)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	if( status = PcdSelectCCfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
	{   
		memset(respon, 0, BUF_LEN);
		if( status = PcdReadCCfileLength(respon, &len), (MI_OK == status) )
		{
			NDEF_DataRead = (uint16_t)( (respon[1] << 8) | (respon[2]) );
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadCCfile(0x0000, NDEF_DataRead, respon, &len), (MI_OK == status) )
			{
				AnswerOk(&respon[1], len - 1);
			}
			else
			{
				AnswerErr(11); 
			} 
		}
		else
		{
			AnswerErr(11); 
		} 
	}
	else
	{    
		AnswerErr(11);     	
	}
}

/*********************************************************************************
* 功	能：void ComReadSystemfile (void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
void ComReadSystemfile (void)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead = 0;
	uint8_t status = 0;
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdSelectSystemfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{   
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadSystemfileLength(respon, &len), (MI_OK == status) )
			{
				NDEF_DataRead = (uint16_t)( (respon[1] << 8) | (respon[2]) );
				memset(respon, 0, BUF_LEN);
				if( status = PcdReadSystemfile(0x0000, NDEF_DataRead, respon, &len), (MI_OK == status) )
				{
					AnswerOk(&respon[1], len - 1);
				}
				else
				{
					AnswerErr(11); 
				} 
			}
			else
			{
				AnswerErr(11); 
			} 
		}
		else
		{    
			AnswerErr(11);     	
		}
	}
	else
	{    
		AnswerErr(11);     	
	}
}

/*********************************************************************************
* 功	能：void ComReadNDEFfile (void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
void ComReadNDEFfile (void)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead_Len = 0;
	uint8_t status = 0;
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdSelectNDEFfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{   
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadNDEFfileLength(respon, &len), (MI_OK == status) )
			{
				NDEF_DataRead_Len = ((uint16_t)respon[1] << 8) | respon[2] ;
				memset(respon, 0, BUF_LEN);
				if( status = PcdReadNDEFfile(0x0000, NDEF_DataRead_Len + 2, respon, &len), (MI_OK == status) )
				{
					AnswerOk(&respon[1], len - 1);
				}
				else
				{
					AnswerErr(11); 
				} 
			}
			else
			{
				AnswerErr(11); 
			} 
		}
		else
		{    
			AnswerErr(11);     	
		}
	}
	else
	{    
		AnswerErr(11);     	
	}
}

/*********************************************************************************
* 功	能：void ComWriteNDEFfile (uint16_t NbByteToWrite, uint8_t *pDataToWrite)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
void ComWriteNDEFfile (uint16_t NbByteToWrite, uint8_t *pDataToWrite)
{	
	uint8_t len = 0, FWTbyte = 0;
	uint8_t EraseLen[2] = {0x00, 0x00};
	uint8_t status = 0;
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdSelectNDEFfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{   
			memset(respon, 0, BUF_LEN);
			if( status = PcdWriteNDEFfile(0x0000, 0x02, EraseLen, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
			{
				memset(respon, 0, BUF_LEN);
				status = PcdWriteNDEFfile(0x0002, NbByteToWrite, pDataToWrite, respon, &len);			//??????
				
				if( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) )					//????????????????
				{
					memset(respon, 0, BUF_LEN);
					pDataToWrite[0] = (uint8_t)( NbByteToWrite >> 8 );
					pDataToWrite[1] = (uint8_t)( NbByteToWrite & 0xFF );
					
					if( status = PcdWriteNDEFfile(0x0000, 0x02, pDataToWrite, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ))
					{
						AnswerOk(&respon[1], len - 1);
					}
					else
					{
						AnswerErr(11); 
					} 
				}
				else if((MI_OK == status) && ((respon[0] & 0xC0) == 0xC0) )		//modify by tianerjun, before is respon[0] == 0xF2							//????????????WTX
				{
					FWTbyte = respon[1];
					memset(respon, 0, BUF_LEN);
					if( status = PcdFWTExtension(FWTbyte, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00)) )
					{							
						pDataToWrite[0] = (uint8_t)( NbByteToWrite >> 8 );
						pDataToWrite[1] = (uint8_t)( NbByteToWrite & 0xFF );
						
						if( status = PcdWriteNDEFfile(0x0000, 0x02, pDataToWrite, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ))
						{
							AnswerOk(&respon[1], len - 1);
						}
						else
						{
							AnswerErr(11); 
						} 
					}
					else
					{
						AnswerErr(11);
					}
				}
				else
				{
					AnswerErr(11); 
				} 
			}
			else
			{    
				AnswerErr(11);     	
			}
		}
		else
		{    
			AnswerErr(11);     	
		}
	}
	else
	{    
		AnswerErr(11);     	
	}
}

/*********************************************************************************
* 功	能：void ComDeselect (void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
void ComDeselect (void)
{
	uint8_t len = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	if( status = PcdDeselect(respon, &len), ( (MI_OK == status) && (respon[0] == 0xC2) && (respon[1] == 0xE0) && (respon[2] == 0xB4)) )
	{   
		AnswerOk(&respon[0], len);
	}
	else
	{    
		AnswerErr(11);     	
	}
}
/*
 *********************************************************************************
 *    软件接口函数
 *********************************************************************************
 */
/*********************************************************************************
* 功	能：void FindICCard(void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
uint8_t FindICCard(void)
{
	uint8_t len,findIC_flow = 0x01;
	uint8_t status = 0x11;
	while(findIC_flow)
	{
		switch(findIC_flow)
		{
			case 0x01:	// 打开天线
				PcdAntennaOn();												//打开13.56M天线
				findIC_flow = 0x02;
				break;
			
			case 0x02:	//发送reqA指令
				memset(g_cardType, 0, 40);
				if(PcdRequest(PICC_REQIDL,g_cardType) == MI_OK)				//请求A卡，返回卡类型，不同类型卡对应不同的UID长度
				{	
					if( (g_cardType[0] & 0x40) == 0x40)
					{	uid_len = 8;	}
					else
					{	uid_len = 4;	}
					findIC_flow = 0x03;
				}
				else
				{
					findIC_flow = 0x00;
				}
				break;
				
			case 0x03:	//防碰撞1
				if(PcdAnticoll(PICC_ANTICOLL1, g_cSNR) == MI_OK )
					findIC_flow = 0x04;
				else
					findIC_flow = 0x02;
				break;
				
			case 0x04:	//选卡1	
				memset(respon, 0, BUF_LEN);
				if (MI_OK == PcdSelect1(g_cSNR, respon, &len))				
				{
					if((uid_len == 8) && ((respon[0] & 0x04) == 0x04))
						findIC_flow = 0x05;
					else if(uid_len == 4)
						findIC_flow = 0x07;
				}
				else
					findIC_flow = 0x02;
				break;
				
			case 0x05:	//防碰撞2
				memset(respon, 0, BUF_LEN);
				if(MI_OK == PcdAnticoll(PICC_ANTICOLL2, &g_cSNR[4]))		
				{
					findIC_flow = 0x06;
				}
				else
					findIC_flow = 0x02;		
				break;
				
			case 0x06:	//选卡2
				if((MI_OK == PcdSelect2(&g_cSNR[4], respon, &len))&&((respon[0] & 0x20) == 0x20))
				{
					findIC_flow = 0x07;
				}
				else
					findIC_flow = 0x02;		
				break;
				
			case 0x07:	//寻卡成功
				if(flag_upload_uid_once)						//如果置位单次上传卡号标志，则让卡休眠，卡号只上传一次
				{
					flag_upload_uid_once = false;
					findIC_flow = 0x08;
					status = MI_OK;
				}
				else
				{
					findIC_flow = 0x00;
					status = MI_OK;
				}				
				break;
				
			case 0x08:	//卡休眠
				PcdHalt();												//使读到卡号的卡进入休眠
				findIC_flow = 0x00;
				break;
			default	:
				findIC_flow = 0x00;
		}
	}
	return status;
}
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
	if( status = PcdRATS(respon, &len), ( (MI_OK == status) && (respon[0] == 0x05) && (respon[1] == 0x78) && (respon[4] == 0x02) ) )
	{   
		memset(respon, 0, BUF_LEN);
		if( status = PcdPPS(respon, &len), ( (MI_OK == status) && (respon[0] == 0xD0) ) )
		{
			memset(respon, 0, BUF_LEN);
			if( status = PcdSelectApplication(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
			{	return status;		}
			else
			{	return 0x11; 		}  
		}
		else
		{	return 0x11; 	}   
	}
	else
	{    return 0x11;   }
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
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		if( status = PcdSelectSystemfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{   
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadSystemfile(0x0004, 0x01, respon, &len), ( (MI_OK == status) && ((respon[1] & 0x40 ) == 0x40) ) )
			{
				memset(respon, 0, BUF_LEN);
				if( status = PcdSendInterrupt(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
				{	return status;		}
				else
				{	return 0x11; 		}  
			}
			else
			{	return 0x11; 	}
		}
		else
		{  	return 0x11;   }
	}
	else
	{  	return 0x11;  }
}

/*********************************************************************************
* 功	能：void ComReadCCfile (void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
uint8_t ReadCCfile (uint8_t *CCfileData, uint8_t *CCfileLen)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	if( status = PcdSelectCCfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
	{   
		memset(respon, 0, BUF_LEN);
		if( status = PcdReadCCfileLength(respon, &len), (MI_OK == status) )
		{
			NDEF_DataRead = (uint16_t)( (respon[1] << 8) | (respon[2]) );
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadCCfile(0x0000, NDEF_DataRead, respon, &len), (MI_OK == status) )
			{
				memcpy(CCfileData, &respon[1], len - 1);
				*CCfileLen = len - 1;
				return status;
			}
			else
			{	return 0x11; 	} 
		}
		else
		{	return 0x11; 	} 
	}
	else
	{  	return 0x11;   }
}

/*********************************************************************************
* 功	能：void ComReadSystemfile (void)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
uint8_t ReadSystemfile (uint8_t *Systemfile_Data, uint8_t *Systemfile_len)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead = 0;
	uint8_t status = 0;
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdSelectSystemfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{   
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadSystemfileLength(respon, &len), (MI_OK == status) )
			{
				NDEF_DataRead = (uint16_t)( (respon[1] << 8) | (respon[2]) );
				memset(respon, 0, BUF_LEN);
				if( status = PcdReadSystemfile(0x0000, NDEF_DataRead, respon, &len), (MI_OK == status) )
				{
					*Systemfile_len = len - 1;
					memcpy(Systemfile_Data, &respon[1], len - 1);
					return status;
				}
				else
				{	return 0x11; 	} 
			}
			else
			{	return 0x11; 	} 
		}
		else
		{   return 0x11; 	}
	}
	else
	{ 	return 0x11; 	}
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
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdSelectNDEFfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{   
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadNDEFfileLength(respon, &len), (MI_OK == status) )
			{
				NDEF_DataRead_Len = ((uint16_t)respon[1] << 8) | respon[2] ;
				memset(respon, 0, BUF_LEN);
				if( status = PcdReadNDEFfile(0x0000, NDEF_DataRead_Len + 2, respon, &len), (MI_OK == status) )
				{
					memcpy(NDEFfile_Data, &respon[1], NDEF_DataRead_Len + 2);
					*NDEFfile_len = NDEF_DataRead_Len + 2;
					return status;
				}
				else
				{	return 0x11; } 
			}
			else
			{	return 0x11; } 
		}
		else
		{  	return 0x11; }
	}
	else
	{ 	return 0x11; }
}

/*********************************************************************************
* 功	能：void ComWriteNDEFfile (uint16_t NbByteToWrite, uint8_t *pDataToWrite)
* 输    入: NULL
* 返	回：MI_OK : the function is succesful. 
* 备	注：
*********************************************************************************/
uint8_t WriteNDEFfile (uint8_t *pDataToWrite)
{	
	uint8_t len = 0, FWTbyte = 0;
	uint8_t EraseLen[2] = {0x00, 0x00};
	uint8_t status = 0;
	uint16_t NbByteToWrite = ((uint16_t)*pDataToWrite << 8) | *(pDataToWrite + 1);
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdSelectNDEFfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{   
			memset(respon, 0, BUF_LEN);
			if( status = PcdWriteNDEFfile(0x0000, 0x02, EraseLen, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
			{
				memset(respon, 0, BUF_LEN);
				status = PcdWriteNDEFfile(0x0002, NbByteToWrite, (pDataToWrite + 2), respon, &len);	
//				status = PcdWriteNDEFfile(0x0000, NbByteToWrite, pDataToWrite, respon, &len);	
				app_debuglog_dump(respon, len);
				
				if( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) )
				{
					memset(respon, 0, BUF_LEN);
					if( status = PcdWriteNDEFfile(0x0000, 0x02, pDataToWrite, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ))
					{	return status;	}
					else
					{	return 0x11; 	} 
				}
				else if((MI_OK == status) && ((respon[0] & 0xC0) == 0xC0) )		//modify by tianerjun, before is respon[0] == 0xF2
				{
					FWTbyte = respon[1];
					memset(respon, 0, BUF_LEN);
					if( status = PcdFWTExtension(FWTbyte, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00)) )
					{							
						if( status = PcdWriteNDEFfile(0x0000, 0x02, pDataToWrite, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ))
						{	return status;		}
						else
						{	return 0x11; 		} 
					}
					else
					{	return 0x11;	}
				}
				else
				{	return 0x11;	} 
			}
			else
			{    return 0x11; 	}
		}
		else
		{  	return 0x11; 	}
	}
	else
	{  	return 0x11;  }
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
	
	if( status = PcdDeselect(respon, &len), ( (MI_OK == status) && (respon[0] == 0xC2) && (respon[1] == 0xE0) && (respon[2] == 0xB4)) )
	{  	return status;	}
	else
	{   return 0x11;  	}
}
/**************************************END OF FILE****************************/

#include "stm32f10x_tim.h"
#include "wiegand.h"

uint8_t wiegandhead;
uint8_t wieganddata[3];
uint8_t wieganddata34[4];
uint8_t wiegandend;
uint8_t wiegandcnt;
uint8_t wiegandflag;
bool	wiegandSend;
volatile uint16_t wiegandDelay;
volatile uint16_t wiegandtimeout;
void WEIGEN_HWInit(void){
  	Data0(HIGH);
	Data1(HIGH);
	WEIGEN_SWInit();	
}

void WEIGEN_SWInit(void){
	wiegandSend = FALSE;
	wiegandcnt = 0x00;
	wiegandflag = 0x00;
	// TIM_Cmd(TIM3, DISABLE);
	wiegandDelay = 0x00;
	wiegandtimeout	= 0;
}

void WiegandInit(uint8_t *pSrc)
{
	uint8_t i,temp,even=0,odd=0;
	
	for(i=0;i<3;i++)
	{
		wieganddata[i] = *(pSrc+i);
	}
		
	temp = wieganddata[0]; 
	for(i=0;i<8;i++)
	{
		if(temp&0x80)
			even++;
	   	temp = (temp <<1);
	}
	temp = wieganddata[1];
	for(i=0;i<4;i++)
	{
		if(temp&0x80)
			even++;
		temp = (temp <<1);
	}
		
	wiegandhead = even%2;	//  偶校验结果
		
	for(i=0;i<4;i++)
	{
		if(temp&0x80)
			odd++;
    	temp = (temp <<1);
	}
	temp = wieganddata[2];
	for(i=0;i<8;i++)
	{
		if(temp&0x80)
			odd++;
		temp = (temp <<1);
	}

	wiegandend = (odd+1)%2;	// 奇校验结果
			
	if(wiegandhead)
		Data1(LOW);
	else
		Data0(LOW);
	TIM_SetAutoreload(Weigand_TIM, 200);
	wiegandflag = 1;
	wiegandSend = TRUE;
	wiegandcnt++;
	ledOn(LED2);
	ledOn(LGREEN);
	TIM_ITConfig(Weigand_TIM, TIM_IT_Update, ENABLE);
	TIM_Cmd(Weigand_TIM, ENABLE);
}

void WiegandOut(void)
{
	if(wiegandcnt<9)
	{
		if(wieganddata[0]&0x80)
			Data1(LOW);
		else
			Data0(LOW);
		wiegandcnt++;
		wieganddata[0] = wieganddata[0] << 1;		
	}
	else if(wiegandcnt<17)
	{
		if(wieganddata[1]&0x80)
			Data1(LOW);
		else
			Data0(LOW);
		wiegandcnt++;
		wieganddata[1] = wieganddata[1] << 1;
	}
	else if(wiegandcnt<25)
	{
		if(wieganddata[2]&0x80)
			Data1(LOW);
		else
			Data0(LOW);
		wiegandcnt++;
		wieganddata[2] = wieganddata[2] << 1;
	}
	else if(wiegandcnt<26)
	{
		if(wiegandend)
			Data1(LOW);
		else
			Data0(LOW);	
		wiegandcnt++;	
	}
	else
	{
		wiegandSend = FALSE;
	    wiegandcnt = 0;
		wiegandDelay = WIEGANDTIME;
		ledOff(LED2);
		ledOff(LGREEN);
		TIM_Cmd(Weigand_TIM, DISABLE);		
	}	
}
void WiegandInit34(uint8_t *pSrc)  
{
	uint8_t i,temp,even=0,odd=0;
	
	for(i=0;i<4;i++)
	{
		wieganddata34[i] = *(pSrc+i);
	}
		
	temp = wieganddata34[0]; 
	for(i=0;i<8;i++)
	{
		if(temp&0x80)
			even++;
	   	temp = (temp <<1);
	}
	temp = wieganddata34[1];
	for(i=0;i<8;i++)
	{
		if(temp&0x80)
			even++;
		temp = (temp <<1);
	}
		
	wiegandhead = even%2;	//  偶校验结果
		
	temp = wieganddata34[2];
	for(i=0;i<8;i++)
	{
		if(temp&0x80)
			odd++;
		temp = (temp <<1);
	}
	temp = wieganddata34[3];
		for(i=0;i<8;i++)
	{
		if(temp&0x80)
			odd++;
		temp = (temp <<1);
	}

	wiegandend = (odd+1)%2;	// 奇校验结果
			
	if(wiegandhead)
		Data1(LOW);
	else
		Data0(LOW);
	// TIM_SetAutoreload(TIM3, 200);
	wiegandSend = TRUE;
	wiegandflag = 1;
	wiegandcnt++;
	ledOn(LED2);
	ledOn(LGREEN);
	TIM_ITConfig(Weigand_TIM, TIM_IT_Update, ENABLE);
	TIM_Cmd(Weigand_TIM, ENABLE);	
}
void WiegandOut34(void)
{
	if(wiegandcnt<9)
	{
		if(wieganddata34[0]&0x80)
			Data1(LOW);
		else
			Data0(LOW);
		wiegandcnt++;
		wieganddata34[0] = wieganddata34[0] << 1;		
	}
	else if(wiegandcnt<17)
	{
		if(wieganddata34[1]&0x80)
			Data1(LOW);
		else
			Data0(LOW);
		wiegandcnt++;
		wieganddata34[1] = wieganddata34[1] << 1;
	}
	else if(wiegandcnt<25)
	{
		if(wieganddata34[2]&0x80)
			Data1(LOW);
		else
			Data0(LOW);
		wiegandcnt++;
		wieganddata34[2] = wieganddata34[2] << 1;
	}
	else if(wiegandcnt<33)
	{
		if(wieganddata34[3]&0x80)
			Data1(LOW);
		else
			Data0(LOW);
		wiegandcnt++;
		wieganddata34[3] = wieganddata34[3] << 1;
	}
	else if(wiegandcnt<34)
	{
		if(wiegandend)
			Data1(LOW);
		else
			Data0(LOW);	
		wiegandcnt++;	
	}
	else
	{
	    wiegandcnt = 0;
		wiegandSend = FALSE;
		wiegandDelay = WIEGANDTIME;
		ledOff(LED2);
		ledOff(LGREEN);
		TIM_Cmd(Weigand_TIM, DISABLE);		
	}			
}



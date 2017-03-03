/**
  ******************************************************************************
  * @file   	hal_nrf.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	hal function for nrf moulde
  ******************************************************************************
  */

#include "main.h"
#include "nrf.h"
#include "app_send_data_process.h"
#include "app_spi_send_data_process.h"

#ifdef NRF_DEBUG
#define nrf_debug  printf
#else
#define nrf_debug(...)
#endif

extern nrf_communication_t nrf_data;
extern uint16_t            list_tcb_table[13][8];
extern WhiteList_Typedef wl;
extern Revicer_Typedef   revicer;

#ifdef ZL_RP551_MAIN_F
void nrf1_spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  
	
	/* Configure SPI_MISO Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF1_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF1_SPI_MISO_PORT, &GPIO_InitStructure);

	/* Configure SPI_MOSI Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF1_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF1_SPI_MOSI_PORT, &GPIO_InitStructure);

	/* Configure SPI_SCK Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF1_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF1_SPI_SCK_PORT, &GPIO_InitStructure);

	/* Configure SPI_CSN Pin */								//CSN ����
	GPIO_InitStructure.GPIO_Pin   = NRF1_SPI_CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF1_SPI_CSN_PORT, &GPIO_InitStructure);

	/* Configure SPI_CE Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF1_SPI_CE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF1_SPI_CE_PORT, &GPIO_InitStructure);

	/* Configure SPI_IRQ Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF1_SPI_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(NRF1_SPI_IRQ_PORT, &GPIO_InitStructure);

	/* NRF1_SPI��ز������� */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	/* ����Ϊ�� */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	/* ��һ����ƽ��ȡ�ź�  ģʽ0 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	/* ������2M */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);

  /* Connect EXTI5 Line to PC.05 pin */
  GPIO_EXTILineConfig(NRF1_RFIRQ_PortSource, GPIO_PinSource5);
	
	EXTI_InitStructure.EXTI_Line    = NRF1_EXTI_LINE_RFIRQ;
	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* NRF1_SPI�ж����� */
	NVIC_PriorityGroupConfig(SYSTEM_MVIC_GROUP_SET);
	NVIC_InitStructure.NVIC_IRQChannel = NRF1_RFIRQ_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NRF1_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NRF_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
	NRF1_CSN_HIGH();		
}

void nrf2_spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);      
	
	/* Configure SPI_MISO Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF2_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF2_SPI_MISO_PORT, &GPIO_InitStructure);

	/* Configure SPI_MOSI Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF2_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF2_SPI_MOSI_PORT, &GPIO_InitStructure);

	/* Configure SPI_SCK Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF2_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF2_SPI_SCK_PORT, &GPIO_InitStructure);

	/* Configure SPI_CSN Pin */								//CSN ����
	GPIO_InitStructure.GPIO_Pin   = NRF2_SPI_CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF2_SPI_CSN_PORT, &GPIO_InitStructure);

	/* Configure SPI_CE Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF2_SPI_CE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF2_SPI_CE_PORT, &GPIO_InitStructure);

	/* Configure SPI_IRQ Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF2_SPI_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(NRF2_SPI_IRQ_PORT, &GPIO_InitStructure);

	/* NRF2_SPI��ز������� */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	/* ����Ϊ�� */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	/* ��һ����ƽ��ȡ�ź�  ģʽ0 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	/* ������2M */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_Cmd(SPI2, ENABLE);
	NRF2_CSN_HIGH();		
}
#endif
/******************************************************************************
  Function:my_nrf_transmit_start
  Description:
  Input:
	    data_buff��	   Ҫ���͵�����
			data_buff_len��Ҫ���͵����鳤��
			nrf_data_type�������������ͣ���Ч����:NRF_DATA_IS_USEFUL
			ACK		:        NRF_DATA_IS_ACK
  Output:
  Return:
  Others:ע�⣺ͨ�ŷ�ʽ���ƣ�����ͬһUID�������·����ݣ�ʱ��Ҫ���3S����
******************************************************************************/
void nrf51822_spi_init(void)
{
#ifdef ZL_RP551_MAIN_E
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Configure SPI_MISO Pin */
	GPIO_InitStructure.GPIO_Pin   = SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);

	/* Configure SPI_MOSI Pin */
	GPIO_InitStructure.GPIO_Pin   = SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);

	/* Configure SPI_SCK Pin */
	GPIO_InitStructure.GPIO_Pin   = SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_SCK_PORT, &GPIO_InitStructure);

	/* Configure SPI_CSN Pin */								//CSN_1 ����
	GPIO_InitStructure.GPIO_Pin   = SPI_CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_CSN_PORT, &GPIO_InitStructure);

	/* Configure SPI_CSN Pin */								//CSN_2 ����
	GPIO_InitStructure.GPIO_Pin   = SPI_CSN_PIN_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_CSN_PORT_2, &GPIO_InitStructure);

	/* Configure SPI_CE Pin */
	GPIO_InitStructure.GPIO_Pin   = SPI_CE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_CE_PORT, &GPIO_InitStructure);

	/* Configure SPI_IRQ Pin */
	GPIO_InitStructure.GPIO_Pin   = SPI_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SPI_IRQ_PORT, &GPIO_InitStructure);

	/* SPI��ز������� */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;			//����Ϊ��
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		//��һ����ƽ��ȡ�ź�  ģʽ0
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;		//������2M
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);

	/* SPI�ж����� */
	NVIC_PriorityGroupConfig(SYSTEM_MVIC_GROUP_SET);
	NVIC_InitStructure.NVIC_IRQChannel = RFIRQ_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NRF1_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NRF_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_EXTILineConfig(RFIRQ_PortSource, RFIRQ_PinSource);
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_Line = NRF1_EXTI_LINE_RFIRQ;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
	SPI_CSN_HIGH();	
	SPI_CSN_HIGH_2();	

	GPIOInit_SE2431L();
#endif

#ifdef ZL_RP551_MAIN_F
	nrf1_spi_init();
	nrf2_spi_init();
#endif
}



void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM3_PREEMPTION_PRIORITY;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM3_SUB_PRIORITY;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

}

/******************************************************************************
  Function:nrf_transmit_start
  Description:
  Input:	data_buff��	   Ҫ���͵�����
			data_buff_len��Ҫ���͵����鳤��
			nrf_data_type�������������ͣ���Ч����:NRF_DATA_IS_USEFUL
										ACK		:NRF_DATA_IS_ACK
  Output:
  Return:
  Others:ע�⣺ͨ�ŷ�ʽ���ƣ�����ͬһUID�������·����ݣ�ʱ��Ҫ���3S����
******************************************************************************/
void nrf_transmit_start( nrf_transmit_parameter_t *t_conf)
{
	/* data header */
	uint8_t i = 0;
	uint8_t send_delay = 0;
	
	if(t_conf->is_pac_add == 1)
	{
		revicer.sen_num++;
	}

	memset(nrf_data.tbuf,0,NRF_TOTAL_DATA_LEN);
	nrf_data.tbuf[i++]  = 0x61;
	memcpy((nrf_data.tbuf + i), t_conf->dist, 4);
	i = i + 4;
	memcpy((nrf_data.tbuf + i), nrf_data.jsq_uid, 4);
	i = i + 4;
	nrf_data.tbuf[i++] = 0x00; // device id
	nrf_data.tbuf[i++] = 0x00; // protocol version
	nrf_data.tbuf[i++] = revicer.sen_seq++;
	nrf_data.tbuf[i++] = revicer.sen_num;
	nrf_data.tbuf[i++] = t_conf->package_type;

	if(t_conf->is_add_table == 1)
	{
		nrf_data.tbuf[i++] = 0x0F; // ACK_TABLE_LEN
		memcpy(nrf_data.tbuf + i, list_tcb_table[t_conf->sel_table], 0x0F);
		i = i + 0x0F;
		nrf_data.tbuf[i++] = rf_var.cmd;
		send_delay = 1;
	}
	else
	{
		nrf_data.tbuf[i++] = 0x00; // ACK_TABLE_LEN
		nrf_data.tbuf[i++] = 0xFF;
		send_delay = 10;
	}
	nrf_data.tbuf[i++] = t_conf->data_len;
	
#ifdef OPEN_ACT_TABLE_SHOW
	{
		uint8_t i = 0, printf_flg = 0;
		uint8_t *pdata = (uint8_t *)list_tcb_table[t_conf->sel_table];
		printf("Seq:%2x Pac:%2x ",revicer.sen_seq-1,revicer.sen_num);
		switch(t_conf->package_type)
		{
			
			case 0: printf("DATA TABLE"); printf_flg = 1; break;
			case 2: printf("PRE  TABLE"); printf_flg = 1; break;
			default: break;
		}
		if(printf_flg == 1)
		{
			printf("[%2d]:",t_conf->sel_table);
			
			for(i=0;i<15;i++)
			{
				printf("%02x ",pdata[i]);
			}
			printf("\r\n");
		}
	}
#endif
	memcpy(nrf_data.tbuf+i,t_conf->data_buf,t_conf->data_len);
	i = i + t_conf->data_len;

	/* xor data */
	nrf_data.tbuf[i] = XOR_Cal(nrf_data.tbuf+1,i-1);
	i++;
	nrf_data.tbuf[i++] = 0x21;

	nrf_data.tlen = i;

	/* ��ʼͨѶ֮ǰ�ȷ�2�Σ�֮������ʱ�ж��ط����� */
	spi_send_data_write_tx_payload(nrf_data.tbuf,nrf_data.tlen,t_conf->transmit_count,t_conf->delay100us,send_delay);
}

/**************************************END OF FILE****************************/



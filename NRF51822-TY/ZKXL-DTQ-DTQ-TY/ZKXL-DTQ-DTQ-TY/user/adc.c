
/*-------------------------------- Include File -----------------------------------------*/
#include "adc.h"
/*-------------------------------- Macro definition -------------------------------------*/
#define ADC_BUFFER_SIZE 10  
/*-------------------------------- Local Variable ----------------------------------------*/
static nrf_adc_value_t       adc_buffer[ADC_BUFFER_SIZE]; /**< ADC buffer. */
static nrf_drv_adc_channel_t m_channel_config =  
{
	 {
		 {                                                       
			.resolution = NRF_ADC_CONFIG_RES_10BIT,                
			.input      = NRF_ADC_CONFIG_SCALING_SUPPLY_ONE_THIRD, 
			.reference  = NRF_ADC_CONFIG_REF_VBG,                  
			.ain        = NRF_ADC_CONFIG_INPUT_DISABLED                           
		 }
    }, 
	NULL
};

adc_struct_t				adc_struct;

/*-------------------------------- Gocal Variable ----------------------------------------*/
adc_struct_t				*ADC = &adc_struct;
/*-------------------------------- Function Declaration ----------------------------------*/
static void adc_event_handler(nrf_drv_adc_evt_t const * p_event);
static void adc_start_sample(void);
static void adc_update(void);
static void bubbleSort(int16_t *buf, uint16_t len);
static void swap(int16_t *a, int16_t *b);
/*----------------------------------------------------------------------------------------*/

void ADC_init(void)
{
    ret_code_t ret_code;
    nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;
	
    ret_code = nrf_drv_adc_init(&config, adc_event_handler);
    APP_ERROR_CHECK(ret_code);

    nrf_drv_adc_channel_enable(&m_channel_config);
	
	/* ADC结构体参数初始化 */
	ADC->sample_count   = 0;		//ADC采集次数，开机初始化为0，第0次采集时加快采集速度，否则LCD电量显示会有延时
	ADC->ok_flg 		= false;
	ADC->update_flg 	= true;		//开机采集一次ADC值
	ADC->start    		= adc_start_sample;
	ADC->update   		= adc_update;

}

static void adc_start_sample(void)
{
	nrf_drv_adc_buffer_convert(adc_buffer,ADC_BUFFER_SIZE);
	nrf_drv_adc_sample();
}
                            

/**< Channel instance. Default configuration used. */

/**
 * @brief ADC interrupt handler.
 */
static void adc_event_handler(nrf_drv_adc_evt_t const * p_event)
{
	uint8_t i;
	uint16_t adc_value,tmp;
    if (p_event->type == NRF_DRV_ADC_EVT_DONE)
    {
        ADC->ok_flg = true;			//读取结果放在adc_buffer[ADC_BUFFER_SIZE]中 
		
		bubbleSort(adc_buffer,ADC_BUFFER_SIZE);
		
		//去掉最大值和最小值，取8次的平均值
		for(i = 1;i < ADC_BUFFER_SIZE -1;i++)
        {
			adc_value = adc_value + adc_buffer[i];
        }
		adc_value = 3600 *  adc_value / (ADC_BUFFER_SIZE -2) / 1024;
		
//		tmp = adc_value / 1000;
//		LCD_diplay_segment(4,tmp);
//		tmp = adc_value % 1000;
//		tmp = tmp / 100;
//		LCD_diplay_segment(3,tmp);
//		tmp = adc_value % 100;
//		tmp = tmp / 10;
//		LCD_diplay_segment(2,tmp);
//		tmp = adc_value % 10;
//		LCD_diplay_segment(1,tmp);	
		
		/* LCD显示电池电量,斯密特原理，防止电池采集误差产生的LCD显示跳变 */
		if(ADC->sample_count)
		{
			if(adc_value > 2920)
				LCD->battery(BATTERY_LEVEL_3);
			else if(adc_value > 2820 && adc_value < 2880)
				LCD->battery(BATTERY_LEVEL_2);
			else if(adc_value > 2720 && adc_value < 2780)
				LCD->battery(BATTERY_LEVEL_1);
			else if(adc_value < 2680)
				LCD->battery(BATTERY_LEVEL_0);
			else
				;
		}
		else		//第一次采集电量在LCD上显示时，不能使用施密特方式，因为可能导致电量不显示；
		{
			if(adc_value >= 2900)
				LCD->battery(BATTERY_LEVEL_3);
			else if(adc_value >= 2800 && adc_value < 2900)
				LCD->battery(BATTERY_LEVEL_2);
			else if(adc_value >= 2700 && adc_value < 2800)
				LCD->battery(BATTERY_LEVEL_1);
			else
				LCD->battery(BATTERY_LEVEL_0);	
		}
		ADC->sample_count++;		//每更新一次LCD显示时，计数+1
    }
}

static void adc_update(void)
{	
	//每次定时器到都采集一次，adc_buffer满后，触发中断
	if(RADIO->rx_window_on_flg)		//如果RX窗处于打开状态，不采集电压，因为这时候采集会导致电压偏低
	{
		if(ADC->update_flg)
		{
			ADC->start();			
			ADC->update_flg = false;
		}
	}
}


void swap(int16_t *a, int16_t *b)  
{  
    uint16_t    temp;  
    temp = *a;  
      *a = *b;  
      *b = temp;  
}

//冒泡排序
void bubbleSort(int16_t *buf, uint16_t len)
{  
    uint16_t    i, j;  
    bool    	flag = 1;   
	
	//只要flag在下一次外循环条件检测的时候值为false，就说明已经排好序，不用继续循环
    for (i = 0; i < len && flag; i++)  
    {  
        flag = false;     		   
        for (j = len - 1; j > i; j--)  
        {  
            if (buf[j] < buf[j-1])  
            {  
                swap(&buf[j], &buf[j-1]);  
                flag = true;  							 //如果有交换，就将标记变量赋1  
            }  
        }  
    } 
} 







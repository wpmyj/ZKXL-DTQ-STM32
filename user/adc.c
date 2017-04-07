
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
	
	/* ADC�ṹ�������ʼ�� */
	ADC->sample_count   = 0;		//ADC�ɼ�������������ʼ��Ϊ0����0�βɼ�ʱ�ӿ�ɼ��ٶȣ�����LCD������ʾ������ʱ
	ADC->ok_flg 		= false;
	ADC->update_flg 	= true;		//�����ɼ�һ��ADCֵ
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
        ADC->ok_flg = true;			//��ȡ�������adc_buffer[ADC_BUFFER_SIZE]�� 
		
		bubbleSort(adc_buffer,ADC_BUFFER_SIZE);
		
		//ȥ�����ֵ����Сֵ��ȡ8�ε�ƽ��ֵ
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
		
		/* LCD��ʾ��ص���,˹����ԭ����ֹ��زɼ���������LCD��ʾ���� */
		if(ADC->sample_count)
		{
			if(adc_value > 2920)
				LCD_diplay_battery_level(BATTERY_LEVEL_3);
			else if(adc_value > 2820 && adc_value < 2880)
				LCD_diplay_battery_level(BATTERY_LEVEL_2);
			else if(adc_value > 2720 && adc_value < 2780)
				LCD_diplay_battery_level(BATTERY_LEVEL_1);
			else if(adc_value < 2680)
				LCD_diplay_battery_level(BATTERY_LEVEL_0);
			else
				;
		}
		else		//��һ�βɼ�������LCD����ʾʱ������ʹ��ʩ���ط�ʽ����Ϊ���ܵ��µ�������ʾ��
		{
			if(adc_value >= 2900)
				LCD_diplay_battery_level(BATTERY_LEVEL_3);
			else if(adc_value >= 2800 && adc_value < 2900)
				LCD_diplay_battery_level(BATTERY_LEVEL_2);
			else if(adc_value >= 2700 && adc_value < 2800)
				LCD_diplay_battery_level(BATTERY_LEVEL_1);
			else
				LCD_diplay_battery_level(BATTERY_LEVEL_0);	
		}
		ADC->sample_count++;		//ÿ����һ��LCD��ʾʱ������+1
//		LCD_display_student_id(adc_value / 10);
		
    }
}

static void adc_update(void)
{	
	//ÿ�ζ�ʱ�������ɼ�һ�Σ�adc_buffer���󣬴����ж�
	if(!RADIO->rx_window_on_flg)		//���RX�����ڴ�״̬�����ɼ���ѹ����Ϊ��ʱ��ɼ��ᵼ�µ�ѹƫ��
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

//ð������
void bubbleSort(int16_t *buf, uint16_t len)
{  
    uint16_t    i, j;  
    bool    	flag = 1;   
	
	//ֻҪflag����һ����ѭ����������ʱ��ֵΪfalse����˵���Ѿ��ź��򣬲��ü���ѭ��
    for (i = 0; i < len && flag; i++)  
    {  
        flag = false;     		   
        for (j = len - 1; j > i; j--)  
        {  
            if (buf[j] < buf[j-1])  
            {  
                swap(&buf[j], &buf[j-1]);  
                flag = true;  							 //����н������ͽ���Ǳ�����1  
            }  
        }  
    } 
} 








/*-------------------------------- Include File -----------------------------------------*/
#include "clock.h"
/*-------------------------------- Macro definition -------------------------------------*/

/*-------------------------------- Local Variable ----------------------------------------*/

clock_struct_t				clock_struct;

/*-------------------------------- Gocal Variable ----------------------------------------*/
clock_struct_t				*CLOCK = &clock_struct;
/*-------------------------------- Function Declaration ----------------------------------*/


void CLOCK_Init(void)
{
	uint32_t err_code;
	
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
	
	CLK_HFCLKStart();		//���������ⲿʱ��
	CLK_LFCLKStart();		//���������ڲ�ʱ��
	CLK_RTCCalibrate();		//RTCУ׼
}


void CLK_HFCLKStart(void)
{
	if(!nrf_drv_clock_hfclk_is_running())		//ȷ��ʱ�Ӳ����ظ�����
	{
		nrf_drv_clock_hfclk_request(NULL);
	}
}

void HFCLK_stop(void)
{
	if(nrf_drv_clock_hfclk_is_running())		//ȷ��ʱ�Ӳ��ظ��ر�
	{
		nrf_drv_clock_hfclk_release();
	}
}


void CLK_LFCLKStart(void)
{
	/* ʹ��SDK��timerʱ��Ҫ�����ڲ���Ƶʱ�� */
	/* Start low frequency crystal oscillator for app_timer(used by bsp)*/
	if(!nrf_drv_clock_lfclk_is_running())
	{
		nrf_drv_clock_lfclk_request(NULL);
	}
}


void LFCLK_stop(void)
{
	if(!nrf_drv_clock_lfclk_is_running())
	{
		nrf_drv_clock_lfclk_release();
	}
}


void CLK_RTCCalibrate(void)
{
//    NRF_CLOCK->EVENTS_DONE = 0;		
//    NRF_CLOCK->TASKS_CAL = 1;
	
	//ע�⣺�ڲ�RCʱ��У׼ʱ�����뱣֤�ⲿ����ʱ���ǿ�����
	//0.25 * 40 = 10S У׼һ��
	nrf_drv_clock_calibration_start(40,RTC_calibrate_handler);
}

void RTC_calibrate_handler(nrf_drv_clock_evt_type_t event)
{
	switch (event)
    {
    	case NRF_DRV_CLOCK_EVT_HFCLK_STARTED:
    		break;
    	case NRF_DRV_CLOCK_EVT_LFCLK_STARTED:
    		break;
		case NRF_DRV_CLOCK_EVT_CAL_DONE:
			nrf_drv_clock_calibration_start(40,RTC_calibrate_handler);
    		break;
		case NRF_DRV_CLOCK_EVT_CAL_ABORTED:
			nrf_drv_clock_calibration_start(40,RTC_calibrate_handler);
    		break;
    	default:
    		break;
    }
}
                          








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
	
	CLK_HFCLKStart();		//开机开启外部时钟
	CLK_LFCLKStart();		//开机开启内部时钟
	CLK_RTCCalibrate();		//RTC校准
}


void CLK_HFCLKStart(void)
{
	if(!nrf_drv_clock_hfclk_is_running())		//确保时钟不被重复开启
	{
		nrf_drv_clock_hfclk_request(NULL);
	}
}

void HFCLK_stop(void)
{
	if(nrf_drv_clock_hfclk_is_running())		//确保时钟不重复关闭
	{
		nrf_drv_clock_hfclk_release();
	}
}


void CLK_LFCLKStart(void)
{
	/* 使用SDK的timer时，要开启内部低频时钟 */
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
	
	//注意：内部RC时钟校准时，必须保证外部晶体时钟是开启的
	//0.25 * 40 = 10S 校准一次
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
                          







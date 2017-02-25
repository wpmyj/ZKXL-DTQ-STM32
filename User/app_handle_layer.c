/**
  ******************************************************************************
  * @file   	app_handle_layer.c
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_serial_cmd_process.h"
#include "app_send_data_process.h"
#include "app_show_message_process.h"
#include "app_card_process.h"
#include "app_systick_package_process.h"
#include "app_spi_send_data_process.h"


/******************************************************************************
  Function:app_handle_layer
  Description:
		App ��ѯ������
  Input :
  Return:
  Others:None
******************************************************************************/
void app_handle_layer(void)
{
	/*clickers send data process */
	App_clickers_send_data_process();

	/* serial cmd processing process */
	App_seirial_cmd_process();

	/* MI Card processing process */
	App_card_process();

	/*clickers systick process */
	//App_clickers_systick_process();
	
	/*printf message show process */
	//message_show_process();
	
	/*clickers send data process */
	App_clickers_single_send_data_process();

	/*revicer spi send data process */
	App_spi_send_data_process();
}

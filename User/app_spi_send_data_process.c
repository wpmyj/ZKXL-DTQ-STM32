#include "app_spi_send_data_process.h"
#include "nrf.h"

static uint8_t spi_send_data_status = 0;
static uint8_t spi_send_data_buffer[SPI_SEND_DATA_BUFFER_COUNT_MAX][260];
static uint8_t spi_send_data_buffer_len[SPI_SEND_DATA_BUFFER_COUNT_MAX];
static uint8_t spi_send_data_write_index, spi_send_data_read_index, spi_send_data_count;

/******************************************************************************
  Function:change_spi_send_data_process_status
  Description:
  Input :
  Output:
  Return:
  Others:None
******************************************************************************/
void change_spi_send_data_process_status( uint8_t new_status )
{
	spi_send_data_status = new_status;
	DEBUG_SPI_LOG("spi_process_status = %d\r\n",spi_send_data_status);
}

/******************************************************************************
  Function:get_spi_send_data_process_status
  Description:
  Input :
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t get_spi_send_data_process_status( void )
{
	return spi_send_data_status ;
}

/******************************************************************************
  Function:App_card_process
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void App_spi_send_data_process(void)
{
	if( spi_send_data_status == 0 )
	{
		if( spi_send_data_count > 0 )
		{
			uint8_t spi_delayms;

			spi_write_tx_payload(spi_send_data_buffer[spi_send_data_read_index],
				spi_send_data_buffer_len[spi_send_data_read_index],
				spi_send_data_buffer[spi_send_data_read_index][spi_send_data_buffer_len[spi_send_data_read_index]] ,
				spi_send_data_buffer[spi_send_data_read_index][spi_send_data_buffer_len[spi_send_data_read_index]+1]);
			
			spi_delayms = spi_send_data_buffer[spi_send_data_read_index][spi_send_data_buffer_len[spi_send_data_read_index]+2];
      DEBUG_SPI_LOG("[%3d] [%3d] spi_delayms_value = %d\r\n",spi_send_data_count,spi_send_data_read_index,spi_delayms);
			spi_send_data_read_index = (spi_send_data_read_index + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
			spi_send_data_count--;

			if( spi_delayms > 0 )
			{
				sw_create_timer(&spi_send_data_timer , spi_delayms, 1, 2,&(spi_send_data_status), NULL);
				change_spi_send_data_process_status(1);
			}
			else
			{
				change_spi_send_data_process_status(0);
			}
		}
	}
	
	if( spi_send_data_status == 2 )
	{
		change_spi_send_data_process_status(0);
	}
}

/******************************************************************************
  Function:spi_send_data_write_tx_payload
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void spi_send_data_write_tx_payload( uint8_t *tx_buf,uint8_t tx_buf_len, uint8_t tx_count,
	                         uint8_t nrf51822_delay100us, uint8_t spi_delayms )
{
	/* store data to spi send data buffer */
	memcpy(spi_send_data_buffer[spi_send_data_write_index],tx_buf, tx_buf_len );
	spi_send_data_buffer_len[spi_send_data_write_index] = tx_buf_len;
	spi_send_data_buffer[spi_send_data_write_index][tx_buf_len] = tx_count;
	spi_send_data_buffer[spi_send_data_write_index][tx_buf_len+1] = nrf51822_delay100us;
	spi_send_data_buffer[spi_send_data_write_index][tx_buf_len+2] = spi_delayms;
	DEBUG_SPI_LOG("[%3d] spi_delayms = %d\r\n",spi_send_data_write_index,spi_delayms);
	spi_send_data_write_index = (spi_send_data_write_index + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
	spi_send_data_count++;
}

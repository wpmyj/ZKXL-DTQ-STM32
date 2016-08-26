/**
  ******************************************************************************
  * @file   	rfsim_spi.c
  * @author  	Tim.Xia
  * @version 	V1.0.0
  * @date   	21-4-2013
  * @brief  	
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "rfsim_spi.h"


/* Exported Variables --------------------------------------------------------*/
/* Exported Variables --------------------------------------------------------*/


/* Exported Functions ------------------------------------------------------- */
/* Exported Functions ------------------------------------------------------- */

/*******************************************************************************
  * @brief  Configures STM32F103RBT SPI2 port.
  * @param  None
  * @retval None
*******************************************************************************/
void spiInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO |\
							SPI_MISO_CLK		|\
							SPI_MOSI_CLK		|\
							SPI_SCK_CLK			|\
							SPI_CSN_CLK			|\
							SPI_CE_CLK			|\
							SPI_IRQ_CLK,
							ENABLE
						  );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = SPI_MISO_PIN;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  	GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = SPI_MOSI_PIN;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  	GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = SPI_SCK_PIN;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  	GPIO_Init(SPI_SCK_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = SPI_CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  	GPIO_Init(SPI_CSN_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = SPI_CE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  	GPIO_Init(SPI_CE_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = SPI_IRQ_PIN;
 	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
  	GPIO_Init(SPI_IRQ_PORT, &GPIO_InitStructure);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
	GPIO_SetBits(SPI_CSN_PORT, SPI_CSN_PIN);
}


/*******************END OF FILE************************************************/










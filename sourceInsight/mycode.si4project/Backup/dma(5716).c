#include "dma.h"

void init_DMA2(uint32_t *src , uint32_t *tar , uint32_t bufferSize){
	 DMA_InitTypeDef DMA_InitStruct;
	//开时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	//初始化DMA
	DMA_InitStruct.DMA_BufferSize=bufferSize;//缓存大小
	DMA_InitStruct.DMA_Channel=DMA_Channel_0;//设置传输通道
	DMA_InitStruct.DMA_DIR=DMA_DIR_MemoryToMemory;//设置传输方向
	DMA_InitStruct.DMA_Memory0BaseAddr=tar;
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Word;
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStruct.DMA_PeripheralBaseAddr=src;
	DMA_InitStruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Word;
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Enable;
	DMA_InitStruct.DMA_Priority=DMA_Priority_High;
	DMA_Init(DMA2_Stream0, &DMA_InitStruct);
	//开启DMA
	DMA_Cmd(DMA2_Stream0, ENABLE);
}


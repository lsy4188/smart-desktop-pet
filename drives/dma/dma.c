#include "dma.h"

/**************************************
函数名：init_DMA2
函数功能：DMA2的初始化
输入参数：src:数据源的地址；tar:数据的目标地址；bufferSize:数据的数量
输出参数：无
其它：
	全字传输，普通模式，内存到内存
****************************************/
void init_DMA2(uint32_t *src , uint32_t *tar , uint32_t bufferSize){
	 DMA_InitTypeDef DMA_InitStruct;
	//开时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	//初始化DMA
	DMA_InitStruct.DMA_BufferSize=bufferSize;//缓存大小
	DMA_InitStruct.DMA_Channel=DMA_Channel_0;//设置传输通道
	DMA_InitStruct.DMA_DIR=DMA_DIR_MemoryToMemory;//设置传输方向是内存到内存
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)tar;//目标地址
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;//设置单次传输(注:突发传输需配置FIFO)
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Word;//全字传输(32bit)
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;//目标地址自增
	DMA_InitStruct.DMA_FIFOMode=DMA_FIFOMode_Disable;//不使用FIFO(缓存模式)
	/* FIFO 模式配置（突发传输必需）
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;*/
	DMA_InitStruct.DMA_Mode=DMA_Mode_Normal;//不启用循环模式
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)src;//源地址
	DMA_InitStruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;//设置单次传输(注:突发传输需配置FIFO)
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Word;//全字传输(32bit)
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Enable;//源地址自增
	DMA_InitStruct.DMA_Priority=DMA_Priority_High;//设置优先级为高
	DMA_Init(DMA2_Stream0, &DMA_InitStruct);
	//开启DMA
	DMA_Cmd(DMA2_Stream0, ENABLE);
}


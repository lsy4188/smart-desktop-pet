#include "adc.h"
/*
PB1-->ADC_IN9
*/
void init_ADC1(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	DMA_InitTypeDef DMA_InitStruct;
	ADC_CommonInitTypeDef ADC_CommonInitStruct;//这个adc公共参数的定义需要放到adc定义前
	ADC_InitTypeDef ADC_InitStruct;
	//开时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_DMA2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	//GPIO初始化
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AN;//设置为模拟输入
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;//GPIO引脚
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;//浮空输入
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	//DMA初始化
	DMA_InitStruct.DMA_BufferSize=BufferSize;//缓存大小
	DMA_InitStruct.DMA_Channel=DMA_Channel_0;//设置传输通道
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralToMemory;//设置传输方向是外设到内存
	DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)buffer;//目标地址
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;//设置单次传输(注:突发传输需配置FIFO)
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//半字传输(16bit)
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;//目标地址自增
	DMA_InitStruct.DMA_FIFOMode=DMA_FIFOMode_Disable;//不使用FIFO(缓存模式)
	/* FIFO 模式配置（突发传输必需）
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;*/
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;//启用循环模式
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)ADC1->DR;//源地址
	DMA_InitStruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;//设置单次传输(注:突发传输需配置FIFO)
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//半字传输(16bit)
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//源地址不自增
	DMA_InitStruct.DMA_Priority=DMA_Priority_High;//设置优先级为高
	DMA_Init(DMA2_Stream0, &DMA_InitStruct);
	//配置ADC共用参数
	ADC_CommonInitStruct.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled;//DMA直接寄存器访问
	ADC_CommonInitStruct.ADC_Mode=ADC_Mode_Independent;//独立模式
	ADC_CommonInitStruct.ADC_Prescaler=ADC_Prescaler_Div4;//168/2/4 = 21mHZ
	ADC_CommonInitStruct.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;//延时5个时钟周期
	ADC_CommonInit(&ADC_CommonInitStruct);
	////配置单个ADC参数
	ADC_InitStruct.ADC_ContinuousConvMode=ENABLE;//使能连续转换模式
	ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;//数据右对齐
	ADC_InitStruct.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None;//不使用外部触发，使用软件触发
	ADC_InitStruct.ADC_NbrOfConversion=1;	//通道的数
	ADC_InitStruct.ADC_Resolution=ADC_Resolution_12b;//分辨率
	ADC_InitStruct.ADC_ScanConvMode=DISABLE;//不扫描
	ADC_Init(ADC1, &ADC_InitStruct);

	//ADC规则通道配置，即设置采样规则，采样时间	(如果有多个通道会按照配置顺序读取)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1,ADC_SampleTime_84Cycles);
	/*ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 2,ADC_SampleTime_84Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 3,ADC_SampleTime_84Cycles);*/
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);//设置DAM的触发条件
	DMA_Cmd(DMA2_Stream0, ENABLE);//使能DMA
	ADC_DMACmd(ADC1, ENABLE);//使能adc的DMA
	ADC_Cmd(ADC1, ENABLE);//使能adc
	ADC_SoftwareStartConv(ADC1);//软件触发ADC开始转换
}

uint16_t get_avg_data(void){
	int i=0,sum=0;
	for(i=0;i<BufferSize;i++){
		sum+=buffer[i];
	}
	return sum/BufferSize;
}



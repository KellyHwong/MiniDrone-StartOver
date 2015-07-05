#include "util.h"

RCC_ClocksTypeDef rcc_clocks;

void rcc_configuration(void)
{
	SystemCoreClockUpdate();					//刷新系统时钟频率变量SystemCoreClock的值
	RCC_GetClocksFreq(&rcc_clocks);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
}

//NVIC中断控制器分组
void nvic_grouping(u8 group_num)
{
	switch(group_num)
	{
	case 0:
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		break;
	case 1:
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
		break;
	case 2:
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		break;
	case 3:
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
		break;
	case 4:
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
		break;
	default:
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		break;
	}
}

void nvic_configuration(uint8_t irq_channel, uint8_t preemption_priority, uint8_t sub_priority)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = irq_channel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = preemption_priority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = sub_priority;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void portOutInit(GPIO_TypeDef *gpio, uint32_t gpio_pin, GPIOMode_TypeDef mode, GPIOOType_TypeDef output_type, GPIOPuPd_TypeDef pull_type, GPIOSpeed_TypeDef speed_type)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = gpio_pin;
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_InitStructure.GPIO_OType = output_type;
	GPIO_InitStructure.GPIO_PuPd = pull_type;
	GPIO_InitStructure.GPIO_Speed = speed_type;
	GPIO_Init(gpio, &GPIO_InitStructure);
}

void portInInit(GPIO_TypeDef *gpio, uint32_t gpio_pin, GPIOMode_TypeDef mode, GPIOPuPd_TypeDef pull_type)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = gpio_pin;
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_InitStructure.GPIO_PuPd = pull_type;
	GPIO_Init(gpio, &GPIO_InitStructure);
}




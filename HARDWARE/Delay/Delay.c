#include "Delay.h"

/**************************************************************************
	使用m内核的dwt计数器
**************************************************************************/



/**************************************************************************
函数功能：初始化DWT周期计数器
入口参数：无
返回  值：1: 初始化成功; 0: 初始化失败（芯片可能不支持DWT） f407igt6_168mhz:最大安全延时约为25.5秒
**************************************************************************/
uint8_t My_Delay_Init(void)
{
    // 1. 启用 TRC (Trace)
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    // 2. 【关键修正】解锁 DWT (STM32F4/F7 必需)
    *(volatile uint32_t *)0xE0001FB0 = 0xC5ACCE55;

    // 3. 清零并使能
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    // 4. 简单测试
    uint32_t start = DWT->CYCCNT;
    HAL_Delay(1);

    // 如果计数器没动，说明初始化失败，返回 0
    if(DWT->CYCCNT == start) return 0;

    return 1;
}


/**************************************************************************
函数功能：DWT-us延时
入口参数：无
返回  值：无
**************************************************************************/
void My_Delayus(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = us * (SystemCoreClock / 1000000);
    while((DWT->CYCCNT - start) < cycles);
}


/**************************************************************************
函数功能：DWT-ms延时
入口参数：无
返回  值：无
**************************************************************************/
void My_Delayms(uint32_t ms)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = ms * (SystemCoreClock / 1000);
    while((DWT->CYCCNT - start) < cycles);
}










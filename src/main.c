/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 */
#include "stm8l15x.h"

volatile int iFlashTimer = 0;

void main(void)
{  
  /* 16 MHz System Clock                                  */
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
  
  /* 1ms Task                                             */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  TIM2_TimeBaseInit(TIM2_Prescaler_128, TIM2_CounterMode_Down, 12500);
  TIM2_ITConfig(TIM2_IT_Update, ENABLE);
  TIM2_Cmd(ENABLE);
  
  /* LED GPIO                                             */
  GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Slow);
  
	while (1)
  {
    wfi();
  }
}

@far @interrupt void Timer2Interrupt(void)
{
  TIM2_ClearFlag(TIM2_FLAG_Update);
      
  if (iFlashTimer > 0)
  {
    --iFlashTimer;
  }
  else
  {
    iFlashTimer = 10;
    GPIO_ToggleBits(GPIOB, GPIO_Pin_5);
  }
}
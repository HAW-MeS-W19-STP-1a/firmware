#ifndef PFSLIB_ADAPT_H_
#define PFSLIB_ADAPT_H_

#include <stdint.h>
#include "stm8l15x.h"

void init_port(void)
{
  GPIO_Init(GPIOE, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast); // SD Card Slave Select
}

void dly_us(volatile int n)
{
  volatile int i;
  do 
  {
    for (i = 0; i < 16; ++i);
  } while (--n);
}



#endif
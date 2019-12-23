#ifndef PFSLIB_ADAPT_H_
#define PFSLIB_ADAPT_H_

#include <stdint.h>
#include "io_map.h"
#include "stm8l15x.h"

void init_port(void)
{
  /* Ports bereits in main() initialisiert */
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
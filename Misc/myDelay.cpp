#include <stdint.h>
#include "myDelay.h"

void  myDelay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


/* File:   uint8toa.c
   Author: M. P. Hayes, UCECE
   Date:   15 May 2007
   Descr: Edited by Adrian Gin
*/

#include <stdint.h>
#include "uint8toa.h"


void uint8toa (uint8_t num, uint8_t *str)
{
   uint8_t d;
   uint8_t i;
   const uint8_t powers[] = {100, 10, 1, 0};

   for (i = 0; (d = powers[i]); i++)
   {
      uint8_t q;
      q = num / d;
      if (q || d == 1)
      {
         *str++ = '0' + q;
         num -= q * d;
      }
   }

   *str = '\0';
}





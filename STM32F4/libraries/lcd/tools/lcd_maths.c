#include "lcd/tools/lcd_math.h"

unsigned int fetch_positive_delta(unsigned int a, unsigned int b)
{
    return (a > b ? a - b : b - a);
}
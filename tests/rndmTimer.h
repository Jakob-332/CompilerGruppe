//
// Created by JBohn on 02.03.2026.
//

#ifndef RNDMTIMER_H
#define RNDMTIMER_H
#include <drivers/uart.h>

#define TIME_SCALING_FACTOR 15000
#define ROUND_COUNT 5

void rndmTimer_test( UartDevice * uart );

#endif //RNDMTIMER_H

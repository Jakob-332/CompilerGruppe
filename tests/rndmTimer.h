//
// Created by JBohn on 02.03.2026.
//

#ifndef RNDMTIMER_H
#define RNDMTIMER_H
#include <drivers/uart.h>

#include "drivers/timer.h"

#define TIME_SCALING_FACTOR 5000
#define ROUND_COUNT 5

void startGame( UartDevice * uart, char difficultyLevel );
uint32_t playRound(UartDevice * uart, TimerDevice * timer0, TimerDevice * counter0, int roundIndex, char *dificultyLevelValue);
void printRound( UartDevice * uart, uint8_t round );
void printGameSummary( UartDevice * uart, const int roundTime[], int falseStartCount, char difficultyLevelValue );
void lauflicht_effekt ( UartDevice * uart );

#endif //RNDMTIMER_H

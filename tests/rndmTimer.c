//
// Created by JBohn on 02.03.2026.
//
#include "rndmTimer.h"

#include <time.h>
#include <drivers/timer.h>
#include <drivers/random.h>

// Hilfsfunktion: Wandelt eine 32-Bit Ganzzahl in einen C-String (Text) um
void uint32_to_string(uint32_t num, char* buffer) {
    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\n';
        buffer[2] = '\0';
        return;
    }

    // Zuerst herausfinden, wie viele Ziffern die Zahl hat
    uint32_t temp = num;
    int len = 0;
    while (temp > 0) {
        len++;
        temp /= 10;
    }

    // Null-Terminierung und Zeilenumbruch am Ende anfügen
    buffer[len + 1] = '\0';
    buffer[len] = '\n';

    // Ziffern von hinten nach vorne berechnen (Modulo 10 liefert die letzte Ziffer)
    while (num > 0) {
        buffer[--len] = (num % 10) + '0'; // + '0' wandelt die Zahl in das ASCII-Zeichen um
        num /= 10;
    }
}

void rndmTimer_test( UartDevice * uart )
{
  rng_init();

  TimerDevice * timer0 = timer_init( 0, TIMER_MODE_TIMER, TIMER_BITMODE_BITMODE_32BIT, 16 );

  char uartBuffer[16]; // Puffer für die Zahl (max. 10 Ziffern bei 32-Bit + \n + \0)

  uint8_t randomValue = rng_getRandomValue();
  uint32_t scaledRandomValue = randomValue * 1000;
  timer_set_compare(timer0, 0, scaledRandomValue);

  uart_writeString( uart, "Start der Random Events\n" );

  // Eigene Funktion zur Umwandlung nutzen
  uint32_to_string(scaledRandomValue, uartBuffer);
  uart_writeString( uart, "Wert: " );
  uart_writeString( uart, uartBuffer );

  timer_start(timer0);

  uart_writeString( uart, "Start Timer\n" );

  for (;;)
  {
    while ( !(timer_get_event( timer0, 0, false )))
    {
      timer_capture( timer0, 1);
    }
    timer_get_event( timer0, 0, true );
    timer_clear( timer0 );
    uart_writeString( uart, "Event des Rnd Timer\n" );

    randomValue = rng_getRandomValue();
    scaledRandomValue = randomValue * 10000;
    timer_set_compare(timer0, 0, scaledRandomValue);

    // Wieder die eigene Funktion nutzen
    uint32_to_string(scaledRandomValue, uartBuffer);
    uart_writeString( uart, "Neuer Wert: " );
    uart_writeString( uart, uartBuffer );
  }
}
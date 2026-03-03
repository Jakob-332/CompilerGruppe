//
// Created by JBohn on 02.03.2026.
//
#include "rndmTimer.h"

#include <time.h>
#include <drivers/timer.h>
#include <drivers/random.h>


void rndmTimer_test( UartDevice * uart )
{
  rng_init();

  TimerDevice * timer0 = timer_init( 0, TIMER_MODE_TIMER, TIMER_BITMODE_BITMODE_32BIT, 16 );
  TimerDevice * counter0 = timer_init( 1, TIMER_MODE_COUNT, TIMER_BITMODE_BITMODE_32BIT, 16 );


  uint8_t randomValue = rng_getRandomValue();
  uint32_t scaledRandomValue = randomValue * TIME_SCALING_FACTOR;
  timer_set_compare(timer0, 0, scaledRandomValue);

  uart_writeString( uart, "Start der Random       +\n" );

  // Eigene Funktion zur Umwandlung nutzen
  /*uint32_to_string(scaledRandomValue, uartBuffer);
  uart_writeString( uart, "Wert: " );
  uart_writeString( uart, uartBuffer );
*/
  timer_start(timer0);

  uart_writeString( uart, "Start Timer\n" );

  for ( int i = 0; i<ROUND_COUNT; i++ )
  {
    uart_writeString(uart, "Runde:");
    uart_writeNumber(uart, i+1);
    uart_writeString(uart, "\n");
    while ( !(timer_get_event( timer0, 0, false )))
    {
      timer_capture( timer0, 1);
    }
    timer_get_event( timer0, 0, true );
    timer_clear( timer0 );
    uart_writeString( uart, "Fertig amchen zum Sturmabwehrschießen\n" );
    timer_start(counter0);

      char readChar;
      for ( readChar = 0; readChar == 0; readChar = uart_readByte( uart ) )
      {
        ;;
      }
    timer_stop(counter0);

    //timer_trigger( counter0 );
    //timer_capture( counter0, 0);
    uart_writeNumber(uart, timer_get_counter(counter0, 0));
    timer_clear(counter0);
    randomValue = rng_getRandomValue();
    scaledRandomValue = randomValue * TIME_SCALING_FACTOR;
    timer_set_compare(timer0, 0, scaledRandomValue);


  }
}
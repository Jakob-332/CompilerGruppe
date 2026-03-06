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
  int roundTime[ROUND_COUNT];
  char readChar;
  int i;
  bool falseStart=false;
  int falseStartCount=0;
  int timeSUM = 0;
  int averageTime=0;

  TimerDevice * timer0 = timer_init( 0, TIMER_MODE_TIMER, TIMER_BITMODE_BITMODE_32BIT, 16 );
  TimerDevice * counter0 = timer_init( 1, TIMER_MODE_TIMER, TIMER_BITMODE_BITMODE_32BIT, 16 );


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

    // Timer run on RNG Time
    while ( !(timer_get_event( timer0, 0, false )) && falseStart == false )
    {
      readChar = uart_readByte( uart );
      if (!(readChar == 0))
      {
        falseStart = true;
      }
    }

    timer_get_event( timer0, 0, true );
    timer_stop(timer0);
    timer_clear( timer0 );

    if (falseStart == false)
    {
      uart_writeString( uart, "JETZT\n" );
      timer_clear(counter0);
      timer_start(counter0);
      timer_capture(counter0,0);

      for ( readChar = 0; readChar == 0; readChar = uart_readByte( uart ) )
      {
        ;;
      }

      timer_capture(counter0,1);
      timer_stop(counter0);

      uint32_t start = timer_get_counter(counter0,0);
      uint32_t stop = timer_get_counter(counter0,1);
      uint32_t delta = (stop - start) / 1000;

      /**
       uart_writeNumber(uart, start);
       uart_writeString(uart, "\n");
       uart_writeNumber(uart, stop);
       uart_writeString(uart, "\n");
       */
      uart_writeNumber(uart, delta);
      uart_writeString(uart, "\n");
      timer_clear(counter0);
      roundTime[i] = delta;

    }
    else
    {
      uart_writeString( uart, "Fehlstart\n" );
      falseStart = false;
      roundTime[i] = 0;
      falseStartCount++;
    }



    randomValue = rng_getRandomValue();
    scaledRandomValue = (randomValue + 200) * TIME_SCALING_FACTOR;
    timer_set_compare(timer0, 0, scaledRandomValue);
    timer_start(timer0);


  }

  for (int i = 0; i<ROUND_COUNT; i++ )
  {

    timeSUM = timeSUM + roundTime[i];
  }
  uart_writeString(uart, "Zeitsummen");
  uart_writeNumber(uart, timeSUM);
  uart_writeString(uart, "\n");

  uart_writeString(uart, "falseStartCount");
  uart_writeNumber(uart, falseStartCount);
  uart_writeString(uart, "\n");

  averageTime = timeSUM / (ROUND_COUNT - falseStartCount);
  averageTime = averageTime ;
  uart_writeNumber(uart, averageTime);
  uart_writeString(uart, "\n");
}
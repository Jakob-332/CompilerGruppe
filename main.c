/**
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Thomas Vogt
 **/

#include "tests/rndmTimer.h"


#include <drivers/uart.h>
#include <drivers/random.h>

#include <tests/timer_test.h>
#include <tests/wdt_test.h>

#include <stdbool.h>
#include <stdint.h>

void init_Game(UartDevice * uart);

const uint32_t roData[ 2] = { 1,2};
uint32_t rwData[ 4] = { 3,4, 5, 6};

uint32_t bssData[ 8];

UartDevice * uart1;


int main( void )
{
  uart1 = uart_init( UARTE0_BASE_ADDRESS );

  init_Game(uart1);

  //Wdt_test(uart1);
  //timer_test( uart1);

  extern void SysTick_init( void );
  SysTick_init();

  return 0;
}

void init_Game(UartDevice * uart)
{
  uart_writeString(uart, "Reflexo v0.1\n");
  uart_writeString(uart, "[s] Start ;; [h] Help ;; [q] Quittierne\n");
  for (;;)
  {
    char readChar;
    for ( readChar = 0; readChar == 0; readChar = uart_readByte( uart ) )
    {
      ;
    }

    if ( readChar == 'h' )
    {
      uart_writeString( uart, "Hilfe\n" );  // <------------------------------------------------ Hier Spielerklärung
    }else if ( readChar == 's' )
    {
      uart_writeString( uart, "Spiel gestartet\n" );
      startGame(uart);
    } else
    {
      uart_writeString(uart, "\033[32mDrücke [s] zum nochmal spielen!\033[31m Drücke [q] zum Verlassen.\033[0m\n");  //
    }
  }





}

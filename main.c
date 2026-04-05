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

//const uint32_t roData[ 2] = { 1,2};
//uint32_t rwData[ 4] = { 3,4, 5, 6};

//uint32_t bssData[ 8];

UartDevice * uart1;

int main( void )
{
  // UART für die Kommunikation über das Terminal einrichten
  uart1 = uart_init( UARTE0_BASE_ADDRESS );

  // Hauptmenü des Spiels laden
  init_Game(uart1);

  //Wdt_test(uart1);
  //timer_test( uart1);

  // System-Tick initialisieren, damit Zeiten im Hintergrund weiterlaufen
  extern void SysTick_init( void );
  SysTick_init();

  return 0;
}

void init_Game(UartDevice * uart)
{
  // Begrüßungstext und Steuerungs-Übersicht anzeigen
  uart_writeString(uart, "Reflexo v1.2\n");
  uart_writeString(uart, "[s] Start ;; [h] Help ;; [q] Quit game\n");

  // Endlosschleife für das Hauptmenü
  for (;;)
  {
    char readChar;

    // Warten, bis der Spieler eine Taste drückt
    for ( readChar = 0; readChar == 0; readChar = uart_readByte( uart ) )
    {
      ;
    }

    // Eingabe auswerten: Hilfe anzeigen
    if ( readChar == 'h' )
    {
      uart_writeString( uart, "\n--- Reflex Arena - Hilfe ---\n"
    "Ziel: Teste deine Reaktionszeit\n\n"
    "Ablauf:\n"
    "1. Starte die Runde.\n"
    "2. Warte auf das Signal \"JETZT\".\n"
    "3. Drücke so schnell wie mölich eine beliebige Taste\n"
    "(Wenn du zu früh drückst,  kassierts du einen Fehlversuch)\n\n"
    "Steuerung:\n"
    "[s] : Spiel starten\n"
    "[h]              : Diese Hilfe anzeigen\n"
    "[q]              : Spiel beenden\n"
    "----------------------------\n" );
    }
    // Eingabe auswerten: Spiel starten
    else if ( readChar == 's' )
    {
      char levelChar = 0;

      // Schwierigkeitsgrad abfragen
      uart_writeString(uart, "Wähle Schwierigkeitsgrad: [1] Leicht, [2] Mittel, [3] Schwer\n");
      while(levelChar == 0){
        levelChar = uart_readByte( uart );
      }

      uart_writeString( uart, "Spiel gestartet\n" );
      startGame(uart, levelChar);
    }
    // Eingabe auswerten: Spiel beenden
    else if ( readChar == 'q' )
    {
      uart_writeString( uart, "Spiel beendet\n" );
      break; // Raus aus der Endlosschleife
    }
    // Falsche Taste gedrückt
    else
    {
      uart_writeString(uart, "\033[32mDrücke [s] zum nochmal spielen!\033[31m Drücke [q] zum Verlassen.\033[0m\n");  //
    }
  }
}
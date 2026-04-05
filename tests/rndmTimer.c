//
// Created by JBohn on 02.03.2026.
//
#include "rndmTimer.h"
#include <limits.h>
#include <math.h>
#include <time.h>
#include <drivers/timer.h>
#include <drivers/random.h>
#include <drivers/nvmc.h>

#include "drivers/Wdt.h"

void setRandomTimer();

void startGame( UartDevice * uart, char difficultyLevel )
{
  // Watchdog starten, damit sich das System nicht komplett aufhängen kann
  Wdt_initialise( 100000, true, 1 );
  Wdt_start();

  // Mach das Terminal mal richtig sauber
  uart_writeString(uart, "\033[2J\033[H");

  // Zufallsgenerator anwerfen und Variablen für die Runden vorbereiten
  rng_init();
  int roundTime[ROUND_COUNT];
  int falseStartCount=0;

  static TimerDevice * timer0 = NULL;
  static TimerDevice * counter0 = NULL;

  // Timer nur initialisieren, wenn sie noch nicht existieren
  if(timer0 == NULL || counter0 == NULL){
    timer0 = timer_init( 0, TIMER_MODE_TIMER, TIMER_BITMODE_BITMODE_32BIT, 16 );
    counter0 = timer_init( 1, TIMER_MODE_TIMER, TIMER_BITMODE_BITMODE_32BIT, 16 );
  }

  // Alle Runden nacheinander durchspielen
  for ( int roundIndex = 0; roundIndex<ROUND_COUNT; roundIndex++ )
  {
    printRound(uart, roundIndex);
    const uint32_t score = playRound(uart, timer0, counter0, roundIndex, &difficultyLevel);

    // Checken ob's ein Fehlstart war (Score = 0)
    if (score == 0 ) {
      falseStartCount++;
      roundTime[roundIndex] = 0;
    } else {
      roundTime[roundIndex] = score; // Zeit merken
    }
  }

  // Am Ende alles auswerten und anzeigen
  printGameSummary(uart, roundTime, falseStartCount, difficultyLevel);

  uart_writeString(uart, "\033[32mDrücke [s] zum nochmal spielen!\033[31m Drücke [q] zum Verlassen.\033[0m\n");
}

uint32_t playRound(UartDevice * uart, TimerDevice * timer0, TimerDevice * counter0, int roundIndex, char *difficultyLevelValue) {
  char readChar;
  bool falseStart = false;
  const uint8_t randomValue = rng_getRandomValue();
  uint32_t offset;

  // Wartezeit je nach Schwierigkeitsgrad anpassen (je schwerer, desto kürzer)
  switch(*difficultyLevelValue){
    case '1':
      offset = 500;
      break;
    case '2':
      offset = 200;
      break;
    case '3':
      offset = 50;
      break;
    default:
      offset = 200;
      *difficultyLevelValue = '2'; // Fallback auf Mittel, falls Quatsch eingegeben wurde
  }

  Wdt_reloadRequest( 0 ); // Watchdog füttern

  // Zufällige Wartezeit berechnen und Timer stellen
  const uint32_t scaledRandomValue = (randomValue + offset) * TIME_SCALING_FACTOR ;
  timer_set_compare(timer0, 0, scaledRandomValue);

  timer_start(timer0);

  // Puffer leeren, falls noch alte Eingaben rumfliegen
  while (uart_readByte(uart) != 0) { }

  // Warten, bis der Timer abläuft... ODER der Spieler zu früh drückt
  while ( !(timer_get_event( timer0, 0, false )) && falseStart == false )
  {
    //lauflicht_effekt(uart);
    readChar = uart_readByte( uart );
    if (!(readChar == 0))
    {
      falseStart = true; // Zu früh gedrückt!
    }
  }

  // Timer aufräumen
  timer_get_event(timer0, 0, true);
  timer_stop(timer0);
  timer_clear( timer0 );

  // Bei Fehlstart brechen wir direkt ab
  if (falseStart) {
   uart_writeString( uart, "Fehlstart\n" );
   return 0;
  }

  // Startschuss!
  uart_writeString( uart, "JETZT\n" );
  timer_start(counter0);
  timer_capture(counter0,0); // Startzeit festhalten

  // Warten auf die Reaktion des Spielers
  for ( readChar = 0; readChar == 0; readChar = uart_readByte( uart ) )
  {
    ;;
  }

  Wdt_reloadRequest( 0 ); // Watchdog nochmal füttern

  timer_capture(counter0,1); // Stoppzeit festhalten
  timer_stop(counter0);

  // Reaktionszeit berechnen (in Millisekunden umrechnen)
  const uint32_t start = timer_get_counter(counter0,0);
  const uint32_t stop = timer_get_counter(counter0,1);
  const uint32_t delta = (stop - start) / 1000;

  // Ergebnis der Runde anzeigen
  uart_writeString(uart, "Score: ");
  uart_writeNumber(uart, delta);
  uart_writeString(uart, "\n");

  // Alles für die nächste Runde sauber machen
  timer_clear(counter0);
  while (uart_readByte(uart) != 0) { }

  return delta;
}

void printRound(UartDevice * uart, uint8_t round) {
  // Zeigt einfach nur an, in welcher Runde wir gerade sind
  uart_writeString(uart, "Runde:");
  uart_writeNumber(uart, round+1);
  uart_writeString(uart, "\n");
}

void printGameSummary(UartDevice * uart, const int roundTime[], int falseStartCount, char difficultyLevelValue) {
  int timeSum = 0;
  int bestTime = INT_MAX;
  int averageTime = 0;

  // Statistiken berechnen
  for (int i = 0; i<ROUND_COUNT; i++ )
  {
    timeSum = timeSum + roundTime[i];
    if (roundTime[i] < bestTime && roundTime[i] != 0) {
      bestTime = roundTime[i];
    }
  }

  // BUGFIX 1: Wenn es NUR Fehlstarts gab, setzen wir die beste Zeit auf 0
  if (bestTime == INT_MAX) {
    bestTime = 0;
  }

  uint8_t difficultyInt = difficultyLevelValue - '0';
  uint32_t current_highscore = nvmc_read_highscore(difficultyInt);

  uart_writeString(uart, "----------------------------\n");
  uart_writeString(uart, "Gesamtpunkte: ");
  uart_writeNumber(uart, timeSum);
  uart_writeString(uart, "\n");

  if (falseStartCount == 0 && (current_highscore == 0 || timeSum < current_highscore)) {
      uart_writeString(uart, "NEUER HIGHSCORE! \n");
      nvmc_write_highscore(difficultyInt, timeSum);
      current_highscore = timeSum; // Für die Anzeige updaten
  }

  uart_writeString(uart, "All-Time Highscore: ");
  uart_writeNumber(uart, current_highscore);
  uart_writeString(uart, "\n");

  uart_writeString(uart, "Beste Zeit: ");
  uart_writeNumber(uart, bestTime);
  uart_writeString(uart, "\n");

  uart_writeString(uart, "Fehlversuche: ");
  uart_writeNumber(uart, falseStartCount);
  uart_writeString(uart, "\n");

  // BUGFIX 2: Division durch Null verhindern!
  int validRounds = ROUND_COUNT - falseStartCount;
  if (validRounds > 0) {
    averageTime = timeSum / validRounds;
  } else {
    averageTime = 0; // Kein Durchschnitt berechenbar bei nur Fehlstarts
  }

  uart_writeString(uart, "Durchschnitt: ");
  uart_writeNumber(uart, averageTime);
  uart_writeString(uart, "\n");

  uart_writeString(uart, "Schwierigkeitsgrad: ");
  uart_writeByte(uart, difficultyLevelValue);
  uart_writeString(uart, "\n");
  uart_writeString(uart, "----------------------------\n");
}


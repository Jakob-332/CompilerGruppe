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

  Wdt_initialise( 100000, true, 1 );
  Wdt_start();

  uart_writeString(uart, "\033[2J\033[H"); //Clears terminal

  rng_init();
  int roundTime[ROUND_COUNT];
  int falseStartCount=0;


  static TimerDevice * timer0 = NULL;
  static TimerDevice * counter0 = NULL;

  if(timer0 == NULL || counter0 == NULL){
    timer0 = timer_init( 0, TIMER_MODE_TIMER, TIMER_BITMODE_BITMODE_32BIT, 16 );
    counter0 = timer_init( 1, TIMER_MODE_TIMER, TIMER_BITMODE_BITMODE_32BIT, 16 );
  }

  for ( int roundIndex = 0; roundIndex<ROUND_COUNT; roundIndex++ )
  {
    printRound(uart, roundIndex);
    const uint32_t score = playRound(uart, timer0, counter0, roundIndex, &difficultyLevel);
    if (score == 0 ) {
      falseStartCount++;
      roundTime[roundIndex] = 0;
    }else {
      roundTime[roundIndex] = score;
    }
  }
  printGameSummary(uart, roundTime, falseStartCount, difficultyLevel);

  uart_writeString(uart, "\033[32mDrücke [s] zum nochmal spielen!\033[31m Drücke [q] zum Verlassen.\033[0m\n");
}





uint32_t playRound(UartDevice * uart, TimerDevice * timer0, TimerDevice * counter0, int roundIndex, char *difficultyLevelValue) {
  char readChar;
  bool falseStart = false;
  const uint8_t randomValue = rng_getRandomValue();
  uint32_t offset;
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
      *difficultyLevelValue = '2';
  }
  Wdt_reloadRequest( 0 );
  const uint32_t scaledRandomValue = (randomValue + offset) * TIME_SCALING_FACTOR ;
  timer_set_compare(timer0, 0, scaledRandomValue);


  timer_start(timer0);
  while (uart_readByte(uart) != 0) { }
  // Timer run on RNG Time
  while ( !(timer_get_event( timer0, 0, false )) && falseStart == false )
  {
    //lauflicht_effekt(uart);
    readChar = uart_readByte( uart );
    if (!(readChar == 0))
    {
      falseStart = true;
    }
  }
  timer_get_event(timer0, 0, true);
  timer_stop(timer0);
  timer_clear( timer0 );
  if (falseStart) {
   uart_writeString( uart, "Fehlstart\n" );
   return 0;
  }
  uart_writeString( uart, "JETZT\n" );
  timer_start(counter0);
  timer_capture(counter0,0);

  for ( readChar = 0; readChar == 0; readChar = uart_readByte( uart ) )
  {
    ;;
  }
  Wdt_reloadRequest( 0 );

  timer_capture(counter0,1);
  timer_stop(counter0);

  const uint32_t start = timer_get_counter(counter0,0);
  const uint32_t stop = timer_get_counter(counter0,1);
  const uint32_t delta = (stop - start) / 1000;

  uart_writeString(uart, "Score: ");
  uart_writeNumber(uart, delta);
  uart_writeString(uart, "\n");
  timer_clear(counter0);
  while (uart_readByte(uart) != 0) { }
  return delta;
}

void printRound(UartDevice * uart, uint8_t round) {
  uart_writeString(uart, "Runde:");
  uart_writeNumber(uart, round+1);
  uart_writeString(uart, "\n");
}

void printGameSummary(UartDevice * uart, const int roundTime[], int falseStartCount, char difficultyLevelValue) {
  int timeSum= 0;
  int bestTime = INT_MAX;
  int averageTime=0;
  for (int i = 0; i<ROUND_COUNT; i++ )
  {
    timeSum = timeSum + roundTime[i];
    if (roundTime[i] < bestTime && roundTime[i] != 0) {
      bestTime = roundTime[i];
    }
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

  averageTime = timeSum / (ROUND_COUNT - falseStartCount);
  uart_writeString(uart, "Durchschnitt: ");
  uart_writeNumber(uart, averageTime);
  uart_writeString(uart, "\n");

  uart_writeString(uart, "Schwierigkeitsgrad: ");
  uart_writeByte(uart, difficultyLevelValue);
  uart_writeString(uart, "\n");
  uart_writeString(uart, "----------------------------\n");
}



void lauflicht_effekt(UartDevice * uart) {
  // Vier Frames für unsere kleine Animation
  const char* frames[] = {
    "[ O . . . ]",
    "[ . O . . ]",
    "[ . . O . ]",
    "[ . . . O ]"
};

  uart_writeString(uart, "Lauflicht startet...\n");

  // Animation 5 mal wiederholen
  for (int loop = 0; loop < 5; ++loop) {
    for (int frame = 0; frame < 4; ++frame) {
      // 1. Cursor an den Anfang der Zeile setzen
      uart_writeString(uart, "\r");

      // 2. Aktuellen Frame zeichnen
      uart_writeString(uart, frames[frame]);

      // 3. Kurze Pause einlegen (Delay)
      // Da du Timer in deinem Code hast (drivers/timer.h),
      // könntest du diese nutzen. Alternativ ein Quick-and-Dirty Busy-Wait:
      for (volatile int delay = 0; delay < 800000; ++delay) {
        // Warten... (Wert muss je nach Taktung des Emulators angepasst werden)
      }
    }
  }
  // Zum Schluss noch ein Zeilenumbruch, damit der nächste Text darunter steht
  uart_writeString(uart, "\nFertig!\n");
}
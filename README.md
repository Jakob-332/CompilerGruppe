### Spiel Starten
Das Spiel wird vollständig über das UART-Terminal bedient.

Wenn das Projekt gemäß der Anleitung eingerichtet ist, genügen ein Klick auf **Build** und anschließend auf **Debug** in der IDE, um den Emulator und das Spiel direkt zu starten.

Detaillierte Informationen zur Einrichtung (z.B. in CLion) und zur manuellen Ausführung über das Terminal findest du in den Abschnitten weiter unten.

### Steuerung
- `[s]` oder `[ENTER]` : Spiel starten
- `[h]` : Hilfe im Menü anzeigen
- `[q]` : Spiel beenden
- `[1]`, `[2]`, `[3]` : Auswahl des Schwierigkeitsgrads (Leicht, Mittel, Schwer)
- **Beliebige Taste** : Zum Reagieren bei "JETZT!" oder als Fehlstart-Auslöser während der Wartezeit.

### Bonusfeatures
- **Highscore:** Die beste Reaktionszeit wird angezeigt und gespeichert.
- **Statistik & Auswertung:** Nach einem Set von 5 Runden liefert das Spiel eine detaillierte Zusammenfassung (Beste Zeit, Durchschnittszeit, Anzahl der Fehlversuche).
- **Schwierigkeitsstufen:** Drei Level passen das Basis-Zeitfenster für die RNG-Wartezeit an.
- **Anti-Cheat:** Wenn die Reaktionszeit zu schnell für menschliche Fähigkeiten ist, wird diese markiert.

### Zeitquelle
Das Spiel nutzt dedizierte Hardware Timer (Timer-Peripherie) für höchste Präzision:
- **Timer 0:** Verarbeitet die generierte Zufallszeit plus Schwierigkeits-Offset über einen Compare-Match.
- **Timer 1:** Dient als Capture-Timer zur exakten Bestimmung der Reaktionszeit in Millisekunden (Delta zwischen Start- und Stop-Capture).




# Systemnahe Programmierung 2 - 2026 - Base Project
![ProjectLogo.svg](ProjectLogo.svg)

Quellcode der Gruppe Compiler für das Projekt Reaktionsspiel in der Vorslesung Systemnahe Programmierung 2.


## Build-Umgebung
- Arm Cross-Compiler: _gcc-arm-none-eabi_
- GDB (Multi-Arch): _gdb-multiarch_
- CMake
- [Renode](https://renode.io/) ([github](https://github.com/renode/renode))
- QEMU Arm System Emulator: _qemu-system-arm_

Hinweis:
Unter anderem bei SUSE, Arch, NixOs scheint der standard GDB ein GDB-Multiarch zu sein.


## Building
Clonen des Quellcodes:

```bash
git clone \
  https://git.thomas-vogt.de/dhbw/systemnahe-programmierung/base-project-sysprog2-2026.git
```

Erstellen des Build Mittels CMake Presets:

```bash
cmake --preset arm-cortex-m4
```

Bauen Mittels CMake Presets:
```bash
cmake --build --preset arm-cortex-m4
```

## Generiertes Output analysieren (optional)
- ELF-Sections anzeige:
  ```bash
  arm-none-eabi-objdump -x cmake-build-arm-cortex-m4/testApp.elf
  ```
- ELF-File disassemblieren (mit Quellcode-Verknüpfung):
  ```bash
  arm-none-eabi-objdump -d cmake-build-arm-cortex-m4/testApp.elf
  arm-none-eabi-objdump -d -S cmake-build-arm-cortex-m4/testApp.elf
  ```

## Renode Emulator starten und mit GDB verbinden

_Renode_ und _GDB_ müssen in unterschiedlichen Konsolen (Shells) gestartet werden.

```bash
renode cmake-build-arm-cortex-m4/platform.resc
```

Falls _renode_ nicht im Pfad liegt, die ausführbare Datei über ihren _absoluten Pfad_ angeben.

**Achtung:**
Der *Build-Pfad* darf keine Leerzeichen enthalten.
Andernfalls findet Renode das Script nicht.

### Start GDB
```bash
gdb-multiarch testApp.elf -ex "target extended-remote localhost:3333" -ex "load" -ex "monitor start"
```

In manchen Distributionen kann der `gdb-multiarch` auch über den Befehl `gdb` aufgerufen werden.

## JetBrains CLion

### Projekt Clonen & Starten

URL:
: https://github.com/Jakob-332/CompilerGruppe

Directory:
: Projektverzeichnis

### Profile Konfigurieren

1. *Debug*: Profil löschen
2. *arm-cortex-m4*: Profil aktivieren (*enable*)

### File Types für Assembler Datein anpassen

- Menü: _File_ -> _Settings_
- Eintrag: _Editor_ -> _File Types_
- _Assembly language file_ -> _File name Patterns_: "*.s" hinzufügen

### Debugging Konfiguration

- Menü: _Run_ -> _Edit Configurations..._
- _Add New Configuration_ -> _Embedded GDB Server_

Name:
: testApp-Renode

Target:
: testApp

Executable Binary:
: testApp

Debugger:
: "Bundeld GDB" oder den zuvor installierten GDB auswählen

Upload executable:
: never

'target remote' args:
: `tcp::3333`

GDB Server:
: Pfad zur Renode Executable

GDB Server Args:
: `$CMakeCurrentBuildDir$/platform.resc`


## 🎮 Spielanleitung: Reflex Arena

Das Ziel von **Reflex Arena** ist es, deine Reaktionszeit auf die Millisekunde genau zu testen. Das Spiel wird vollständig über das UART-Terminal gesteuert.

**So funktioniert eine Runde:**
1. **Starten:** Drücke `[s]`, um das Spiel zu starten.
2. **Schwierigkeitsgrad wählen:** Entscheide dich mit `[1]` (Leicht), `[2]` (Mittel) oder `[3]` (Schwer) für dein Level.
3. **Warten:** Das System lässt dich eine zufällige Zeitspanne zappeln.
4. **Reagieren:** Sobald das Signal **"JETZT"** auf dem Bildschirm erscheint, musst du so schnell wie möglich eine **beliebige Taste** drücken.
5. **Ergebnis:** Deine Reaktionszeit wird dir direkt in Millisekunden (ms) angezeigt.

**Achtung – Fehlstart!**
Wenn du ungeduldig bist und eine Taste drückst, *bevor* "JETZT" erscheint, wird die Runde sofort als Fehlversuch ("Fehlstart") gewertet und bringt dir 0 Punkte für diese Runde.

### Steuerung
* `[s]` : Spiel starten
* `[h]` : Hilfe im Menü anzeigen
* `[q]` : Spiel beenden
* `[1]`, `[2]`, `[3]` : Level-Auswahl (1 = Leicht, 2 = Mittel, 3 = Schwer)
* **Beliebige Taste** : Reagieren bei "JETZT" oder Auslöser für einen Fehlstart während der Wartezeit

---

## Implementierte Bonusfeatures

Das Spiel enthält über die Grundanforderungen hinaus folgende Sonderfunktionen:

* **Statistik & Auswertung:** Nach einem Set von Runden liefert dir das Spiel eine detaillierte Zusammenfassung. Du siehst deine Gesamtpunkte, deine beste Einzelzeit, die Anzahl der Fehlversuche sowie deine Durchschnittszeit (bereinigt um die Fehlstarts).
* **Dauerhafter Highscore:** Die beste Gesamtzeit wird über den NVMC (Non-Volatile Memory Controller) im Flash-Speicher gesichert. Sie bleibt auch nach einem Neustart des Mikrocontrollers erhalten und wird als "All-Time Highscore" für den jeweiligen Schwierigkeitsgrad angezeigt.
* **Schwierigkeitsstufen:** Die drei wählbaren Level passen das Basis-Zeitfenster der Zufalls-Wartezeit (RNG) über einen Offset an. Je schwerer das Level, desto unvorhersehbarer wird die Wartezeit.
* **System-Überwachung (Watchdog Timer):** Um die Stabilität des Spiels zu gewährleisten, läuft im Hintergrund ein Watchdog Timer (WDT) mit. Dieser überwacht den Programmfluss. Bei zu langer Inaktivität (z. B. wenn man nach dem Startschuss einfach nicht reagiert) bricht der WDT die aktuelle Runde sicherheitshalber ab und führt einen System-Reset durch. So hängt sich der Mikrocontroller niemals auf und man landet wieder im Hauptmenü, um direkt eine neue Runde starten zu können.

---
## ⚙️ Wichtiger Hinweis zur PC-Anpassung (Emulator)

Da das Spiel hardwarenah entwickelt wurde und über den Emulator (Renode) läuft, kann die reale Ausführungsgeschwindigkeit je nach Leistung deines Host-PCs leicht schwanken.

**Anpassung der Timer-Geschwindigkeit:**
Sollten die Wartezeiten im Emulator viel zu lang oder zu kurz ausfallen, kannst du das Spiel kalibrieren. Öffne dazu die Datei `rndmTimer.h`. Dort findest du die Konstante **`TIME_SCALING_FACTOR`**.

Indem du diesen Wert veränderst, kannst du die Geschwindigkeit der Timer skalieren, bis die Berechnungen perfekt auf die Rechenleistung deines PCs abgestimmt sind.


### Gruppe Compiler Mitglieder
 - Lisa Bach 
 - Robin Hübner
 - Nikita Lechner
 - Philipp Schaude
 - Jakob Bohnert

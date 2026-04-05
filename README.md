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

## Spielnaleitung


### Gruppe Compiler Mitglieder
 - Lisa Bach 
 - Robin Hübner
 - Nikita Lechner
 - Philipp Schaude
 - Jakob Bohnert

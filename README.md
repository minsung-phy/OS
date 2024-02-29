# Minsung OS
Simple CLI-based OS development project

## Development timeframe
- 12.26.2023 ~ 02.29.2024

## Setup
- 'C'
- 'Linux (Ubuntu 22.04.3 LTS)'
- **Install Assembler and Emulator**
  '''
  sudo apt-get install qemu
  '''
- **Install Kernel Compiler**
  '''
  sudo apt-get install i386-elf-binutils
  sudo apt-get install i386-elf-gcc
  sudo apt-get install i386-elf-gdb
  '''

  '''
  export PATH="/usr/local/Cellar/x86_64-elf-binutils/<version>/bin/:/usr/local/Cellar/x86_64-elf-gcc/<version>/bin/:$PATH"
  '''
## Key features and functions
- Bootloader
- VGA Drivers
- Keyboard Drivers
- Shells
- Dynamic Memory Management

## Usage
- 'make run'

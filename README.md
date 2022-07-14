{
    "preLaunchTask": "preRun",
    "type": "cppdbg",
    "name": "ESP32 OpenOCD",
    "request": "launch",
    "cwd": "${workspaceFolder}/build",
    "program": "${workspaceFolder}/build/blink.elf",
    "miDebuggerPath": "C:/Users/anton/.espressif/tools/xtensa-esp32-elf/esp-2021r1-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gdb.exe",
    "setupCommands": [
      {"text": "target remote localhost:3333"},
      {"text": "set remote hardware-watchpoint-limit 2"},
      {"text": "monitor reset halt"},
      {"text": "flushregs"},
      {"text": "mon program_esp build/bootloader/bootloader.bin 0x1000 verify"},
      {"text": "mon program_esp build/partition_table/partition-table.bin 0x8000 verify"},
      {"text": "mon program_esp build/blink.bin 0x10000"},
      {"text": "monitor reset halt"},
      {"text": "flushregs"}
    ]
  }
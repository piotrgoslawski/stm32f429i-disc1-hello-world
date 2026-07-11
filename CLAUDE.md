# STM32F429I-DISC1 Hello World

Bare-metal CMake project targeting the STM32F429I-Discovery board (STM32F429ZI,
168 MHz, 2 MB FLASH, 256 KB RAM). See README.md for the full pinout table and
CLion setup; this file covers what a coding agent needs to know.

## Build

```sh
export PATH="/opt/st/stm32cubeclt_1.22.0/GNU-tools-for-STM32/bin:/opt/st/stm32cubeclt_1.22.0/Ninja/bin:$PATH"
cmake --preset Debug
cmake --build build/Debug
```

`CMakePresets.json` already injects this PATH for tools that read presets (CLion, `cmake --preset`),
but a plain shell needs the `export` first. Output: `build/Debug/hello_lcd.elf`.

## HAL driver source

The project's own `Drivers/` tree is stripped down and has **no SPI HAL**. For
any SPI/peripheral work, pull headers/sources from the full STM32Cube F4 package
instead:

```
/home/yesiot/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/
```

## Clock configuration

HSE = 8 MHz crystal on-board. PLL: M=8, N=336, P=2 → 168 MHz SYSCLK.
SPI5 is on APB2 (84 MHz); current prescaler is /8 → ~10.5 MHz SCK.
Changing PLL or prescaler values affects both the LCD SPI clock and any
timing-sensitive peripheral — recompute downstream clocks, don't just bump one field.

## Working with this codebase

- This is register/peripheral-level embedded code: a config that compiles fine
  can still be wrong (bad alternate-function mapping, wrong prescaler, wrong
  clock source) and only shows up on real hardware. When changing GPIO AF
  selections, clock trees, or SPI/DMA timing, cross-check against the STM32F429
  reference manual (RM0090) rather than relying on the HAL compiling cleanly.
- I can't observe the physical board (LCD output, LED blink rate). A successful
  build is necessary but not sufficient — after a change that affects observable
  behavior, say so explicitly and ask for on-hardware confirmation rather than
  reporting the task done from build success alone.
- Flashing: STM32CubeProgrammer or `st-flash write build/Debug/hello_lcd.bin 0x08000000`.
  Flashing is a physical, hard-to-observe action — confirm with the user before
  invoking it, since a bad image can leave the board in a state that needs a
  debugger/SWD to recover.

#ifndef PIZEROLIUPCB_CFLAGS_H
#define PIZEROLIUPCB_CFLAGS_H 1
// Board config for pico_shared HW_CONFIG 18: Liu Szuping's PCB for the
// Waveshare RP2350-PiZero (RP2350B, 16 MB flash) — HSTX video on the PCB's
// HDMI port (the PiZero's own HDMI port is not used), PCM5102 I2S DAC, two
// NES/SNES controller ports with separate CLK/LAT, an SD card on SPI1 and
// PIO-USB host on the PiZero's PIO-USB port (GPIO 28/29). PSRAM is optional
// and has to be soldered onto the PiZero (QMI CS1 = GPIO 47).
// The build uses PICO_BOARD=waveshare_rp2350_pizero (boards/, copied from
// pico_shared): the SD pins need the RP2350B pin tables.
// Pin values mirror pico-infonesPlus/pico_shared/BoardConfigs.cmake HW_CONFIG 18.
#define PICO_SCANVIDEO_COLOR_PIN_BASE 8
#define PICO_SCANVIDEO_PIXEL_RCOUNT 5
#define PICO_SCANVIDEO_PIXEL_GCOUNT 5
#define PICO_SCANVIDEO_PIXEL_BCOUNT 5
// also affects palette conversion for hstx! pico_hdmi expects RGB555.
#define PICO_SCANVIDEO_PIXEL_RSHIFT 10
#define PICO_SCANVIDEO_PIXEL_GSHIFT 5
#define PICO_SCANVIDEO_PIXEL_BSHIFT 0
#define PICO_SCANVIDEO_SYNC_PIN_BASE 6
// SD pins on hardware SPI1 (GPIO 30 = SCK, 31 = TX, 40 = RX; CS 43 is a plain
// GPIO). GPIO 40 is only in the vendored pico_fatfs SPI1 table on RP2350B,
// hence the board header above. SD_TX = MOSI, SD_RX = MISO.
#define SD_TX 31
#define SD_RX 40
#define SD_SCK 30
#define SD_CS 43
#define USE_SD 1
#define USE_HSTX 1
// UART0 on GPIO 0/1 (the board header's default): free on this PCB, so stdio
// and the UART keyboard input path stay on (UART_ENABLED 1 in BoardConfigs).
// I2S audio: PCM5102 DAC. Driver 2 = PICO_AUDIO_I2S_DRIVER_PCM5000A (see
// audio_i2s.h). Audio always ALSO goes out over HDMI; without a
// headphone-detect pin the build drives both sinks simultaneously (see
// i_picosound.c). Same pins as the Murmulator M2.
#define DOOM_AUDIO_I2S_DRIVER 2
#define PICO_AUDIO_I2S_DATA_PIN 9
#define PICO_AUDIO_I2S_CLOCK_PIN_BASE 10
#define PICO_AUDIO_I2S_CLOCK_PINS_SWAPPED 0
#define PICO_AUDIO_I2S_PIO 1
#define PICO_AUDIO_I2S_RESET_PIN -1
#define PICO_AUDIO_I2S_INTERRUPT_PIN -1
#define PICO_AUDIO_I2S_INTERRUPT_IS_BUTTON 0
// PCM510x has no volume register and benefits from the DC blocker in
// audio_i2s.c (see the I2S_AUDIO_COMPENSATE_DC_OFFSET note in audio_i2s.h).
#define I2S_AUDIO_COMPENSATE_DC_OFFSET 1
// No TLV320 codec and no Wii extension port on this board. The pins are unused
// (doom_wiipad.cpp compiles away), but WIIPAD_I2C must stay a valid i2c
// instance: tlv320dac3100.c uses it as an i2c_inst_t* in always-compiled code.
#define WIIPAD_I2C i2c1
#define WII_PIN_SDA -1
#define WII_PIN_SCL -1
// Pin the pico_hdmi audio Data-Island ring at 0x20076000. That address is
// SHORTPTR_BASE + 0x40000 — the top of Doom's zone heap (see i_system.c's
// I_ZoneBase()) — and __HeapLimit is 0x20080000, so the 36 KB ring lives in
// the 40 KB SRAM gap between the zone and the SCRATCH banks. Neither Doom's
// zone allocator nor the pico-sdk's stack region touches this address, so
// the ring costs zero zone bytes AND zero .bss bytes. The vendored
// hstx_data_island_queue.c honors this at compile time; without it, it
// would malloc from the zone (its historical behaviour).
#define HSTX_DI_RING_ADDRESS 0x20076000u

// Override the emu8950 OPL native rate (49716 Hz) with a standard HDMI-audio
// rate. pico_hdmi's ACR N/CTS table only covers 32k/44.1k/48k/88.2k/96k/etc.,
// and 49716 falls through to the 48 kHz default — sink plays at 48000 while
// we push at 49716, causing constant FIFO overruns that sound like static.
//
// 48000 Hz is the exact-lock rate for a 25.2 MHz pixel clock (N=6144,
// CTS=25200) AND matches the sample-frequency code baked into
// hstx_packet.c's channel_status_bit[] table. Strict HDMI monitors require
// the IEC 60958 channel status sample-rate field to agree with the actual
// stream rate, so both must be 48 kHz.
#undef PICO_SOUND_SAMPLE_FREQ
#define PICO_SOUND_SAMPLE_FREQ 48000
// HSTX lanes on the PCB's HDMI port (BoardConfigs HW_CONFIG 18; inverted:
// D- = D+ - 1), the same as the Fruit Jam and the Murmulator M2. pico_hdmi
// (video_output.c) reads these.
#define GPIOHSTXCK 13
#define GPIOHSTXD0 15
#define GPIOHSTXD1 17
#define GPIOHSTXD2 19
#define GPIOHSTXINVERTED 1

// USB host is PIO-USB on the PiZero's PIO-USB port, D+ on GPIO 28 and D- on
// GPIO 29. No VBUS-enable GPIO on this board. Keep in sync with the
// ENABLE_PIO_USB default (ON) in src/pico/CMakeLists.txt.
#define HAS_USBPIO
#define PIN_USB_HOST_DP (28u)
#define PIN_USB_HOST_DM (29u)

// pico_shared BoardConfigs.cmake identity of this board. Consumed by the
// vendored nespad.cpp to pick its PIO program variant; 18 takes the original
// program (separate CLK/LAT per port).
#define HW_CONFIG 18
// Two legacy NES/SNES controller ports (SNES auto-detected), polled via PIO
// by the vendored pico_shared nespad driver — see i_usbhid.cpp. Each port has
// its own CLK and LAT. The PCB's ESP32 socket (BlueRetro, for Bluetooth
// controllers) shares these pins: remove the module to use the physical ports.
#define NES_PIN_CLK 2
#define NES_PIN_DATA 4
#define NES_PIN_LAT 3
#define NES_PIO pio1
#define NES_PIN_CLK_1 5
#define NES_PIN_DATA_1 7
#define NES_PIN_LAT_1 6
#define NES_PIO_1 pio1

// --- Status LEDs (src/pico/doom_leds.c) -------------------------------------
// -1 means the board does not have it, as with the NES pins above.
// Plain LED, blinked every 60 frames (LED_GPIO_PIN 25 in BoardConfigs).
#define DOOM_LED_PIN 25
// No NeoPixels on this board, so no VU meter.
#define DOOM_VU_WS2812_PIN -1

// Move the WAD base address up so it lives past the bootloader app slot.
// Standalone build: 0x10080000 (image at 0x10000000, WHX right after the
// 512 KB slot).
// pico-bootLoader build (BUILD_FOR_BOOTLOADER=1): 0x10200000. The PiZero has
// 16 MB of flash, so this is the Fruit Jam map:
//   0x10000000 bootloader (512 KB)
//   0x10080000 emulator app slot (1.5 MB, DOOM_APP_SIZE=0x180000 in the build)
//   0x10200000 doom WHX -> ends 0x103B7898
// The WHX sits above the app slot so it survives re-flashing doom (or a smaller
// emulator). The build script passes matching -DDOOM_APP_SIZE / picotool -o;
// keep them in sync. See cmake/BootPartition.cmake for the full flash-map
// rationale.
#undef TINY_WAD_ADDR
#if BUILD_FOR_BOOTLOADER
#define TINY_WAD_ADDR 0x10200000
#else
#define TINY_WAD_ADDR 0x10080000
#endif

// --- SD card and PSRAM ------------------------------------------------------
// SD SPI instance for the vendored pico_fatfs driver (the SD_* pins above are
// valid hardware-SPI1 pins on RP2350B) and the PSRAM chip select: QMI CS1 =
// GPIO 47. The PiZero has no PSRAM unless one is soldered on, so
// doom_tiny_full needs it fitted; doom_tiny does not use it. Values mirror
// pico-infonesPlus/pico_shared/BoardConfigs.cmake HW_CONFIG 18 (PSRAM_CS_PIN
// defaults to 47 there).
// The card is used by every build, for save games and settings
// (src/pico/doom_sdcard.c); doom_tiny_full additionally streams the WHD off
// it at boot.
#define SDCARD_SPI spi1
#define SDCARD_PIO pio1
#define PSRAM_CS_PIN 47
#if WHD_LOAD_FROM_SD
// The full WHD is copied from /roms/doom/doom.whd into PSRAM at boot
// (src/pico/whd_sdload.c) and read zero-copy through the cached XIP CS1
// window — same address for standalone and bootloader builds.
#undef TINY_WAD_ADDR
#define TINY_WAD_ADDR 0x11000000
#endif

#endif

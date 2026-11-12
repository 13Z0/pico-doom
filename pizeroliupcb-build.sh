#!/bin/sh
# HW_CONFIG 18: Liu Szuping's PCB for the Waveshare RP2350-PiZero (RP2350B,
# 16 MB flash) — HSTX video, PCM5102 DAC, two NES/SNES ports, SD on SPI1.
# USB host is PIO-USB on GPIO 28/29 — the ENABLE_PIO_USB default (ON) matches
# HAS_USBPIO in pizeroliupcb_cflags.h. PICO_BOARD=waveshare_rp2350_pizero comes
# from boards/ (not in the pico-sdk); the SD pins need its RP2350B pin tables.
cd "$(dirname "$0")" || exit 1
TAG=pizeroliupcb
# USB host runs on PIO: PICO_PIO_USB_PATH is required (matches HAS_USBPIO).
NEED_PIO_USB=1
. ./pico-env.sh
BUILD=build_${TAG}
export CFLAGS="-include $(pwd)/${TAG}_cflags.h -g3 -ggdb"
export CXXFLAGS="-include $(pwd)/${TAG}_cflags.h -g3 -ggdb"
cmake -S . -B $BUILD \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DPICO_SDK_PATH="$PICO_SDK_PATH" \
    -DPICO_EXTRAS_PATH="$PICO_EXTRAS_PATH" \
    -DPICOTOOL_FETCH_FROM_GIT_PATH="$(pwd)/picotool" \
    -DPICO_BOARD=waveshare_rp2350_pizero \
    -DUSE_HSTX=1 \
    ${CMAKE_ARGS} "$@" || exit 1

make -C $BUILD -j$(nproc) || exit 1

PICOTOOL=./picotool/picotool-build/picotool
if [ ! -x "$PICOTOOL" ]; then
    PICOTOOL=$(command -v picotool)
fi
if [ -z "$PICOTOOL" ]; then
    echo "picotool not found (looked at ./picotool/picotool-build/picotool and PATH)" >&2
    exit 1
fi
# Standalone (non-bootloader) build: doom owns flash from 0x10000000 and the
# WHX is flashed at 0x10080000, matching the non-bootloader TINY_WAD_ADDR in
# pizeroliupcb_cflags.h. (The bootloader build uses a different map -- see
# pizeroliupcb-build-forbootloader.sh.)
"$PICOTOOL" uf2 convert doom1.whx -t bin $BUILD/src/doom1-whx.uf2 -o 0x10080000 --family data

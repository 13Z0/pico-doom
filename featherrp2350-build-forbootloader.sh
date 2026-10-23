#!/bin/sh
# HW_CONFIG 14 (Adafruit Feather RP2350 + TLV320) built for the pico-bootLoader.
# Same flash map as every other board: a 1.5 MB app slot at 0x10080000 and the
# WHX at 0x10200000, ending 0x103B7898.
cd "$(dirname "$0")" || exit 1
TAG=featherrp2350
# USB host runs on PIO: PICO_PIO_USB_PATH is required (matches HAS_USBPIO).
NEED_PIO_USB=1
. ./pico-env.sh
BUILD=build_bl_${TAG}
export CFLAGS="-include $(pwd)/${TAG}_cflags.h -g3 -ggdb"
export CXXFLAGS="-include $(pwd)/${TAG}_cflags.h -g3 -ggdb"
cmake -S . -B $BUILD \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DPICO_SDK_PATH="$PICO_SDK_PATH" \
    -DPICO_EXTRAS_PATH="$PICO_EXTRAS_PATH" \
    -DPICOTOOL_FETCH_FROM_GIT_PATH="$(pwd)/picotool" \
    -DPICO_BOARD=adafruit_feather_rp2350 \
    -DUSE_HSTX=1 \
    -DBUILD_FOR_BOOTLOADER=ON \
    -DDOOM_APP_SIZE=0x180000 \
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
# pico-bootLoader flash map: WHX lives at 0x10200000 (past the 1.5 MB slot
# reserved for the doom_tiny image at 0x10080000), as on every board. Must
# match TINY_WAD_ADDR in featherrp2350_cflags.h (the BUILD_FOR_BOOTLOADER branch), and
# the address the pico-bootLoader's updateAll converts doom1.whx at.
"$PICOTOOL" uf2 convert doom1.whx -t bin $BUILD/src/doom1-whx-bootloader.uf2 -o 0x10200000 --family data

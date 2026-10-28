//
// Copyright(C) 2026 Frank Hoedemakers
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	The 1 ms PIO-USB frame, run here instead of by Pico-PIO-USB, with a
//	workaround for erratum RP2350-E9. See doom_usb_frame.c.
//

#ifndef DOOM_USB_FRAME_H
#define DOOM_USB_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif

// Call once, after tusb_init() has brought up the PIO-USB host. Needs
// skip_alarm_pool in the PIO-USB configuration (d_main.c, D_DoomLoop).
void doom_usb_frame_start(void);

#ifdef __cplusplus
}
#endif

#endif // DOOM_USB_FRAME_H

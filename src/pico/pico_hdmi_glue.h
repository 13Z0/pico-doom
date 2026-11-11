/*
 *  pico_hdmi_glue.h
 *
 *  Doom-specific glue over pico_shared's pico_hdmi driver. We bypass the
 *  driver's own 154 KB static FRAMEBUFFER and install our own scanline
 *  callback (defined in i_video.c) that reads Doom's 8bpp frame_buffer[]
 *  directly.
 *
 *  This module also owns the runtime audio-sink routing (SINK_HDMI when
 *  headphones are unplugged, SINK_HEADPHONES when plugged, and the I2S or PWM
 *  output in DVI display mode, which carries no audio) and exposes the
 *  hstx_push_audio_sample() function normally provided by pico_hdmi/hstx.c
 *  (which we don't compile — its FRAMEBUFFER would waste RAM).
 */

#ifndef PICO_HDMI_GLUE_H
#define PICO_HDMI_GLUE_H 1

#include <stdbool.h>
#include <stdint.h>

#include "video_output.h"
#include "audio_i2s.h"       // PICO_AUDIO_I2S_DRIVER_* ids

// Which audio_i2s driver this board carries — set in the board's force-included
// <tag>_cflags.h as a numeric literal (1 = TLV320, 2 = PCM5000A, values from
// audio_i2s.h). Defaults to the TLV320 (Fruit Jam) when a header predates the
// multi-board split.
#ifndef DOOM_AUDIO_I2S_DRIVER
#define DOOM_AUDIO_I2S_DRIVER PICO_AUDIO_I2S_DRIVER_TLV320
#endif

// Boards without an I2S DAC (driver 0 = PICO_AUDIO_I2S_DRIVER_NONE, e.g. the
// Olimex RP2040-PICO-PC) play over HDMI and, where the board has one, the PWM
// audio jack. The I2S ring is then never allocated, so nothing may enqueue to
// it or pace the mixer by it; the HDMI data-island ring does the pacing alone
// (in DVI mode the PWM jack's fill does).
#define DOOM_AUDIO_HAS_I2S (DOOM_AUDIO_I2S_DRIVER != PICO_AUDIO_I2S_DRIVER_NONE)

// Headphone-detect is only available on the TLV320 with its INT pin wired
// (Fruit Jam). Boards without it (PCM5100A boards, Feather's TLV320 breakout)
// can't switch sinks at runtime, so they drive HDMI and I2S simultaneously.
#if DOOM_AUDIO_I2S_DRIVER == PICO_AUDIO_I2S_DRIVER_TLV320 && PICO_AUDIO_I2S_INTERRUPT_PIN != -1
#define DOOM_AUDIO_EXCLUSIVE_SINK 1
#else
#define DOOM_AUDIO_EXCLUSIVE_SINK 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DOOM_SINK_HDMI = 0,
    DOOM_SINK_HEADPHONES = 1,
} doom_audio_sink_t;

extern volatile doom_audio_sink_t doom_audio_sink;

// DVI display mode: no audio on the cable, and the HSTX driver does not drain
// the data-island ring. i_picosound.c then sends nothing to HDMI.
extern volatile bool doom_audio_dvi;

// Bring up HSTX video + HDMI audio data-island stream. Registers
// `scanline_cb` and `vsync_cb` (both invoked from DMA_IRQ_0 on core1) plus
// `bg_task` (invoked from core1's main loop between DMA IRQs — safe to do
// heavy per-frame work like palette conversion here). Sets the HDMI audio
// sample rate then launches core1 running the HSTX servicing loop. Call
// once from I_InitGraphics on core0.
void doom_hdmi_init(video_output_scanline_cb_t scanline_cb,
                    video_output_vsync_cb_t vsync_cb,
                    video_output_task_fn bg_task,
                    uint32_t audio_sample_rate);

// Push one stereo sample to the HDMI data-island audio stream. Accumulates
// four samples then encodes+queues one packet; drops on high watermark.
// Lifted verbatim from pico_shared/pico_hdmi/hstx.c (which we don't compile).
void hstx_push_audio_sample(int left, int right);

// Poll the TLV320 headphone-detect state. Call once per frame from core0.
// Sets doom_audio_sink for the jack and the display mode. HDMI mode: HDMI, or
// the headphones while they are plugged in, and the internal speaker never
// plays. DVI mode: I2S, to the speaker unless headphones are plugged in.
void doom_poll_headphone(void);

// Switches the HDMI output between HDMI and DVI while the picture runs, and
// the sound with it (at the next doom_poll_headphone).
void doom_hdmi_set_dvi(bool dvi);

#ifdef __cplusplus
}
#endif

#endif

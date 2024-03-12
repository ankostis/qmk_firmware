/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#ifdef VIA_ENABLE
/* VIA configuration. */
#    define DYNAMIC_KEYMAP_LAYER_COUNT 7
#endif // VIA_ENABLE

#ifndef __arm__
/* Disable unused features. */
#    define NO_ACTION_ONESHOT
#endif // __arm__

#define SPLIT_LAYER_STATE_ENABLE  // comm overhead
#define SPLIT_MODS_ENABLE
#define SPLIT_LED_STATE_ENABLE

#define CAPS_WORD_INVERT_ON_SHIFT

/* Charybdis-specific features. */

#ifdef POINTING_DEVICE_ENABLE

#define CHARYBDIS_DRAGSCROLL_REVERSE_Y      // mimic up/down movement: scroll towards drag

/**
 * Forbid low DPIs where PWM3360 misbehaves
 * (ie. emits sudden V spikes in small movements)
*/
#define CHARYBDIS_MINIMUM_DEFAULT_DPI           400  // default(400)
#define CHARYBDIS_DEFAULT_DPI_CONFIG_STEP       400  // default(200)
#define CHARYBDIS_DRAGSCROLL_DPI                400  // default(100)

// Drag-scroll changes DPI on activation, but this causes issues,
// better keep both identical (400).  So have to compensate drag-scroll divider
//  (`CHARYBDIS_DRAGSCROLL_BUFFER_SIZE`)
//
// Maccel mouse reports are DPI-independent, configure scroll speed with a fixed factor.
#define CHARYBDIS_DRAGSCROLL_BUFFER_SIZE        12          // default(6)

// Send coalesced events with scroll values ≥ ±1 (vs queueing ±1 events)?
#define CHARYBDIS_DRAGSCROLL_SEND_COALESCE

// https://www.desmos.com/calculator/vtfkbxwj8s
#define MACCEL_CPI         120     // --/++ faster/slower
#define MACCEL_TAKEOFF     6.8     // --/++ curve starts smoothlier/abruptlier
#define MACCEL_GROWTH_RATE 0.6     // --/++ curve reaches max limit slower/faster
#define MACCEL_OFFSET      1.0     //--/++ growth kicks in earlier/later
#define MACCEL_LIMIT       9.6     //maximum acceleration factor

// If need, reduce mouse events frequency to cope with MCU load;
// note that it delays (but smoothens) drag-scrolling, particularly when
// queued (ie. `CHARYBDIS_DRAGSCROLL_SEND_COALESCE` undefined).
// #undef  POINTING_DEVICE_TASK_THROTTLE_MS
// #define POINTING_DEVICE_TASK_THROTTLE_MS 5

#define MACCEL_USE_KEYCODES

// For "maccel" configs, see `keymap.c` file.
#define MOUSE_EXTENDED_REPORT
#define EECONFIG_USER_DATA_SIZE 24

// To view mouse's distance/velocity while configuring maccel,
// set `CONSOLE_ENABLE = yes` in `rules.mk` and uncomment the lines below,
// and run `qmk console` in the shell:
#define MACCEL_DEBUG
#ifdef MACCEL_DEBUG
#   undef PRINTF_SUPPORT_DECIMAL_SPECIFIERS
#   define PRINTF_SUPPORT_DECIMAL_SPECIFIERS 1
#endif  // MACCEL_DEBUG

#endif // POINTING_DEVICE_ENABLE

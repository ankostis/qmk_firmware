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
#define CHARYBDIS_MINIMUM_DEFAULT_DPI 600   // default(400)
#define CHARYBDIS_MINIMUM_SNIPING_DPI 200   // default(200)

#endif // POINTING_DEVICE_ENABLE

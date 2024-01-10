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
#include QMK_KEYBOARD_H

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
#    include "timer.h"
#endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

enum charybdis_keymap_layers {
    LAYER_BASE = 0,
    LAYER_FUNCTION,
    LAYER_NAVIGATION,
    LAYER_MEDIA,
    LAYER_POINTER,
    LAYER_NUMERAL,
    LAYER_SYMBOLS,
};

// Automatically enable sniping-mode on the pointer layer.
#define CHARYBDIS_AUTO_SNIPING_ON_LAYER LAYER_POINTER

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
static uint16_t auto_pointer_layer_timer = 0;

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS 1000
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD 8
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#endif     // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#define ESC_MED LT(LAYER_MEDIA, KC_ESC)
#define SPC_NAV LT(LAYER_NAVIGATION, KC_SPC)
#define TAB_PTR LT(LAYER_POINTER, KC_TAB)
#define LA2_PTR LT(LAYER_POINTER, KC_SLSH)
#define DEL_SYM LT(LAYER_SYMBOLS, KC_DEL)
#define BSP_NUM LT(LAYER_NUMERAL, KC_BSPC)
#define ENT_FUN LT(LAYER_FUNCTION, KC_ENT)

#define APP_RAL MT(MOD_RALT, KC_APP)
#define PRN_CTL MT(MOD_RCTL, KC_PSCR)

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif // !POINTING_DEVICE_ENABLE

// Strange, but these do not work!?
#define KC_UNDO LCTL(KC_Z)
#define KC_AGIN LCTL(KC_Y)

// clang-format off
/** \brief NUM+ColemakDH layout (4 rows, 2x6 columns, 5+3 thumbs), punctuations
 * in the outer/surrounding keys, miryoku hold_n_tap in the thumbs.
 */
#define LAYOUT_LAYER_BASE                                                                              \
     KC_GRV,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,     KC_6,   KC_7,   KC_8,   KC_9,   KC_0, KC_EQL, \
    KC_LBRC,   KC_Q,   KC_W,   KC_F,   KC_P,   KC_B,     KC_J,   KC_L,   KC_U,   KC_Y,KC_SCLN,KC_RBRC, \
    KC_QUOT,   KC_A,   KC_R,   KC_S,   KC_T,   KC_G,     KC_M,   KC_N,   KC_E,   KC_I,   KC_O,KC_QUOT, \
    KC_COMM,   KC_Z,   KC_X,   KC_C,   KC_D,   KC_V,     KC_K,   KC_H,KC_BSLS,LA2_PTR,KC_MINS, KC_DOT, \
                            ESC_MED,SPC_NAV,TAB_PTR,                                  DEL_SYM,BSP_NUM, \
                                    APP_RAL,PRN_CTL,                                  ENT_FUN

/** Convenience row shorthands. */
#define _________________DEAD_HALF_ROW_________________  XXXXXXX,XXXXXXX,XXXXXXX,XXXXXXX,XXXXXXX,XXXXXXX
#define ________________HOME_ROW_GACS_L________________  XXXXXXX,KC_LGUI,KC_LALT,KC_LCTL,KC_LSFT,XXXXXXX
#define ________________HOME_ROW_GACS_R________________  XXXXXXX,KC_LSFT,KC_LCTL,KC_LALT,KC_LGUI,XXXXXXX

/*
 * Layers used on the Charybdis 4x6.
 *
 * These layers started off heavily inspired by the Miryoku layout, but trimmed
 * down for 3x5 and then enhanced for 4x6.
 *
 * See https://github.com/manna-harbour/miryoku for the original layout.
 */

/**
 * \brief Function layer.
 *
 * Secondary left-hand layer has function keys mirroring the numerals on the
 * primary layer with above F9 on the top row, plus system keys on the inner
 * column. App is on the tertiary thumb key and other thumb keys are duplicated
 * from the base layer to enable auto-repeat.
 */
#define LAYOUT_LAYER_FUNCTION                                                                          \
    _________________DEAD_HALF_ROW_________________,  _________________DEAD_HALF_ROW_________________, \
    XXXXXXX, KC_F12,  KC_F7,  KC_F8,  KC_F9,KC_PSCR,  _________________DEAD_HALF_ROW_________________, \
    XXXXXXX, KC_F11,  KC_F4,  KC_F5,  KC_F6,KC_SCRL,  ________________HOME_ROW_GACS_R________________, \
    XXXXXXX, KC_F10,  KC_F1,  KC_F2,  KC_F3,KC_PAUS,  _________________DEAD_HALF_ROW_________________, \
                            XXXXXXX,XXXXXXX,XXXXXXX,                                  XXXXXXX,XXXXXXX, \
                                    APP_RAL,PRN_CTL,                                  _______


/**
 * \brief Media layer.
 *
 * Tertiary left- and right-hand layer is media and RGB control.  This layer is
 * symmetrical to accommodate the left- and right-hand trackball.
 */
#define LAYOUT_LAYER_MEDIA                                                                             \
    _________________DEAD_HALF_ROW_________________,  _________________DEAD_HALF_ROW_________________, \
    XXXXXXX,XXXXXXX,RGB_RMOD,RGB_TOG,RGB_MOD,XXXXXXX,XXXXXXX,RGB_RMOD,RGB_TOG,RGB_MOD,XXXXXXX,XXXXXXX, \
    XXXXXXX,KC_MPRV,KC_VOLD,KC_MUTE,KC_VOLU,KC_MNXT,  KC_MPRV,KC_VOLD,KC_MUTE,KC_VOLU,KC_MNXT,XXXXXXX, \
    XXXXXXX,XXXXXXX,XXXXXXX,XXXXXXX, EE_CLR,QK_BOOT,  QK_BOOT, EE_CLR,XXXXXXX,XXXXXXX,XXXXXXX,XXXXXXX, \
                            _______,KC_MPLY,KC_MSTP,                                  KC_MSTP,KC_MPLY, \
                                    APP_RAL,PRN_CTL,                                  XXXXXXX



/** \brief Mouse emulation and pointer functions.  The mouse buttons are located
 * above track ball close to the right side home-row, along with copy, paste, undo and
 * redo keys, allowing one-hand editing & browsing.
 */
#define LAYOUT_LAYER_POINTER                                                                           \
    _________________DEAD_HALF_ROW_________________,  _________________DEAD_HALF_ROW_________________, \
    _________________DEAD_HALF_ROW_________________,   KC_CUT,DPI_MOD,KC_BTN3,S_D_MOD,KC_UNDO,KC_AGIN, \
    ________________HOME_ROW_GACS_L________________,  KC_COPY,KC_BTN2,KC_BTN1,XXXXXXX, KC_SPC, KC_TAB, \
    _________________DEAD_HALF_ROW_________________,  KC_PSTE,XXXXXXX,DRGSCRL,_______,SNIPING,KC_CAPS, \
                            XXXXXXX,XXXXXXX,_______,                                  KC_BTN2,KC_BTN1, \
                                    APP_RAL,PRN_CTL,                                  KC_BTN3

/**
 * \brief Navigation layer.
 *
 * Primary right-hand layer (left home thumb) is navigation and editing. Cursor
 * keys are on the home position, line and page movement below, clipboard above,
 * caps lock and insert on the inner column. Thumb keys are duplicated from the
 * base layer to avoid having to layer change mid edit and to enable auto-repeat.
 */
#define LAYOUT_LAYER_NAVIGATION                                                                        \
    _________________DEAD_HALF_ROW_________________,  _________________DEAD_HALF_ROW_________________, \
    _________________DEAD_HALF_ROW_________________,  _________________DEAD_HALF_ROW_________________, \
    ________________HOME_ROW_GACS_L________________,  KC_CAPS,KC_LEFT,KC_DOWN,  KC_UP,KC_RGHT,XXXXXXX, \
    _________________DEAD_HALF_ROW_________________,   KC_INS,KC_HOME,KC_PGDN,KC_PGUP, KC_END,XXXXXXX, \
                            XXXXXXX,_______,XXXXXXX,                                   KC_ENT,KC_BSPC, \
                                    APP_RAL,PRN_CTL,                                   KC_DEL


/**
 * \brief Numeral layout.
 *
 * Primary left-hand layer (right home thumb) is numerals and symbols. Numerals
 * are in the standard numpad locations with symbols in the remaining positions.
 * `KC_DOT` is duplicated from the base layer.
 */
#define LAYOUT_LAYER_NUMERAL                                                                           \
    _________________DEAD_HALF_ROW_________________,  _________________DEAD_HALF_ROW_________________, \
    XXXXXXX,KC_LBRC,   KC_7,   KC_8,   KC_9,KC_RBRC,  _________________DEAD_HALF_ROW_________________, \
    XXXXXXX,KC_SCLN,   KC_4,   KC_5,   KC_6, KC_EQL,  ________________HOME_ROW_GACS_R________________, \
    XXXXXXX, KC_GRV,   KC_1,   KC_2,   KC_3,KC_BSLS,  _________________DEAD_HALF_ROW_________________, \
                             KC_DOT,   KC_0,KC_MINS,                                  XXXXXXX,_______, \
                                    APP_RAL,PRN_CTL,                                  XXXXXXX


/**
 * \brief Symbols layer.
 *
 * Secondary left-hand layer has shifted symbols in the same locations to reduce
 * chording when using mods with shifted symbols. `KC_LPRN` is duplicated next to
 * `KC_RPRN`.
 */
#define LAYOUT_LAYER_SYMBOLS                                                                           \
    _________________DEAD_HALF_ROW_________________,  _________________DEAD_HALF_ROW_________________, \
    XXXXXXX,KC_LCBR,KC_AMPR,KC_ASTR,KC_LPRN,KC_RCBR,  _________________DEAD_HALF_ROW_________________, \
    XXXXXXX,KC_COLN, KC_DLR,KC_PERC,KC_CIRC,KC_PLUS,  ________________HOME_ROW_GACS_R________________, \
    XXXXXXX,KC_TILD,KC_EXLM,  KC_AT,KC_HASH,KC_PIPE,  _________________DEAD_HALF_ROW_________________, \
                            KC_LPRN,KC_RPRN,KC_UNDS,                                  _______,XXXXXXX, \
                                    APP_RAL,PRN_CTL,                                  XXXXXXX

/**
 * \brief Add Home Row mod to a layout.
 *
 * Expects a 12-key per row layout, at least x3 rows (at least 36 keycodes).
 * Adds support for GACS (Gui, Alt, Ctl, Shift) home row.
 *
 * This is meant to be used with `LAYER_ALPHAS_QWERTY` defined above, eg.:
 *
 *     HOME_ROW_MOD_GACS(LAYER_ALPHAS_QWERTY)
 */
#define _HOME_ROW_MOD_GACS(                                                     \
    L00, L01, L02, L03, L04, L05, R06, R07, R08, R09, R10, R11,                 \
    L12, L13, L14, L15, L16, L17, R18, R19, R20, R21, R22, R23,                 \
    L24, L25, L26, L27, L28, L29, R30, R31, R32, R33, R34, R35,                 \
    ...)                                                                        \
          L00,         L01,         L02,         L03,         L04,         L05, \
          R06,         R07,         R08,         R09,         R10,         R11, \
          L12,         L13,         L14,         L15,         L16,         L17, \
          R18,         R19,         R20,         R21,         R22,         R23, \
          L24, LGUI_T(L25), LALT_T(L26), LCTL_T(L27), LSFT_T(L28),         L29, \
          R30, RSFT_T(R31), RCTL_T(R32), LALT_T(R33), RGUI_T(R34),         R35, \
    __VA_ARGS__
#define HOME_ROW_MOD_GACS(...) _HOME_ROW_MOD_GACS(__VA_ARGS__)

#define LAYOUT_wrapper(...) LAYOUT(__VA_ARGS__)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT_wrapper(HOME_ROW_MOD_GACS(LAYOUT_LAYER_BASE)),
  [LAYER_FUNCTION] = LAYOUT_wrapper(LAYOUT_LAYER_FUNCTION),
  [LAYER_NAVIGATION] = LAYOUT_wrapper(LAYOUT_LAYER_NAVIGATION),
  [LAYER_MEDIA] = LAYOUT_wrapper(LAYOUT_LAYER_MEDIA),
  [LAYER_NUMERAL] = LAYOUT_wrapper(LAYOUT_LAYER_NUMERAL),
  [LAYER_POINTER] = LAYOUT_wrapper(LAYOUT_LAYER_POINTER),
  [LAYER_SYMBOLS] = LAYOUT_wrapper(LAYOUT_LAYER_SYMBOLS),
};
// clang-format on

#ifdef POINTING_DEVICE_ENABLE
#    ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (abs(mouse_report.x) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD || abs(mouse_report.y) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD) {
        if (auto_pointer_layer_timer == 0) {
            layer_on(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
            rgb_matrix_mode_noeeprom(RGB_MATRIX_NONE);
            rgb_matrix_sethsv_noeeprom(HSV_GREEN);
#        endif // RGB_MATRIX_ENABLE
        }
        auto_pointer_layer_timer = timer_read();
    }
    return mouse_report;
}

void matrix_scan_user(void) {
    if (auto_pointer_layer_timer != 0 && TIMER_DIFF_16(timer_read(), auto_pointer_layer_timer) >= CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS) {
        auto_pointer_layer_timer = 0;
        layer_off(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
        rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
#        endif // RGB_MATRIX_ENABLE
    }
}
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#    ifdef CHARYBDIS_AUTO_SNIPING_ON_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    charybdis_set_pointer_sniping_enabled(layer_state_cmp(state, CHARYBDIS_AUTO_SNIPING_ON_LAYER));
    return state;
}
#    endif // CHARYBDIS_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLE

#ifdef RGB_MATRIX_ENABLE
// Forward-declare this helper function since it is defined in
// rgb_matrix.c.
void rgb_matrix_update_pwm_buffers(void);
#endif

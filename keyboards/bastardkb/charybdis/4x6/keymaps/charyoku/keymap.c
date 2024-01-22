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
#define LA2_NAV LT(LAYER_NAVIGATION, KC_SLSH)
#define TAB_PTR LT(LAYER_POINTER, KC_TAB)
#define LA2_PTR LT(LAYER_POINTER, KC_DOT)
#define LA3_PTR LT(LAYER_POINTER, KC_END)
#define DEL_SYM LT(LAYER_SYMBOLS, KC_DEL)
#define BSP_NUM LT(LAYER_NUMERAL, KC_BSPC)
#define ENT_FUN LT(LAYER_FUNCTION, KC_ENT)

#define ALT_SCL MT(MOD_RALT, KC_SCLN)
#define CTL_COM MT(MOD_RCTL, KC_COMM)

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif // !POINTING_DEVICE_ENABLE

/* The `KC_COPY/KC_UNDO` etc are *Mac* only keys. */
#define KC_UNDO LCTL(KC_Z)
#define KC_AGIN LCTL(KC_Y)
#define KC_CUT LCTL(KC_X)
#define KC_COPY LCTL(KC_C)
#define KC_PSTE LCTL(KC_V)

// clang-format off
/** \brief NUM+ColemakDH layout (4 rows, 2x6 columns, 5+3 thumbs), punctuations
 * in the outer/surrounding keys, miryoku hold_n_tap in the thumbs.
 *
 * - Put F-keys in place of number-keys (top non-miryoku row) to facilitate
 *   app shortcuts with one hand, eg. *midnight commander*, *vscode*.
 *   Besides, a non-char key is handy in Esc's position ;-)
 *
 * **Deviation from Miryoku:**
 *
 * - Swap `,` with `-`, the former being much usefull in programming, the latter's place
 *   in the num+left thumb rhymes with `.`, both there when num-typing.
 */
#define LAYOUT_LAYER_BASE                                                                              \
      KC_F1,  KC_F2,  KC_F3,  KC_F4,  KC_F5,  KC_F6,    KC_F7,  KC_F8,  KC_F9, KC_F10, KC_F11, KC_F12, \
     KC_APP,   KC_Q,   KC_W,   KC_F,   KC_P,   KC_B,     KC_J,   KC_L,   KC_U,   KC_Y,KC_QUOT,KC_RBRC, \
    CW_TOGG,   KC_A,   KC_R,   KC_S,   KC_T,   KC_G,     KC_M,   KC_N,   KC_E,   KC_I,   KC_O,KC_CAPS, \
    KC_PSCR,   KC_Z,   KC_X,   KC_C,   KC_D,   KC_V,     KC_K,   KC_H,KC_MINS,LA2_PTR,LA2_NAV, KC_DOT, \
                            ESC_MED,SPC_NAV,TAB_PTR,                                  DEL_SYM,BSP_NUM, \
                                    ALT_SCL,CTL_COM,                                  ENT_FUN

/** Convenience row shorthands. */
#define _________________DEAD_HALF_ROW_________________  XXXXXXX,XXXXXXX,XXXXXXX,XXXXXXX,XXXXXXX,XXXXXXX
#define _________________TRNS_HALF_ROW_________________  _______,_______,_______,_______,_______,_______
#define ________________HOME_ROW_GACS_L________________  _______,KC_LGUI,KC_LALT,KC_LCTL,KC_LSFT,XXXXXXX
#define ________________HOME_ROW_GACS_R________________  XXXXXXX,KC_LSFT,KC_LCTL,KC_LALT,KC_LGUI,_______
#define ________________KEYB_CTRL_ROW_L________________   EE_CLR,QK_BOOT, QK_RBT,DB_TOGG,CW_TOGG,XXXXXXX
#define ________________KEYB_CTRL_ROW_R________________  XXXXXXX,CW_TOGG,DB_TOGG, QK_RBT,QK_BOOT, EE_CLR

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
    _________________DEAD_HALF_ROW_________________,  _________________TRNS_HALF_ROW_________________, \
    _______, KC_F12,  KC_F7,  KC_F8,  KC_F9,KC_PSCR,  _________________DEAD_HALF_ROW_________________, \
    _______, KC_F11,  KC_F4,  KC_F5,  KC_F6,KC_SCRL,  ________________HOME_ROW_GACS_R________________, \
    _______, KC_F10,  KC_F1,  KC_F2,  KC_F3,KC_PAUS,  _________________DEAD_HALF_ROW_________________, \
                             KC_ESC,XXXXXXX,XXXXXXX,                                   KC_DEL,KC_BSPC, \
                                    ALT_SCL,CTL_COM,                                  _______


/**
 * \brief Media layer.
 *
 * Tertiary left- and right-hand layer is media and RGB control.  This layer is
 * symmetrical to accommodate the left- and right-hand trackball.
 */
#define LAYOUT_LAYER_MEDIA                                                                             \
    _________________DEAD_HALF_ROW_________________,  _________________DEAD_HALF_ROW_________________, \
    _________________DEAD_HALF_ROW_________________,  RGB_VAI,RGB_HUI,RGB_SAI,RGB_MOD,RGB_SPI,RGB_TOG, \
    ________________HOME_ROW_GACS_L________________,  KC_MPRV,KC_VOLD,KC_MUTE,KC_VOLU,KC_MNXT,RGB_M_P, \
    _________________DEAD_HALF_ROW_________________,  XXXXXXX,KC_BRID,XXXXXXX,KC_BRIU,XXXXXXX,XXXXXXX, \
                            _______,KC_MPLY,KC_MSTP,                                  KC_MSTP,KC_MPLY, \
                                    ALT_SCL,CTL_COM,                                  XXXXXXX



/** \brief Mouse layer featuring common editing keys with *all other right layers*.
 *
 * All mouse buttons & layer activator are located close and around the right trackball,
 * along with copy, paste, undo and redo keys, to facilitate one-hand editing & browsing.
 *
 * Right thumb keys are duplicated from the base layer to avoid having to change layer
 * mid edit just to press Enter or delete a char, and to enable auto-repeat.
 *
 * Without mouse emulation (useless with trackball).
 *
 * **Rational:** with the right-thumb occupied on the trackball, it's easier
 * placing the x3 *mouse buttons* on the primary keys (vs on the right thumb-cluster).
 * And to allow using trackball single-handedly (eg. when browsing),
 * the *ring finger* is chosen as layer activator, as it provides better versasility vs
 * pinning the "short" pinky, which would make the top-alphas row harder to reach and
 * anything on the 5th column unreachable (the original *charybdis 4x6 uses the pinky).
 */
#define LAYOUT_LAYER_POINTER                                                                           \
    _________________DEAD_HALF_ROW_________________,  _________________DEAD_HALF_ROW_________________, \
    _________________DEAD_HALF_ROW_________________,  KC_UNDO,KC_AGIN,KC_BTN3,XXXXXXX,DPI_MOD, KC_TAB, \
    ________________HOME_ROW_GACS_L________________,   KC_CUT,KC_BTN2,KC_BTN1,XXXXXXX,S_D_MOD, KC_SPC, \
    _________________DEAD_HALF_ROW_________________,  KC_COPY,KC_PSTE,DRGSCRL,_______,SNP_TOG, QK_REP, \
                             KC_ESC,KC_BTN1,_______,                                   KC_DEL,KC_BSPC, \
                                    KC_BTN2,KC_BTN3,                                   KC_ENT

/**
 * \brief Navigation layer featuring common editing keys with *all other right layers*.
 *
 * Primary right-hand layer activated by left home thumb & right pinky is for navigation and
 * editing. Cursor keys and line and page movement are on a "cross" centered around
 * home position, while clipboard, undos and other editing keys on prinary & thumbs
 * *mimic the pointer layer*.
 * Practically, with alternating right ring & pinky pinning, single-handed
 * editing & mouse is possible.
 *
 * Notice the "Caps Word" (`CW_TOGG`) in the top-2nd key (part of the `KEYB_CTRL_ROW`)
 * that types in capital just the next word (including underscores)
 */
#define LAYOUT_LAYER_NAVIGATION                                                                        \
    ________________KEYB_CTRL_ROW_L________________,  ________________KEYB_CTRL_ROW_R________________, \
    _________________DEAD_HALF_ROW_________________,  KC_UNDO,KC_AGIN,  KC_UP, KC_INS,KC_PGUP, KC_TAB, \
    ________________HOME_ROW_GACS_L________________,   KC_CUT,KC_LEFT,KC_DOWN,KC_RGHT,KC_PGDN, KC_SPC, \
    _________________DEAD_HALF_ROW_________________,  KC_COPY,KC_PSTE,KC_HOME,LA3_PTR,_______, QK_REP, \
                             KC_ESC,_______,XXXXXXX,                                   KC_DEL,KC_BSPC, \
                                    ALT_SCL,CTL_COM,                                   KC_ENT


/**
 * \brief Numeral layout.
 *
 * Primary left-hand layer (right home thumb) is numerals and symbols. Numerals
 * are in the standard numpad locations with symbols in the remaining positions.
 * `KC_DOT` is duplicated from the base layer.
 *
 * Editing keys (copy-paste, undo) shared with all other right layers.
 */
#define LAYOUT_LAYER_NUMERAL                                                                           \
    ________________KEYB_CTRL_ROW_L________________,  ________________KEYB_CTRL_ROW_R________________, \
    _______,KC_LBRC,   KC_7,   KC_8,   KC_9,KC_RBRC,  KC_UNDO,KC_AGIN,XXXXXXX,XXXXXXX,XXXXXXX, KC_TAB, \
    _______,KC_SCLN,   KC_4,   KC_5,   KC_6, KC_EQL,   KC_CUT,KC_LSFT,KC_LCTL,KC_LALT,KC_LGUI, KC_SPC, \
    _______, KC_GRV,   KC_1,   KC_2,   KC_3,KC_BSLS,  KC_COPY,KC_PSTE,KC_HOME,XXXXXXX,XXXXXXX, QK_REP, \
                             KC_DOT,   KC_0,KC_COMM,                                   KC_DEL,KC_BSPC, \
                                     KC_SPC, KC_TAB,                                   KC_ENT


/**
 * \brief Symbols layer.
 *
 * Secondary left-hand layer has shifted symbols in the same locations to reduce
 * chording when using mods with shifted symbols. `KC_LPRN` is duplicated next to
 * `KC_RPRN`.
 *
 * Editing keys (copy-paste, undo) shared with all other right layers.
 */
#define LAYOUT_LAYER_SYMBOLS                                                                           \
    ________________KEYB_CTRL_ROW_L________________,  ________________KEYB_CTRL_ROW_R________________, \
    _______,KC_LCBR,KC_AMPR,KC_ASTR,KC_LPRN,KC_RCBR,  KC_UNDO,KC_AGIN,XXXXXXX,XXXXXXX,XXXXXXX, KC_TAB, \
    _______,KC_COLN, KC_DLR,KC_PERC,KC_CIRC,KC_PLUS,   KC_CUT,KC_LSFT,KC_LCTL,KC_LALT,KC_LGUI, KC_SPC, \
    _______,KC_TILD,KC_EXLM,  KC_AT,KC_HASH,KC_PIPE,  KC_COPY,KC_PSTE,KC_HOME,XXXXXXX,XXXXXXX, QK_REP, \
                            KC_LPRN,KC_RPRN,  KC_LT,                                   KC_DEL,KC_BSPC, \
                                     KC_SPC, KC_TAB,                                   KC_ENT

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

bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        // Keycodes that continue Caps Word, with shift applied.
        case KC_A ... KC_Z:
        case KC_MINS:
            add_weak_mods(MOD_BIT(KC_LSFT));  // Apply shift to next key.
            return true;

        // Keycodes that continue Caps Word, without shifting.
        case KC_1 ... KC_0:
        case KC_BSPC:
        case KC_DEL:
        case KC_UNDS:
        // extra from defaults
        case KC_SLSH:
        case KC_LEFT:
        case KC_RGHT:
            return true;

        default:
            return false;  // Deactivate Caps Word.
    }
}

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

#define MOD_CTRL_LED_INDEX 53  // 1st-thumb-right
#define MOD_SHIFT_LED_INDEX 54  // 2nd-thumb-right
#define MOD_ALT_LED_INDEX 55  // 3rd-thumb-right

/**
 * Indicate *Caps_Word* with top-inner keys
 * (left-side works only if `SPLIT_LAYER_STATE_ENABLE`)
 */
#define CAPS_WORD_LED_INDEX0 20  // inner-top-left
#define CAPS_WORD_LED_INDEX1 49  // inner-top-right

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case LAYER_POINTER:
            rgb_matrix_set_color_all(RGB_YELLOW);
            break;
        case LAYER_FUNCTION:
            rgb_matrix_set_color_all(RGB_MAGENTA);
            break;
        case LAYER_NAVIGATION:
            rgb_matrix_set_color_all(RGB_CYAN);
            break;
        case LAYER_MEDIA:
            // Just mark volume up/dowe/mute keys, and
            // let colors of kbd funcs to shine.
            rgb_matrix_set_color(47, RGB_MAGENTA);
            rgb_matrix_set_color(42, RGB_MAGENTA);
            rgb_matrix_set_color(39, RGB_MAGENTA);
            break;
        case LAYER_NUMERAL:
            rgb_matrix_set_color_all(RGB_BLUE);
            break;
        case LAYER_SYMBOLS:
            rgb_matrix_set_color_all(RGB_CORAL);
            break;
        default:
            break;
    }
    if (host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color(50, RGB_WHITE);
    }
    if (is_caps_word_on()) {
        rgb_matrix_set_color(CAPS_WORD_LED_INDEX0, RGB_WHITE);
        rgb_matrix_set_color(CAPS_WORD_LED_INDEX1, RGB_WHITE);
    }
    uint8_t mods = get_mods();
    if (mods & MOD_MASK_SHIFT) {
        rgb_matrix_set_color(MOD_SHIFT_LED_INDEX, RGB_GREEN);
    }
    if (mods & MOD_MASK_CTRL) {
        rgb_matrix_set_color(MOD_CTRL_LED_INDEX, RGB_RED);
    }
    if (mods & MOD_MASK_ALT) {
        rgb_matrix_set_color(MOD_ALT_LED_INDEX, RGB_BLUE);
    }

    return false;
}

#endif      // RGB_MATRIX_ENABLE

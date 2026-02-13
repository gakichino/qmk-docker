/*
Copyright 2022 aki27

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include <stdio.h>
#include "quantum.h"
#include "naginata.h"
NGKEYS naginata_keys;


// Defines names for use in layer keycodes and the keymap
enum layer_number {
    _QWERTY = 0,
    _LOWER = 1,
    _NAGINATA = 2,
    _NUMBERS = 3,
    _Layer4 = 4,
    _RAISE = 5,
    _TRACKBALL = 6,
};

enum custom_keycodes {
    EISU = NG_SAFE_RANGE,
    KANA2,
    LCTLTOG, // Macのライブ変換をオン/オフする
};

void matrix_init_user(void) {
    // 薙刀式
    uint16_t ngonkeys[] = {KC_H, KC_J};
    uint16_t ngoffkeys[] = {KC_F, KC_G};
    set_naginata(_NAGINATA, ngonkeys, ngoffkeys);
    // 薙刀式
}


#define LW_MHEN LT(1,KC_INT5)  // lower
#define RS_HENK LT(2,KC_INT4)  // raise
#define DEL_ALT ALT_T(KC_DEL)

/*
#define CPI_SW USER00
#define SCRL_SW USER01
#define ROT_R15 USER02
#define ROT_L15 USER03
#define SCRL_MO USER04
#define SCRL_TO USER05
#define SCRL_IN USER06
*/

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT( // mac用
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
       LGUI_T(KC_TAB),    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                                          KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_BSLS,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      LCTL_T(KC_ESC),    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                                          KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      LALT_T(KC_LNG1),    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                                          KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, RALT_T(KC_LNG2),
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
                        LCTL(KC_UP), MO(3),   LSFT_T(KC_SPC),  MO(5), MS_BTN1,             MS_BTN2,  RGUI_T(KC_BSPC), RSFT_T(KC_ENT), KC_RGUI,  LCTL(KC_DOWN),
                                                                 KC_PGUP, DF(1),    KC_PGDN, XXXXXXX, XXXXXXX, XXXXXXX
                                                            //`--------------'  `--------------'
    ),
  [_LOWER] = LAYOUT( // win用
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
       LGUI_T(KC_TAB),    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                                          KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_BSLS,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      LCTL_T(KC_ESC),    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                                          KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      LALT_T(KC_LNG1),    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                                          KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, RALT_T(KC_LNG2),
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
                        LGUI(KC_TAB), MO(3),   LSFT_T(KC_SPC),  MO(6), MS_BTN1,             MS_BTN2,  RCTL_T(KC_BSPC), RSFT_T(KC_ENT), KC_RGUI,  LCTL(LALT(KC_TAB)),
                                                                 KC_PGUP, DF(0),    KC_PGDN, XXXXXXX, XXXXXXX, XXXXXXX
                                                            //`--------------'  `--------------'
    ),
  [_RAISE] = LAYOUT( // mac function & scroll
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
       LALT(LGUI(KC_KB_POWER)),    KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,                                          KC_F6,    KC_F7,    KC_F8,    KC_F9,   KC_F10,  KC_NO,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      KC_TRNS,   KC_F11,   KC_F12,   KC_NO,   LGUI(LSFT(KC_4)),  LGUI(LSFT(KC_3)),                                        KC_LEFT,   KC_DOWN, KC_UP, KC_RIGHT, KC_NO, KC_NO,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      KC_TRNS,   UG_NEXT, UG_TOGG, RGB_M_SN,   KC_NO,  LGUI(LCTL(KC_SPC)),                                       KC_MPLY, KC_MSTP, KC_NO,  KC_NO, KC_NO, KC_TRNS,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
                        KC_TRNS, KC_TRNS,   KC_NO,  KC_NO,   LGUI(KC_LEFT),             LGUI(KC_RIGHT),  RGUI_T(KC_DEL), KC_TRNS, KC_TRNS,  KC_NO,
                                                                 LGUI(KC_MINS), MS_BTN3,    LGUI(KC_EQL), XXXXXXX, XXXXXXX, XXXXXXX
                                                            //`--------------'  `--------------'
    ),
  [_TRACKBALL] = LAYOUT( // win function & scroll
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
       LGUI(KC_L),    KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,                                          KC_F6,    KC_F7,    KC_F8,    KC_F9,   KC_F10,  KC_NO,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      KC_TRNS,   KC_F11,   KC_F12,   KC_NO,   LGUI(LSFT(KC_S)),  LGUI(KC_PSCR),                                        KC_LEFT,   KC_DOWN, KC_UP, KC_RIGHT, KC_NO, KC_NO,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      KC_TRNS,   UG_NEXT, UG_TOGG, RGB_M_SN,   KC_NO,  LGUI(KC_DOT),                                       KC_MPLY, KC_MSTP, KC_NO,  KC_NO, KC_NO, KC_TRNS,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
                        KC_TRNS, KC_TRNS,   KC_NO,  KC_NO,   KC_WBAK,             KC_WFWD,  RCTL_T(KC_DEL), KC_TRNS, KC_TRNS,  KC_NO,
                                                                 LCTL(KC_MINS), MS_BTN3,    LCTL(KC_EQL), XXXXXXX, XXXXXXX, XXXXXXX
                                                            //`--------------'  `--------------'
    ),
  [_NAGINATA] = LAYOUT(
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      LGUI_T(KC_TAB),    NG_I,    NG_W,    NG_E,    NG_R,    NG_T,                                          NG_Y,    NG_U,    NG_I,    NG_O,   NG_P,  KC_TRNS,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      LCTL_T(KC_ESC),    NG_A,    NG_S,    NG_D,    NG_F,    NG_G,                                          NG_H,    NG_J,    NG_K,    NG_L, NG_SCLN, KC_TRNS,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      LALT_T(KC_LNG1),    NG_Z,    NG_X,    NG_C,    NG_V,    NG_B,                                          NG_N,    NG_M, NG_COMM,  NG_DOT, NG_SLSH, RALT_T(KC_LNG2),
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
                        KC_TRNS, MO(3),   LSFT_T(KC_SPC),  KC_TRNS, MS_BTN1,             MS_BTN2,  RGUI_T(KC_BSPC), RSFT_T(KC_ENT), KC_RGUI,  LCTL(KC_DOWN),
                                                                 LCTL(KC_MINS), KC_TRNS,    LCTL(KC_EQL), XXXXXXX, XXXXXXX, XXXXXXX
    ),
  [_NUMBERS] = LAYOUT( // numbers & symbols
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
       KC_NO,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                                          KC_6,    KC_7,    KC_8,    KC_9,   KC_0,  KC_GRV,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
       KC_TRNS,    LSFT(KC_1),    LSFT(KC_2),    LSFT(KC_3),    LSFT(KC_4),    LSFT(KC_5),                                          LSFT(KC_6),    LSFT(KC_7),    LSFT(KC_8),    LSFT(KC_9),   LSFT(KC_0),  LSFT(KC_GRV),
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      KC_TRNS, LSFT(KC_MINS), LSFT(KC_EQL), KC_MINS, KC_EQL, XXXXXXX,                                       KC_LBRC, KC_RBRC, LSFT(KC_LBRC), LSFT(KC_RBRC), LSFT(KC_SLSH), KC_TRNS,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
                        XXXXXXX, KC_TRNS, XXXXXXX,  XXXXXXX,   MS_BTN1,             MS_BTN2,  KC_TRNS, KC_TRNS, KC_TRNS,  XXXXXXX,
                                                                 KC_VOLD, DF(0),  KC_VOLU, XXXXXXX, XXXXXXX, XXXXXXX
                                                            //`--------------'  `--------------'
    ),
  [_Layer4] = LAYOUT(
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
                        XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX,   XXXXXXX,             XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX,
                                                                 XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
                                                            //`--------------'  `--------------'
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_QWERTY]    = { ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_LOWER]     = { ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_RAISE]     = { ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_TRACKBALL] = { ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_NAGINATA]  = { ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_NUMBERS]    = { ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_Layer4]    = { ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
};
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
      case EISU:
        if (record->event.pressed) {
          // 薙刀式
          naginata_off();
          // 薙刀式
        }
        return false;
        break;
      case KANA2:
        if (record->event.pressed) {
          // 薙刀式
          naginata_on();
          // 薙刀式
        }
        return false;
        break;
    }
  
    // 薙刀式
    if (!process_naginata(keycode, record))
        return false;
    // 薙刀式
  
    return true;
  }


layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
    case _LOWER:
        rgblight_sethsv_range(0, 0, 0, 0, 2);
        cocot_set_scroll_mode(false);
        break;
    case _NAGINATA:
        rgblight_sethsv_range(HSV_YELLOW, 0, 2);
        cocot_set_scroll_mode(false);
        break;
    case _NUMBERS:
        rgblight_sethsv_range(HSV_CYAN, 0, 2);
        cocot_set_scroll_mode(false);
        break;
    case _Layer4:
        rgblight_sethsv_range(HSV_ORANGE, 0, 2);
        cocot_set_scroll_mode(false);
        break;
    case _RAISE:
        rgblight_sethsv_range(HSV_RED, 0, 2);
        cocot_set_scroll_mode(true);
        break;
    case _TRACKBALL:
        rgblight_sethsv_range(HSV_GREEN, 0, 2);
        cocot_set_scroll_mode(true);
        break;
    default:
        rgblight_sethsv_range( 0, 0, 0, 0, 2);
        cocot_set_scroll_mode(false);
        break;
    }
    rgblight_set_effect_range( 2, 10);
      return state;
};


#ifdef OLED_ENABLE
bool oled_task_user(void) {
    render_logo();
    oled_write_layer_state();
    return false;
}
#endif


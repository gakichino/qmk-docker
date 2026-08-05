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

// LalaPad Gen2 踏襲レイヤー
enum layer_number {
    _MAC = 0,
    _WIN = 1,
    _NUM = 2,
    _SYM = 3,
    _FN = 4,
    _MACSYS = 5,
    _WINSYS = 6,
};

#define CTL_A    LCTL_T(KC_A)
#define CTL_SCLN RCTL_T(KC_SCLN)
#define SFT_Z    LSFT_T(KC_Z)
#define NUM_SLSH LT(_NUM, KC_SLSH)
#define SYM_SPC  LT(_SYM, KC_SPC)
#define GUI_TAB  LGUI_T(KC_TAB)
#define CTL_TAB  LCTL_T(KC_TAB)
#define GUI_BSP  RGUI_T(KC_BSPC)
#define CTL_BSP  RCTL_T(KC_BSPC)
#define SFT_ENT  LSFT_T(KC_ENT)
#define ALT_EN   LALT_T(KC_LNG1)
#define ALT_JA   RALT_T(KC_LNG2)
#define FN_DOT   LT(_FN, KC_DOT)
#define SFT_0    LSFT_T(KC_0)

// ---------------------------------------------------------------------------
// Combos (LalaPad + A+S スクロール)
// ---------------------------------------------------------------------------
enum combo_events {
    AS_FN = 0,
    SPC_TAB_MAC,
    SPC_TAB_WIN,
};

const uint16_t PROGMEM as_combo[]    = {CTL_A, KC_S, COMBO_END};
const uint16_t PROGMEM spc_tab_mac[] = {SYM_SPC, GUI_TAB, COMBO_END};
const uint16_t PROGMEM spc_tab_win[] = {SYM_SPC, CTL_TAB, COMBO_END};

combo_t key_combos[] = {
    [AS_FN]       = COMBO(as_combo, MO(_FN)),
    [SPC_TAB_MAC] = COMBO(spc_tab_mac, MO(_MACSYS)),
    [SPC_TAB_WIN] = COMBO(spc_tab_win, MO(_WINSYS)),
};

// ---------------------------------------------------------------------------
// Keymaps
// ---------------------------------------------------------------------------
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_MAC] = LAYOUT(
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                                          KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX,   CTL_A,    KC_S,    KC_D,    KC_F,    KC_G,                                          KC_H,    KC_J,    KC_K,    KC_L,CTL_SCLN, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
       ALT_EN,   SFT_Z,    KC_X,    KC_C,    KC_V,    KC_B,                                          KC_N,    KC_M, KC_COMM,  KC_DOT,NUM_SLSH,  ALT_JA,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
                     LCTL(KC_UP), KC_ESC,  SYM_SPC, GUI_TAB, MS_BTN2,             MS_BTN1,  GUI_BSP, SFT_ENT, KC_ESC, LCTL(KC_DOWN),
                                                                 KC_PGUP,   DF(1),    KC_PGDN, XXXXXXX, XXXXXXX, XXXXXXX
                                                            //`--------------'  `--------------'
    ),
  [_WIN] = LAYOUT(
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                                          KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX,   CTL_A,    KC_S,    KC_D,    KC_F,    KC_G,                                          KC_H,    KC_J,    KC_K,    KC_L,CTL_SCLN, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
       ALT_EN,   SFT_Z,    KC_X,    KC_C,    KC_V,    KC_B,                                          KC_N,    KC_M, KC_COMM,  KC_DOT,NUM_SLSH,  ALT_JA,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
                    LGUI(KC_TAB), KC_ESC,  SYM_SPC, CTL_TAB, MS_BTN2,             MS_BTN1,  CTL_BSP, SFT_ENT, KC_ESC, LCTL(LALT(KC_TAB)),
                                                                 KC_PGUP,   DF(0),    KC_PGDN, XXXXXXX, XXXXXXX, XXXXXXX
                                                            //`--------------'  `--------------'
    ),
  [_NUM] = LAYOUT(
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX, KC_SLSH,    KC_7,    KC_8,    KC_9, KC_MINS,                                       XXXXXXX, KC_BSPC,  KC_DEL,  KC_TAB, XXXXXXX, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX, KC_ASTR,    KC_4,    KC_5,    KC_6, KC_PLUS,                                       KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, XXXXXXX, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      KC_TRNS,   SFT_0,    KC_1,    KC_2,    KC_3,  KC_EQL,                                       KC_HOME, KC_PGDN, KC_PGUP,  KC_END, XXXXXXX, KC_TRNS,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
                        KC_TRNS, KC_TRNS,  KC_SPC,  FN_DOT, KC_TRNS,             KC_TRNS, KC_TRNS,S(KC_ENT), KC_TRNS,  KC_TRNS,
                                                                 KC_VOLD, KC_TRNS,    KC_VOLU, XXXXXXX, XXXXXXX, XXXXXXX
                                                            //`--------------'  `--------------'
    ),
  [_SYM] = LAYOUT(
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX, KC_QUES, KC_DQUO, KC_EXLM, KC_BSPC, KC_TILD,                                       XXXXXXX, KC_UNDS, KC_LPRN, KC_RPRN, KC_PERC, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX,   KC_AT, KC_QUOT,  KC_DLR,  KC_DEL, KC_SLSH,                                       KC_HASH, XXXXXXX, KC_LBRC, KC_RBRC, KC_RCTL, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      KC_TRNS, KC_LSFT,  KC_GRV, KC_CIRC, KC_PIPE, KC_BSLS,                                       KC_ASTR, KC_AMPR, KC_LCBR, KC_RCBR, MO(_FN), KC_TRNS,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
                        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,
                                                                 KC_TRNS, KC_TRNS,    KC_TRNS, XXXXXXX, XXXXXXX, XXXXXXX
                                                            //`--------------'  `--------------'
    ),
  [_FN] = LAYOUT(
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX, KC_MUTE,   KC_F7,   KC_F8,   KC_F9,  KC_F12,                                       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX, KC_MPLY,   KC_F4,   KC_F5,   KC_F6,  KC_F11,                                       KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, XXXXXXX, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      KC_TRNS, KC_MSTP,   KC_F1,   KC_F2,   KC_F3,  KC_F10,                                       XXXXXXX, KC_LGUI, KC_LCTL, KC_LALT, XXXXXXX, KC_TRNS,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
                        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,
                                                                 KC_TRNS, MS_BTN3,    KC_TRNS, XXXXXXX, XXXXXXX, XXXXXXX
                                                            //`--------------'  `--------------'
    ),
  [_MACSYS] = LAYOUT(
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
       QK_BOOT, XXXXXXX,LGUI(LSFT(KC_3)),LGUI(LSFT(KC_4)),LGUI(LSFT(KC_5)), XXXXXXX,              XXXXXXX,  TO(_MAC),  TO(_WIN), XXXXXXX, QK_BOOT, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX, XXXXXXX, UG_TOGG, XXXXXXX, KC_LCTL, XXXXXXX,                                       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      KC_TRNS,   UG_NEXT, UG_TOGG, RGB_M_SN,   SCRL_IN,  LGUI(LCTL(KC_SPC)),                                       KC_NO, KC_NO, UG_NEXT, UG_PREV, XXXXXXX, KC_TRNS,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
          LALT(LGUI(KC_KB_POWER)), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,
                                                          LGUI(KC_MINS), MS_BTN3, LGUI(KC_EQL), XXXXXXX, XXXXXXX, XXXXXXX
                                                            //`--------------'  `--------------'
    ),
  [_WINSYS] = LAYOUT(
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
       QK_BOOT, XXXXXXX,LGUI(KC_PSCR),LGUI(LSFT(KC_S)),LGUI(LSFT(KC_S)), XXXXXXX,                 XXXXXXX,  TO(_MAC),  TO(_WIN), XXXXXXX, QK_BOOT, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      XXXXXXX, XXXXXXX, UG_TOGG, XXXXXXX,LGUI(KC_H), XXXXXXX,                                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
      KC_TRNS,   UG_NEXT, UG_TOGG, RGB_M_SN,   SCRL_IN,  LGUI(KC_DOT),                                       KC_NO, KC_NO, UG_NEXT, UG_PREV, XXXXXXX, KC_TRNS,
  //|-------------------------------------------------------|                                   |-------------------------------------------------------|
                     LGUI(KC_L), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,
                                                          LCTL(KC_MINS), MS_BTN3, LCTL(KC_EQL), XXXXXXX, XXXXXXX, XXXXXXX
                                                            //`--------------'  `--------------'
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_MAC]    = { ENCODER_CCW_CW(LCTL(KC_RGHT), LCTL(KC_LEFT)) },
    [_WIN]    = { ENCODER_CCW_CW(LGUI(LCTL(KC_RGHT)), LGUI(LCTL(KC_LEFT))) },
    [_NUM]    = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [_SYM]    = { ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_FN]     = { ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_MACSYS] = { ENCODER_CCW_CW(LGUI(KC_MINS), LGUI(KC_EQL)) },
    [_WINSYS] = { ENCODER_CCW_CW(LCTL(KC_MINS), LCTL(KC_EQL)) },
};
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
    case _WIN:
        rgblight_sethsv_range(0, 0, 0, 0, 2);
        cocot_set_scroll_mode(false);
        break;
    case _NUM:
        rgblight_sethsv_range(HSV_CYAN, 0, 2);
        cocot_set_scroll_mode(true);
        break;
    case _SYM:
        rgblight_sethsv_range(HSV_YELLOW, 0, 2);
        cocot_set_scroll_mode(false);
        break;
    case _FN:
        rgblight_sethsv_range(HSV_GREEN, 0, 2);
        cocot_set_scroll_mode(true);
        break;
    case _MACSYS:
        rgblight_sethsv_range(HSV_RED, 0, 2);
        cocot_set_scroll_mode(true);
        break;
    case _WINSYS:
        rgblight_sethsv_range(HSV_ORANGE, 0, 2);
        cocot_set_scroll_mode(true);
        break;
    default:
        rgblight_sethsv_range(0, 0, 0, 0, 2);
        cocot_set_scroll_mode(false);
        break;
    }
    rgblight_set_effect_range(2, 10);
    return state;
}

#ifdef OLED_ENABLE
bool oled_task_user(void) {
    render_logo();
    oled_write_layer_state();
    return false;
}
#endif

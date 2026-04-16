/*
 * naginata_v17u.c の enable_naginata を、レイヤー用コードとして oonisi に移植したもの。
 * 元: eswai / QMK naginata 実装（COMBO 非使用の同時押し判定）
 */
#include QMK_KEYBOARD_H
#include "layer_chord.h"
#include "naginata.h"

static uint16_t s_on_keys[2];
static uint16_t s_off_keys[2];

static uint16_t s_buf;
static uint8_t  s_npress;
static uint8_t layer_chord_layer = 0; // layer_chord_*を配置しているレイヤー番号


void layer_chord_on(void) {
    layer_on(layer_chord_layer);
}

void layer_chord_off(void) {
    layer_off(layer_chord_layer);
}

void layer_chord_init(const uint16_t on_keys[2], const uint16_t off_keys[2]) {
    s_on_keys[0]  = on_keys[0];
    s_on_keys[1]  = on_keys[1];
    s_off_keys[0] = off_keys[0];
    s_off_keys[1] = off_keys[1];
    s_buf         = 0;
    s_npress      = 0;
}

bool process_layer_chord(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        s_npress++;
        if (s_buf == 0 && s_npress == 1) {
            if (keycode == s_on_keys[0] || keycode == s_on_keys[1] || keycode == s_off_keys[0] || keycode == s_off_keys[1]) {
                s_buf = keycode;
                return false;
            }
        } else {
            if ((keycode == s_on_keys[0] && s_buf == s_on_keys[1]) || (keycode == s_on_keys[1] && s_buf == s_on_keys[0])) {
                layer_chord_on();
                s_buf    = 0;
                s_npress = 0;
                return false;
            }
            if ((keycode == s_off_keys[0] && s_buf == s_off_keys[1]) || (keycode == s_off_keys[1] && s_buf == s_off_keys[0])) {
                layer_chord_off();
                s_buf    = 0;
                s_npress = 0;
                return false;
            }
            tap_code16(s_buf);
            s_buf    = 0;
            s_npress = 0;
            return true;
        }
    } else {
        s_npress = 0;
        if (s_buf > 0) {
            tap_code16(s_buf);
            s_buf = 0;
            return false;
        }
    }

    s_buf = 0;
    return true;
}
#pragma once

#include <stdint.h>
#include "action.h"

void layer_chord_on(void);
void layer_chord_off(void);
/**
 * オン用2キー / オフ用2キーの「ほぼ同時押し」を検出する（naginata の enable_naginata と同型）。
 * レイヤー番号は keymap 側の set_naginata() と揃える（on/off 時は naginata_on/off が参照）。
 */
void layer_chord_init(const uint16_t on_keys[2], const uint16_t off_keys[2]);

/** @return false でイベントを消費 */
bool process_layer_chord(uint16_t keycode, keyrecord_t *record);

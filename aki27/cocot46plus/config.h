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

#pragma once

#define DYNAMIC_KEYMAP_LAYER_COUNT 7

/* key matrix size */
#define MATRIX_ROWS 10
#define MATRIX_COLS 6

#define ADNS5050_SCLK_PIN         B2
#define ADNS5050_SDIO_PIN         B4
#define ADNS5050_CS_PIN           B5

#define POINTING_DEVICE_ROTATION_180
#define OLED_FONT_H "keyboards/aki27/cocot46plus/glcdfont.c"

#define NAGINATA_TATEGAKI
// #define NAGINATA_YOKOGAKI

// #define NAGINATA_EDIT_WIN // JP106
// #define UNICODE_SELECTED_MODES UC_WINC

#define NAGINATA_EDIT_MAC // US101
// Macはunicode入力を使わない
#define MAC_LIVE_CONVERSION // Macでライブ変換をオンにしている場合

// #define NAGINATA_EDIT_LINUX
// #define UNICODE_SELECTED_MODES UC_LNX

// 初回起動時後置シフトをデフォルト有効にする
// 初回起動以降切り替える場合は、NG_KOTIを押して切り替える
#define NAGINATA_KOUCHI_SHIFT // シフトを後置でも有効にする

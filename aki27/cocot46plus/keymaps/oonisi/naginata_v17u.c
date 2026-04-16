/* Copyright 2018-2019 eswai <@eswai>
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
#include "naginata.h"

// 編集モード
// #include "unicode.h"

#include <string.h>

#define NGBUFFER 10 // キー入力バッファのサイズ

// カーソル定義
// あとで縦書き横書きに合わせて置換するための仮の定義
#define NGUP X_F21
#define NGDN X_F22
#define NGLT X_F23
#define NGRT X_F24

static uint8_t ng_chrcount = 0; // 文字キー入力のカウンタ
static bool is_naginata = false; // 薙刀式がオンかオフか
static uint8_t naginata_layer = 0; // NG_*を配置しているレイヤー番号
static uint32_t keycomb = 0UL; // 同時押しの状態を示す。32bitの各ビットがキーに対応する。
static uint16_t ngon_keys[2]; // 薙刀式をオンにするキー(通常HJ)
static uint16_t ngoff_keys[2]; // 薙刀式をオフにするキー(通常FG)

// // EEPROMに保存する設定
// typedef union {
//   uint32_t raw;
//   struct {
//     uint8_t os;
//     bool live_conv :1;
//     bool tategaki :1;
//     bool kouchi_shift :1;
//   };
// } naginata_config_t;

// naginata_config_t naginata_config;

// 31キーを32bitの各ビットに割り当てる
#define B_Q    (1UL<<0)
#define B_W    (1UL<<1)
#define B_E    (1UL<<2)
#define B_R    (1UL<<3)
#define B_T    (1UL<<4)

#define B_Y    (1UL<<5)
#define B_U    (1UL<<6)
#define B_I    (1UL<<7)
#define B_O    (1UL<<8)
#define B_P    (1UL<<9)

#define B_A    (1UL<<10)
#define B_S    (1UL<<11)
#define B_D    (1UL<<12)
#define B_F    (1UL<<13)
#define B_G    (1UL<<14)

#define B_H    (1UL<<15)
#define B_J    (1UL<<16)
#define B_K    (1UL<<17)
#define B_L    (1UL<<18)
#define B_SCLN (1UL<<19)

#define B_Z    (1UL<<20)
#define B_X    (1UL<<21)
#define B_C    (1UL<<22)
#define B_V    (1UL<<23)
#define B_B    (1UL<<24)

#define B_N    (1UL<<25)
#define B_M    (1UL<<26)
#define B_COMM (1UL<<27)
#define B_DOT  (1UL<<28)
#define B_SLSH (1UL<<29)

#define B_SHFT (1UL<<30)

// 文字入力バッファ
static uint16_t ninputs[NGBUFFER];

// キーコードとキービットの対応
// メモリ削減のため配列はNG_Qを0にしている
const uint32_t ng_key[] = {
  [NG_Q    - NG_Q] = B_Q,
  [NG_W    - NG_Q] = B_W,
  [NG_E    - NG_Q] = B_E,
  [NG_R    - NG_Q] = B_R,
  [NG_T    - NG_Q] = B_T,

  [NG_Y    - NG_Q] = B_Y,
  [NG_U    - NG_Q] = B_U,
  [NG_I    - NG_Q] = B_I,
  [NG_O    - NG_Q] = B_O,
  [NG_P    - NG_Q] = B_P,

  [NG_A    - NG_Q] = B_A,
  [NG_S    - NG_Q] = B_S,
  [NG_D    - NG_Q] = B_D,
  [NG_F    - NG_Q] = B_F,
  [NG_G    - NG_Q] = B_G,

  [NG_H    - NG_Q] = B_H,
  [NG_J    - NG_Q] = B_J,
  [NG_K    - NG_Q] = B_K,
  [NG_L    - NG_Q] = B_L,
  [NG_SCLN - NG_Q] = B_SCLN,

  [NG_Z    - NG_Q] = B_Z,
  [NG_X    - NG_Q] = B_X,
  [NG_C    - NG_Q] = B_C,
  [NG_V    - NG_Q] = B_V,
  [NG_B    - NG_Q] = B_B,

  [NG_N    - NG_Q] = B_N,
  [NG_M    - NG_Q] = B_M,
  [NG_COMM - NG_Q] = B_COMM,
  [NG_DOT  - NG_Q] = B_DOT,
  [NG_SLSH - NG_Q] = B_SLSH,

  [NG_SHFT - NG_Q] = B_SHFT,
  [NG_SHFT2 - NG_Q] = B_SHFT,
};

// カナ変換テーブル
typedef struct {
  uint32_t key;
  char kana[6];
} naginata_keymap;

// ロング
typedef struct {
  uint32_t key;
  char kana[26];
} naginata_keymap_long;

// 編集モード
// UNICODE
// typedef struct {
//   uint32_t key;
//   char win[7];
//   char mac[15];
// } naginata_keymap_unicode;

const PROGMEM naginata_keymap ngmap[] = {
  // 清音
  {.key = B_J                      , .kana = "a"       }, // あ
  {.key = B_K                      , .kana = "i"       }, // い
  {.key = B_L                      , .kana = "u"       }, // う
  {.key = B_SHFT|B_O               , .kana = "e"       }, // え
  {.key = B_SHFT|B_N               , .kana = "o"       }, // お
  {.key = B_F                      , .kana = "ka"      }, // か
  {.key = B_W                      , .kana = "ki"      }, // き
  {.key = B_H                      , .kana = "ku"      }, // く
  {.key = B_S                      , .kana = "ke"      }, // け
  {.key = B_V                      , .kana = "ko"      }, // こ
  {.key = B_SHFT|B_U               , .kana = "sa"      }, // さ
  {.key = B_U                      , .kana = "sa"      }, // さ
  {.key = B_R                      , .kana = "si"      }, // し
  {.key = B_O                      , .kana = "su"      }, // す
  {.key = B_SHFT|B_A               , .kana = "se"      }, // せ
  {.key = B_B                      , .kana = "so"      }, // そ
  {.key = B_N                      , .kana = "ta"      }, // た
  {.key = B_SHFT|B_G               , .kana = "ti"      }, // ち
  {.key = B_SHFT|B_L               , .kana = "tu"      }, // つ
  {.key = B_E                      , .kana = "te"      }, // て
  {.key = B_D                      , .kana = "to"      }, // と
  {.key = B_M                      , .kana = "na"      }, // な
  {.key = B_SHFT|B_D               , .kana = "ni"      }, // に
  {.key = B_SHFT|B_B               , .kana = "nu"      }, // ぬ
  {.key = B_SHFT|B_R               , .kana = "ne"      }, // ね
  {.key = B_SHFT|B_J               , .kana = "no"      }, // の
  {.key = B_C                      , .kana = "ha"      }, // は
  {.key = B_X                      , .kana = "hi"      }, // ひ
  {.key = B_SHFT|B_X               , .kana = "hi"      }, // ひ
  {.key = B_SHFT|B_SCLN            , .kana = "hu"      }, // ふ
  {.key = B_P                      , .kana = "he"      }, // へ
  {.key = B_Z                      , .kana = "ho"      }, // ほ
  {.key = B_SHFT|B_Z               , .kana = "ho"      }, // ほ
  {.key = B_SHFT|B_F               , .kana = "ma"      }, // ま
  {.key = B_SHFT|B_S               , .kana = "mi"      }, // み
  {.key = B_SHFT|B_COMM            , .kana = "mu"      }, // む
  {.key = B_SHFT|B_W               , .kana = "me"      }, // め
  {.key = B_SHFT|B_K               , .kana = "mo"      }, // も
  {.key = B_SHFT|B_H               , .kana = "ya"      }, // や
  {.key = B_SHFT|B_P               , .kana = "yu"      }, // ゆ
  {.key = B_SHFT|B_I               , .kana = "yo"      }, // よ
  {.key = B_DOT                    , .kana = "ra"      }, // ら
  {.key = B_SHFT|B_E               , .kana = "ri"      }, // り
  {.key = B_I                      , .kana = "ru"      }, // る
  {.key = B_SLSH                   , .kana = "re"      }, // れ
  {.key = B_SHFT|B_SLSH            , .kana = "re"      }, // れ
  {.key = B_A                      , .kana = "ro"      }, // ろ
  {.key = B_SHFT|B_DOT             , .kana = "wa"      }, // わ
  {.key = B_SHFT|B_C               , .kana = "wo"      }, // を
  {.key = B_COMM                   , .kana = "nn"      }, // ん
  {.key = B_SCLN                   , .kana = "-"       }, // ー

  // 濁音
  {.key = B_J|B_F                  , .kana = "ga"      }, // が
  {.key = B_J|B_W                  , .kana = "gi"      }, // ぎ
  {.key = B_F|B_H                  , .kana = "gu"      }, // ぐ
  {.key = B_J|B_S                  , .kana = "ge"      }, // げ
  {.key = B_J|B_V                  , .kana = "go"      }, // ご
  {.key = B_F|B_U                  , .kana = "za"      }, // ざ
  {.key = B_J|B_R                  , .kana = "zi"      }, // じ
  {.key = B_F|B_O                  , .kana = "zu"      }, // ず
  {.key = B_J|B_A                  , .kana = "ze"      }, // ぜ
  {.key = B_J|B_B                  , .kana = "zo"      }, // ぞ
  {.key = B_F|B_N                  , .kana = "da"      }, // だ
  {.key = B_J|B_G                  , .kana = "di"      }, // ぢ
  {.key = B_F|B_L                  , .kana = "du"      }, // づ
  {.key = B_J|B_E                  , .kana = "de"      }, // で
  {.key = B_J|B_D                  , .kana = "do"      }, // ど
  {.key = B_J|B_C                  , .kana = "ba"      }, // ば
  {.key = B_J|B_X                  , .kana = "bi"      }, // び
  {.key = B_F|B_SCLN               , .kana = "bu"      }, // ぶ
  {.key = B_F|B_P                  , .kana = "be"      }, // べ
  {.key = B_J|B_Z                  , .kana = "bo"      }, // ぼ
  {.key = B_F|B_L|B_SCLN           , .kana = "vu"      }, // ゔ

  // 半濁音
  {.key = B_M|B_C                  , .kana = "pa"      }, // ぱ
  {.key = B_M|B_X                  , .kana = "pi"      }, // ぴ
  {.key = B_V|B_SCLN               , .kana = "pu"      }, // ぷ
  {.key = B_V|B_P                  , .kana = "pe"      }, // ぺ
  {.key = B_M|B_Z                  , .kana = "po"      }, // ぽ

  // 小書き
  {.key = B_Q|B_H                  , .kana = "xya"     }, // ゃ
  {.key = B_Q|B_SHFT|B_H           , .kana = "xya"     }, // ゃ
  {.key = B_Q|B_P                  , .kana = "xyu"     }, // ゅ
  {.key = B_Q|B_SHFT|B_P           , .kana = "xyu"     }, // ゅ
  {.key = B_Q|B_I                  , .kana = "xyo"     }, // ょ
  {.key = B_Q|B_SHFT|B_I           , .kana = "xyo"     }, // ょ
  {.key = B_Q|B_J                  , .kana = "xa"      }, // ぁ
  {.key = B_Q|B_SHFT|B_J           , .kana = "xa"      }, // ぁ
  {.key = B_Q|B_K                  , .kana = "xi"      }, // ぃ
  {.key = B_Q|B_SHFT|B_K           , .kana = "xi"      }, // ぃ
  {.key = B_Q|B_L                  , .kana = "xu"      }, // ぅ
  // {.key = B_Q|B_SHFT|B_L           , .kana = "xu"      }, // ぅ
  {.key = B_Q|B_O                  , .kana = "xe"      }, // ぇ
  {.key = B_Q|B_SHFT|B_O           , .kana = "xe"      }, // ぇ
  {.key = B_Q|B_N                  , .kana = "xo"      }, // ぉ
  {.key = B_Q|B_SHFT|B_N           , .kana = "xo"      }, // ぉ
  // {.key = B_Q|B_L                  , .kana = "xwa"     }, // ゎ
  {.key = B_Q|B_DOT                , .kana = "xwa"     }, // ゎ
  {.key = B_G                      , .kana = "xtu"     }, // っ

  // 清音拗音 濁音拗音 半濁拗音
  {.key = B_R|B_H                  , .kana = "sya"     }, // しゃ
  {.key = B_R|B_P                  , .kana = "syu"     }, // しゅ
  {.key = B_R|B_I                  , .kana = "syo"     }, // しょ
  {.key = B_J|B_R|B_H              , .kana = "zya"     }, // じゃ
  {.key = B_J|B_R|B_P              , .kana = "zyu"     }, // じゅ
  {.key = B_J|B_R|B_I              , .kana = "zyo"     }, // じょ
  {.key = B_W|B_H                  , .kana = "kya"     }, // きゃ
  {.key = B_W|B_P                  , .kana = "kyu"     }, // きゅ
  {.key = B_W|B_I                  , .kana = "kyo"     }, // きょ
  {.key = B_J|B_W|B_H              , .kana = "gya"     }, // ぎゃ
  {.key = B_J|B_W|B_P              , .kana = "gyu"     }, // ぎゅ
  {.key = B_J|B_W|B_I              , .kana = "gyo"     }, // ぎょ
  {.key = B_G|B_H                  , .kana = "tya"     }, // ちゃ
  {.key = B_G|B_P                  , .kana = "tyu"     }, // ちゅ
  {.key = B_G|B_I                  , .kana = "tyo"     }, // ちょ
  {.key = B_J|B_G|B_H              , .kana = "dya"     }, // ぢゃ
  {.key = B_J|B_G|B_P              , .kana = "dyu"     }, // ぢゅ
  {.key = B_J|B_G|B_I              , .kana = "dyo"     }, // ぢょ
  {.key = B_D|B_H                  , .kana = "nya"     }, // にゃ
  {.key = B_D|B_P                  , .kana = "nyu"     }, // にゅ
  {.key = B_D|B_I                  , .kana = "nyo"     }, // にょ
  {.key = B_X|B_H                  , .kana = "hya"     }, // ひゃ
  {.key = B_X|B_P                  , .kana = "hyu"     }, // ひゅ
  {.key = B_X|B_I                  , .kana = "hyo"     }, // ひょ
  {.key = B_J|B_X|B_H              , .kana = "bya"     }, // びゃ
  {.key = B_J|B_X|B_P              , .kana = "byu"     }, // びゅ
  {.key = B_J|B_X|B_I              , .kana = "byo"     }, // びょ
  {.key = B_M|B_X|B_H              , .kana = "pya"     }, // ぴゃ
  {.key = B_M|B_X|B_P              , .kana = "pyu"     }, // ぴゅ
  {.key = B_M|B_X|B_I              , .kana = "pyo"     }, // ぴょ
  {.key = B_S|B_H                  , .kana = "mya"     }, // みゃ
  {.key = B_S|B_P                  , .kana = "myu"     }, // みゅ
  {.key = B_S|B_I                  , .kana = "myo"     }, // みょ
  {.key = B_E|B_H                  , .kana = "rya"     }, // りゃ
  {.key = B_E|B_P                  , .kana = "ryu"     }, // りゅ
  {.key = B_E|B_I                  , .kana = "ryo"     }, // りょ

  // 清音外来音 濁音外来音
  {.key = B_M|B_E|B_K              , .kana = "thi"     }, // てぃ
  {.key = B_M|B_E|B_P              , .kana = "thu"     }, // てゅ
  {.key = B_J|B_E|B_K              , .kana = "dhi"     }, // でぃ
  {.key = B_J|B_E|B_P              , .kana = "dhu"     }, // でゅ
  {.key = B_M|B_D|B_L              , .kana = "toxu"    }, // とぅ
  {.key = B_J|B_D|B_L              , .kana = "doxu"    }, // どぅ
  {.key = B_M|B_R|B_O              , .kana = "sye"     }, // しぇ
  {.key = B_M|B_G|B_O              , .kana = "tye"     }, // ちぇ
  {.key = B_J|B_R|B_O              , .kana = "zye"     }, // じぇ
  {.key = B_J|B_G|B_O              , .kana = "dye"     }, // ぢぇ
  {.key = B_V|B_SCLN|B_J           , .kana = "fa"      }, // ふぁ
  {.key = B_V|B_SCLN|B_K           , .kana = "fi"      }, // ふぃ
  {.key = B_V|B_SCLN|B_O           , .kana = "fe"      }, // ふぇ
  {.key = B_V|B_SCLN|B_N           , .kana = "fo"      }, // ふぉ
  {.key = B_V|B_SCLN|B_P           , .kana = "fyu"     }, // ふゅ
  {.key = B_V|B_K|B_O              , .kana = "ixe"     }, // いぇ
  {.key = B_V|B_L|B_K              , .kana = "wi"      }, // うぃ
  {.key = B_V|B_L|B_O              , .kana = "we"      }, // うぇ
  {.key = B_V|B_L|B_N              , .kana = "uxo"     }, // うぉ
  {.key = B_F|B_L|B_J              , .kana = "va"      }, // ゔぁ
  {.key = B_F|B_L|B_K              , .kana = "vi"      }, // ゔぃ
  {.key = B_F|B_L|B_O              , .kana = "ve"      }, // ゔぇ
  {.key = B_F|B_L|B_N              , .kana = "vo"      }, // ゔぉ
  {.key = B_F|B_L|B_P              , .kana = "vuxyu"   }, // ゔゅ
  {.key = B_V|B_H|B_J              , .kana = "kuxa"    }, // くぁ
  {.key = B_V|B_H|B_K              , .kana = "kuxi"    }, // くぃ
  {.key = B_V|B_H|B_O              , .kana = "kuxe"    }, // くぇ
  {.key = B_V|B_H|B_N              , .kana = "kuxo"    }, // くぉ
  {.key = B_V|B_H|B_DOT            , .kana = "kuxwa"   }, // くゎ
  {.key = B_F|B_H|B_J              , .kana = "guxa"    }, // ぐぁ
  {.key = B_F|B_H|B_K              , .kana = "guxi"    }, // ぐぃ
  {.key = B_F|B_H|B_O              , .kana = "guxe"    }, // ぐぇ
  {.key = B_F|B_H|B_N              , .kana = "guxo"    }, // ぐぉ
  {.key = B_F|B_H|B_DOT            , .kana = "guxwa"   }, // ぐゎ
  {.key = B_V|B_L|B_J              , .kana = "tsa"     }, // つぁ
  // ウィ系とかぶっている
  {.key = B_V|B_L|B_K              , .kana = "tsi"     }, // つぃ
  {.key = B_V|B_L|B_O              , .kana = "tse"     }, // つぇ
  {.key = B_V|B_L|B_N              , .kana = "tso"     }, // つぉ

  // 追加
  {.key = B_SHFT            , .kana = " "},
  {.key = B_Q               , .kana = ""},
  {.key = B_T               , .kana = ","},
  {.key = B_V|B_SHFT        , .kana = ","},
  {.key = B_J|B_K|B_F       , .kana = ";"},
  {.key = B_J|B_K|B_G       , .kana = ":"},
  {.key = B_J|B_K|B_G       , .kana = ":"},
  {.key = B_J|B_K|B_V       , .kana = "<"},
  {.key = B_J|B_K|B_B       , .kana = ">"},
  {.key = B_J|B_K|B_D       , .kana = "?"},
  {.key = B_J|B_K|B_C       , .kana = "!"},
  
  {.key = B_Y               , .kana = "."},
  {.key = B_M|B_SHFT        , .kana = "."SS_TAP(X_ENTER)},
  // {.key = B_U               , .kana = SS_TAP(X_BSPC)},

  // enter
  {.key = B_V|B_M           , .kana = SS_TAP(X_ENTER)},
  // enter+シフト(連続シフト)
  {.key = B_SHFT|B_V|B_M    , .kana = SS_TAP(X_ENTER)},

  {.key = B_J|B_K|B_T       , .kana = "/"}, // ・

};

// 薙刀式のレイヤー、オンオフするキー
void set_naginata(uint8_t layer, uint16_t *onk, uint16_t *offk) {
  naginata_layer = layer;
  ngon_keys[0] = *onk;
  ngon_keys[1] = *(onk+1);
  ngoff_keys[0] = *offk;
  ngoff_keys[1] = *(offk+1);

  naginata_config.raw = eeconfig_read_user();
  if (naginata_config.os != NG_WIN && naginata_config.os != NG_MAC && naginata_config.os != NG_LINUX) {
    naginata_config.os = NG_WIN;
    naginata_config.live_conv = 1;
    naginata_config.tategaki = 1;
#ifdef NAGINATA_KOUCHI_SHIFT
    naginata_config.kouchi_shift = 1;
#else
    naginata_config.kouchi_shift = 0;
#endif
    eeconfig_update_user(naginata_config.raw);
  }
  // ng_set_unicode_mode(naginata_config.os);
  // copyTYtable();
}

// 薙刀式をオン
void naginata_on(void) {
  is_naginata = true;
  keycomb = 0UL;
  naginata_clear();
  layer_on(naginata_layer);

  tap_code(KC_LNG1); // Mac
  tap_code(KC_INT4); // Win
}

// 薙刀式をオフ
void naginata_off(void) {
  is_naginata = false;
  keycomb = 0UL;
  naginata_clear();
  layer_off(naginata_layer);

  tap_code(KC_LNG2); // Mac
  tap_code(KC_INT5); // Win
}

// 薙刀式のon/off状態を返す
bool naginata_state(void) {
  return is_naginata;
}

// バッファから先頭n文字を削除する
void compress_buffer(int n) {
  if (ng_chrcount == 0) return;
  for (int j = 0; j < NGBUFFER; j++) {
    if (j + n < NGBUFFER) {
      ninputs[j] = ninputs[j + n];
    } else {
      ninputs[j] = 0;
    }
  }
  ng_chrcount -= n;
}

// キー集合のビット数（最長マッチ用）
static int popcount32(uint32_t x) {
  int c = 0;
  while (x) {
    c++;
    x &= x - 1;
  }
  return c;
}

// key_mask で指定したビットに対応するキーをバッファから1つずつ削除する（シフトを特別扱いしない相互シフト用）
static void remove_keys_from_buffer(uint32_t key_mask) {
  while (key_mask != 0 && ng_chrcount > 0) {
    bool removed = false;
    for (int i = 0; i < ng_chrcount; i++) {
      uint32_t b = ng_key[ninputs[i] - NG_Q];
      if (b & key_mask) {
        key_mask &= ~b;
        for (int j = i; j + 1 < NGBUFFER; j++) ninputs[j] = ninputs[j + 1];
        ninputs[NGBUFFER - 1] = 0;
        ng_chrcount--;
        removed = true;
        break;
      }
    }
    if (!removed) break;
  }
}

// modifierが押されたら薙刀式レイヤーをオフしてベースレイヤーに戻す
// get_mods()がうまく動かない
static int n_modifier = 0;

bool process_modifier(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case KC_LCTL:
    case KC_LSFT:
    case KC_LALT:
    case KC_LGUI:
    case KC_RCTL:
    case KC_RSFT:
    case KC_RALT:
    case KC_RGUI:
    case LCTL_T(0x01) ... LCTL_T(0xFF):
    case LSFT_T(0x01) ... LSFT_T(0xFF):
    case LALT_T(0x01) ... LALT_T(0xFF):
    case LGUI_T(0x01) ... LGUI_T(0xFF):
    case RCTL_T(0x01) ... RCTL_T(0xFF):
    case RSFT_T(0x01) ... RSFT_T(0xFF):
    case RALT_T(0x01) ... RALT_T(0xFF):
    case RGUI_T(0x01) ... RGUI_T(0xFF):
      if (record->event.pressed) {
        n_modifier++;
        layer_off(naginata_layer);
      } else {
        n_modifier--;
        if (n_modifier <= 0) {
          n_modifier = 0;
          layer_on(naginata_layer);
        }
      }
      return true;
      break;
  }
  return false;
}

static uint16_t vbnm_buf = 0; // 押しているNかMのキーコード
static uint8_t nkeypress = 0; // 同時にキーを押している数

// 薙刀式の起動処理(COMBOを使わない)
bool enable_naginata(uint16_t keycode, keyrecord_t *record) {
  // キープレス
  if (record->event.pressed) {
    nkeypress++;
    // 1キー目、NMの前に他のキーを押していないこと
    if (vbnm_buf == 0 && nkeypress == 1) {
      // かなオンキーの場合
      if (keycode == ngon_keys[0] || keycode == ngon_keys[1] || keycode == ngoff_keys[0] || keycode == ngoff_keys[1]) {
        vbnm_buf = keycode;
        return false;
      }
    // ２キー目
    } else {
      // ２キー目、１キー目、両方ともかなオンキー
      if ((keycode == ngon_keys[0] && vbnm_buf == ngon_keys[1]) ||
          (keycode == ngon_keys[1] && vbnm_buf == ngon_keys[0])) {
        naginata_on();
        vbnm_buf = 0;
        nkeypress = 0;
        return false;
      } else if ((keycode == ngoff_keys[0] && vbnm_buf == ngoff_keys[1]) ||
          (keycode == ngoff_keys[1] && vbnm_buf == ngoff_keys[0])) {
        naginata_off();
        vbnm_buf = 0;
        nkeypress = 0;
        return false;
      // ２キー目はかなオンキーではない
      } else {
        tap_code(vbnm_buf); // 1キー目を出力
        vbnm_buf = 0;
        nkeypress = 0;
        return true; // 2キー目はQMKにまかせる
      }
    }
  } else {
    nkeypress = 0;
    // N/M単押しだった
    if (vbnm_buf > 0) {
      tap_code(vbnm_buf);
      vbnm_buf = 0;
      return false;
    }
  }

  vbnm_buf = 0;
  return true;
}

// バッファをクリアする
void naginata_clear(void) {
  for (int i = 0; i < NGBUFFER; i++) {
    ninputs[i] = 0;
  }
  ng_chrcount = 0;
  n_modifier = 0;
  nkeypress = 0;
  vbnm_buf = 0;
}

// 薙刀式の入力処理
bool process_naginata(uint16_t keycode, keyrecord_t *record) {

  // まれに薙刀モードオンのまま、レイヤーがオフになることがあるので、対策
  if (n_modifier == 0 && is_naginata && !layer_state_is(naginata_layer))
    layer_on(naginata_layer);
  if (n_modifier == 0 && !is_naginata && layer_state_is(naginata_layer))
    layer_off(naginata_layer);
  if (n_modifier > 0 && layer_state_is(naginata_layer))
    layer_off(naginata_layer);

  if (!is_naginata)
    // return true;
    return enable_naginata(keycode, record);

  if (process_modifier(keycode, record))
    return true;

  if (record->event.pressed) {
    switch (keycode) {
      case NG_SHFT ... NG_SHFT2:
        if (!naginata_config.kouchi_shift) {
          if (ng_chrcount >= 1) {
            naginata_type();
            keycomb = 0UL;
          }
          ninputs[ng_chrcount] = keycode; // キー入力をバッファに貯める
          ng_chrcount++;
          keycomb |= ng_key[keycode - NG_Q]; // キーの重ね合わせ
          return false;
          break;
        } else {
          // 後置シフト時も NG_SHFT をバッファに積み、シフト押しっぱなしで K → も などが打てるようにする
          ninputs[ng_chrcount] = keycode;
          ng_chrcount++;
          keycomb |= ng_key[keycode - NG_Q];
          return false;
          break;
        }
      case NG_Q ... NG_SLSH:
        ninputs[ng_chrcount] = keycode; // キー入力をバッファに貯める
        ng_chrcount++;
        keycomb |= ng_key[keycode - NG_Q]; // キーの重ね合わせ
        // バッファが一杯になったら処理を開始
        if (ng_chrcount >= NGBUFFER) {
          naginata_type();
        }
        return false;
        break;
    }
  } else { // key release

    switch (keycode) {
      case NG_Q ... NG_SHFT2:
        // どれかキーを離したら処理を開始する
        keycomb &= ~ng_key[keycode - NG_Q]; // キーの重ね合わせ
        if (ng_chrcount > 0) {
          naginata_type();
        }
        return false;
        break;
    }
  }
  return true;
}

// キー入力を文字に変換して出力する（シフトを特別扱いしない：バッファ|keycomb の和集合で最長マッチ）
void naginata_type(void) {
  naginata_keymap bngmap;

  while (ng_chrcount > 0) {
    // バッファとまだ押しているキー(keycomb)の和集合
    uint32_t full_comb = keycomb;
    uint32_t buffer_bits = 0;
    for (int i = 0; i < ng_chrcount; i++) {
      uint32_t b = ng_key[ninputs[i] - NG_Q];
      full_comb |= b;
      buffer_bits |= b;
    }

    // 特殊: 薙刀オン/オフ
    if (full_comb == (B_N | B_M)) {
      naginata_on();
      remove_keys_from_buffer(B_N | B_M);
      return;
    }
    if (full_comb == (B_V | B_B)) {
      naginata_off();
      remove_keys_from_buffer(B_V | B_B);
      return;
    }

    // テーブルから full_comb の部分集合で最長の仮名を探す（バッファに1キー以上含まれるものだけ＝removeで進むため）
    // 同点なら「バッファ先頭の文字キー（Shift以外）」を含む方を優先（K→J の順で「もの」になるように）
    uint32_t best_key = 0;
    uint32_t first_char_bit = 0;  // バッファで先頭の非Shiftキーのビット
    for (int i = 0; i < ng_chrcount; i++) {
      uint16_t kc = ninputs[i];
      if (kc < NG_SHFT || kc > NG_SHFT2) {
        first_char_bit = ng_key[kc - NG_Q];
        break;
      }
    }
    for (int i = 0; i < (int)(sizeof ngmap / sizeof(naginata_keymap)); i++) {
      memcpy_P(&bngmap, &ngmap[i], sizeof(bngmap));
      if ((bngmap.key & full_comb) != bngmap.key || (buffer_bits & bngmap.key) == 0) continue;
      int new_pop = popcount32(bngmap.key);
      int best_pop = popcount32(best_key);
      bool better = (new_pop > best_pop)
          || (new_pop == best_pop && best_pop > 0 && first_char_bit != 0
              && (bngmap.key & first_char_bit) != 0 && (best_key & first_char_bit) == 0);
      if (better) best_key = bngmap.key;
    }
    if (best_key != 0) {
      for (int i = 0; i < (int)(sizeof ngmap / sizeof(naginata_keymap)); i++) {
        memcpy_P(&bngmap, &ngmap[i], sizeof(bngmap));
        if (bngmap.key == best_key) {
          send_string(bngmap.kana);
          break;
        }
      }
      remove_keys_from_buffer(best_key);
      continue;
    }

    // マッチなし: 先頭1キーを消費
    compress_buffer(1);
    return;
  }
}

// バッファの頭からnt文字の範囲を検索キーにしてテーブル検索し、文字に変換して出力する
// 検索に成功したらtrue、失敗したらfalseを返す
bool naginata_lookup(int nt, bool shifted) {
  naginata_keymap bngmap; // PROGMEM buffer

  // 編集モード
  // naginata_keymap_long bngmapl; // PROGMEM buffer
  // naginata_keymap_unicode bngmapu; // PROGMEM buffer

  // keycomb_bufはバッファ内のキーの組み合わせ、keycombはリリースしたキーを含んでいない
  uint32_t keycomb_buf = 0UL;

  // バッファ内のキーを組み合わせる
  for (int i = 0; i < nt; i++) {
    keycomb_buf |= ng_key[ninputs[i] - NG_Q];
  }

  switch (keycomb_buf) {
    // send_stringできないキー、長すぎるマクロはここで定義
    case B_N|B_M:
      naginata_on();
      compress_buffer(nt);
      return true;
      break;
    case B_V|B_B:
      naginata_off();
      compress_buffer(nt);
      return true;
      break;
    default:
      // キーから仮名に変換して出力する

      // 通常の仮名
      for (int i = 0; i < sizeof ngmap / sizeof bngmap; i++) {
        memcpy_P(&bngmap, &ngmap[i], sizeof(bngmap));
        if (keycomb_buf == bngmap.key) {
          send_string(bngmap.kana);
          compress_buffer(nt);
          return true;
        }
      }
  }
  return false;
}


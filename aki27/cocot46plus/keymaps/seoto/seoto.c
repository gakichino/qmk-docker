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
static bool ime_is_on = false; // キーボード側で追跡するIME状態
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

static void update_ime_state(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    switch (keycode) {
      case KC_LNG1:
      case KC_INT4:
        ime_is_on = true;
        return;
      case KC_LNG2:
      case KC_INT5:
        ime_is_on = false;
        return;
    }
  }

  // Mod-Tap(LALT_T(KC_LNG1) / RALT_T(KC_LNG2)) のタップ確定を拾う
  if (!record->event.pressed && record->tap.count > 0 && IS_QK_MOD_TAP(keycode)) {
    uint16_t tap_keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
    switch (tap_keycode) {
      case KC_LNG1:
      case KC_INT4:
        ime_is_on = true;
        return;
      case KC_LNG2:
      case KC_INT5:
        ime_is_on = false;
        return;
    }
  }
}

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


const PROGMEM naginata_keymap ngmap[] = {
  {.key = B_Q                      , .kana = "p"       },
  {.key = B_W                      , .kana = "z"       },
  {.key = B_E                      , .kana = "h"       },
  {.key = B_R                      , .kana = "f"       },
  {.key = B_T                      , .kana = "/"      },
  {.key = B_Y                      , .kana = ";"      },
  {.key = B_U                      , .kana = "q"      },
  {.key = B_I                      , .kana = "o"      },
  {.key = B_O                      , .kana = "w"      },
  {.key = B_O|B_SHFT               , .kana = "~"      },
  {.key = B_P                      , .kana = "x"      },
  {.key = B_A                      , .kana = "d"      },
  {.key = B_S                      , .kana = "s"      },
  {.key = B_D                      , .kana = "n"      },
  {.key = B_F                      , .kana = "t"      },
  {.key = B_G                      , .kana = "g"      },
  {.key = B_H                      , .kana = "l"      },
  {.key = B_J                      , .kana = "a"      },
  {.key = B_K                      , .kana = "i"      },
  {.key = B_K|B_SHFT               , .kana = "nn"      },
  {.key = B_L                      , .kana = "u"      },
  {.key = B_L|B_SHFT               , .kana = "-"      },
  {.key = B_SCLN                   , .kana = "e"      },
  {.key = B_SCLN|B_SHFT            , .kana = "xtu"      },
  {.key = B_Z                      , .kana = "b"      },
  {.key = B_X                      , .kana = "r"      },
  {.key = B_C                      , .kana = "m"      },
  {.key = B_V                      , .kana = "c"      },
  {.key = B_B                      , .kana = "k"      },
  {.key = B_N                      , .kana = "y"      },
  {.key = B_M                      , .kana = ","      },
  {.key = B_COMM                   , .kana = "j"      },
  {.key = B_DOT                    , .kana = "."      },
  {.key = B_SLSH                   , .kana = "v"      },
};

const PROGMEM naginata_keymap_long ngmapl_mac[] = {
// 編集モード Mac
  // {.key = B_J|B_K|B_Q		, .kana = SS_LCMD(SS_LCTL("e"))}, // ^{End}
  // {.key = B_J|B_K|B_R		, .kana = SS_LCMD("s")}, // ^s
  // {.key = B_D|B_F|B_Y		, .kana = SS_LCTL("a")}, // {Home}
  // {.key = B_D|B_F|B_U		, .kana = SS_LSFT(SS_LCTL("e"))SS_TAP(X_BSPC)}, // +{End}{BS}
  // {.key = B_D|B_F|B_SCLN		, .kana = SS_LCTL("k")}, // ^i
  // {.key = B_D|B_F|B_N		, .kana = SS_LCTL("e")}, // {End}
  // {.key = B_D|B_F|B_SLSH		, .kana = SS_LCTL("j")}, // ^u
  // {.key = B_M|B_COMM|B_E		, .kana = SS_LCTL("a")SS_TAP(X_ENTER)SS_TAP(X_SPACE)SS_TAP(X_SPACE)SS_TAP(X_SPACE)SS_LCTL("e")}, // {Home}{改行}{Space 3}{End}
  // {.key = B_M|B_COMM|B_R		, .kana = SS_LCTL("a")SS_TAP(X_ENTER)SS_TAP(X_SPACE)SS_LCTL("e")}, // {Home}{改行}{Space 1}{End}
  // {.key = B_C|B_V|B_Y		, .kana = SS_LSFT(SS_LCTL("a"))}, // +{Home}
  // {.key = B_C|B_V|B_U		, .kana = SS_LCMD("x")}, // ^x
  // {.key = B_C|B_V|B_I		, .kana = SS_LCMD("v")}, // ^v
  // {.key = B_C|B_V|B_O		, .kana = SS_LCMD("y")}, // ^y
  // {.key = B_C|B_V|B_P		, .kana = SS_LCMD("z")}, // ^z
  // {.key = B_C|B_V|B_H		, .kana = SS_LCMD("c")}, // ^c
  // {.key = B_C|B_V|B_L		, .kana = SS_TAP(X_PGUP)}, // ^{PgUp}
  // {.key = B_C|B_V|B_SCLN		, .kana = SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)}, // ^{PgUp 5}
  // {.key = B_C|B_V|B_N		, .kana = SS_LSFT(SS_LCTL("e"))}, // +{End}
  // {.key = B_C|B_V|B_DOT		, .kana = SS_TAP(X_PGDN)}, // ^{PgDn}
  // {.key = B_C|B_V|B_SLSH		, .kana = SS_TAP(X_PGDN)SS_TAP(X_PGDN)SS_TAP(X_PGDN)SS_TAP(X_PGDN)SS_TAP(X_PGDN)}, // ^{PgDn 5}
};

// 編集モードunicode
// const PROGMEM naginata_keymap_unicode ngmapu[] = {
//   {.key = B_J|B_K|B_W		, .win   = "｜",		.mac = "nagitatesenn"}, // ｜{改行}
//   // {.key = B_J|B_K|B_T		, .win   = "・",		.mac = "nagichuutenn"}, // ・
//   {.key = B_J|B_K|B_A		, .win   = "……",		.mac = "nagitentenn"}, // ……{改行}
//   {.key = B_J|B_K|B_S		, .win   = "《",		.mac = "nagikakkohio"}, // 《{改行}
//   {.key = B_J|B_K|B_F		, .win   = "「",		.mac = "nagikakkohiu"}, // 「{改行}
//   {.key = B_J|B_K|B_G		, .win   = "(",		.mac = "nagikakkohika"}, // ({改行}
//   {.key = B_J|B_K|B_Z		, .win   = "││",		.mac = "nagitatesenni"}, // ││{改行}
//   {.key = B_J|B_K|B_X		, .win   = "》",		.mac = "nagikakkomio"}, // 》{改行}
//   {.key = B_J|B_K|B_V		, .win   = "」",		.mac = "nagikakkomiu"}, // 」{改行}
//   {.key = B_J|B_K|B_B		, .win   = ")",		.mac = "nagikakkomika"}, // ){改行}
//   {.key = B_M|B_COMM|B_Q		, .win   = "／",		.mac = "naginaname"}, // ／{改行}
//   {.key = B_M|B_COMM|B_T		, .win   = "〇",		.mac = "nagimaru"}, // 〇{改行}
//   {.key = B_M|B_COMM|B_A		, .win   = "【",		.mac = "nagikakkohia"}, // 【{改行}
//   {.key = B_M|B_COMM|B_S		, .win   = "〈",		.mac = "nagikakkohii"}, // 〈{改行}
//   {.key = B_M|B_COMM|B_D		, .win   = "『",		.mac = "nagikakkohie"}, // 『{改行}
//   {.key = B_M|B_COMM|B_Z		, .win   = "】",		.mac = "nagikakkomia"}, // 】{改行}
//   {.key = B_M|B_COMM|B_X		, .win   = "〉",		.mac = "nagikakkomii"}, // 〉{改行}
//   {.key = B_M|B_COMM|B_C		, .win   = "』",		.mac = "nagikakkomie"}, // 』{改行}
// };

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
}

// 薙刀式をオフ
void naginata_off(void) {
  is_naginata = false;
  keycomb = 0UL;
  naginata_clear();
  layer_off(naginata_layer);
}

// 薙刀式のon/off状態を返す
bool naginata_state(void) {
  return is_naginata;
}

void naginata_set_ime_state(bool on) {
  ime_is_on = on;
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

void switchOS(uint8_t os) {
  naginata_config.os = os;
  eeconfig_update_user(naginata_config.raw);
  // 編集モード
  // ng_set_unicode_mode(naginata_config.os);
}

// 編集モード
// void ng_set_unicode_mode(uint8_t os) {
//   switch (os) {
//     case NG_WIN:
//       set_unicode_input_mode(UNICODE_MODE_WINDOWS);
//       break;
//     case NG_MAC:
//       set_unicode_input_mode(UNICODE_MODE_MACOS);
//       break;
//     case NG_LINUX:
//       set_unicode_input_mode(UNICODE_MODE_LINUX);
//       break;
//   }
// }

void mac_live_conversion_toggle() {
  naginata_config.live_conv ^= 1;
  eeconfig_update_user(naginata_config.raw);
}

// 出典 https://programming-place.net/ppp/contents/c/rev_res/string014.html
char* replace(char* s, const char* before, const char* after)
{
  const size_t before_len = strlen(before);
  if (before_len == 0) {
      return s;
  }

  const size_t after_len = strlen(after);
  char* p = s;

  for (;;) {
    p = strstr(p, before);
    if (p == NULL) {
      break;
    }
    const char* p2 = p + before_len;
    memmove(p + after_len, p2, strlen(p2) + 1);
    memcpy(p, after, after_len);
    p += after_len;
  }

  return s;
}

void tategaki_toggle() {
  naginata_config.tategaki ^= 1;
  eeconfig_update_user(naginata_config.raw);

  // copyTYtable();
}

// void copyTYtable() {
//   memcpy_P(&ngmapl_ty, &ngmapl_tate, sizeof(ngmapl_ty));

//   for (int i = 0; i < sizeof(ngmapl_ty) / sizeof(ngmapl_ty[0]); i++) {
//     if (naginata_config.tategaki) {
//       replace(ngmapl_ty[i].kana, SS_TAP(NGUP), SS_TAP(X_UP));
//       replace(ngmapl_ty[i].kana, SS_TAP(NGDN), SS_TAP(X_DOWN));
//       replace(ngmapl_ty[i].kana, SS_TAP(NGLT), SS_TAP(X_LEFT));
//       replace(ngmapl_ty[i].kana, SS_TAP(NGRT), SS_TAP(X_RIGHT));
//     } else {
//       replace(ngmapl_ty[i].kana, SS_TAP(NGUP), SS_TAP(X_LEFT));
//       replace(ngmapl_ty[i].kana, SS_TAP(NGDN), SS_TAP(X_RIGHT));
//       replace(ngmapl_ty[i].kana, SS_TAP(NGLT), SS_TAP(X_DOWN));
//       replace(ngmapl_ty[i].kana, SS_TAP(NGRT), SS_TAP(X_UP));
//     }
//   }
// }

void kouchi_shift_toggle() {
  naginata_config.kouchi_shift ^= 1;
  eeconfig_update_user(naginata_config.raw);
}

void ng_show_os(void) {
  switch (naginata_config.os) {
    case NG_WIN:
      send_string("win");
      break;
    case NG_MAC:
      send_string("mac");
      if (naginata_config.live_conv) {
        send_string("/:lc");
      } else {
        send_string("/-lc");
      }
      break;
    case NG_LINUX:
      send_string("linux");
      break;
  }
  if (naginata_config.tategaki) {
    send_string("/tate");
  } else {
    send_string("/yoko");
  }
  if (naginata_config.kouchi_shift) {
    send_string("/:kouchi");
  } else {
    send_string("/-kouchi");
  }
}

void mac_send_string(const char *str) {
  send_string(str);
  if (!naginata_config.live_conv) tap_code(KC_SPC);
  tap_code(KC_ENT);
}

// void ng_send_unicode_string(const char *str) {
//   switch (naginata_config.os) {
//     case NG_LINUX:
//       tap_code(KC_INT5);
//       send_unicode_string(str);
//       tap_code(KC_INT4);
//       break;
//     case NG_WIN:
//     case NG_MAC:
//       send_unicode_string(str);
//       tap_code(KC_ENT);
//       break;
//   }
// }

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
  // キーボード側でIME状態を追跡（外部操作のIME切替は追跡不可）
  update_ime_state(keycode, record);

  // まれに薙刀モードオンのまま、レイヤーがオフになることがあるので、対策
  if (n_modifier == 0 && is_naginata && !layer_state_is(naginata_layer))
    layer_on(naginata_layer);
  if (n_modifier == 0 && !is_naginata && layer_state_is(naginata_layer))
    layer_off(naginata_layer);
  if (n_modifier > 0 && layer_state_is(naginata_layer))
    layer_off(naginata_layer);

  // OS切り替え(UNICODE出力)
  if (record->event.pressed) {
    switch (keycode) {
      case NG_ON:
        naginata_on();
        return false;
        break;
      case NG_OFF:
        naginata_off();
        return false;
        break;
      case NG_CLR:
        naginata_clear();
        return false;
        break;
      case NGSW_WIN:
        switchOS(NG_WIN);
        return false;
        break;
      case NGSW_MAC:
        switchOS(NG_MAC);
        return false;
        break;
      case NGSW_LNX:
        switchOS(NG_LINUX);
        return false;
        break;
      case NG_MLV:
        mac_live_conversion_toggle();
        return false;
        break;
      case NG_SHOS:
        ng_show_os();
        return false;
        break;
      case NG_TAYO:
        tategaki_toggle();
        return false;
        break;
      case NG_KOTI:
        kouchi_shift_toggle();
        return false;
        break;
    }
  }

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

    // 特殊: NG_SHFT2 単体でエンター
    if (full_comb == B_SHFT && ng_chrcount >= 1 && ninputs[0] == NG_SHFT2) {
      tap_code(KC_ENT);
      remove_keys_from_buffer(B_SHFT);
      return;
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
          if (ime_is_on && best_key == B_U) {
            send_string("yo");
          } else if (ime_is_on && best_key == B_COMM) {
            send_string("yu");
          } else if (ime_is_on && best_key == B_P) {
            send_string("ya");
          } else {
            send_string(bngmap.kana);
          }
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

  // NG_SHFT2はスペースの代わりにエンターを入力する
  if (keycomb_buf == B_SHFT && ninputs[0] == NG_SHFT2) {
    tap_code(KC_ENT);
    compress_buffer(nt);
    return true;
  }

  if (shifted) {
    // 連続シフトを有効にする
    if ((keycomb & B_SHFT) == B_SHFT) keycomb_buf |= B_SHFT;

    // 編集モードを連続する
    // if ((keycomb & (B_D | B_F))    == (B_D | B_F))    keycomb_buf |= (B_D | B_F);
    // if ((keycomb & (B_C | B_V))    == (B_C | B_V))    keycomb_buf |= (B_C | B_V);
    // if ((keycomb & (B_J | B_K))    == (B_J | B_K))    keycomb_buf |= (B_J | B_K);
    // if ((keycomb & (B_M | B_COMM)) == (B_M | B_COMM)) keycomb_buf |= (B_M | B_COMM);

    // 濁音、半濁音を連続する
    if ((keycomb & B_F) == B_F) keycomb_buf |= B_F;
    if ((keycomb & B_J) == B_J) keycomb_buf |= B_J;
    if ((keycomb & B_V) == B_V) keycomb_buf |= B_V;
    if ((keycomb & B_M) == B_M) keycomb_buf |= B_M;
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
    // case B_M|B_COMM|B_B: //　　　×　　　×　　　×{改行 2}
    //   switch (naginata_config.os) {
    //     case NG_WIN:
    //     case NG_LINUX:
    //       // ng_send_unicode_string("　　　×　　　×　　　×");
    //       tap_code(KC_ENT);
    //       compress_buffer(nt);
    //       return true;
    //       break;
    //     case NG_MAC:
    //       mac_send_string("nagibatu");
    //       compress_buffer(nt);
    //       return true;
    //       break;
    //   }
    //   break;
    // case B_M|B_COMM|B_W: // {改行}{End}《》{改行}{↑}
    //   switch (naginata_config.os) {
    //     case NG_WIN:
    //     case NG_LINUX:
    //       tap_code(KC_ENT);
    //       tap_code(KC_END);
    //       // ng_send_unicode_string("《》");
    //       tap_code(NGUP);
    //       compress_buffer(nt);
    //       return true;
    //       break;
    //     case NG_MAC:
    //       tap_code(KC_ENT);
    //       tap_code(KC_END);
    //       mac_send_string("nagikakkohio");
    //       mac_send_string("nagikakkomio");
    //       tap_code(NGUP);
    //       compress_buffer(nt);
    //       return true;
    //       break;
    //   }
    //   break;
    // case B_M|B_COMM|B_F: // 」{改行 2}「{改行}
    //   switch (naginata_config.os) {
    //     case NG_WIN:
    //     case NG_LINUX:
    //       // ng_send_unicode_string("」");
    //       tap_code(KC_ENT);
    //       // ng_send_unicode_string("「");
    //       compress_buffer(nt);
    //       return true;
    //       break;
    //     case NG_MAC:
    //       mac_send_string("nagikakkomiu");
    //       tap_code(KC_ENT);
    //       mac_send_string("nagikakkohiu");
    //       compress_buffer(nt);
    //       return true;
    //       break;
    //   }
    //   break;
    // case B_M|B_COMM|B_V: // 」{改行 2}{Space}
    //   switch (naginata_config.os) {
    //     case NG_WIN:
    //     case NG_LINUX:
    //       // ng_send_unicode_string("」");
    //       tap_code(KC_ENT);
    //       tap_code(KC_SPC);
    //       compress_buffer(nt);
    //       return true;
    //       break;
    //     case NG_MAC:
    //       mac_send_string("nagikakkomiu");
    //       tap_code(KC_ENT);
    //       tap_code(KC_SPC);
    //       compress_buffer(nt);
    //       return true;
    //       break;
    //   }
    //   break;
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
      // 編集モード Mac
      // if (naginata_config.os == NG_MAC) {
      //   for (int i = 0; i < sizeof ngmapl_mac / sizeof bngmapl; i++) {
      //     memcpy_P(&bngmapl, &ngmapl_mac[i], sizeof(bngmapl));
      //     if (keycomb_buf == bngmapl.key) {
      //       send_string(bngmapl.kana);
      //       compress_buffer(nt);
      //       return true;
      //     }
      //   }
      // }
      // 編集モード 共通
      // for (int i = 0; i < sizeof ngmapl / sizeof bngmapl; i++) {
      //   memcpy_P(&bngmapl, &ngmapl[i], sizeof(bngmapl));
      //   if (keycomb_buf == bngmapl.key) {
      //     send_string(bngmapl.kana);
      //     compress_buffer(nt);
      //     return true;
      //   }
      // }
      // 編集モード 縦書き横書き
      // for (int i = 0; i < sizeof ngmapl_ty / sizeof bngmapl; i++) {
      //   if (keycomb_buf == ngmapl_ty[i].key) {
      //     send_string(ngmapl_ty[i].kana);
      //     compress_buffer(nt);
      //     return true;
      //   }
      // }
      // 編集モード UNICODE文字
      // for (int i = 0; i < sizeof ngmapu / sizeof bngmapu; i++) {
      //   memcpy_P(&bngmapu, &ngmapu[i], sizeof(bngmapu));
      //   if (keycomb_buf == bngmapu.key) {
      //     switch (naginata_config.os) {
      //       case NG_WIN:
      //       case NG_LINUX:
      //         ng_send_unicode_string(bngmapu.win);
      //         break;
      //       case NG_MAC:
      //         mac_send_string(bngmapu.mac);
      //         break;
      //     }
      //     compress_buffer(nt);
      //     return true;
      //   }
      // }
  }
  return false;
}


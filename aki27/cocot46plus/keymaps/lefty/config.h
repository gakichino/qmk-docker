#pragma once

// Auto Mouse Layer（トラックボール操作で _MOUSE を自動起動）
#define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#define AUTO_MOUSE_DEFAULT_LAYER 7
#define AUTO_MOUSE_TIME 5000

// _MOUSE 追加のためレイヤー上限を拡張（VIA は lefty では無効）
#undef DYNAMIC_KEYMAP_LAYER_COUNT
#define DYNAMIC_KEYMAP_LAYER_COUNT 9

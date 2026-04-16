VIA_ENABLE = yes
MOUSEKEY_ENABLE = yes
RGBLIGHT_ENABLE = yes
ENCODER_MAP_ENABLE = yes

# 薙刀エンジン（oonisi 配下の naginata_v17u.c）＋レイヤー同時押し
SRC += layer_chord.c

EXTRAFLAGS += -flto
LTO_ENABLE = yes
CONSOLE_ENABLE = no
COMMAND_ENABLE = no

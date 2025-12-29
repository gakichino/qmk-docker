#!/bin/bash

# QMKファームウェアのビルド用ヘルパースクリプト

set -e


KEYBOARD=$1
KEYMAP=$2

echo "🔨 ファームウェアをビルドしています..."
echo "   キーボード: $KEYBOARD"
echo "   キーマップ: $KEYMAP"
echo ""

docker compose exec -T qmk qmk compile -kb "$KEYBOARD" -km "$KEYMAP"

if [ $? -eq 0 ]; then
    # キーボード名とキーマップ名からファイル名を生成
    # aki27/cocot46plus -> aki27_cocot46plus
    KEYBOARD_SAFE=$(echo "$KEYBOARD" | tr '/' '_')
    HEX_FILE="${KEYBOARD_SAFE}_${KEYMAP}.hex"
    
    # qmk_firmwareからfirmwareディレクトリにコピー
    if [ -f "qmk_firmware/${HEX_FILE}" ]; then
        cp "qmk_firmware/${HEX_FILE}" "firmware/${HEX_FILE}"
        echo ""
        echo "✅ ビルドが成功しました！"
        echo "   ファームウェアファイル: firmware/${HEX_FILE}"
    else
        echo ""
        echo "⚠️  ビルドは成功しましたが、.hexファイルが見つかりませんでした"
        echo "   ファイルを探しています..."
        find qmk_firmware -name "*.hex" -type f -newer qmk_firmware/.build 2>/dev/null | head -3
    fi
else
    echo ""
    echo "❌ ビルドが失敗しました"
    exit 1
fi


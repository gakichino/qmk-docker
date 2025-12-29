#!/bin/bash

# QMK Docker環境のセットアップスクリプト

set -e

echo "🚀 QMK Docker環境のセットアップを開始します..."

# QMK firmwareのクローン
if [ ! -d "qmk_firmware" ]; then
    echo "📦 QMK firmwareリポジトリをクローンしています..."
    git clone https://github.com/qmk/qmk_firmware.git --depth 1 --recurse-submodules --shallow-submodules qmk_firmware
else
    echo "✅ QMK firmwareリポジトリは既に存在します"
fi

if [ ! -n "$(ls -A custom_keyboard)" ]; then
    echo "📦 自作キーボードのリポジトリをクローンしています..."
    git clone https://github.com/aki27kbd/qmk_firmware.git custom_keyboard
    cp -r custom_keyboard/qmk_firmware/keyboards/aki27/ qmk_firmware/keyboards/
else
    echo "✅ 自作キーボードのリポジトリは既に存在します"
fi


# コンテナ内でQMKのセットアップを実行
echo "⚙️  QMKのセットアップを実行しています..."
qmk setup -y



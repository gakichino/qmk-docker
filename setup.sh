#!/bin/bash

# QMK Docker環境のセットアップスクリプト

set -e

echo "🚀 QMK Docker環境のセットアップを開始します..."

# QMK firmwareのクローン
if [ ! -n "$(ls -A qmk_firmware)" ]; then
    echo "📦 QMK firmwareリポジトリをクローンしています..."
    git clone https://github.com/qmk/qmk_firmware.git --depth 1 --recurse-submodules --shallow-submodules qmk_firmware
else
    echo "✅ QMK firmwareリポジトリは既に存在します"
fi

if [ ! -n "$(ls -A custom_keyboard)" ]; then
    echo "📦 自作キーボードのリポジトリをクローンしています..."
    git clone {自作キーボードのリポジトリのURL} custom_keyboard
    # git clone https://github.com/aki27kbd/qmk_firmware.git custom_keyboard
else
    echo "✅ 自作キーボードのリポジトリは既に存在します"
fi

cp -rf {自作キーボードのリポジトリ内のキーボード定義のパス} qmk_firmware/keyboards/
# cp -rf custom_keyboard/keyboards/aki27/ qmk_firmware/keyboards/


# コンテナ内でQMKのセットアップを実行
echo "⚙️  QMKのセットアップを実行しています..."
# 選択肢がある場合は空行（Enter）を送ってデフォルトを選択
printf '\n\n\n' | qmk setup -y || true

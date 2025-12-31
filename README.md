# QMK Firmware Docker環境

Dockerコンテナ上でQMK firmwareを動かして自作キーボードのマッピングを行うための環境です。

## 前提条件

- Docker がインストールされていること
- Docker Compose がインストールされていること（または `docker compose` コマンドが使用可能）

## セットアップ

### カスタムしたいキーボードの値に書き換え
`setup.sh`の
{自作キーボードのリポジトリのURL}
{自作キーボードのリポジトリ内のキーボード定義のパス}
を更新

### 自動セットアップスクリプトを実行

```bash
docker compose up -d
docker compose tail -f qmk
```


このスクリプトは以下を自動で実行します：
- QMK firmwareリポジトリのクローン
- firmwareディレクトリの作成
- Dockerコンテナの起動
- QMKの初期セットアップ
- ログの確認
   - QMK is ready to goになったらOK
   - Ctrl + C でログの確認を終了

### キーマッピングの作成・編集

1. キーマップファイルを編集します。例：

```bash
# キーマップファイルの場所
qmk_firmware/keyboards/<keyboard_name>/keymaps/<keymap_name>/keymap.c
```

2. または、QMK CLIを使用して新しいキーマップを作成：

```bash
qmk new-keymap -kb <keyboard_name> -km <keymap_name>
```

### ファームウェアのビルド

#### ヘルパースクリプトを使用

```bash
# ホストから直接ビルド
./qmk-build.sh <keyboard_name> <keymap_name>

# 例: cocot46plusキーボードのdefaultキーマップをビルド
./qmk-build.sh aki27/cocot46plus default
```

ビルドが成功すると、`firmware/` ディレクトリに `.hex` ファイルが生成されます。

### ファームウェアの書き込み
QMK Toolboxを使用して書き込み
1. QMK Toolboxでhexファイルを開く
2. QMK ToolboxでAuto Flashにチェックを入れる
3. キーボードのリセットボタンを素早く2回押すとブートローダーモードになり、書き込みが始まる

### よく使うコマンド （コンテナ内で実行）

```bash
# 利用可能なキーボードの一覧
qmk list-keyboards

# 特定のキーボードのキーマップ一覧
qmk list-keymaps -kb <keyboard_name>

# キーマップのJSON形式での確認
qmk c2json -km <keymap_name> -kb <keyboard_name> keymap.c

# キーマップのコンパイルチェック（実際にはビルドしない）
qmk lint -kb <keyboard_name> -km <keymap_name>
```

## ディレクトリ構造

```
qmk-docker/
├── setup.sh                # 自動セットアップスクリプト
├── qmk-build.sh            # ビルド用ヘルパースクリプト
├── qmk_firmware/           # QMK firmwareリポジトリ（クローンしたもの）
├── custom_keyboard/        # QMK firmwareリポジトリ（クローンしたもの）
└── firmware/               # ビルド済みファームウェアの出力先
```

## トラブルシューティング

### コンテナが起動しない

```bash
# ログを確認
docker compose logs qmk

# コンテナの状態を確認
docker compose ps
```

### ビルドエラー

```bash
# QMKのセットアップを再実行
docker compose exec qmk qmk setup

# 依存関係を更新
docker compose exec qmk qmk doctor
```

## 参考リンク

- [QMK Firmware公式ドキュメント](https://docs.qmk.fm/)
- [QMK CLIドキュメント](https://github.com/qmk/qmk_cli)
- [QMK Firmware GitHub](https://github.com/qmk/qmk_firmware)



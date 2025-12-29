# QMK Firmware開発環境用Dockerfile
# 公式イメージを使用する場合は不要ですが、カスタマイズが必要な場合に使用

FROM qmkfm/qmk_cli:latest

# 追加のツールや設定が必要な場合はここに記述
# 例: エディタのインストールなど
# RUN apt-get update && apt-get install -y vim nano



# デフォルトコマンド
CMD ["/bin/bash", "-c", "/root/opt/setup.sh; tail -f /dev/null"]


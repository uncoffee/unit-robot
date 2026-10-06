# unit-robot
## master_program
* i2c.py（ライブラリはsmbus2を使った。通信形式を書いた。）
* units.py（様々なユニットに対応したclassがあるよ。）
* unitstart.py（プログラムの最初に呼び出すべき関数を記述。）

## slave_program
### 開発環境はアルディーノIDEかつ動作環境(スレーブ)をアルディーノNANOにする前提のプログラムのなので、注意してください。
* default.cpp（ユニットの基底プログラムを記述　※ユニットを追加するときにこれをコピーして使ってくれ。）
* rover.cpp（ローバーのプログラム。）
* 以下略
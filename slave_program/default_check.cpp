#include "default.hpp"

void setup() {
    // 名前の変更は info() 関数を経由して行う
    constexpr uint8_t name = 0x01; // slave_name
    constexpr uint8_t add = 0x08; // 重複しなければなんでもいい。
    unit::info(name, add);
    
    // セットアップ実行
    unit::setup();
    task::show();
}

void loop() {
    // タイマー用変数の更新
    timer::mil = millis();

    if (i2c::i2c_flag) {
        i2c::run_program();
    }
}
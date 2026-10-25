#include "default.hpp"
class unit {
private:
public:
    unit() = delete;
    
    static constexpr uint8_t name = 0x01; // roverのID（仮）
    static constexpr uint8_t add = 0x08; // 重複しなければなんでもいい。

    static void setup() {

    }
};

void setup() {
    // 名前の変更とスレーブアドレスの設定は info() 関数を経由して行う　スレーブアドレスはI2Cのクラスで設定しろよ？うるせええ default_programは半神のクラスなんだよ！
    default_program::info(unit::name, unit::add);
    
    // セットアップ実行
    default_program::setup();
    unit::setup();
    task::show();
}

timer sleep_modoki = new timer(20);

void loop() {
    // タイマー用変数の更新
    timer::mil = millis();

    if (sleep_modoki.check()) {
        i2c::run_program();
        unit::rover_run();
    }
}
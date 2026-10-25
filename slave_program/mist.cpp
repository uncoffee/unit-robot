#include "default.hpp"

namespace unit {
    constexpr uint8_t SPLASH_PIN = 12;
    timer run_timer = new timer(0);

    void set_time(uint8_t time) {
      run_timer.restart(time * 1000); // s -> ms
    }

    constexpr uint8_t name = 0x02; // roverのID（仮）
    constexpr uint8_t add = 0x09; // 重複しなければなんでもいい。
    
    void mist_run() {
      if (run_timer.check()) {
        digitalWrite(SPLASH_PIN, LOW);
        return;
      }

      digitalWrite(SPLASH_PIN, HIGH);
    }

    void setup() {
      new task("ミストの噴出時間", set_time);

    }
}

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
        unit::mist_run();
    }
}
#include "default.hpp"

// グローバル変数の定義
uint8_t fir_byte = 0;
uint8_t sec_byte = 0;
bool i2c_flag = false;
String result = "";

// static メンバ変数の実体定義および初期化
tasks* tasks::inst_list[tasks::MAX_TASKS];
uint8_t tasks::inst_count = 0;

uint32_t timer::millis = 0;

const char* unit::unit_name = "default_unit";
uint8_t unit::slave_address = 0x08; // 任意のI2Cスレーブアドレス

// 受信処理
void receiveEvent(int howMany) {
    fir_byte = Wire.read();
    
    if (howMany > 1) {
        sec_byte = Wire.read();
    } else {
        sec_byte = 0;
    }

    while (Wire.available()) {
        char _ = Wire.read();
    }
    i2c_flag = true;
}

// 送信処理
void requestEvent() {
    Wire.write(result.c_str());
}

void setup() {
    // 名前の変更は info() 関数を経由して行う
    unit::info("units", 8);
    
    // セットアップ実行
    unit::setup();
    tasks::show();
}

void loop() {
    // タイマー用変数の更新
    timer::millis = ::millis();

    if (i2c_flag) {
        i2c_flag = false;
        tasks::search(fir_byte, sec_byte);
    }
}
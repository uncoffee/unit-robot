#include "default.hpp"

"""

実装中です。


"""
namespace unit {
    constexpr uint8_t name = 0x01; // roverのID（仮）
    constexpr uint8_t add = 0x08; // 重複しなければなんでもいい。

    Adafruit_BMP280 bmp; // I2C接続
    uint8_t BMP_ID = 0x76 // 初期値として0x76に設定
    uint8_t temp = 0.0; // ℃
    uint8_t pres = 0.0; // hPa

    void BME_get() {
      temp = bmp.readTemperature();
      pres = bmp.readPressure() / 100.0F;
    }

    bool _BME() {
      if (bmp.begin(BMP_ID)) {
        return true;
      }
      BMP_ID = 0x77

      if (bmp.begin(BMP_ID)) {
        return true;
      }

      Serial.println("温湿度センサーが見つかりません")
      return false;
    }

    void data_receive(uint8_t _=0) {
      i2c::result_set(temp, pres);
    }
    

    void setup() {
      _BME();

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
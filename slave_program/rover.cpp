#include "default.hpp"
class unit {
private:
    // ピン配置の定義
    static constexpr int FRONT_R = 11; // 前進のPIN1
    static constexpr int BACK_R = 12; // 後退のPIN1
  
    static constexpr int FRONT_L = 9; // 前進のPIN2
    static constexpr int BACK_L = 10; // 後退のPIN2
    static bool right;
    static bool left;
    static uint8_t duty; // 255の値が100%
    static timer run_timer;

    static void set_time(uint8_t time) {
      run_timer.restart(time * 1000); // s -> ms
    }
  
    static void change_direction(uint8_t bool_num) {
      if(bool_num = 0) {
        right = false;
        left = false;
        
      } else if (bool_num = 1) {
        right = true;
        left = false;
        
      } else if (bool_num = 2) {
        right = false;
        left = true;
        
      } else if (bool_num = 3) {
        right = true;
        left = true;
      }
    }

    static void set_speed(uint8_t speed) {
      duty = speed;
    }

    static void how_speed(uint8_t _) {
      i2c::result_set(duty);
    }
  
public:
    unit() = delete;
    
    static constexpr uint8_t name = 0x01; // roverのID（仮）
    static constexpr uint8_t add = 0x08; // 重複しなければなんでもいい。

    static void setup() {
      new task("roverの稼働時間を設定。",set_time);
      new task("進行方向を設定。引数は 0b Left Right。",change_direction);
      new task("速度設定。0~255で指定",set_speed);
      new task("現在の速度を返信",how_speed);
      
    }
  
    static void _rover_stop() {
      analogWrite(FRONT_R, 0);
      analogWrite(BACK_R, 0);
      analogWrite(FRONT_L, 0);
      analogWrite(BACK_L, 0);
    }

    static void rover_run() {
      if (run_timer.check()) {
        _rover_stop();
        return;
      }
    
      if (right) {
        analogWrite(FRONT_R, duty);
        analogWrite(BACK_R, 0);
      } else {
        analogWrite(BACK_R, duty);
        analogWrite(FRONT_R, 0);
      }
    
      if (left) {
        analogWrite(FRONT_L, duty);
        analogWrite(BACK_L, 0);
      } else {
        analogWrite(BACK_L, duty);
        analogWrite(FRONT_L, 0);
      }
    }
};
bool unit::right = true;
bool unit::left = true;
uint8_t unit::duty = 255; // 255の値が100%
timer unit::run_timer = new timer(0);

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
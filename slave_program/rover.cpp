#include <Wire.h>
#include <avr/wdt.h>

// 変更禁止ゾーン----------------------------------------------------
const int LED_PIN = 13; // Arduino Nano などの標準内蔵LED（ピン13）

bool led_flag = false; // ledのフラグ
bool reboot_flag = false; // rebootのフラグ

unsigned long nowtime = millis(); // 現在時刻(プログラム開始からMS毎で増加)
unsigned long time_start = 0; // 稼働開始時間
int time_goal = 0; // 稼働終了時間

volatile bool isReady = false; // マスターから命令が来ていたら真になる
String flag = ""; // 稼働時間を受け取るときは真になる
String inputBuffer = ""; // 受け取ったメッセをまとめてぶち込む
String receivedMessage = ""; // 最後に受け取ったメッセージを持っておくやつ
String sendMsg = ""; // readが来たら送り返す文字を入れとくやつ
String send_text = ""; // 文字数を先に伝えるから、その間は返答を持っておくやつ
String sendLength = ""; // 送り返す文字の長さを保存しとく

// ----------------------------------------------------


// ここから下は自由にしてくれ----------------------------------------------------
// 基本設定
#define SLAVE_ADDRESS 0x08 // 0x08から0x77まで(わかってると思うけど。16進数だよ？)
const String job = "rover"; // ユニット固有の変数を宣言
const int clock_blank = 20; // 何ミリ毎秒ごとに実行するか
const bool test_mode = false; // テストモードか否か

// ピン配置の定義
const int MOTOR_FRONT_R = 11; // 前進のPIN1
const int MOTOR_BACK_R = 12; // 後退のPIN1

const int MOTOR_FRONT_L = 9; // 前進のPIN2
const int MOTOR_BACK_L = 10; // 後退のPIN2

// プログラムで使うグローバル変数
bool right = true;
bool left = true;
int speed = 100; // 0->停止　100->全速前進

// ----------------------------------------------------

// オリジナルの処理を追加しよう。
void add_receive(String task) {
  if (task == "rf") {
    right = true;
    return;
  }
  if (task == "lf") {
    left = true;
    return;
  }
  if (task == "rb") {
    right = false;
    return;
  }
  if (task == "lb") {
    left = false;
    return;
  }
  if (task == "setspeed") {
    flag = "setspeed";
    return;
  }
  if (flag == "setspeed") {
    speed = task.toInt();
    flag = "";
    return;
  }
  if (task == "howspeed") {
    send_text = String(speed);
    return;
  }
}

// マスターからの命令に対応した動作 voidじゃないとだめ。
void defa_receive(String task) {
  if (test_mode) {
    Serial.println(task);
  }

  if (task == "result") {
    sendMsg = send_text;
    return;
  }
  if (task == "num") {
    sendMsg = String(send_text.length());
    return;
  }
  if (task == "who") {
    send_text = job;
    return;
  }
  if (task == "reboot") { 
    reboot_flag = true;
    return;
  }

  if (task == "led_on") {
    led_flag = true;
    return;
  }
  if (task == "led_off") {
    led_flag = false;
    return;
  }
  if (task == "settime") {
    flag = "time";
    return;
  }
  if (flag == "time") {
    time_goal = task.toInt() * 1000; // MS単位で処理するため1000をかけてS単位にする
    time_start = millis();
    flag = "";
    return;
  }
  add_receive(task);

  if (test_mode) {
    Serial.println("taskが拾われなかった。");
  }
}

void reboot() {
  if (reboot_flag) {
    wdt_enable(WDTO_15MS);
    while (1) {}
  }
}

// 受信（割り込み処理）
void receiveEvent(int howMany) {
  while (Wire.available()) {
    char c = Wire.read();
    
    if (c == '?') {
      receivedMessage = inputBuffer;
      inputBuffer = "";
      isReady = true;
    } else {
      inputBuffer += c;
    }
  }
}

// 持っていた命令を取り出す
String getMessage() {
  noInterrupts();
  String temp = receivedMessage;
  isReady = false;
  interrupts();
  return temp;
}

// 送信（割り込み処理）
void requestEvent() {
  Wire.write(sendMsg.c_str());
}

// 内蔵ledの制御
void l_switch() {
  if (led_flag) {
    digitalWrite(LED_PIN, HIGH); // LED消灯
  } else {
    digitalWrite(LED_PIN, LOW); // LED消灯
  }
}
bool clock() {
  if (nowtime % clock_blank == 0){
    return true;
  } else {
    return false;
  }
}

bool time() {
  if (nowtime - time_start > time_goal) {
    return true;
  } else {
    return false;
  }
}

void runtask() {
  if (isReady) {
    String msg = getMessage();
    defa_receive(msg);
  }
}
// ここから固有の関数----------------------------------------

void motorstop() {
  // 前進後退　出力を0にする
  analogWrite(MOTOR_FRONT_R, 0); 
  analogWrite(MOTOR_FRONT_L, 0);
  analogWrite(MOTOR_BACK_R, 0);
  analogWrite(MOTOR_BACK_L, 0);
  digitalWrite(LED_PIN, LOW); // LED消灯
}

void rover_run() {
  if (time()) {
    motorstop();
    return;
  }

  int duty = map(speed, 0, 100, 0, 255); // 0〜100% の値を Arduino の PWM 範囲（0〜255）に変換

  if (right) {
    analogWrite(MOTOR_FRONT_R, duty);
    analogWrite(MOTOR_BACK_R, 0);
  } else {
    analogWrite(MOTOR_BACK_R, duty);
    analogWrite(MOTOR_FRONT_R, 0);
  }

  if (left) {
    analogWrite(MOTOR_FRONT_L, duty);
    analogWrite(MOTOR_BACK_L, 0);
  } else {
    analogWrite(MOTOR_BACK_L, duty);
    analogWrite(MOTOR_FRONT_L, 0);
  }
}

// ----------------------------------------

void setup() {
  Serial.print(job);
  Serial.println("_unit program start");

  // I2Cの関数の指定
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);


  // シリアル通信の開始
  Wire.begin(SLAVE_ADDRESS);
  Serial.begin(9600);

  // ピンのモード設定
  pinMode(LED_PIN, OUTPUT);

  // setupの最初でウォッチドッグを無効化（リセットループ防止）
  wdt_disable();



  // 以下追加セットアップ------------------------------
  // 初期状態は停止
  motorstop();

  // pin設定
  pinMode(MOTOR_FRONT_R, OUTPUT);
  pinMode(MOTOR_BACK_R, OUTPUT);
  pinMode(MOTOR_FRONT_L, OUTPUT);
  pinMode(MOTOR_BACK_L, OUTPUT);
  
}

void loop() {
  nowtime = millis();
  // 元からある関数
  runtask();
  if (clock()) {
    reboot();
    l_switch();

    // 以下オリジナル関数
    rover_run();

  }
}
#include <Wire.h>
#include <avr/wdt.h>

// 変更禁止ゾーン
const int LED_PIN = 13; // Arduino Nano などの標準内蔵LED（ピン13）

bool led = false;
unsigned long timer_start = 0; // 稼働時間の操作用
int timer_end = 0; // 稼働時間の操作用



volatile bool isReady = false; // マスターから命令が来ていたら真になる
String flag = ""; // 稼働時間を受け取るときは真になる
String inputBuffer = ""; // 受け取ったメッセをまとめてぶち込む
String receivedMessage = ""; //最後に受け取ったメッセージを持っておくやつ
String sendMsg = ""; // readが来たら送り返す文字を入れとくやつ
String send_text = ""; // 文字数を先に伝えるから、その間は返答を持っておくやつ
String sendLength = ""; // 送り返す文字の長さを保存しとく

bool reboot_flag = false; // rebootのフラグ



// ここから下は自由にしてくれ。
#define SLAVE_ADDRESS 0x08 // 0x08から0x77まで(わかってると思うけど。16進数だよ？)
const String job = "rover"; //ユニット固有の変数を宣言
const int blank_MS = 10; //何ミリ毎秒ごとに実行するか

// ピン配置の定義
const int MOTOR_FRONT_R = 11; //前進のPIN1
const int MOTOR_BACK_R = 12; //後退のPIN1

const int MOTOR_FRONT_L = 9; //前進のPIN2
const int MOTOR_BACK_L = 10; //後退のPIN2

// プログラムで使うグローバル変数
bool right = true;
bool left = true;
int speed = 100; // 0->停止　100->全速前進

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

//マスターからの命令に対応した動作 voidじゃないとだめ。
void defa_receive(String task) {
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
    led = true;
    return;
  }
  if (task == "led_off") {
    led = false;
    return;
  }
  if (task == "settime") {
    flag = "time";
    return;
  }
  if (flag == "time") {
    timer_end = task.toInt() * 1000; //MS単位で処理するため1000をかけてS単位にする。
    timer_start = millis();
    flag = "";
    return;
  }
  add_receive(task);
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
  if (led) {
    digitalWrite(LED_PIN, HIGH); // LED消灯
  } else {
    digitalWrite(LED_PIN, LOW); // LED消灯
  }
}

bool timer() {
  if (millis() - timer_start > timer_end) {
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
//ここから固有の関数

void motorstop() {
  // 前進後退　出力を0にする
  analogWrite(MOTOR_FRONT_R, 0); 
  analogWrite(MOTOR_FRONT_L, 0);
  analogWrite(MOTOR_BACK_R, 0);
  analogWrite(MOTOR_BACK_L, 0);
  digitalWrite(LED_PIN, LOW); // LED消灯
}

void rover_run() {
  if (timer()) {
    motorstop();
    return;
  }

  Serial.println("動いてるはず");

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


//ここまで

void setup() {
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.print(job);
  Serial.println("_unit program start");

  // シリアル通信の開始
  Serial.begin(9600);

  // ピンのモード設定
  pinMode(LED_PIN, OUTPUT);

  // setupの最初でウォッチドッグを無効化（リセットループ防止）
  wdt_disable();

  // 初期状態は停止
  motorstop();

  // 以下追加セットアップ
  pinMode(MOTOR_FRONT_R, OUTPUT);
  pinMode(MOTOR_BACK_R, OUTPUT);
  pinMode(MOTOR_FRONT_L, OUTPUT);
  pinMode(MOTOR_BACK_L, OUTPUT);
  
}

void loop() {
  // 元からある関数
  runtask();
  if ()
  reboot();
  l_switch();

  // 以下オリジナル関数
  rover_run();
}
#include <Wire.h>
#include "i2c.hpp"
//環境はArduino IDEを想定しているためStringのincludeはしていない。ほかの環境を使ってやるときは自分で書き加えて。

// 変更禁止ゾーン
// ピン配置の定義
const int LED_PIN = 13; // Arduino Uno などの標準内蔵LED（ピン13）

bool led = false;
int time = 0; // 0->停止　100->100秒後停止

unsigned long previousMillis = 0;

volatile bool isReady = false; // マスターから命令が来ていたら真になる
String get_info = ""; // 稼働時間を受け取るときは真になる
String inputBuffer = ""; // 受け取ったメッセをまとめてぶち込む
String receivedMessage = ""; //最後に受け取ったメッセージを持っておくやつ
String sendMsg = ""; // readが来たら送り返す文字を入れとくやつ
String cache = ""; // 文字数を先に伝えるから、その間は返答を持っておくやつ
String sendLength = ""; // 送り返す文字の長さを保存しとく


// ここから下は自由にしてくれ。
#define SLAVE_ADDRESS 0x08 // 0x08から0x77まで(わかってると思うけど。16進数だよ？)
const String job = "rover"; //ユニット固有の変数を宣言
const int blank = 10; //一秒間に何回処理を繰り返すか。※1000以下の偶数の数字にして。割り切れない。

// ピン配置の定義
const int MOTOR_FRONT_R = 11; //前進のPIN1
const int MOTOR_BACK_R = 12; //後退のPIN1

const int MOTOR_FRONT_L = 9; //前進のPIN2
const int MOTOR_BACK_L = 10; //後退のPIN2

// プログラムで使うグローバル変数
bool right = true;
bool left = true;
bool motorflag = false;
int speed = 100; // 0->停止　100->全速前進



//マスターからの命令に対応した動作
void tasks(String receive) {
  //通信用の命令受付
  if (receive == "result") {
    sendMsg = cache;
  }

  if (receive == "num") {
    sendMsg = String(cache.length());
  }

  if (receive == "who") {
    cache = job;
  }

  if (receive == "stop") { 
    stop();
  }

  if (receive == "led_on") {
    led = true;
  }

  if (receive == "led_off") {
    led = false;
  }

  if (receive == "settime") {
    get_info = "time";
  } else if (get_info == "time") {
    time = receive.toInt() * 1000 / blank;
    get_info = "";
  }

  //ここから下に追加する
  if (receive == "rf") {
    right = true;
  }

  if (receive == "rb") {
    right = false;
  }

  if (receive == "lf") {
    left = true;
  }

  if (receive == "lb") {
    left = false;
  }

  if (reveive == "motor") {
    motorflag = true;
  }

  if (receive == "whatspeed") {
    cache = String(speed);
  }

  if (receive == "speed") {
    get_info = "speed";
  } else if (get_info == "speed") {
    speed = receive.toInt();
    get_info = "";
  }
}

//変数をすべて初期値にする
void stop() {
  //通信に使う変数を宣言
  isReady = false;
  get_info = "";
  inputBuffer = "";
  receivedMessage = "";
  sendMsg = "";
  cache = "";
  sendLength = "";
  right = true;
  left = true;
  led = false;
  speed = 100;
  time = 0;
  previousMillis = 0;
}

// 送信（割り込み処理）
void requestEvent() {
  Wire.write(sendMsg.c_str());
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

// 内蔵ledの制御
void l_switch() {
  if (led) {
    digitalWrite(LED_PIN, HIGH); // LED消灯
  } else {
    digitalWrite(LED_PIN, LOW); // LED消灯
  }
}
// ここから固有の関数
// モーターを動かす関数
void move() {
  if (motorflag) {
    int duty = map(speed, 0, 100, 0, 255);
    motorstop();

    if (right) {
      analogWrite(MOTOR_FRONT_R, duty);
    } else {
      analogWrite(MOTOR_BACK_R, duty);
    }

    if (left) {
      analogWrite(MOTOR_FRONT_L, duty);
    } else {
      analogWrite(MOTOR_BACK_L, duty);
    }
  } else {
    analogWrite(MOTOR_FRONT_R, LOW);
    analogWrite(MOTOR_FRONT_L, LOW);
    
    analogWrite(MOTOR_BACK_R, LOW);
    analogWrite(MOTOR_BACK_L, LOW);
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
  pinMode(MOTOR_FRONT_R, OUTPUT);
  pinMode(MOTOR_BACK_R, OUTPUT);
  pinMode(MOTOR_FRONT_L, OUTPUT);
  pinMode(MOTOR_BACK_L, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // 初期状態は停止
  motorstop();
}

void loop() {
  if (isReady) {
  String msg = getMessage();
  Serial.println("受信メッセージ: ");
  Serial.println(msg);
  decode_task(tasks ,msg);
  }

  // 前回の実行から指定時間が経過したかチェック
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= blank) {
    previousMillis = currentMillis;
    // ここに定期実行したい処理を書く

    l_switch();

    if (time > 0) {
      move();
      time = time - 1; // time はint型なのでNoProblem!
    } else if (time < 1) {
      motorflag = false;
    }
  }
}
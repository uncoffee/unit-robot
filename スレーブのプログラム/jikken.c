#include <Wire.h>

#define SLAVE_ADDRESS 0x10

volatile bool isReady = false;
String receivedMessage = ""; 

String inputBuffer = ""; // 受け取ったメッセをまとめてぶち込む   
String sendMsg = ""; // readが来たら送り返す文字を入れとくやつ
String cache = ""; // 文字型だったら適当に入れられる便利な奴
String job = "rover";
String sendLength = ""; // 送り返す文字の長さを保存しとく

void setup() {
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.println("I2C Slave Ready.");

  // シリアル通信の開始
  Serial.begin(9600);

  // ピンのモード設定
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // 初期状態は停止
  motorStop();
}

void loop() {  
  if (isReady) {
    String msg = getMessage();
    Serial.print("受信メッセージ: ");
    Serial.println(msg);
    TextCheck(msg);
  }
  
  if (go) {
    // 1. スピード50%で前進
    Serial.println("前進（スピード50%） - LED点灯");
    motorMove(50, "forward");
    delay(3000); // 3秒待機

    // 2. 停止
    Serial.println("停止 - LED消灯");
    motorStop();
    delay(2000); // 2秒待機

    go = false;
  }

  if (back) {
    // 3. スピード100%で後退
    Serial.println("後退（スピード100%） - LED点灯");
    motorMove(100, "backward");
    delay(3000); // 3秒待機

    // 4. 停止
    Serial.println("停止 - LED消灯");
    motorStop();
    delay(2000); // 2秒待機

    back = false;
  }

  delay(10);
}

String getMessage() {
  noInterrupts();
  String temp = receivedMessage;
  isReady = false;
  interrupts();
  return temp;    
}

void TextCheck(String receive) {
  if (receive == "result") {
    sendMsg = cache;
  }

  if (receive == "num") {
    sendMsg = sendLength;
  }

  if (receive == "who") {
    cache = job;
    Serial.println(String(job.length()));
    sendLength = String(job.length());
  }

  if (receive == "go") {
    go = true;
    Serial.println("go");
  }

  if (receive == "back") {
    back = true;
    Serial.println("back");
  }
}

// 割り込み処理（Serial.printは使わない）
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

// 割り込み処理（Serial.printは使わない）

void requestEvent() {
  Serial.print("送信メッセージ: ");
  Serial.println(sendMsg);
  Wire.write(sendMsg.c_str());
}

// ピン配置の設定
const int MOTOR_IN1 = 14; // GP14 に相当するピン
const int MOTOR_IN2 = 15; // GP15 に相当するピン
const int LED_PIN   = 13; // Arduino Uno などの標準内蔵LED（ピン13）

// 動作フラグ（必要に応じて true に変更して使用してください）
bool go   = false;
bool back = false;

// モーターを停止する関数
void motorStop() {
  analogWrite(MOTOR_IN1, 0); // 出力を0にする
  analogWrite(MOTOR_IN2, 0);
  digitalWrite(LED_PIN, LOW); // LED消灯
}

// モーターを動かす関数（speedは 0〜100 % で指定）
void motorMove(int speed, String direction) {
  // 0〜100% の値を Arduino の PWM 範囲（0〜255）に変換
  int duty = map(speed, 0, 100, 0, 255);

  if (direction == "forward") {
    analogWrite(MOTOR_IN1, duty);
    analogWrite(MOTOR_IN2, 0);
    digitalWrite(LED_PIN, HIGH); // LED点灯
  } 
  else if (direction == "backward") {
    analogWrite(MOTOR_IN1, 0);
    analogWrite(MOTOR_IN2, duty);
    digitalWrite(LED_PIN, HIGH); // LED点灯
  }
}


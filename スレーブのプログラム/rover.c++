#include <Wire.h>
//環境はArduino IDEを想定しているためStringのincludeはしていない。ほかの環境を使ってやるときは自分で書き加えて。

//スレーブID
#define SLAVE_ADDRESS 0x10

//通信に使う変数を宣言
volatile bool isReady = false; // マスターから命令が来ていたら真になる
bool need_time = false; // 稼働時間を受け取るときは真になる
String inputBuffer = ""; // 受け取ったメッセをまとめてぶち込む
String receivedMessage = ""; //最後に受け取ったメッセージを持っておくやつ
String sendMsg = ""; // readが来たら送り返す文字を入れとくやつ
String cache = ""; // 文字数を先に伝えるから、その間は返答を持っておくやつ
String sendLength = ""; // 送り返す文字の長さを保存しとく

//ユニット固有の変数を宣言
const String job = "rover";
String direction = ""; // 動きの命令:"stop"停止,"go"前進,"back"後退,"left"左に動く,"right"右に動く
int speed = 50; // 速さ
int time = 0;

// ピン配置の設定
const int MOTOR_IN1 = 14; // GP14 に相当するピン
const int MOTOR_IN2 = 15; // GP15 に相当するピン
const int LED_PIN   = 13; // Arduino Uno などの標準内蔵LED（ピン13）

//処理の設定
const int blank = 10; //一秒間に何回処理を繰り返すか。※1000以下の偶数の数字にして。割り切れない。
unsigned long previousMillis = 0;

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

//マスターからの命令に対応した動作
void decode_task(String receive) {
  //通信用の命令受付
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

  //メインの命令受付
  if (receive == "go") {
    direction = "go";
    Serial.println("go");
    need_time = true;
  }

  if (receive == "back") {
    direction = "back";
    Serial.println("back");
    need_time = true;
  }

  if (receive == "led") {
    direction = "led";
    Serial.println("led");
    need_time = true;
  }

  if (need_time) {
    time = receive.toInt() * 1000 / blank;
  }
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

// モーターを停止する関数
void motorStop() {
  analogWrite(MOTOR_IN1, 0); // 出力を0にする
  analogWrite(MOTOR_IN2, 0);
  digitalWrite(LED_PIN, LOW); // LED消灯
}

// モーターを動かす関数（speedは 0〜100 % で指定）
void Move() {
  // 0〜100% の値を Arduino の PWM 範囲（0〜255）に変換
  int duty = map(speed, 0, 100, 0, 255);

  if (direction == "go") {
    analogWrite(MOTOR_IN1, duty);
    analogWrite(MOTOR_IN2, 0);
    digitalWrite(LED_PIN, HIGH); // LED点灯
  } 
  else if (direction == "back") {
    analogWrite(MOTOR_IN1, 0);
    analogWrite(MOTOR_IN2, duty);
    digitalWrite(LED_PIN, HIGH); // LED点灯
  }else if (direction == "led") {
    digitalWrite(LED_PIN, HIGH);
  }else{
    analogWrite(MOTOR_IN1, 0); // 出力を0にする
    analogWrite(MOTOR_IN2, 0);
    digitalWrite(LED_PIN, LOW); // LED消灯
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // 前回の実行から指定時間が経過したかチェック
  if (currentMillis - previousMillis >= blank) {
    previousMillis = currentMillis; // 時間を更新

      // ここに定期実行したい処理を書く
    if (isReady) {
      String msg = getMessage();
      Serial.println("受信メッセージ: ");
      Serial.println(msg);
      decode_task(msg);
    }
    if (time > 0) {
      Move();
      time = time - 1;
    }else{
      motorStop();
      direction = "none";
    }
  }
}
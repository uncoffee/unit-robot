#include <Wire.h>
#include <avr/wdt.h>
//環境はArduino IDEを想定しているためStringのincludeはしていない。ほかの環境を使ってやるときは自分で書き加えて。

//スレーブID
#define SLAVE_ADDRESS 0x10

bool reboot_bool = false;

// ピン配置の設定
const int MOTOR_FRONT_R = 11; //前進のPIN1
const int MOTOR_BACK_R = 12; //後退のPIN1

const int MOTOR_FRONT_L = 9; //前進のPIN2
const int MOTOR_BACK_L = 10; //後退のPIN2

const int LED_PIN = 13; // Arduino Uno などの標準内蔵LED（ピン13）

const String job = "rover"; //ユニット固有の変数を宣言

const int blank_time = 20; //一秒間に何回処理を繰り返すか。※1000以下の偶数の数字にして。割り切れない。


//通信に使う変数を宣言
volatile bool isReady = false; // マスターから命令が来ていたら真になる
bool need_time = false; // 稼働時間を受け取るときは真になる
String inputBuffer = ""; // 受け取ったメッセをまとめてぶち込む
String receivedMessage = ""; //最後に受け取ったメッセージを持っておくやつ
String sendMsg = ""; // readが来たら送り返す文字を入れとくやつ
String cache = ""; // 文字数を先に伝えるから、その間は返答を持っておくやつ
String sendLength = ""; // 送り返す文字の長さを保存しとく

//モータ切り替え true->前進 false->後退
bool right = true;
bool left = true;

bool led = false;

int speed = 100; // 0->停止　100->全速前進
int time = 0; // 0->停止　100->100秒後停止

unsigned long previousMillis = 0;

//追加したタスクをここに設定する
void addtasks() {
  //aaaaaaaaa
  return;
}

//マスターからの命令に対応した動作
void tasks(String receive) {
  //通信用の命令受付
  if (receive == "result") {
    sendMsg = cache;
    return;
  }

  if (receive == "num") {
    sendMsg = String(cache.length());
    return;
  }

  if (receive == "who") {
    cache = job;
    return;
  }

  if (receive == "reboot") { 
    reboot_bool = ture;
    return;
  }

  //メインの命令受付
  if (receive == "led_on") {
    led = true;
    return;
  }

  if (receive == "led_off") {
    led = false;
    return;
  }

  if (receive == "rf") {
    right = true;
    return;
  }

  if (receive == "rb") {
    right = false;
    return;
  }

  if (receive == "lf") {
    left = true;
    return;
  }

  if (receive == "lb") {
    left = false;
    return;
  }

  if (receive == "time") {
    need_time = true;
    return;
  } 
  
  if (need_time) {
    time = receive.toInt() * 1000 / blank_time;
    need_time = false;
    return;
  }

  addtasks();
}

//定期処理にする関数　※millis()の引数は経過時間をmsで返してくる
bool blank_def() {
  int now = millis();
  if (now - millis() >= blank_time) {
    time -= 1;
    return true;
  } else {
    return false;
  }
}

void default_process() {
  if (reboot_bool) {
  wdt_enable(WDTO_15MS); // 15ミリ秒後にタイムアウトするように設定
  while (1) {}           // タイムアウトまで無限ループして待つ
  }
}

//変数をすべて初期値にする
void stop() {
  //通信に使う変数を宣言
  isReady = false;
  need_time = false;
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
void l_switch() {
  if (led) {
    digitalWrite(LED_PIN, HIGH); // LED消灯
  } else {
    digitalWrite(LED_PIN, LOW); // LED消灯
  }
}

// モーターを停止する関数
void moterstop() {
  analogWrite(MOTOR_FRONT_R, LOW);
  analogWrite(MOTOR_FRONT_L, LOW);
  
  analogWrite(MOTOR_BACK_R, LOW);
  analogWrite(MOTOR_BACK_L, LOW);
}

// モーターを動かす関数（speedは 0〜100 % で指定）
void move() {
  // 0〜100% の値を Arduino の PWM 範囲（0〜255）に変換
  int duty = map(speed, 0, 100, 0, 255);

  moterstop();

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
}

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
  moterstop();

  // setupの最初でウォッチドッグを無効化（リセットループ防止）
  wdt_disable();
}

void loop() {
  if (isReady) {
  String msg = getMessage();
  Serial.println("受信メッセージ: ");
  Serial.println(msg);
  decode_task(msg);
  }

  // 前回の実行から指定時間が経過したかチェック
  unsigned long currentMillis = 
  if (blank_def()) {
    // ここに定期実行したい処理を書く

    l_switch();

    if (time > 0) {
      move();
      time = time - 1; // time はint型なのでNoProblem!
    } else if (time < 1) {
      moterstop();
    }
  }
}
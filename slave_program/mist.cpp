#include <Wire.h>
#include <avr/wdt.h>

// 変更禁止ゾーン
const int LED_PIN = 13; // Arduino Nano などの標準内蔵LED（ピン13）
bool led = false;
int timer_count = 0; // 0->停止　100->100秒後停止
unsigned long previousMillis = 0;
volatile bool isReady = false; // マスターから命令が来ていたら真になる
String get_info = ""; // 稼働時間を受け取るときは真になる
String inputBuffer = ""; // 受け取ったメッセをまとめてぶち込む
String receivedMessage = ""; //最後に受け取ったメッセージを持っておくやつ
String sendMsg = ""; // readが来たら送り返す文字を入れとくやつ
String send_text = ""; // 文字数を先に伝えるから、その間は返答を持っておくやつ
String sendLength = ""; // 送り返す文字の長さを保存しとく
bool reboot_flag = false; // rebootのフラグ
int currentMillis = 0; // 定期実行の経過時間確認用


// ここから下は自由にしてくれ。
#define SLAVE_ADDRESS 0x09 // 0x08から0x77まで(わかってると思うけど。16進数だよ？)
const String job = "mist"; //ユニット固有の変数を宣言
const int blank_MS = 10; //何ミリ毎秒ごとに実行するか

const int SPLASH_PIN = 12; //前進のPIN1

bool mist_flag = false; // プログラムで使うグローバル変数

// オリジナルの処理を追加しよう。
void add_receive(String task) {
  if (1) {
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
    get_info = "time";
    return;
  }
  if (get_info == "time") {
    timer_count = task.toInt() * 1000 / blank;
    get_info = "";
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


bool blank() {
  if (millis() - currentMillis >= blank_MS) {
    currentMillis = millis();
    return true
  } else {
    return false
  }
  
}

bool timer() {
  if (timer_count <= 0) {
    return false;
  } else {
    timer_count -= 1000 / blank;
    return true;
  }
}

void runtask() {
  if (isReady) {
    String msg = getMessage();
    defa_receive(msg);
  }
}
//ここから固有の関数
void mist_splash() {
  if (mist_flag) {
    digitalWrite(SPLASH_PIN, HIGH);
  } else {
    digitalWrite(SPLASH_PIN, LOW);
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
  moterstop();

  // 以下追加セットアップ
  pinMode(MOTOR_FRONT_R, OUTPUT);
  pinMode(MOTOR_BACK_R, OUTPUT);
  pinMode(MOTOR_FRONT_L, OUTPUT);
  pinMode(MOTOR_BACK_L, OUTPUT);
  
}

void loop() {
  // 前回の実行から指定時間が経過したかチェック
  runtask();

  if (blank()) {
    // ここに定期実行したい処理を書く
    mist_splash();
    reboot();
    l_switch();

    // 一定時間動き続ける制御の時に使う。
    if (timer()) {
      mist_flag = true;
    } else {
      mist_flag = false;
    }
  }
}
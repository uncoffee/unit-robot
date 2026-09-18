#include <Wire.h>
#include <avr/wdt.h>
//環境はArduino IDEを想定しているためStringのincludeはしていない。ほかの環境を使ってやるときは自分で書き加えて。

// 変更禁止ゾーン
// ピン配置の定義
const int LED_PIN = 13; // Arduino Uno などの標準内蔵LED（ピン13）

bool led = false;
int timer_count = 0; // 0->停止　100->100秒後停止

unsigned long previousMillis = 0;

volatile bool isReady = false; // マスターから命令が来ていたら真になる
String get_info = ""; // 稼働時間を受け取るときは真になる
String inputBuffer = ""; // 受け取ったメッセをまとめてぶち込む
String receivedMessage = ""; //最後に受け取ったメッセージを持っておくやつ
String sendMsg = ""; // readが来たら送り返す文字を入れとくやつ
String cache = ""; // 文字数を先に伝えるから、その間は返答を持っておくやつ
String sendLength = ""; // 送り返す文字の長さを保存しとく

bool reboot_flag = false; // rebootのフラグ

int currentMillis = 0; // 定期実行の経過時間確認用


// ここから下は自由にしてくれ。
#define SLAVE_ADDRESS 0x08 // 0x08から0x77まで(わかってると思うけど。16進数だよ？)
const String job = "unit_name"; //ユニット固有の変数を宣言
const int blank_time = 10; //一秒間に何回処理を繰り返すか。※1000以下の偶数の数字にして。割り切れない。



// オリジナルの処理を追加しよう。
void addtasks(String receive) {
  if (1) {
    return;
  }

}

//マスターからの命令に対応した動作 voidじゃないとだめ。
void tasks(String receive) {
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
    reboot_flag = true;
    return;
  }

  if (receive == "led_on") {
    led = true;
    return;
  }

  if (receive == "led_off") {
    led = false;
    return;
  }

  if (receive == "settime") {
    get_info = "time";
    return;
  }
  if (get_info == "time") {
    timer_count = receive.toInt() * 1000 / blank;
    get_info = "";
    return;
  }
  addtasks(receive);

}

void reboot_def() {
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
  if (millis() - currentMillis >= blank_time) {
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
//ここから固有の関数





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

  // setupの最初でウォッチドッグを無効化（リセットループ防止）
  wdt_disable();

  // 初期状態は停止
  moterstop();
}

void loop() {
  // 命令の受け取り
  if (isReady) {
  String msg = getMessage();
  Serial.println("受信メッセージ: ");
  Serial.println(msg);
  decode_task(tasks ,msg);
  }

  // 前回の実行から指定時間が経過したかチェック
  if (blank()) {
    // ここに定期実行したい処理を書く

    l_switch();

    // 一定時間動き続ける制御の時に使う。
    if (timer()) {
}
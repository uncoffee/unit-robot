#include <Wire.h>
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
#define SLAVE_ADDRESS 0x09 // 0x08から0x77まで(わかってると思うけど。16進数だよ？)
const String job = "mist"; //ユニット固有の変数を宣言

const int blank = 10; // 一秒間に何回処理を繰り返すか。※1000以下の偶数の数字にして。割り切れない。
const int MIST_PIN = 12;
bool sprayflag = false; // ミスト制御


//マスターからの命令に対応した動作
void tasks(String receive) {
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
  if (receive == spray) {
    sprayflag = true;
  }
}

//変数をすべて初期値にする
void stop() {
  // 既定の変数
  isReady = false;
  get_info = "";
  inputBuffer = "";
  receivedMessage = "";
  sendMsg = "";
  cache = "";
  sendLength = "";
  led = false;
  time = 0;
  previousMillis = 0;
  // 追加された変数
  sprayflag = false;
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
//ここから固有の関数
void spray() {
  if (sprayflag) {
    analogWrite(MIST_PIN, HIGH); // LED消灯
  } else {
    analogWrite(MIST_PIN, LOW); // LED消灯
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
  // 命令の受け取り
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

    // 一定時間動き続ける制御の時に使う。
    if (time > 0) {
      time = time - 1; 
      spray();
    } else if (time < 1) {
      sprayflag = false
    }
  }
}
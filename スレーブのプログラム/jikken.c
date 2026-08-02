#include <Wire.h>
#include <string>

#define SLAVE_ADDRESS 0x10

String inputBuffer = "";     // 受信中の文字を溜めるバッファ
String receivedMessage = ""; // '?' の直前までをまとめた文字列
volatile bool isReady = false; // メッセージが完成したかどうかのフラグ

String send = ""; // 送信する文字
String num = ""; // 送信する文字の長さ

void setup() {
  Serial.begin(9600);
  
  // スレーブとしてI2Cバスに参加
  Wire.begin(SLAVE_ADDRESS);
  
  // イベントハンドラの設定
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
  Serial.println("I2C Slave Ready.");
}

void loop() {  
  // メッセージが完成（'?'を受信）したか確認
  if (isReady) {
    // まとめられた文字列を取得
    String msg = getMessage();
    // 取得した文字列を使った処理（例：シリアル出力）
    send = TextCheck(msg);
    // 文字数をカウント
    num = CountText(send);

    delay(100);
}

// 完成したメッセージを取得してフラグをクリアする関数
String getMessage() {
  String temp = receivedMessage;
  isReady = false; // 処理が終わったのでフラグを戻す
  return temp;    // まとめられた文字列を返す
}

String TextCheck(String text) {
    if (text == "who") {
        return "rover";
    }
}

CountText(String text) {
  std::string s = text;
  return s.length();
}

// マスターからデータを受信したときの処理（割り込み処理）
void receiveEvent(int howMany) {
  while (Wire.available()) {
    char c = Wire.read();
    
    if (c == '?') {
      // '?' が来たら、それまでに溜まった文字列を確定させる
      Serial.print("送られてきた文字")
      Serial.println(inputBuffer);

      receivedMessage = inputBuffer;
      inputBuffer = ""; // 次の受信のためにバッファをリセット
      isReady = true;   // 完成フラグを立てる
    } else {
      // '?' 以外の文字はバッファに1文字ずつ追加する
      inputBuffer += c;
    }
  }
}

// マスターからデータを要求されたときの処理
int count = 0;

void requestEvent() {
  if count == 0 {
    Wire.write(num);
    count = 1;
  } else {
    Wire.write(send); 
    count = 0;
  }
}
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
}

void loop() {  
  if (isReady) {
    String msg = getMessage();
    Serial.print("受信メッセージ: ");
    Serial.println(msg);
    TextCheck(msg);
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
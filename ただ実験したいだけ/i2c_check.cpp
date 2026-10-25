#include <Arduino.h>
#include <Wire.h>
#include <avr/wdt.h>


// 受信処理
void receiveEvent(int howMany) {
    while (Wire.available()) {
        Serial.println(Wire.read());
    }
}

// 送信処理
void requestEvent() {
    Wire.write(0x11);
}

void setup() {
    // I2Cの関数の指定
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
    Wire.begin(0x08); // 小文字の slave_address を使用
    Serial.begin(9600);
}

void loop() {}
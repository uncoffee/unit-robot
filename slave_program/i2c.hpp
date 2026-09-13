#include <Wire.h>
#ifndef MYLIB_HPP
#define MYLIB_HPP

class Default_codes() {
private:
  volatile bool isReady = false; // マスターから命令が来ていたら真になる
  String get_info = ""; // 稼働時間を受け取るときは真になる
  String inputBuffer = ""; // 受け取ったメッセをまとめてぶち込む
  String receivedMessage = ""; //最後に受け取ったメッセージを持っておくやつ
  String sendMsg = ""; // readが来たら送り返す文字を入れとくやつ
  String cache = ""; // 文字数を先に伝えるから、その間は返答を持っておくやつ
  String sendLength = ""; // 送り返す文字の長さを保存しとく

  void decode_task(Func func, String orders) {
  int order_len = orders.length();
  int count = 0;
  String order = "";
  while(order_len > count) {
    if (orders[count] == "!") {
      func(order);
    } else {
      order += orders[count];
    }
  }
}



}


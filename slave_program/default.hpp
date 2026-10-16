#ifndef DEFAULT_HPP
#define DEFAULT_HPP

#include <Arduino.h>
#include <Wire.h>
#include <avr/wdt.h>

// i2c 関連の変数
extern uint8_t fir_byte;
extern uint8_t sec_byte;
extern bool i2c_flag;
extern String result;

// 受信・送信イベントのプロトタイプ宣言
void receiveEvent(int howMany);
void requestEvent();

class tasks {
private:
    void (*func)(uint8_t);
    const char* name = "";

    static constexpr uint8_t MAX_TASKS = 10;
    static tasks* inst_list[MAX_TASKS];
    static uint8_t inst_count;

public:
    tasks(const char* name, void (*func)(uint8_t)) : name(name), func(func) {
        if (inst_count < MAX_TASKS) {
            inst_list[inst_count] = this;
            inst_count++;
        }
    }

    static void search(uint8_t order, uint8_t change) {
        if (order < inst_count && inst_list[order] != nullptr) {
            inst_list[order]->func(change);
        }
    }

    static void show() {
        Serial.println("オーダー番号|役割");
        for (uint8_t i = 0; i < inst_count; i++) {
            Serial.print(i);
            Serial.print("番\t");
            Serial.println(inst_list[i]->name);
        }
    }
};

class timer {
private:
    uint16_t goal = 0;
    uint32_t start = 0;

public:
    static uint32_t millis; // クラス内での初期化(=0)を削除

    timer(uint16_t goal) {
        this->goal = goal;
        this->start = millis;
    }

    bool check() {
        if (millis - this->start >= this->goal) {
            this->start = millis;
            return true;
        } else {
            return false;
        }
    }
    
    void restart(uint16_t goal) {
        this->goal = goal;
        this->start = millis;
    }
};

class unit {
private:
    unit() = delete;
    static const char* unit_name;          // 変数名を unit_name に統一
    static uint8_t slave_address;

public:
    static constexpr uint8_t LED_PIN = 13; // constexprに変更してクラス内初期化を許可


    static void info(const char* name, uint8_t address) {
        unit_name = name;
        slave_address = address;

    }

    static void setup() {
        Wire.onReceive(receiveEvent);
        Wire.onRequest(requestEvent);

        Wire.begin(slave_address); // 小文字の slave_address を使用
        Serial.begin(9600);
        MCUSR = 0;
        wdt_disable();
        pinMode(LED_PIN, OUTPUT);
        new tasks("rebootします", reboot);
        new tasks("13pinのライト", led);
        new tasks("自分の役職を発表", name);
    }

    static void reboot(uint8_t _ = 0) {
        wdt_enable(WDTO_15MS);
        while (1) {}
    }

    static void led(uint8_t flag) {
        digitalWrite(LED_PIN, flag);
    }

    static void name(uint8_t _ = 0) {
        result = unit_name;
    } 
};

#endif
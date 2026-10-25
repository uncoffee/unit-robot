#ifndef DEFAULT_HPP
#define DEFAULT_HPP

#include <Arduino.h>
#include <Wire.h>
#include <avr/wdt.h>



class task {
private:
    void (*func)(uint8_t);
    const char* name;
    static constexpr int8_t MAX_TASK = 10;
    static task* inst_list[MAX_TASK];
    static uint8_t _count;

public:
    task(const char* name, void (*func)(uint8_t)) : name(name), func(func) {
        if (_count < MAX_TASK) {
            inst_list[_count] = this;
            _count++;
        }
    }

    static void search(uint8_t job_num, uint8_t value) {
        if (job_num < _count && inst_list[job_num] != nullptr) {
            inst_list[job_num]->func(value);
        }
    }

    static void show() {
        Serial.println("オーダー番号|役割");
        for (uint8_t i = 0; i < _count; i++) {
            Serial.print(i);
            Serial.print("番\t");
            Serial.println(inst_list[i]->name);
        }
    }
};
uint8_t task::_count = 0;
static task* task::inst_list[task::MAX_TASK];
bool goal_flag = false;



class timer {
private:
    uint32_t blank = 0;
    uint32_t start = 0;

public:
    static uint32_t mil;

    timer(uint32_t blank=0) {
        this->blank = blank;
        this->start = mil;
    }

    bool check() {
        if (mil - this->start >= this->blank) {
            goal_flag = true;
            return true;
        } else {
            return false;
        }
    }
    
    void restart(uint32_t blank) {
        goal_flag = true;
        this->blank = blank;
        this->start = mil;
    }
};
uint32_t timer::mil = 0;



namespace i2c {
    uint8_t job_id;
    uint8_t set_value;

    uint8_t fir_byte;
    uint8_t sec_byte;
    
    bool i2c_flag;
    
    // 受信処理
    void receiveEvent(int howMany) {
        job_id = Wire.read();
        
        if (howMany > 1) {
            set_value = Wire.read();
        } else {
            set_value = 0;
        }

        while (Wire.available()) {
            char _ = Wire.read();
        }
        i2c_flag = true;
    }

    // 送信処理
    void requestEvent() {
        Wire.write(fir_byte);
        Wire.write(sec_byte);
        fir_byte = 0;
        sec_byte = 0;
    }

    void result_set(uint8_t first, uint8_t second=0) {
        fir_byte = first;
        sec_byte = second;
    }

    void run_program() {
        if(i2c_flag) {
            task::search(job_id, set_value);
            i2c_flag = false;
        }
    }
}



namespace default_program {
    uint8_t unit_name;
    uint8_t slave_address;
    
    void name(uint8_t _ = 0) {
        i2c::result_set(unit_name);
    } 
    
    void reboot(uint8_t _ = 0) {
        wdt_enable(WDTO_15MS);
        while (1) {}
    }
    
    void led(uint8_t flag) {
        digitalWrite(LED_PIN, flag);
    }

    default_program() = delete;
  
    constexpr uint8_t LED_PIN = 13;
  
    void info(uint8_t name, uint8_t address) {
        unit_name = name;
        slave_address = address;
    }
  
    void setup() {
        Wire.onReceive(i2c::receiveEvent);
        Wire.onRequest(i2c::requestEvent);
        Wire.begin(slave_address); // 小文字の slave_address を使用
        Serial.begin(9600);
        
        MCUSR = 0;
        wdt_disable();
        pinMode(LED_PIN, OUTPUT);
        new task("自分の役職を発表", name);
        new task("13pinのライト", led);
        new task("rebootします", reboot);
    }
}


#endif
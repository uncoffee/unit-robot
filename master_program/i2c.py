from smbus2 import SMBus, i2c_msg
import time

from master_program.tool_box import custom_print


class I2C_class:
    sleep = 0.1
    read_byte = 8

    def __init__(self, slave_id:int | None=None, bus_number:int=1, pri_bool:bool=False):
        self.slave_id = slave_id
        self.bus_number = bus_number
        self.pri = custom_print(pri_bool)

    def send(self, *args:int):
        for raw_text in args:
            time.sleep(I2C_class.sleep)
            try:
                with SMBus(self.bus_number) as bus:
                    # self.slave_id を使用して書き込みメッセージを作成
                    write_msg = i2c_msg.write(self.slave_id, raw_text)
                    bus.i2c_rdwr(write_msg)
                    
                self.pri.p(f"[Success] Sent to {hex(self.slave_id)}: '{raw_text}'")
            except Exception as e:
                self.pri.p(f"[Error] Failed to send to {hex(self.slave_id)}: {e}")

    def read(self):
        time.sleep(I2C_class.sleep)
        try:
            with SMBus(self.bus_number) as bus:
                # 指定バイト数（デフォルト32）だけ一気に読み込む
                read_msg = i2c_msg.read(self.slave_id, I2C_class.read_byte)
                bus.i2c_rdwr(read_msg)
                
                data_received = list(read_msg)
                
                try:
                    # 最初の \0 (ヌル文字) より前の有効な文字列だけを取り出す
                    decoded_text = bytes(data_received).decode('utf-8').split('\x00')[0]
                    return decoded_text
                except UnicodeDecodeError:
                    return data_received
                    
        except Exception as e:
            self.pri.p(f"[Error] Failed to read from {hex(self.slave_id)}: {e}")
            return None
               
    def ask(self, question: str | int) -> str | int:
        self.send(question)
        self.send("result")
        result = str(self.read()) #c++のほうが上手くいけば多分大丈夫
            
        return result

def scan_i2c_bus(bus_number=1):
    devices = []
    # I2Cバスを開く
    with SMBus(bus_number) as bus:
        # i2cdetectコマンドと同じ範囲（0x03 〜 0x77）をスキャン
        for address in range(0x03, 0x78):
            try:
                # 空のデータを書き込んで応答があるか確認
                bus.write_quick(address)
                # 応答があれば、16進数の文字列にしてリストに追加
                devices.append(f"0x{address:02x}")
            except OSError:
                # 応答がない（機器が繋がっていない）場合はエラーになるのでスルー
                pass
    return devices


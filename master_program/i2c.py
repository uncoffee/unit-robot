from smbus2 import SMBus
import time

class I2C_class:
    sleep = 0.1
    read_byte = 2
    master_id = 1

    def __init__(self, slave_id:int | None=None, print:any=print):
        self.slave_id = slave_id
        self.print = print

    def send(self, fir_byte:int, sec_byte:int=0):
        time.sleep(I2C_class.sleep)
        print(f"{fir_byte}, {sec_byte}")
        try:


            if fir_byte > 0xff and sec_byte > 0xff:
                raise ValueError("引数一つ目もしくは二つ目の値は0x00~0xff以内である必要があります")
            
            with SMBus(I2C_class.master_id) as bus:
                bus.write_byte_data(self.slave_id, fir_byte, sec_byte)
                
            self.print(f"[Success] Sent to {self.slave_id}: '{fir_byte}', '{sec_byte}'")
        except Exception as e:
            self.print(f"[Error] Failed to send to {self.slave_id}: {e}")

    def read(self) -> list:
        time.sleep(I2C_class.sleep)
        read_list = []
        try:
            with SMBus(I2C_class.master_id) as bus:
                for _ in range(I2C_class.read_byte):
                    read_msg = int(bus.read_byte(self.slave_id))
                    read_list.append(read_msg)

            return read_list
                    
        except Exception as e:
            self.print(f"[Error] Failed to read from {self.slave_id}: {e}")
            return None
               
    def ask(self, fir_byte:int, sec_byte:int=0x00) -> list:
        self.send(fir_byte, sec_byte)
        return self.read()

def scan_i2c_bus(bus_number=1):
    devices = []
    # I2Cバスを開く
    with SMBus(bus_number) as bus:
        # i2cdetectコマンドと同じ範囲（0x03 〜 0x77）をスキャン
        for address in range(0x03, 0x77):
            try:
                # 空のデータを書き込んで応答があるか確認
                bus.write_quick(address)
                # 応答があれば、16進数の文字列にしてリストに追加
                devices.append(f"0x{address:02x}")
            except OSError:
                # 応答がない（機器が繋がっていない）場合はエラーになるのでスルー
                pass
    return devices


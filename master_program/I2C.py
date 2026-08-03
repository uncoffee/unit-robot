import time
from smbus2 import SMBus, i2c_msg

class I2C_class:
    def __init__(self, slave_id, bus_number=1):
        self.slave_id = slave_id
        self.bus_number = bus_number

    def sending(self, raw_text):
        """
        インスタンスに設定された self.slave_id に対して文字列を送信する
        :param text: 送信する文字列
        """

        text = raw_text + "?" # スレイブ側で送信終了を検知するための「?」を末尾につける
        try:
            # 文字列をバイト列に変換
            data_bytes = list(text.encode('utf-8'))
            
            with SMBus(self.bus_number) as bus:
                # self.slave_id を使用して書き込みメッセージを作成
                write_msg = i2c_msg.write(self.slave_id, data_bytes)
                bus.i2c_rdwr(write_msg)
                
            print(f"[Success] Sent to {hex(self.slave_id)}: '{text}'")
        except Exception as e:
            print(f"[Error] Failed to send to {hex(self.slave_id)}: {e}")

    def reading(self, num):
        """
        インスタンスに設定された self.slave_id から指定したバイト数分のデータを受信する
        :param num: 受信するバイト数
        :return: 受信した文字列（デコード失敗時は生バイトデータ、エラー時はNone）
        """
        try:
            with SMBus(self.bus_number) as bus:
                # self.slave_id を使用して読み込みメッセージを作成
                read_msg = i2c_msg.read(self.slave_id, num)
                bus.i2c_rdwr(read_msg)
                
                # 受信データをリストに変換
                data_received = list(read_msg)
                
                # バイト列を文字列にデコードする
                try:
                    decoded_text = bytes(data_received).decode('utf-8').rstrip('\x00')
                    return decoded_text
                except UnicodeDecodeError:
                    return data_received
                    
        except Exception as e:
            print(f"[Error] Failed to read from {hex(self.slave_id)}: {e}")
            return None
        
    def ask(self, text):
        count = 0 
        result = None

        if count > 10:# 10回再送したらエラー吐く
            raise ConnectionError("送りすぎ")
        count += 1
        print(f"{text}を送ったよ")
        self.sending(text)
        time.sleep(0.1)
        self.sending("num")
        time.sleep(0.1)
        a = self.reading(1)
        print(a)
        print(type(a))
        num = int(a) #1桁指定されてから受け取る
        time.sleep(0.1)
        self.sending("result")
        time.sleep(0.1)
        result = str(self.reading(num)) #c++のほうが上手くいけば多分大丈夫
        print(result)
            
        return result
        
# --- 使い方（インスタンス化と実行）の例 ---
if __name__ == "__main__":
    # スレイブアドレス 0x08 用のインスタンスを作成
    device = I2C_class(slave_id=0x10)
    
    # 1. 送信 (sending)
    # 引数に slave_id を渡す必要がなくなり、スッキリします
    device.sending("Hello!")
    
    time.sleep(0.1)
    
    # 2. 受信 (reading)
    # こちらもバイト数を指定するだけで、設定された slave_id から読み込みます
    print("Reading from slave...")
    data = device.reading(3)
    print(f"Received: {data}")



#-------------------------------------------------------------------------------------------
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

# スキャンを実行して結果を表示
if __name__ == "__main__":
    print("I2Cスキャンを開始します...")
    connected_devices = scan_i2c_bus(1)#masterのスレーブアドレスを引数に入れる
    
    if connected_devices:
        print(f"検出されたデバイスのアドレス: {connected_devices}")
    else:
        print("デバイスは見つかりませんでした。")


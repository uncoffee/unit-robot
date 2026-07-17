from json_get import load_json_to_dict
from communication import I2CCommunicator ,scan_i2c_bus
from auto_install import Runners
import importlib as lib

def start():
    # i2c機器を探す
    connected_devices = scan_i2c_bus(1)
    if connected_devices == None:
        raise ConnectionError(f"スレーブが見つかりませんでした")

    # I2CのJSONファイルを読み込む
    json_file_path = "units.json"

    result_dict = load_json_to_dict(json_file_path)
    if result_dict == None:
        raise FileNotFoundError(f"{json_file_path}ファイルが見つかりません")
        
    # ライブラリをダウンロード
    lib_dict = {}

    for devices in connected_devices:
        print(f"スレーブアドレス:{devices} が見つかりました")
        devices_name = result_dict.get(devices)
        if devices_name == None:
            raise ModuleNotFoundError(f"スレーブアドレス:{devices} に対応した宛先が見つかりませんでした")
        
        lib_dict[devices_name] = {"lib":Runners(devices_name),"id":devices}

    return lib_dict

if __name__ == "__main__":
    start()
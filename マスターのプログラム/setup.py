from マスターのプログラム.json_to_dict import load_json
from マスターのプログラム.I2C import I2C_class ,scan_i2c_bus

import importlib
import importlib

def create_instance(module_name, class_name, *args, **kwargs):
    try:
        # 1. 文字列からモジュールを動的にインポート
        module = importlib.import_module(module_name)
        
        # 2. モジュールから「文字列の指定に一致するクラス」を取得
        TargetClass = getattr(module, class_name)
        
        # 3. 取得したクラスをインスタンス化して返す
        # (*args, **kwargs を渡すことで、引数があるコンストラクタにも対応)
        instance = TargetClass(*args, **kwargs)
        return instance

    except ImportError:
        print(f"エラー: モジュール '{module_name}' が見つかりません。")
    except AttributeError:
        print(f"エラー: クラス '{class_name}' がモジュール内に見つかりません。")
    return None
"""
# --- 利用例 ---
# 'my_library' モジュールの 'MyClass' を指定し、引数 "田中" を渡してインスタンス化
obj = create_instance("my_library", "MyClass", "田中")

if obj:
    # 呼び出しテスト
    print(obj.greet())  # 出力: こんにちは、田中さん！
"""

master_add = 0x01

def start():
    # i2c機器を探す
    slave_adds = scan_i2c_bus(master_add)
    print(f"スレーブID{slave_adds}が見つかりました")

    for slave_add in slave_adds:
        # i2c通信用のクラスからインスタンスを作成
        i2c_inst = I2C_class(int(slave_add, 16))
        slave_name = i2c_inst.ask("who?",3)# そのスレーブが何なのか確認する

    # if slave_adds == None:
    #     raise ConnectionError(f"スレーブが見つかりませんでした")

    # else:
    #     print(f"スレーブアドレス:{slave_adds} が見つかりました")


    # # i2c通信用のクラスからインスタンスを作成
    # i2c_com = I2C_class(slave_adds)

    # # I2CのJSONファイルを読み込む
    # json_file_path = "/home/souta/unit-robot/units.json"

    # result_dict = load_json(json_file_path)

    # # mastar側の命令クラスからインスタンスを作成

        
    # # ライブラリをダウンロード
    # lib_dict = {}

    # for slave_add in slave_adds:
    #     print(f"スレーブアドレス:{slave_add} が見つかりました")
    #     info = result_dict.get(slave_add)
    #     name = info.get("name")
    #     path = info.get("path")
    #     if info == None:
    #         raise ModuleNotFoundError(f"名前:{name}/スレーブアドレス{slave_add} に対応した宛先が見つかりませんでした\n")
        
    #     lib_dict[name] = {"slave_instance":create_instance(path),"id":slave_add}

    # return lib_dict

if __name__ == "__main__":
    result = start()
    print(result)
from json_to_dict import load_json
from I2C import I2C_class ,scan_i2c_bus
import importlib

def create_instance(class_name, *args, **kwargs):
    try:
        # 1. 文字列からモジュールを動的にインポート
        module = importlib.import_module("units")
        
        # 2. モジュールから「文字列の指定に一致するクラス」を取得
        TargetClass = getattr(module, class_name)
        
        # 3. 取得したクラスをインスタンス化して返す
        # (*args, **kwargs を渡すことで、引数があるコンストラクタにも対応)
        instance = TargetClass(*args, **kwargs)
        return instance
    
    except AttributeError:
        print(f"エラー: クラス '{class_name}' がモジュール内に見つかりません。")
    return None
"""
# --- 利用例 ---
# 'my_library' モジュールの 'MyClass' を指定し、引数 "banana" を渡してインスタンス化
obj = create_instance("my_library", "MyClass", "banana")

if obj:
    # 呼び出しテスト
    print(obj.greet())  # 出力: こんにちは、bananaさん！
"""

master_add = 0x01

def start():
    # スレーブの対応表(json)からdictに変換
    json_file_path = "/home/souta/unit-robot/units.json"
    result_dict = load_json(json_file_path)

    # i2c機器を探す
    slave_adds = scan_i2c_bus(master_add)
    print(f"スレーブID{slave_adds}が見つかりました")

    UnitsDict = {}

    for slave_add in slave_adds:
        # i2c通信用のクラスからインスタンスを作成
        i2c_inst = I2C_class(int(slave_add, 16))
        slave_name = i2c_inst.ask("who",2)# そのスレーブが何なのか確認する
        print(f"{slave_name}が接続されていることを確認しました")
        UnitsDict[slave_name] = create_instance(slave_name,i2c_inst)

    return UnitsDict

if __name__ == "__main__":
    result = start()
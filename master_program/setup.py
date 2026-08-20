from I2C import I2C_class ,scan_i2c_bus
import importlib
import time

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

def start():
    # i2c機器を探す
    slave_adds = scan_i2c_bus(master_add)
    print(f"スレーブID{slave_adds}が見つかりました")

    UnitsDict = {}

    for slave_add in slave_adds:
        # i2c通信用のクラスからインスタンスを作成
        i2c_inst = I2C_class(int(slave_add,16))
        slave_name = i2c_inst.ask("who")# そのスレーブが何なのか確認する
        print(f"{slave_name}が接続されていることを確認しました")
        UnitsDict[slave_name] = create_instance(slave_name,i2c_inst)


    return UnitsDict

master_add = 0x01
if __name__ == "__main__":
    units = start()
    print(result)
    units["rover"].go()
    time.sleep(5)
    units["rover"].back()   
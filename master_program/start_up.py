from i2c import I2C_class, scan_i2c_bus
from tool_box import create_instance, custom_print, id_to_name
from units import UnitsDict



class setup:
    def __init__(self, master_add:int=1, test:bool=False):
        self.master_add = master_add
        self.print = custom_print(test).p

    def setup(self) -> dict:
        #ファイルの場所を設定。
        slave_ins_file = "units"

        # i2c機器を探す
        slave_adds = scan_i2c_bus(self.master_add)
        self.print(f"スレーブID{slave_adds}が見つかりました")

        UniDic = UnitsDict()

        for slave_add in slave_adds:
            # i2c通信用のクラスからインスタンスを作成
            i2c_inst = I2C_class(int(slave_add,16), self.print)

            # そのスレーブが何なのか確認する
            slave_id = i2c_inst.ask(0)
            print(slave_id)
            try:
                slave_name = id_to_name(slave_id[0])
                self.print(f"{slave_name}が接続されていることを確認しました")
                inst = create_instance(slave_ins_file,slave_name,i2c_inst)

            except:
                continue

            UniDic[slave_name] = inst

        return UniDic
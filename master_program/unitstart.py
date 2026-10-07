from master_program.i2c import I2C_class, scan_i2c_bus
from master_program.tool_box import create_instance, custom_print
from master_program.units import UnitsDict


class setup:
    def setup(master_add:int=1, cus_pri:bool=False) -> dict:
        pri = custom_print(cus_pri)
        #ファイルの場所を設定。
        slave_ins_file = "master_program.units"

        # i2c機器を探す
        slave_adds = scan_i2c_bus(master_add)
        pri.p(f"スレーブID{slave_adds}が見つかりました")

        UniDic = UnitsDict()

        for slave_add in slave_adds:
            # i2c通信用のクラスからインスタンスを作成
            i2c_inst = I2C_class(int(slave_add,16), 1, cus_pri)
            slave_name = i2c_inst.ask("who")# そのスレーブが何なのか確認する
            pri.p(f"{slave_name}が接続されていることを確認しました")
            UniDic[slave_name] = create_instance(slave_ins_file,slave_name,i2c_inst)

        return UniDic
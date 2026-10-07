import importlib

def create_instance(file_name:str, object_name:str, *args:any):
    try:
        # 1. 文字列からモジュールを動的にインポート
        module = importlib.import_module(file_name)
        
        # 2. モジュールから「文字列の指定に一致するクラス」を取得
        TargetClass = getattr(module, object_name)
        
        # 3. 取得したクラスをインスタンス化して返す
        # (*args, **kwargs を渡すことで、引数があるコンストラクタにも対応)
        print(*args)
        instance = TargetClass(*args)
        return instance
    
    except AttributeError:
        print(f"エラー: クラス '{object_name}' がモジュール内に見つかりません。")

"""
これを使いまわして増やしてくれ。

class UNIT_NAME(units):
    def __init__(self,slave_instance):
        super().__init__(slave_instance)

"""

class units:
    def __init__(self, slave_instance):
        self.ins = slave_instance #communication.pyのI2CCommunicatorのインスタンスを受け取る(通信用)

    def stop(self) -> None:
        self.ins.send("stop") #強制停止

    def led(self,switch:bool) -> None:
        if (switch):
            self.ins.send("led_on")
        else:
            self.ins.send("led_off")

    def settime(self,time:int) -> None:
        self.ins.send("time",time)

class rover(units):
    DIRECTION_GO  = {"right": "rf", "left": "lf"}
    DIRECTION_BACK = {"right": "rb", "left": "lb"}
    DIRECTION_RIGHT = {"right": "rb", "left": "lf"}
    DIRECTION_LEFT = {"right": "rf", "left": "lb"}

    def __init__(self,slave_instance):
        super().__init__(slave_instance)
        self.status:dict[str, str] = {} #一応unit側は両方前入力がデフォルトだけど、変更に備えて最初は定義しない。

    def _MotorOn(self,DIRECTION:dict[str, str],time:int) -> None:
        change = []
        for key in DIRECTION.keys():
            if self.status.get(key) != DIRECTION[key]:
                self.status[key] = DIRECTION[key]
                change.append(DIRECTION[key])

        self.ins.send(*change,"time",time)

    def go(self,time:int) -> None:
        self._MotorOn(self.DIRECTION_GO,time)

    def back(self,time:int) -> None:
        self._MotorOn(self.DIRECTION_BACK,time)

    def right(self,time:int) -> None:
        self._MotorOn(self.DIRECTION_RIGHT,time)

    def left(self,time:int) -> None:
        self._MotorOn(self.DIRECTION_LEFT,time)

    def set_speed(self,speed:int) -> None:
        self.ins.send("setspeed",speed)

    def how_speed(self) -> int:
        return int(self.ins.ask("howspeed"))

class mist(units):
    def __init__(self,slave_instance):
        super().__init__(slave_instance)

    def spray(self,time:int) -> None:
        self.ins.send("time",time)

class sensor(units):
    def __init__(self,slave_instance):
        super().__init__(slave_instance)
    
    def howtemp() -> float:
        return float(self.ins.ask("howtemp"))

    def howpres() -> float:
        return float(self.ins.ask("howpres"))


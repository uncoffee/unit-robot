class UnitsDict(dict):
    # すべての要素に関数を適用する共通メソッド geminiすげえ。俺はラムダ式を使えない。ムズイ
    def apply_all(self, action) -> None:
        for unit in self.values():
            action(unit)

    def reboot(self) -> None:
        self.apply_all(lambda unit: unit.reboot())

    def stop(self) -> None:
        self.apply_all(lambda unit: unit.st(0))

    def led(self,switch:bool):
        self.apply_all(lambda unit: unit.led(switch))

"""
これを使いまわして増やしてくれ。

class UNIT_NAME(units):
    def __init__(self,slave_instance):
        super().__init__(slave_instance)

"""
class units:
    def __init__(self, slave_instance):
        self.ins = slave_instance #communication.pyのI2CCommunicatorのインスタンスを受け取る(通信用)

    def reboot(self) -> None:
        self.ins.send("reboot") #強制停止

    def led(self,switch:bool) -> None:
        if (switch):
            self.ins.send("led_on")
        else:
            self.ins.send("led_off")

    def stop(self):
        self.st(0)

    def st(self, time:int) -> None:
        self.ins.send("settime",time)

class rover(units):
    DIRECTION_GO  = {"right": "rf", "left": "lf"}
    DIRECTION_BACK = {"right": "rb", "left": "lb"}
    DIRECTION_RIGHT = {"right": "rb", "left": "lf"}
    DIRECTION_LEFT = {"right": "rf", "left": "lb"}

    def __init__(self,slave_instance):
        super().__init__(slave_instance)
        self.status:dict[str, str] = {} #一応unit側は両方前入力がデフォルトだけど、変更に備えて最初は定義しない。

    def _MotorOn(self,DIRECTION:dict[str, str],time:int) -> None:
        # geminiすげえ。俺はヘルパー関数の存在を知らんかった。
        change = []
        for key in DIRECTION.keys():
            if self.status.get(key) != DIRECTION[key]:
                self.status[key] = DIRECTION[key]
                change.append(DIRECTION[key])

        self.ins.send(*change)
        self.st(time)

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
        self.st(time)

class sensor(units):
    def __init__(self,slave_instance):
        super().__init__(slave_instance)
    
    def howtemp() -> float:
        return float(self.ins.ask("howtemp"))

    def howpres() -> float:
        return float(self.ins.ask("howpres"))

class tracking_sys:
    def __init__(self, rover_ins):
        self.rover_ins = rover_ins

    def tracking(self, tracking_info:dict[str, int | list[int]])):
        pass
        
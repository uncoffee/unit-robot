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
        self.result_s:dict[str,str] = {} #一応unit側は両方前入力がデフォルトだけど、変更に備えて最初は定義しない。

    def _MotorOn(self,DIRECTION:dict[str:str],time:int) -> None:
        change = []
        for key in DIRECTION.keys():
            if self.status[key] != self.result_s[key]:
                self.result_s[key] = self.status[key]
                change.append(self.status[key])

        self.ins.send(*change,"time",time,"run")

    def go(self,time:int) -> None:
        self._MotorOn(DIRECTION_GO,time)

    def back(self,time:int) -> None:
        self._MotorOn(DIRECTION_BACK,time)

    def right(self,time:int) -> None:
        self._MotorOn(DIRECTION_RIGHT,time)

    def left(self,time:int) -> None:
        self._MotorOn(DIRECTION_LEFT,time)

    def setspeed(self,speed:int) -> None:
        self.ins.send("speed",speed)

    def howspeed(self) -> int:
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


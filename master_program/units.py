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
    def __init__(self,slave_instance):
        super().__init__(slave_instance)
        self.result_s:dict[str,str] = {} #一応unit側は両方前入力がデフォルトだけど、変更に備えて最初は定義しない。

    def _MotorOn(self,DIRECTION:dict[str:str],time:int) -> None:
        change = []
        for key in DIRECTION.keys():
            if self.status[key] != self.result_s[key]:
                self.result_s[key] = self.status[key]
                change.append(self.status[key])

        self.ins.send(*change,"time",time,"motor")

    def go(self,time:int) -> None:
        DIRECTION_GO  = {"right": "rf", "left": "lf"}
        self._MotorOn(DIRECTION_GO,time)

    def back(self,time:int) -> None:
        DIRECTION_BACK = {"right": "rb", "left": "lb"}
        self._MotorOn(DIRECTION_BACK,time)

    def right(self,time:int) -> None:
        DIRECTION_RIGHT = {"right": "rb", "left": "lf"}
        self._MotorOn(DIRECTION_RIGHT,time)

    def left(self,time:int) -> None:
        DIRECTION_LEFT = {"right": "rf", "left": "lb"}
        self._MotorOn(DIRECTION_LEFT,time)

    def setspeed(self,speed:int) -> None:
        self.ins.send("speed",speed)

    def getspeed(self) -> int:
        return int(self.ins.ask("whatspeed"))

class mist(units):
    def __init__(self,slave_instance):
        super().__init__(slave_instance)

    def spray(self,time:int) -> None:
        self.ins.send("spray","time",time)

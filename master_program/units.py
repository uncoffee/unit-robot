class units:
    def __init__(self, slave_instance):
        self.ins = slave_instance #communication.pyのI2CCommunicatorのインスタンスを受け取る(通信用)
        self.cache = dict()

    def stop(self) -> None:
        self.ins.send("stop") #強制停止

class rover(units):
    def __init__(self,slave_instance):
        super().__init__(slave_instance)

    def led(self,switch:bool)  -> None:
        if (switch):
            self.ins.send("led_on")
        else:
            self.ins.send("led_off")

    def go(self,time:int) -> None:
        self.ins.send("rf","lf","time",time)

    def back(self,time:int) -> None:
        self.ins.send("rb","lb","time",time)

    def right(self,time:int) -> None:
        self.ins.send("rb","lf","time",time)

    def left(self,time:int) -> None:
        self.ins.send("rf","lb","time",time)

    def set_speed(self,speed:int) -> None:
        self.ins.send("speed",speed)

    def get_speed(self) -> int:
        return int(self.ins.ask("whatspeed"))
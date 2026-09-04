class units:
    def __init__(self, slave_instance):
        self.ins = slave_instance #communication.pyのI2CCommunicatorのインスタンスを受け取る(通信用)
        self.cache = dict()

    def stop(self) -> None:
        self.ins.sending("stop") #強制停止

class rover(units):
    def __init__(self,slave_instance):
        super().__init__(slave_instance)

    def led(self,switch:bool)  -> None:
        if (switch):
            self.ins.sending("led_on")
        else:
            self.ins.sending("led_off")

    def go(self,time:int) -> None:
        self.ins.sending("rf","lf","time",time)

    def back(self,time:int) -> None:
        self.ins.sending("rb","lb","time",time)

    def right(self,time:int) -> None:
        self.ins.sending("rb","lf","time",time)

    def left(self,time:int) -> None:
        self.ins.sending("rf","lb","time",time)
        
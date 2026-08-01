class moter:
    def __init__(self, slave_instance):
        self.sins = slave_instance #communication.pyのI2CCommunicatorのインスタンスを受け取る(通信用)

    def stop(self):
        self.sins.sending("stop") #I2CCommunicatorのsendingメソッドを呼び出す

class rover(moter):
    pass
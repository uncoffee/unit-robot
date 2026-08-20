class moter:
    def __init__(self, slave_instance):
        self.sins = slave_instance #communication.pyのI2CCommunicatorのインスタンスを受け取る(通信用)

    def stop(self):
        self.sins.sending("stop") #units[class moterを継承したclass名].stop()で作動。モーターが停止する関数。

class rover(moter):
    def on(self):
        self.sins.sending("on") #units["rover"].onで作動。ユニット制御のマイコンのランプが光る関数。主に通信確認やプログラムの動作確認に使う想定。

    def off(self):
        self.sins.sending("off") #units["rover"].offで作動。ユニット制御のマイコンのランプが消える関数。主に通信確認やプログラムの動作確認に使う想定。

    def go(self):
        self.sins.sending("go") #units["rover"].goで作動。ローバー(機体)が前に進む関数。現在地から前に進みたいときに使用する。

    def back(self):
        self.sins.sending("back")#units["rover"].backで作動。ローバー(機体)が後ろに戻る関数。現在地から後ろに戻りたいときに使用する。

    """
    rover関数の使用例：

    #動作確認
    units["rover"].on
    time.sleep(1)
    units["rover"].off
    units["rover"].go
    units["rover"].back
    
    """
        
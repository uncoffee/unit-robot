class units:
    def __init__(self, slave_instance):
        self.sins = slave_instance #communication.pyのI2CCommunicatorのインスタンスを受け取る(通信用)
        self.cache = dict()

    def stop(self) -> None:
        self.sins.sending("stop") #強制停止

    def pause(self) -> dict:
        self.cache

class rover(moter):
    def __init__(self):
        self.CM_S = 40 #仮設定　みつぎ　早くやれ。

    def on(self)  -> None:
        self.sins.sending("on") #units["rover"].onで作動。ユニット制御のマイコンのランプが光る関数。主に通信確認やプログラムの動作確認に使う想定。

    def off(self)  -> None:
        self.sins.sending("off") #units["rover"].offで作動。ユニット制御のマイコンのランプが消える関数。主に通信確認やプログラムの動作確認に使う想定。

    def go(self,cm:int) -> None:
        time = cm / self.SpinTime
        self.sins.sending("go",time) #units["rover"].goで作動。ローバー(機体)が前に進む関数。現在地から前に進みたいときに使用する。

    def back(self,cm:int) -> None:
        time = cm / self.SpinTime
        self.sins.sending("back",time)#units["rover"].backで作動。ローバー(機体)が後ろに戻る関数。現在地から後ろに戻りたいときに使用する。

    """
    rover関数の使用例：

    #動作確認
    units["rover"].on
    time.sleep(1)
    units["rover"].off
    units["rover"].go
    units["rover"].back
    
    """
        
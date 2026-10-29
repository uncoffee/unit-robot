class UnitsDict(dict):
    def apply_all(self, action) -> None:
        for unit in self.values():
            action(unit)

    def reboot(self) -> None:
        self.apply_all(lambda unit: unit.reboot())

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
        self.ins = slave_instance

    def reboot(self) -> None:
        self.ins.send(2) #強制停止

    def led(self,switch:bool) -> None:
        if (switch):
            self.ins.send(1,1)
        else:
            self.ins.send(1,0)

class rover(units):
    DIRECTION_GO  = 3 # 0b11
    DIRECTION_BACK = 0 # 0b00

    # ここちょっとわかりずらい。　1:前進 0:後退　前進するタイヤの反対側に進むから、直感と違う感じになってる。
    DIRECTION_RIGHT = 2 # 0b01
    DIRECTION_LEFT = 1 # 0b10

    # 命令の番号
    SET_TIME = 3
    CHANGE_DIRECTION = 4 
    SET_SPEED = 5
    HOW_SPEED = 6
    

    def __init__(self,slave_instance):
        super().__init__(slave_instance)

    def go(self,time:int) -> None:
        self.ins.send(rover.CHANGE_DIRECTION, rover.DIRECTION_GO)
        self.ins.send(rover.SET_TIME, time)

    def back(self,time:int) -> None:
        self.ins.send(rover.CHANGE_DIRECTION, rover.DIRECTION_BACK)
        self.ins.send(rover.SET_TIME, time)

    def right(self,time:int) -> None:
        self.ins.send(rover.CHANGE_DIRECTION, rover.DIRECTION_RIGHT)
        self.ins.send(rover.SET_TIME, time)

    def left(self,time:int) -> None:
        self.ins.send(rover.CHANGE_DIRECTION, rover.DIRECTION_LEFT)
        self.ins.send(rover.SET_TIME, time)

    def set_speed(self,speed:int) -> None:
        self.ins.send(rover.SET_SPEED,speed)

    def how_speed(self) -> int:
        return self.ins.ask(rover.HOW_SPEED)

    def stop(self) -> None:
        self.ins.send(rover.SET_TIME, 0)

class mist(units):
    SET_TIME = 3

    def __init__(self,slave_instance):
        super().__init__(slave_instance)

    def spray(self,time:int) -> None:
        self.ins.send(mist.SET_TIME, time)

    def stop(self) -> None:
        self.ins.send(rover.SET_TIME, 0)
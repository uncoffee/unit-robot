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
    DIRECTION_GO  = {"right": "rf", "left": "lf"}
    DIRECTION_BACK = {"right": "rb", "left": "lb"}
    DIRECTION_RIGHT = {"right": "rb", "left": "lf"}
    DIRECTION_LEFT = {"right": "rf", "left": "lb"}

    def __init__(self,slave_instance):
        super().__init__(slave_instance)
        self.status:dict[str, str] = {}

    def _MotorOn(self,DIRECTION:dict[str, str],time:int) -> None:
        change = []
        for key in DIRECTION.keys():
            if self.status.get(key) != DIRECTION[key]:
                self.status[key] = DIRECTION[key]
                change.append(DIRECTION[key])

        self.ins.send(*change)
        self.st(time)

    def go(self,time:int) -> None:
        self._MotorOn(rover.DIRECTION_GO,time)

    def back(self,time:int) -> None:
        self._MotorOn(rover.DIRECTION_BACK,time)

    def right(self,time:int) -> None:
        self._MotorOn(rover.DIRECTION_RIGHT,time)

    def left(self,time:int) -> None:
        self._MotorOn(rover.DIRECTION_LEFT,time)

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
    
    def howtemp(self) -> float:
        return float(self.ins.ask("howtemp"))

    def howpres(self) -> float:
        return float(self.ins.ask("howpres"))

        
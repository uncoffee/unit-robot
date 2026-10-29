from camera import YOLOCamera
from start_up import setup
import time

class banana:
    def __init__(self, units):
        self.units = units

    def __enter__(self, *args):
        pass

    def __exit__(self, *args):
        self.units.reboot()

inst = YOLOCamera(0.70)


@inst.add_func
def run(serch_person,unit_dict):
    unit_dict.led(True)
    time.sleep(5)
    unit_dict.led(False)
    time.sleep(1)
    unit_dict["rover"].set_speed(50) #max 255
    unit_dict["rover"].left(60)
    while True:
        result = serch_person()

        if not result == None:
            print(result)
            ing_info = result["shape"]
            persons_dict = result["persons"]

            ing_x = ing_info[0]
            ing_center = ing_x // 2

            hantei = persons_dict[0][0] - ing_center

            if -50 < hantei < 50:
                unit_dict["rover"].stop()
                unit_dict["mist"].spray(15)
                time.sleep(15)
                unit_dict.reboot()
                break

        time.sleep(1)


setup_inst = setup()
units_dict = setup_inst.setup()
with_you = banana(units_dict)

with with_you:
    run(units_dict)
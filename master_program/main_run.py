from camera import YOLOCamera
from kansoku import kansoku
from start_up import setup
import time

camera_inst = YOLOCamera()
kansoku_inst = kansoku()

def run(func_dict,unit_dict):
    serch_person = func_dict["camera"]
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


@kansoku_inst.add_func
@camera_inst.add_func
def check(scan,kansoku):
    print(scan())
    print(kansoku())

time.sleep(5)
check()

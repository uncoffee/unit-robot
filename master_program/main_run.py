from camera import YOLOCamera
from master_program.start_up import setup
import time
inst = YOLOCamera()

@inst.add_func
def run(serch_person,unit_dict):
    unit_dict.led(True)
    time.sleep(5)
    unit_dict.led(False)
    time.sleep(1)
    unit_dict["rover"].set_speed(100) #max 255
    unit_dict["rover"].left(60)
    while True:
        result = serch_person()

        if not result == None:
            ing_info = result["shape"]
            persons_dict = result["persons"]

            ing_x = ing_info[0]
            ing_center = ing_x // 2

            hantei = persons_dict[0][0] - ing_center

            if -25 < hantei < 25:
                unit_dict["rover"].stop()
                unit_dict["mist"].spray(5)
                time.sleep(5)
                unit_dict.reboot()
                break

        time.sleep(1)




run(setup.setup())
        
from camera import YOLOCamera
import time
inst = YOLOCamera()

@inst.add_func
def run(serch_person):
    while True:
        a = serch_person()
        print(type(a))
        print(a)
        time.sleep(1)

run()
        
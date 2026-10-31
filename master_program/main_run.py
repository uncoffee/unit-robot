from camera import YOLOCamera
from kansoku import kansoku
from start_up import setup
import time
import json

data = {
    "name": "rover",
    "id": 1,
    "skills": ["Python", "Docker"],
    "is_active": True,
}

# 日本語文字化け防止のため ensure_ascii=False、整形表示のため indent=4 を指定
with open("data.json", "w", encoding="utf-8") as f:
    json.dump(data, f, ensure_ascii=False, indent=4)

camera_inst = YOLOCamera()
kansoku_inst = kansoku()

def serch_and_destroy(serch_person,unit_dict):
    unit_dict["rover"].set_speed(50) #max 255
    unit_dict["rover"].left(30)
    while True:
        result = serch_person()
        persons = result["persons"]

        if not persons == None:
            print(result)

            if 290 < persons[0][0] < 330 and 0.8 < persons[0][3] < 1.3:
                unit_dict["rover"].stop()
                unit_dict["mist"].spray(15)
                time.sleep(15)
                break

        time.sleep(0.5)


@kansoku_inst.add_func
@camera_inst.add_func
def check(scan,kansoku):
    while True:
        print(scan())
        print(kansoku())

time.sleep(5)
check()

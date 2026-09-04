from master_program.unitstart import setup
import time

#テストコード
if __name__ == "__main__":
    master_add = 0x01
    units = setup.setup(master_add)

    print(units)
    print(type(units["rover"]))
    units["rover"].led(True)
    time.sleep(3)
    units["rover"].led(False)
    time.sleep(3)
    units["rover"].go(5)
    time.sleep(8)
    units["rover"].back(5)
    time.sleep(8)
    units["rover"].right(5)
    time.sleep(8)
    units["rover"].left(5)
    time.sleep(8)
    units["rover"].set_speed(50)
    time.sleep(3)
    print(units["rover"].get_speed())
    time.sleep(3)
    units.stop()
    print(units["rover"].get_speed())
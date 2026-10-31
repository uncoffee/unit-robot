from start_up import setup
import time

def global_test():
    units.led(True)
    time.sleep(3)
    units.led(False)
    time.sleep(3)
    units.led(True)
    units.reboot()
    time.sleep(5)

def rover_test():
    print(units)
    print(type(units["rover"]))
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
    units["rover"].go(5)
    time.sleep(2)
    units["rover"].stop()
    units["rover"].back(2)
    print(units["rover"].how_speed())
    units["rover"].reboot()
    time.sleep(5)
    print(units["rover"].how_speed())

def mist_test():
    units.led(True)
    time.sleep(2)
    units["mist"].spray(10)
    time.sleep(10)
    units.led(False)
    units["mist"].stop()

#テストコード
if __name__ == "__main__":
    master_add = 0x01
    test_mode = True
    setuper = setup(master_add, test_mode)
    units = setuper.setup()
    mist_test()
    


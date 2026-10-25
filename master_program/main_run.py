from master_program.start_up import setup
from master_program.start_up import I2C_class
from slave_program.camera import YOLOCamera
import time

inst = YOLOCamera()

@inst.add_func
def run(serch_human):
    master_add = 0x01
    test_mode = True
    setuper = setup(master_add, test_mode)
    units = setuper.setup()
    print(serch_human())
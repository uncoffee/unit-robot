from smbus2 import SMBus, i2c_msg

with SMBus(1) as bus:
    bus.write_byte_data(0x08, 0x01, 0x42)
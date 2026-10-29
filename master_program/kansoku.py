from smbus2 import SMBus
import bme280

class sensor:
    slave_add = 0x76
    def __init__(self):
        with SMBus(1) as bus:
            self.calibration_params = bme280.load_calibration_params(bus, sensor.slave_add)
    
    def kansoku(self) -> dict:
        with SMBus(1) as bus:
            data = bme280.sample(bus, sensor.slave_add, self.calibration_params)
        return {"temp":data.temperature, "hum":data.humidity, "pres":data.pressure}
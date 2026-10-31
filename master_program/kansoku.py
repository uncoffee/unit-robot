from bmp280 import BMP280
from smbus2 import SMBus

class kansoku:
    def getdata(self):
        temp = self.bmp280.get_temperature()
        pres = self.bmp280.get_pressure()
        return {"temp":temp, "pres":pres}

    def add_func(self, func):
        def wrapper(*args, **keywords):
            with SMBus(1) as bus:
                self.bmp280 = BMP280(i2c_dev=bus, i2c_addr=0x76)
                result = func(self.getdata, *args, **keywords)
            return result
        return wrapper                
                




a = kansoku()

@a.add_func
def check(func):
    print(func())


if __name__ == "__main__":
    check()
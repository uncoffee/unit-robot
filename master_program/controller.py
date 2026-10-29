import keyboard

class push_key:
    def __init__(self, key_name):
        self.key_name = key_name
        self.push_flag = False

    def check(self):
        if keyboard.is_pressed(self.key_name):
            self.push_flag = True

        else:
            self.push_flag = False

        return self.push_flag
        
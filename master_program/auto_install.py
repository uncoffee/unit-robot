import importlib

class Runners:
    def __init__(self, file_name):
        # ファイル（モジュール）を記憶する
        self.module = importlib.import_module(file_name)
        
    def run(self, function_name):
        # 【修正】文字列から実際の関数を取り出す
        func = getattr(self.module, function_name)
        # 実行する
        func()

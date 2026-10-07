import importlib

class custom_print:
    def __init__(self, pri_bool:bool=False):
        self.pri_bool = pri_bool

    def p(self, text:str):
        if self.pri_bool:
            print(text)

def create_instance(file_name:str, object_name:str, *args:any):
    try:
        # 1. 文字列からモジュールを動的にインポート
        module = importlib.import_module(file_name)
        
        # 2. モジュールから「文字列の指定に一致するクラス」を取得
        TargetClass = getattr(module, object_name)
        
        # 3. 取得したクラスをインスタンス化して返す
        # (*args, **kwargs を渡すことで、引数があるコンストラクタにも対応)
        print(*args)
        instance = TargetClass(*args)
        return instance
    
    except AttributeError:
        print(f"エラー: クラス '{object_name}' がモジュール内に見つかりません。")
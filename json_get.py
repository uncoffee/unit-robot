import json

def load_json_to_dict(file_path):
    """
    指定されたパスのJSONファイルを読み込み、辞書型として返す関数
    :param file_path: JSONファイルのパス (例: 'data.json')
    :return: 変換された辞書データ（エラー時はNone）
    """
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            # json.load() でファイルの中身を辞書（dict）に変換
            data = json.load(f)
            return data
            
    except FileNotFoundError:
        print(f"[Error] ファイルが見つかりません: {file_path}")
        return None
    except json.JSONDecodeError as e:
        print(f"[Error] JSONのパース（解析）に失敗しました: {e}")
        return None
        
import cv2
from ultralytics import YOLO

class YOLOCamera:
    def __init__(self, threshold:float=0.80, model_path:str="yolov8n.pt", camera_id:int=0): #open-cvでやってみたけど微妙だった。
        print(f"YOLOモデル ({model_path}) をロード中...")
        self.model = YOLO(model_path)
        self.threshold = threshold
        self.camera_id = camera_id
        
    def __enter__(self):
        self.cap = cv2.VideoCapture(self.camera_id)
        if not self.cap.isOpened():
            raise RuntimeError("カメラを開けませんでした。")

    def __exit__(self, *args):
        if self.cap is not None: #isの使い方覚えようね。
            self.cap.release()

    def search_human(self) -> dict: # "shape":画像のXYサイズ,"persons"X座標が昇順になったXY座標のリスト。
        ret, frame = self.cap.read()
        if not ret:
            print("フレームの取得に失敗しました。")
            return {}

        # classes=[0] がperson
        result = self.model.predict(frame, classes=[0], conf=self.threshold)[0] 
        boxes = result.boxes

        # Tensor型らしい。なにそれ。
        xywh_list = boxes.xywh.cpu().numpy().tolist()
        centers = [[box[0], box[1]] for box in xywh_list] # 検出したboxの中心座標のみ抽出

        # 左から右の順番で人の座標を返す。
        sorted_list = sorted(centers, key=lambda box: box[0]) 
        return {"shape":result.orig_shape, "persons":sorted_list}

    def add_func(self, func):
        def wrapper(*args, **keywords):
            with self:
                result = func(self.search_human, *args, **keywords) # resultいりますかね？行動ログとか作るんかね？俺は嫌だよ。

            return result
        return wrapper

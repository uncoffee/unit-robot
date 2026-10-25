import cv2
from ultralytics import YOLO

class YOLOCamera:
    def __init__(self, camera_id:int=0, threshold:float=0.75, model_path:str="yolov8n.pt"): #open-cvでやってみたけど微妙だった。
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

    def search_human(self) -> list[list]: # X座標でソートされたX,Y座標のネスト。
        ret, frame = self.cap.read()
        if not ret:
            print("フレームの取得に失敗しました。")
            return []
            
        results = self.model.predict(frame, classes=[0], conf=self.threshold) # classes=[0] がperson
        return results[0].boxes.xyxy

    def add_func(self, func):
        def wrapper(*args, **keywords):
            with self:
                result = func(self.search_human, *args, **keywords) # resultいりますかね？行動ログとか作るんかね？俺は嫌だよ。

            return result
        return wrapper
            

if __name__ == "__main__":
    try:
        camera_app = YOLOCamera(camera_id=0, model_path="yolov8n.pt")
        camera_app.run()
    except Exception as e:
        print(f"エラーが発生しました: {e}")

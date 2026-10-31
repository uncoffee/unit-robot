import cv2
from ultralytics import YOLO

# YOLOv8nモデルの読み込み
model = YOLO("yolov8n.pt")

# パラメータ設定
KNOWN_HEIGHT = 170.0  # 人の平均身長 (cm)
# 事前準備で計算した焦点距離 (ピクセル単位)。環境に合わせて調整してください。
FOCAL_LENGTH = 800.0  

# Webカメラのキャプチャ開始 (0は内蔵カメラ)
cap = cv2.VideoCapture(1)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # YOLOv8で推論を実行 (class 0 は 'person')
    results = model(frame, classes=[0], verbose=False)

    for result in results:
        boxes = result.boxes
        for box in boxes:
            # バウンディングボックス座標 (x1, y1, x2, y2)
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            
            # バウンディングボックスの高さ (ピクセル)
            pixel_height = y2 - y1

            if pixel_height > 0:
                distance_cm = (KNOWN_HEIGHT * FOCAL_LENGTH) / pixel_height
                distance_m = distance_cm / 100.0
import cv2

class camera():
    def __init__():
        # HOG人検出器の初期化
        hog = cv2.HOGDescriptor()
        hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())
        cap = cv2.VideoCapture(0)

    def capture()->dict[str:int]:
        ret, frame = cap.read()
        if not ret:
            break
        resized_frame = cv2.resize(frame, (640, 480))
        boxes, weights = hog.detectMultiScale(resized_frame, winStride=(8, 8), padding=(4, 4), scale=1.05)


    

# カメラの開始


while True:
    


    # 処理軽量化のためリサイズ（任意）
    

    # 人を検出
    # winStride, padding などのパラメータで精度と速度を調整できます


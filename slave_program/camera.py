import cv2

class camera():
    # HOG人検出器の初期化
    hog = cv2.HOGDescriptor()
    hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())
    cap = cv2.VideoCapture(0)

    def capture()->tuple:
        ret, frame = camera.cap.read()
        if not ret:
            pass
        resized_frame = cv2.resize(frame, (640, 480))
        boxes, weights = camera.hog.detectMultiScale(resized_frame, winStride=(8, 8), padding=(4, 4), scale=1.05)
        return boxes


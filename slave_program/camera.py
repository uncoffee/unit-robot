import cv2

class camera():
    # HOG人検出器の初期化
    hog = cv2.HOGDescriptor()
    hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())
    cap = cv2.VideoCapture(0)
    scale = {"x":640,"y":480}

    def capture() -> tuple:
        ret, frame = camera.cap.read()
        if not ret:
            pass
        resized_frame = cv2.resize(frame, camera.scale["x"],camera.scale["y"])
        boxes, weights = camera.hog.detectMultiScale(resized_frame, winStride=(8, 8), padding=(4, 4), scale=1.05)
        return boxes

    def target() -> dict:
        box_infos = camera.capture()
        target_list = []
        for box in boxs:
            sum_x = box[0][0] + box[1][0]
            target_list.append(int(sum_X // 2))

        result = {camera.scale ,"targets":target_list}

        return result




            



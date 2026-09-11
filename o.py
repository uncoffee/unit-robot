import cv2

# USBカメラの準備（0は標準のカメラ。外付けUSBカメラの場合は 1 や 2 に変更）
cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("カメラを開くことができませんでした")
    exit()

while True:
    # 1フレーム（画像）を読み込む
    ret, frame = cap.read()

    if not ret:
        print("フレームを取得できませんでした")
        break

    # --- ここで画像処理を実行 ---
    # 例: グレースケール（白黒）に変換
    processed_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # 画面に表示
    cv2.imshow('USB Camera Processing', processed_frame)

    # 'q'キーが押されたら終了
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 後処理
cap.release()
cv2.destroyAllWindows()
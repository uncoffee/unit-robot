import serial

# 特定したポート名と、u-blox標準の通信速度を指定
port = '/dev/ttyACM0'
baudrate = 9600

try:
    ser = serial.Serial(port, baudrate, timeout=3)
    print(f"GPSデバイス（{port}）からデータを受信中...（終了は Ctrl+C）")

    while True:
        if ser.in_waiting > 0:
            # 1行読み込んでテキストとしてデコード
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            
            # $GPRMC や $GPGGA など、GPS特有のデータが表示されます
            if line:
                with open("result.txt", 'a', encoding='utf-8') as f:
                    f.write(line + '\n')

except serial.SerialException as e:
    print(f"エラー: ポートを開けませんでした。デバイス名を確認してください。: {e}")
except KeyboardInterrupt:
    print("\n終了します。")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()

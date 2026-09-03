import serial.tools.list_ports

ports = serial.tools.list_ports.comports()

print("--- 接続されているUSBデバイスの解析 ---")
for p in ports:
    # hwid（ハードウェアID）の中に内部ポート番号（3-2 など）が含まれています
    if "3-2" in p.hwid:
        print(f"🎉 発見しました！")
        print(f"【Pythonで指定するポート名】: {p.device}")
        print(f"デバイスの説明: {p.description}")
        print(f"詳細な場所（HWID）: {p.hwid}")
        break
else:
    print("usb 3-2 にシリアル通信機器（値を出力するデバイス）が見つかりませんでした。")
    print("機器の電源が入っているか、データ通信用のケーブルか確認してください。")

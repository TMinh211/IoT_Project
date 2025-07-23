from flask import Flask, request
import cv2
import os
import time

app = Flask(__name__)

SAVE_FOLDER = "baodong_images"
os.makedirs(SAVE_FOLDER, exist_ok=True)

last_capture_time = 0
# cooldown = 5  # giây, tránh chụp ảnh liên tục nếu chuyển động kéo dài

def capture_image():
    timestamp = time.strftime("%Y%m%d_%H%M%S")
    filename = f"{SAVE_FOLDER}/capture_{timestamp}.jpg"

    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("❌ Không mở được camera")
        return False

    ret, frame = cap.read()
    cap.release()

    if ret:
        cv2.imwrite(filename, frame)
        print(f"✅ Đã chụp và lưu ảnh: {filename}")
        return True
    else:
        print("❌ Không đọc được ảnh từ camera")
        return False

# @app.route("/canh-bao")
# def canh_bao():
#     global last_capture_time
#     now = time.time()

#     if now - last_capture_time > cooldown:
#         success = capture_image()
#         if success:
#             last_capture_time = now
#             print("✅ Đã chụp ảnh → trả về cho ESP: chup anh thanh cong")
#             return "chup anh thanh cong"  # 🔥 CHUỖI NÀY PHẢI ĐÚNG
#         else:
#             return "chup that bai"
#     else:
#         return "bo qua"

@app.route("/canh-bao")
def canh_bao():
    global last_capture_time
    now = time.time()
    print(f"🔔 Nhận cảnh báo lúc {now}, chênh lệch: {now - last_capture_time:.2f}s")

    if now - last_capture_time > cooldown:
        success = capture_image()
        if success:
            last_capture_time = now
            print("✅ Đã chụp ảnh → trả về cho ESP: chup anh thanh cong")
            return "chup anh thanh cong"
        else:
            return "chup that bai"
    else:
        print("⏳ Trong thời gian cooldown, bỏ qua")
        return "bo qua"

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080)
  

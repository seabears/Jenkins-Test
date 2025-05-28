import sys
import struct
import threading
from PyQt5.QtWidgets import QApplication
from rpi_backlight import Backlight

from can_handler import create_can_bus
from dashboard import Dashboard
from detection_mode import DetectionMode

current_mode = "driving"
status_text = {...}

bus = create_can_bus()
backlight = Backlight()
detection_handler = None

app = QApplication(sys.argv)
dashboard_window = Dashboard(bus)  # Dashboard 인스턴스

def can_receiver():
    global current_mode, detection_handler
    print("CAN 메시지 수신 대기 중...")
    while True:
        message = bus.recv()
        if message:
            # 모드 전환 처리
            if message.arbitration_id == 0x02 and current_mode != 'detection':
                current_mode = "detection"
                print("모드 전환 → detection")
                backlight.power = False
                dashboard_window.hide()
                if not detection_handler:
                    detection_handler = DetectionMode(bus)

            elif message.arbitration_id == 0x03 and current_mode != 'driving':
                current_mode = "driving"
                print("모드 전환 → driving")
                backlight.power = True
                dashboard_window.showFullScreen()

            # 온도·습도 업데이트
            if message.arbitration_id == 0x21:
                temperature = struct.unpack('<H', message.data[0:2])[0] / 10.0
                humidity = struct.unpack('<H', message.data[4:6])[0] / 10.0
                dashboard_window.update_env_signal.emit(temperature, humidity)
                print(f"UI 업데이트 → 온도 {temperature}°C, 습도 {humidity}%")

            # detection 모드에서 추가 메시지 처리
            if current_mode == 'detection' and detection_handler:
                detection_handler.handle_can_message(message)

# CAN 수신용 별도 쓰레드
can_thread = threading.Thread(target=can_receiver)
can_thread.daemon = True
can_thread.start()

if __name__ == '__main__':
    dashboard_window.showFullScreen()
    try:
        sys.exit(app.exec_())  # PyQt 이벤트 루프 실행
    except KeyboardInterrupt:
        print("프로그램 종료!")


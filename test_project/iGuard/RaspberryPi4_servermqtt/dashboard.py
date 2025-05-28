import sys
import os
from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QVBoxLayout, QPushButton, QHBoxLayout, QFrame
)
from PyQt5.QtGui import QPixmap, QPainter, QPainterPath, QFont, QPen, QColor
from PyQt5.QtCore import Qt, QRectF, pyqtSignal
from can_handler import send_can_message
from music_player import MusicScreen

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

class Dashboard(QWidget):
    update_env_signal = pyqtSignal(float, float)  # temperature, humidity 신호

    def __init__(self, bus):
        super().__init__()
        self.bus = bus
        self.current_ac_level = None
        self.ac_buttons = {}

        self.setWindowTitle("차량 대시보드")
        self.setStyleSheet("background-color: #e6f2fa;")
        self.setGeometry(0, 0, 800, 480)

        main_layout = QVBoxLayout()
        main_layout.setSpacing(10)
        main_layout.setContentsMargins(30, 10, 30, 10)

        # === Temperature & Humidity Row ===
        temp_hum_layout = QHBoxLayout()
        self.temp_label = QLabel("Waiting for connection...")
        self.temp_label.setFont(QFont("Arial", 30, QFont.Bold))
        self.temp_label.setStyleSheet("color: #1e3f66;")

        self.hum_label = QLabel("")
        self.hum_label.setFont(QFont("Arial", 30, QFont.Bold))
        self.hum_label.setStyleSheet("color: #1e3f66;")

        temp_hum_layout.addStretch()
        temp_hum_layout.addWidget(self.temp_label)
        temp_hum_layout.addSpacing(30)
        temp_hum_layout.addWidget(self.hum_label)
        temp_hum_layout.addStretch()
        main_layout.addLayout(temp_hum_layout)

        # === A/C Control Card (Title + Buttons) ===
        ac_card_layout = QVBoxLayout()
        ac_card_layout.setSpacing(8)

        ac_label = QLabel("❄ A/C Control")
        ac_label.setAlignment(Qt.AlignCenter)
        ac_label.setFont(QFont("Arial", 15, QFont.Bold))
        ac_label.setStyleSheet("color: #1e3f66;")
        ac_card_layout.addWidget(ac_label)

        fan_layout = QHBoxLayout()
        fan_layout.setSpacing(10)
        for label, code in zip(["OFF", "LOW", "MID", "HIGH"], [0, 1, 2, 3]):
            btn = QPushButton(label)
            btn.setFixedSize(90, 40)
            btn.setStyleSheet(self._button_style(active=False))
            btn.clicked.connect(self.make_ac_callback(code))
            fan_layout.addWidget(btn)
            self.ac_buttons[code] = btn

        ac_card_layout.addLayout(fan_layout)

        ac_card = QFrame()
        ac_card.setStyleSheet("""
            QFrame {
                background-color: #ffffff;
                border: 2px solid #4682b4;
                border-radius: 15px;
                padding: 10px;
            }
        """)
        ac_card.setLayout(ac_card_layout)
        main_layout.addWidget(ac_card, alignment=Qt.AlignCenter)

        # === Bottom Icon Buttons ===
        button_row = QHBoxLayout()
        button_row.setSpacing(30)
        button_row.setAlignment(Qt.AlignCenter)

        buttons = [
            ("전화", os.path.join(BASE_DIR, "icons/phonecall.png"), None),
            ("지도", os.path.join(BASE_DIR, "icons/navermap.png"), None),
            ("카카오톡", os.path.join(BASE_DIR, "icons/kakaotalk.jpg"), None),
            ("노래", os.path.join(BASE_DIR, "icons/babyshark.jpg"), "music"),
        ]

        for name, img_path, app_id in buttons:
            if app_id:
                callback = lambda aid=app_id: self.launch_app(aid)
            else:
                callback = lambda: None

            btn = RoundImageButton(img_path, 80, callback)
            label = QLabel(name)
            label.setAlignment(Qt.AlignCenter)
            label.setFont(QFont("Arial", 11, QFont.Bold))
            label.setStyleSheet("color: #1e3f66;")

            vbox = QVBoxLayout()
            vbox.setSpacing(2)
            vbox.addWidget(btn)
            vbox.addWidget(label)

            container = QWidget()
            container.setLayout(vbox)
            button_row.addWidget(container)

        main_layout.addSpacing(10)
        main_layout.addLayout(button_row)

        self.setLayout(main_layout)

        # === Signal 연결 ===
        self.update_env_signal.connect(self.update_environment)

    def update_environment(self, temperature, humidity):
        self.temp_label.setText(f"{temperature:.1f}°C")
        self.hum_label.setText(f"{humidity:.1f}%")

    def _button_style(self, active):
        if active:
            return """
                QPushButton {
                    background-color: #4682b4;
                    color: white;
                    font-size: 13px;
                    font-weight: bold;
                    border-radius: 8px;
                }
                QPushButton:hover {
                    background-color: #5a9bd5;
                }
            """
        else:
            return """
                QPushButton {
                    background-color: #ffffff;
                    border: 2px solid #4682b4;
                    border-radius: 8px;
                    font-size: 13px;
                    font-weight: bold;
                    color: #1e3f66;
                }
                QPushButton:hover {
                    background-color: #deeffa;
                }
            """

    def make_ac_callback(self, code):
        return lambda _: self.send_ac_message(code)

    def send_ac_message(self, level_code):
        can_id = 0x17
        data = bytearray([0x00]*7 + [level_code & 0xFF])
        #data = [level_code] + [0] * 7
        send_can_message(self.bus, can_id, data)
        print(f"AC 모드 {level_code} 전송 → CAN ID {hex(can_id)}, 데이터 {data}")

        self.current_ac_level = level_code
        for code, btn in self.ac_buttons.items():
            btn.setStyleSheet(self._button_style(active=(code == level_code)))

    def launch_app(self, app_name):
        if app_name == "music":
            self.music_window = MusicScreen(self.bus)
            self.music_window.showFullScreen()

class RoundImageButton(QLabel):
    def __init__(self, image_path, size, callback):
        super().__init__()
        self.setFixedSize(size, size)
        self.setCursor(Qt.PointingHandCursor)
        self.callback = callback
        self.image_path = image_path
        self.size = size
        self.update_pixmap()

    def update_pixmap(self):
        rounded = QPixmap(self.size, self.size)
        rounded.fill(Qt.transparent)

        painter = QPainter(rounded)
        painter.setRenderHint(QPainter.Antialiasing)

        path = QPainterPath()
        path.addRoundedRect(QRectF(0, 0, self.size, self.size), 15, 15)
        painter.setClipPath(path)

        pixmap = QPixmap(self.image_path).scaled(
            self.size, self.size, Qt.KeepAspectRatioByExpanding, Qt.SmoothTransformation
        )
        painter.drawPixmap(0, 0, pixmap)

        pen = QPen(QColor("#4682b4"), 2)
        painter.setPen(pen)
        painter.setBrush(Qt.NoBrush)
        painter.drawPath(path)

        painter.end()
        self.setPixmap(rounded)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.callback()

if __name__ == '__main__':
    import can
    app = QApplication(sys.argv)
    win = Dashboard(bus=bus)
    win.showFullScreen()
    sys.exit(app.exec_())


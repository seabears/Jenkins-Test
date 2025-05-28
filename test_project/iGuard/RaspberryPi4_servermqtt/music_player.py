import sys
import os
from PyQt5.QtWidgets import QWidget, QLabel, QVBoxLayout, QPushButton, QHBoxLayout, QApplication
from PyQt5.QtGui import QPixmap, QFont
from PyQt5.QtCore import Qt
from can_handler import send_can_message

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

class MusicScreen(QWidget):
    def __init__(self, bus):
        super().__init__()
        self.bus = bus
        self.setWindowTitle("노래 플레이어")
        self.setGeometry(0, 0, 800, 480)
        self.setStyleSheet("background-color: #d6eaf8;")

        self.songs = {
            1: {"title": "babyshark", "image": os.path.join(BASE_DIR, "icons/babyshark.jpg")},
            2: {"title": "bbororo", "image": os.path.join(BASE_DIR, "icons/bbororo.jpg")},
            3: {"title": "totoro", "image": os.path.join(BASE_DIR, "icons/totoro.jpg")}
        }
        self.current_track = 1

        # 이미지 및 제목
        self.image_label = QLabel()
        self.image_label.setAlignment(Qt.AlignCenter)

        self.title_label = QLabel()
        self.title_label.setAlignment(Qt.AlignCenter)
        self.title_label.setFont(QFont("Arial", 28, QFont.Bold))
        self.title_label.setStyleSheet("color: #1e3f66;")

        # 주요 컨트롤 버튼
        self.prev_btn = QPushButton("◀︎◀︎")
        self.play_btn = QPushButton("▶︎")
        self.stop_btn = QPushButton("■")
        self.next_btn = QPushButton("▶︎▶︎")

        for btn in [self.prev_btn, self.play_btn, self.stop_btn, self.next_btn]:
            btn.setFixedSize(130, 50)
            btn.setStyleSheet("""
                QPushButton {
                    background-color: #ffffff;
                    border: 2px solid #4682b4;
                    border-radius: 10px;
                    font-size: 24px;
                    font-weight: bold;
                    color: #1e3f66;
                }
                QPushButton:hover {
                    background-color: #deeffa;
                }
            """)

        self.prev_btn.clicked.connect(self.play_prev)
        self.next_btn.clicked.connect(self.play_next)
        self.play_btn.clicked.connect(self.send_play_command)
        self.stop_btn.clicked.connect(self.send_stop_command)

        # 종료 버튼
        self.close_button = QPushButton("X", self)
        self.close_button.setFixedSize(40, 40)
        self.close_button.setStyleSheet("""
            QPushButton {
                background-color: #e74c3c;
                color: white;
                border: none;
                font-weight: bold;
                font-size: 20px;
                border-radius: 5px;
            }
            QPushButton:hover {
                background-color: #c0392b;
            }
        """)
        self.close_button.clicked.connect(self.stop_close)

        # 볼륨 버튼
        self.vol_up_btn = QPushButton("▲", self)
        self.vol_down_btn = QPushButton("▼", self)

        for btn in [self.vol_up_btn, self.vol_down_btn]:
            btn.setFixedSize(40, 40)
            btn.setStyleSheet("""
                QPushButton {
                    background-color: #ffffff;
                    border: 2px solid #4682b4;
                    border-radius: 5px;
                    font-size: 18px;
                    font-weight: bold;
                    color: #1e3f66;
                }
                QPushButton:hover {
                    background-color: #deeffa;
                }
            """)

        self.vol_up_btn.clicked.connect(self.send_vol_up_command)
        self.vol_down_btn.clicked.connect(self.send_vol_down_command)

        # 메인 레이아웃
        layout = QVBoxLayout()
        layout.addSpacing(20)
        layout.addWidget(self.image_label)
        layout.addSpacing(10)
        layout.addWidget(self.title_label)
        layout.addSpacing(20)

        control_row = QHBoxLayout()
        control_row.setSpacing(30)
        control_row.setAlignment(Qt.AlignCenter)
        control_row.addWidget(self.prev_btn)
        control_row.addWidget(self.play_btn)
        control_row.addWidget(self.stop_btn)
        control_row.addWidget(self.next_btn)

        layout.addLayout(control_row)
        layout.addStretch()
        self.setLayout(layout)

        self.play_song(self.current_track)

        # 초기 위치
        self.close_button.move(740, 20)
        self.vol_up_btn.move(740, 180)
        self.vol_down_btn.move(740, 230)

        self.close_button.raise_()
        self.vol_up_btn.raise_()
        self.vol_down_btn.raise_()

    def resizeEvent(self, event):
        self.close_button.move(self.width() - 60, 20)
        self.vol_up_btn.move(self.width() - 60, 180)
        self.vol_down_btn.move(self.width() - 60, 230)
        self.close_button.raise_()
        self.vol_up_btn.raise_()
        self.vol_down_btn.raise_()

    def play_song(self, track):
        song = self.songs.get(track)
        if not song:
            return
        self.title_label.setText(song["title"])
        if os.path.exists(song["image"]):
            pixmap = QPixmap(song["image"]).scaled(320, 320, Qt.KeepAspectRatio, Qt.SmoothTransformation)
            self.image_label.setPixmap(pixmap)
        else:
            self.image_label.setText("이미지 없음")
            self.image_label.setPixmap(QPixmap())

        can_id = 0x05
        data = [track, 0, 0, 0, 0, 0, 0, 0]
        send_can_message(self.bus, can_id, data)
        print(f"곡 {track} 선택 → CAN 메시지 전송 완료: {data}")

    def play_next(self):
        self.current_track += 1
        if self.current_track > len(self.songs):
            self.current_track = 1
        self.play_song(self.current_track)

    def play_prev(self):
        self.current_track -= 1
        if self.current_track < 1:
            self.current_track = len(self.songs)
        self.play_song(self.current_track)

    def stop_close(self):
        can_id = 0x06
        data = [0] * 8
        send_can_message(self.bus, can_id, data)
        self.close()

    def send_stop_command(self):
        can_id = 0x06
        data = [0] * 8
        send_can_message(self.bus, can_id, data)
        print(f"정지 버튼 → CAN ID {hex(can_id)}, 데이터: {data}")

    def send_play_command(self):
        can_id = 0x07
        data = [0] * 8
        send_can_message(self.bus, can_id, data)
        print(f"재생 버튼 → CAN ID {hex(can_id)}, 데이터: {data}")

    def send_vol_up_command(self):
        can_id = 0x08
        data = [0] * 8
        send_can_message(self.bus, can_id, data)
        print(f"볼륨 ↑ 버튼 → CAN ID {hex(can_id)}, 데이터: {data}")

    def send_vol_down_command(self):
        can_id = 0x09
        data = [0] * 8
        send_can_message(self.bus, can_id, data)
        print(f"볼륨 ↓ 버튼 → CAN ID {hex(can_id)}, 데이터: {data}")

if __name__ == '__main__':
    import can
    bus = can.interface.Bus(channel='can0', bustype='socketcan')
    app = QApplication(sys.argv)
    win = MusicScreen(bus)
    win.showFullScreen()
    sys.exit(app.exec_())


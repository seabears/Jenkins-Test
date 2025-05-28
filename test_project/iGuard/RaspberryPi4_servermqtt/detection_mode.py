import os
import base64
import paho.mqtt.client as mqtt
from can_handler import send_can_message

class DetectionMode:
    def __init__(self, bus):
        self.bus = bus
        self.image_file = "image.jpg"
        self.broker_ip = "172.20.10.4"
        self.port = 1883
        self.username = "master"
        self.password = "1234"

        self.topic_publish = "Car/image"
        self.topic_trigger = "Car/trigger"
        self.topic_message = "Car/LCD"
        self.music_topic = "Car/music"
        self.inside_topic = "Car/inside"
        self.outside_topic = "Car/outside"
        self.topic_detection = "Car/detection"

        self.client = mqtt.Client()
        self.client.username_pw_set(self.username, self.password)
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.client.connect(self.broker_ip, self.port, 60)
        self.client.loop_start()

    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print("MQTT 연결 성공 (Detection)")
            client.subscribe(self.topic_trigger)
            client.subscribe(self.topic_message)
            client.subscribe(self.music_topic)
        else:
            print(f"MQTT 연결 실패, 코드: {rc}")

    def on_message(self, client, userdata, msg):
        payload = msg.payload.decode().strip()
        print(f"{msg.topic} -> {payload}")

        if msg.topic == self.topic_trigger and payload == "trigger":
            self.capture_and_publish()

        elif msg.topic == self.topic_message:
            payload_bytes = payload.encode('ascii', errors='ignore')
            fixed_length = 32
            padded_payload = payload_bytes.ljust(fixed_length, b'\x00')
            for i in range(0, fixed_length, 8):
                chunk = padded_payload[i:i+8]
                send_can_message(self.bus, 0x010, chunk)

        elif msg.topic == self.music_topic:
            if ":" in payload:
                state, music_number = payload.split(":")
                music_number = int(music_number)
                if state == "1":
                    send_can_message(self.bus, 0x005, [music_number] + [0]*7)
                elif state == "0":
                    send_can_message(self.bus, 0x006, [0]*8)
            else:
                if payload == "1":
                    send_can_message(self.bus, 0x005, [0xFF] + [0]*7)
                elif payload == "0":
                    send_can_message(self.bus, 0x006, [0]*8)

    def capture_and_publish(self):
        print("사진 촬영 중...")
        os.system(f"fswebcam -d /dev/video0 --jpeg 20 -r 640x480 {self.image_file}")
        with open(self.image_file, "rb") as f:
            img_bytes = f.read()
        encoded_image = base64.b64encode(img_bytes).decode()
        self.client.publish(self.topic_publish, encoded_image)
        print("이미지 MQTT 발행 완료!")

    def handle_can_message(self, message):
        import struct

        if message.arbitration_id == 0x01:
            self.client.publish(self.topic_detection, "1")
            print(f"MQTT 발행 ({self.topic_detection}): 1")

        elif message.arbitration_id == 0x21:
            temperature = struct.unpack('<H', message.data[0:2])[0] / 10.0
            humidity = struct.unpack('<H', message.data[4:6])[0] / 10.0
            payload = f"Temperature: {temperature}°C, Humidity: {humidity}%"
            self.client.publish(self.inside_topic, payload)
            print(f"MQTT 발행 ({self.inside_topic}): {payload}")

        elif message.arbitration_id == 0x20:
            data0 = struct.unpack('<I', message.data[0:4])[0]
            data1 = struct.unpack('<I', message.data[4:8])[0]
            ext_air = (data0 >> 16) & 0xFFFF
            int_co2 = data0 & 0xFFFF
            mode = (data1 >> 16) & 0xFFFF
            speed = data1 & 0xFFFF
            payload = f"Ext Air: {ext_air}, CO2: {int_co2}, Mode: {mode}, Speed: {speed}"
            self.client.publish(self.outside_topic, payload)
            print(f"MQTT 발행 ({self.outside_topic}): {payload}")


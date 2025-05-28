import can

def create_can_bus(interface='can0'):
    return can.interface.Bus(channel=interface, bustype='socketcan')

def send_can_message(bus, can_id, data_list):
    if isinstance(data_list, bytes):
        data_list = bytearray(data_list)
    can_msg = can.Message(arbitration_id=can_id, data=data_list, is_extended_id=False)
    try:
        bus.send(can_msg)
        print(f"CAN 메시지 전송: ID={hex(can_id)}, Data={list(data_list)}")
    except can.CanError as e:
        print(f"CAN 전송 실패: {e}")


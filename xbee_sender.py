import can
import time
import threading
from digi.xbee.devices import XBeeDevice, RemoteXBeeDevice
from digi.xbee.models.address import XBee64BitAddress
from luma.core.interface.serial import i2c
from luma.oled.device import sh1107
from luma.core.render import canvas
from PIL import ImageFont, Image, ImageDraw

# --- KONFIGURACJA ---
XBEE_PORT, XBEE_BAUD = "/dev/serial0", 115200
REMOTE_ADDR = "0013A200424118F3"
TARGET_CAN_ID, TARGET_CAN_ID_2 = 0x607, 0x608

# --- INICJALIZACJA EKRANÓW I CAN ---
try:
    serial1 = i2c(port=1, address=0x3C)
    oled3C = sh1107(serial1, width=128, height=128)
    serial2 = i2c(port=1, address=0x3D)
    oled3D = sh1107(serial2, width=128, height=128)
except Exception as e:
    print(f"Błąd sprzętu: {e}")
    exit()

try:
	bus = can.interface.Bus(channel='can0', interface='socketcan')
except Exception as e:
	print(f"Błąd CAN: {e}")
	exit()

def get_font(name, size):
    try: return ImageFont.truetype(f"/usr/share/fonts/truetype/dejavu/{name}.ttf", size)
    except: return ImageFont.load_default()

font_75, font_20, font_25 = get_font("DejaVuSans", 75), get_font("DejaVuSans", 20), get_font("DejaVuSans", 25)

# --- CACHE: PRZYGOTOWANIE STAŁYCH ELEMENTÓW (TŁA) ---
# Ekran 1
bg1 = Image.new("1", (128, 128), 0)
draw_bg1 = ImageDraw.Draw(bg1)
draw_bg1.text((35, 95), "KM/H", font=font_20, fill=1)

# Ekran 2
bg2 = Image.new("1", (128, 128), 0)
draw_bg2 = ImageDraw.Draw(bg2)
def fix_phase_shift(byte_data):
    return bytes(int('{:08b}'.format(b)[::-1], 2) for b in byte_data)

flag_bits = fix_phase_shift(bytes([0x04,0x00,0xcc,0x33,0x9c,0x33,0x9c,0x3f,0x64,0x0e,0x64,0x0e,0xe4,0x33,0x9c,0x33,0x9c,0x3f,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00]))
flag_icon = Image.frombytes('1', (16, 16), flag_bits)
bg2.paste(flag_icon, (10, 10))

# --- DANE ---
car_data = {"R": 0, "S": 0, "OT": 0, "CLT": 0, "IAT": 0, "EGT": 0, "V": 0.0}
base_data = {"lap": "01", "total_laps": "7", "delta": "0"}

# --- XBEE ---
xbee_vehicle = XBeeDevice(XBEE_PORT, XBEE_BAUD)
xbee_paddock = RemoteXBeeDevice(xbee_vehicle, XBee64BitAddress.from_hex_string(REMOTE_ADDR))

def on_data_received(xbee_message):
    try:
        payload = xbee_message.data.decode("utf8")
        parts = dict(item.split(":") for item in payload.split(";"))
        base_data["lap"] = parts.get("L", "01")
        base_data["delta"] = parts.get("D", "0")
        base_data["total_laps"] = parts.get("T", "7")
    except: pass

# --- WĄTEK EKRANÓW (DZIAŁA ASYNCHRONICZNIE) ---
def display_updater():
    last_speed_update = 0
    last_delta_update = 0
    """Obsługuje ekrany tak szybko, jak pozwala magistrala I2C, nie blokując CAN/XBee"""
    while True:
        now = time.time()
        if now - last_speed_update >= 0.2:
        # Ekran 1 - Prędkość (Adres 0x3C)
            with canvas(oled3C) as draw:
                draw.bitmap((10, 10), bg1, fill="white")
                s_str = str(car_data["S"])
                # Zamiast textlength (powolne), używamy stałych pozycji dla 1, 2 lub 3 cyfr
                pos_x = 45 if len(s_str) == 1 else (25 if len(s_str) == 2 else 5)
                draw.text((pos_x, 20), s_str, font=font_75, fill="white")
            last_speed_update = now

        # Ekran 2 - Delta (Adres 0x3D)
        if now - last_delta_update >= 1.0:
            with canvas(oled3D) as draw:
                draw.bitmap((0, 0), bg2, fill="white")
                draw.text((80, 10), f"{base_data['lap']}/{base_data['total_laps']}", font=font_20, fill="white")

                delta_val = int(base_data['delta'])

                if delta_val >= 0:
                    display_delta = f"+{delta_val}"
                    status_text = "ZAPAS"
                else:
                    display_delta = f"{delta_val}"
                    status_text = "STRATA"

                draw.text((20, 60), f"D: {display_delta} s", font=font_25, fill="white")
                draw.text((30, 100), status_text, font=font_20, fill="white")
            last_delta_update = now

        time.sleep(0.001) # Minimalna przerwa, by nie przegrzać rdzenia

# --- PĘTLA GŁÓWNA ---
try:
    xbee_vehicle.open()
    xbee_vehicle.add_data_received_callback(on_data_received)

    # Uruchomienie ekranów w osobnym wątku
    threading.Thread(target=display_updater, daemon=True).start()

    last_xbee_time = time.time()

    while True:
        # 1. Odbiór CAN - timeout skrócony do minimum
        msg = bus.recv(timeout=0.001)
        if msg and msg.is_extended_id:
            if msg.arbitration_id == TARGET_CAN_ID:
                car_data["R"] = int.from_bytes(msg.data[0:2], 'big')
                car_data["S"], car_data["V"] = msg.data[2], msg.data[3] / 10.0
            elif msg.arbitration_id == TARGET_CAN_ID_2:
                car_data["OT"], car_data["IAT"], car_data["CLT"] = msg.data[0], msg.data[1], msg.data[2]
                car_data["EGT"] = int.from_bytes(msg.data[3:5], 'big')

        # 2. Wysyłka XBee - co 200ms
        now = time.time()
        if now - last_xbee_time >= 0.2:
            payload = f"R:{car_data['R']};S:{car_data['S']};OT:{car_data['OT']};IAT:{car_data['IAT']};EGT:{car_data['EGT']};CLT:{car_data['CLT']};V:{car_data['V']:.1f}"
            print(payload)
            try:
                xbee_vehicle.send_data(xbee_paddock, payload)
            except: pass
            last_xbee_time = now

finally:
    if xbee_vehicle.is_open():
        xbee_vehicle.close()

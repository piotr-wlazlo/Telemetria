# TELEMETRIA
System telemetrii bolidesa, składający się z modułu znajdującego się w bolidesie oraz modułu głównego działającego na RPi.

## STRUKTURA
- xbee_sender.py
- telemetria-esp/
- telemetria-rpi/

### `xbee_sender.py`
Dotychczasowy program działający w bolidzie.

### `telemetria-esp/`
Zmodyfikowana i dostosowana do ESP32 wersja `xbee_sender.py`.
Odpowiada za:
- odczyt danych z pojazdu po CAN,
- wyświetlanie parametrów (m.in. prędkość, aktualne okrążenie, czas, itd.) na ekranach OLED,
- wysyłanie parametrów pojazdu z pomocą Xbee,
- odbieranie informacji z modułu głównego (`telemetria-rpi`).

### `telemetria-rpi/`
Oprogramowanie modułu głównego działającego na RPi.
Odpowiada za:
- postawienie lokalnej strony internetowej,
- odbieranie telemetrii pojazdu przez Xbee,
- prezentowanie i zapisywanie odebranych danych,
- wysłanie do bolidesa liczby okrążeń i maksymalnego czasu,
- aktualizowanie aktualnego okrążenia,
- wysyłanie do bolidesa informacji o czasie wyścigu.

## STATUS
`telemetria-esp/`, `telemetria-rpi/` oraz `docs/` są w trakcie powstawania.

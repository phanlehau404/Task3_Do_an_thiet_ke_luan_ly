import serial
import time
from datetime import datetime

# Open the port with DTR/RTS disabled from the beginning
ser = serial.Serial(
    port='COM7',
    baudrate=115200,
    timeout=1,
    dsrdtr=False,   # Do not use DTR
    rtscts=False    # Do not use RTS
)

# Ensure reset signals are fully disabled
ser.setDTR(False)
ser.setRTS(False)

# Wait for ESP32 to stabilize (avoid reset when opening port)
time.sleep(2)

# Get current time
now = datetime.now()
hour = now.hour
minute = now.minute
second = now.second
day = now.day
month = now.month
year = now.year % 100

# Create 6-byte payload
payload = bytes([hour, minute, second, day, month, year])

# Send data
ser.write(payload)
ser.flush()

print(f"Sent successfully → {hour:02d}:{minute:02d}:{second:02d} - "
      f"{day:02d}/{month:02d}/20{year:02d}")

# Do not close the port immediately (avoid reset pulse)
time.sleep(0.5)

# Disable reset signals before closing
ser.setDTR(False)
ser.setRTS(False)

# Give driver some time to stabilize to avoid reset
time.sleep(0.2)

ser.close()

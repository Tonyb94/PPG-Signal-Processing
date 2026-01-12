import serial
import time
import numpy as np

# ===================== SERIAL CONFIG =====================
SERIAL_PORT = "COM7"
BAUDRATE = 115200

ser = serial.Serial(
    port=SERIAL_PORT,
    baudrate=BAUDRATE,
    timeout=1
)

print(f"Serial opened on {SERIAL_PORT}")

# ===================== PPG PARAMETERS =====================
FS = 100
WINDOW_DURATION = 5.0          # seconds
SETTLING_TIME = 0.150          # 150 ms
N_WINDOWS = 6
WINDOW_ORDER = ["R", "IR"] * 3

samples_per_window = int(FS * WINDOW_DURATION)

# ===================== SIGNAL GENERATOR =====================
def generate_ppg_signal(n_samples, freq=1.2, noise=150, amplitude=1000):
    t = np.arange(n_samples) / FS
    sig = 2048 + amplitude * np.sin(2 * np.pi * freq * t)
    sig += np.random.normal(0, noise, size=n_samples)
    return np.clip(sig, 0, 4095).astype(np.uint16)

# ===================== PREPARE WINDOWS =====================
windows = []

for color in WINDOW_ORDER:
    amp = 900 if color == "R" else 1100
    sig = generate_ppg_signal(samples_per_window, amplitude=amp)
    windows.append(sig)

total_samples = samples_per_window * N_WINDOWS
print(f"Prepared {total_samples} samples")

# ===================== MAIN LOOP =====================
print("Waiting for MCU trigger (button press)...")

simulation_started = False
window_idx = 0
sample_idx = 0
start_time = None
last_window_end_time = None

while True:
    c = ser.read(1)

    # ===================== MCU START =====================
    if c == b'C' and not simulation_started:
        simulation_started = True
        window_idx = 0
        sample_idx = 0
        start_time = time.time()
        last_window_end_time = None
        print("MCU start received → simulation armed")

    # ===================== MCU REQUEST SAMPLE =====================
    elif c == b'R' and simulation_started:

        # End of all windows
        if window_idx >= N_WINDOWS:
            elapsed = time.time() - start_time
            print(f"Simulation completed in {elapsed:.2f} s")
            simulation_started = False
            continue

        # Settling time between windows
        if sample_idx == 0 and last_window_end_time is not None:
            if time.time() - last_window_end_time < SETTLING_TIME:
                continue

        # Send sample
        val = windows[window_idx][sample_idx]
        ser.write(bytes([
            val & 0xFF,
            (val >> 8) & 0x0F
        ]))

        sample_idx += 1

        # End of window
        if sample_idx >= samples_per_window:
            sample_idx = 0
            window_idx += 1
            last_window_end_time = time.time()

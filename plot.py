import csv
import matplotlib.pyplot as plt
import os

times = []
clean = []
less_noisy = []
noisy = []

csv_file = 'waveforms.csv'
if not os.path.exists(csv_file):
    print(f"Error: {csv_file} not found.")
    exit(1)

with open(csv_file, 'r') as f:
    reader = csv.DictReader(f)
    for row in reader:
        times.append(float(row['Time']))
        clean.append(float(row['Clean']))
        less_noisy.append(float(row['LessNoisy']))
        noisy.append(float(row['Noisy']))

plt.figure(figsize=(10, 8))

# Plot clean wave
plt.subplot(3, 1, 1)
plt.plot(times, clean, label='Clean', color='blue', linewidth=2)
plt.title('Clean Sine Wave')
plt.ylabel('Amplitude')
plt.grid(True)
plt.legend(loc='upper right')

# Plot less noisy wave
plt.subplot(3, 1, 2)
plt.plot(times, less_noisy, label='Less Noisy (Variance=0.1)', color='green', linewidth=1.5)
plt.title('Slightly Noisy Sine Wave')
plt.ylabel('Amplitude')
plt.grid(True)
plt.legend(loc='upper right')

# Plot noisy wave
plt.subplot(3, 1, 3)
plt.plot(times, noisy, label='Very Noisy (Variance=1.0)', color='red', linewidth=1)
plt.title('Very Noisy Sine Wave')
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.grid(True)
plt.legend(loc='upper right')

plt.tight_layout()
plt.savefig('waveforms.png')
print("Plot saved to waveforms.png")

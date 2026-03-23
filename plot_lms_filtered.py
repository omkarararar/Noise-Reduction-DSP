import csv
import matplotlib.pyplot as plt
import os
import sys

csv_file = 'lms_filtered_waveforms.csv'
if not os.path.exists(csv_file):
    print(f"Error: {csv_file} not found. Please run the lms_filter C program first.")
    sys.exit(1)

times = []
clean = []
less_noisy = []
noisy = []
lms_clean_less = []
lms_clean_heavy = []

with open(csv_file, 'r') as f:
    reader = csv.DictReader(f)
    for row in reader:
        times.append(float(row['Time']))
        clean.append(float(row['Clean']))
        less_noisy.append(float(row['LessNoisy']))
        noisy.append(float(row['Noisy']))
        lms_clean_less.append(float(row['LMSCleanLess']))
        lms_clean_heavy.append(float(row['LMSCleanHeavy']))

plt.figure(figsize=(12, 10))
plt.suptitle('Dual-Microphone Active Noise Cancellation (LMS Filter)', fontsize=16)

# Plot Less Noisy vs Filtered
plt.subplot(2, 1, 1)
plt.plot(times, clean, label='True Clean Signal', color='blue', linewidth=2, linestyle='dashed')
plt.plot(times, less_noisy, label='Primary Mic (Speech + Little Noise)', color='green', alpha=0.5)
plt.plot(times, lms_clean_less, label='LMS Output (Denoised)', color='darkgreen', linewidth=2)
plt.title('ANC on Slightly Noisy Signal')
plt.ylabel('Amplitude')
plt.grid(True)
plt.legend(loc='upper right')

# Plot Noisy vs Filtered
plt.subplot(2, 1, 2)
plt.plot(times, clean, label='True Clean Signal', color='blue', linewidth=2, linestyle='dashed')
plt.plot(times, noisy, label='Primary Mic (Speech + Heavy Noise)', color='red', alpha=0.3)
plt.plot(times, lms_clean_heavy, label='LMS Output (Denoised)', color='darkred', linewidth=2)
plt.title('ANC on Very Noisy Signal')
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.grid(True)
plt.legend(loc='upper right')

plt.tight_layout()
plt.savefig('lms_filtered_waveforms.png')
print("Plot saved to lms_filtered_waveforms.png")

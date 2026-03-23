import csv
import matplotlib.pyplot as plt
import os
import sys

csv_file = 'mmse_filtered_waveforms.csv'
if not os.path.exists(csv_file):
    print(f"Error: {csv_file} not found. Please run the mmse_filter C program first.")
    sys.exit(1)

times = []
clean = []
less_noisy = []
noisy = []
filtered_less_noisy = []
filtered_noisy = []

with open(csv_file, 'r') as f:
    reader = csv.DictReader(f)
    for row in reader:
        times.append(float(row['Time']))
        clean.append(float(row['Clean']))
        less_noisy.append(float(row['LessNoisy']))
        noisy.append(float(row['Noisy']))
        filtered_less_noisy.append(float(row['MMSELessNoisy']))
        filtered_noisy.append(float(row['MMSENoisy']))

plt.figure(figsize=(12, 10))
plt.suptitle('Sine Wave MMSE (LMS ALE) Filter Denoising', fontsize=16)

# Plot Less Noisy vs Filtered
plt.subplot(2, 1, 1)
plt.plot(times, clean, label='Clean', color='blue', linewidth=2, linestyle='dashed')
plt.plot(times, less_noisy, label='Less Noisy Original', color='green', alpha=0.5)
plt.plot(times, filtered_less_noisy, label='MMSE Filtered Less Noisy', color='darkgreen', linewidth=2)
plt.title('Denoising Slightly Noisy Sine Wave')
plt.ylabel('Amplitude')
plt.grid(True)
plt.legend(loc='upper right')

# Plot Noisy vs Filtered
plt.subplot(2, 1, 2)
plt.plot(times, clean, label='Clean', color='blue', linewidth=2, linestyle='dashed')
plt.plot(times, noisy, label='Very Noisy Original', color='red', alpha=0.3)
plt.plot(times, filtered_noisy, label='MMSE Filtered Very Noisy', color='darkred', linewidth=2)
plt.title('Denoising Very Noisy Sine Wave')
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.grid(True)
plt.legend(loc='upper right')

plt.tight_layout()
plt.savefig('mmse_filtered_waveforms.png')
print("Plot saved to mmse_filtered_waveforms.png")

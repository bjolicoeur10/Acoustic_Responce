import numpy as np
import matplotlib.pyplot as plt

def create_equilateral_triangle(base_length, height, interval):
    num_steps = base_length // interval + 1
    t = [i * interval for i in range(num_steps)]
    values = []
    half_num_steps = num_steps // 2

    for i in range(half_num_steps):
        values.append(i * (height / half_num_steps))
    for i in range(half_num_steps):
        values.append(height - i * (height / half_num_steps))

    return t, values

time_limit = 32000
interval = 4
num_repeats = 3
max_triangles = 100
max_steepest_slope_corr = 150  # mT/m/ms
max_gradient_height = 50 # mT/m
added_zeros_per_repeat = 1000 

plt.figure(figsize=(10, 5))

t_all = []
values_all = []
above_x_axis = True

num_triangles = 1
desired_height = max_gradient_height 

while True:
    triangle_length = int(time_limit / num_triangles)

    for repeat_idx in range(num_repeats + 1):
        t, values = create_equilateral_triangle(triangle_length, desired_height, interval)
        t_shifted = [val + t_all[-1] + interval if t_all else val for val in t]

        if above_x_axis:
            values_all.extend(values)
        else:
            values_all.extend([-v for v in values])

        t_all.extend(t_shifted)
        above_x_axis = not above_x_axis
    

        if repeat_idx % 2 == 1:
            t_all.extend([t_all[-1] + interval] * added_zeros_per_repeat)
            values_all.extend([0] * added_zeros_per_repeat)
    
  
    steepest_slope = max(abs((values_all[i + 1] - values_all[i]) / interval) for i in range(len(values_all) - 1))
    steepest_slope_corr = steepest_slope * 10**3
    

    if steepest_slope_corr > max_steepest_slope_corr:
        desired_height = (max_steepest_slope_corr / steepest_slope_corr) * desired_height
        if desired_height > max_gradient_height:
            desired_height = max_gradient_height

    num_triangles += 1

    if num_triangles > max_triangles:
        break

min_length = min(len(t_all), len(values_all))
t_all = t_all[:min_length]
values_all = values_all[:min_length]


step_t_all = []
step_values_all = []
for i in range(len(t_all) - 1):
    step_t_all.extend([t_all[i], t_all[i + 1]])
    step_values_all.extend([values_all[i], values_all[i]])


plt.plot(step_t_all, step_values_all)
plt.axhline(y=0, color='black', linewidth=0.8, linestyle='--')
plt.xlabel('Time (us)')
plt.ylabel('Value')
plt.title(f'Grad (Number of Repeats: {num_repeats}, Height: {desired_height:.6f} (mT/m))\nSteepest Slope: {steepest_slope_corr:.6f} (mT/m/ms)')
plt.grid(True)
plt.show()

print(f"Steepest Slope: {steepest_slope_corr:.6f}")


sampling_rate = 1.0
num_samples = len(values_all)
duration = num_samples / sampling_rate

time_array = np.linspace(0, duration, num_samples, endpoint=False)

fft_values = np.fft.fft(values_all)
freqs = np.fft.fftfreq(num_samples, d=1.0/sampling_rate)

step_time_array = []
for i in range(len(time_array) - 1):
    step_time_array.extend([time_array[i], time_array[i + 1]])


plt.figure(figsize=(10, 5))
plt.subplot(2, 1, 1)
plt.plot(step_time_array, values_all)
plt.xlabel('Time')
plt.ylabel('Amplitude')
plt.title('Gradients')

plt.subplot(2, 1, 2)
plt.plot(freqs, np.abs(fft_values))
plt.xlabel('Frequency (MHz)')
plt.ylabel('Magnitude')
plt.title('Fourier Transform')
plt.tight_layout()

plt.show()

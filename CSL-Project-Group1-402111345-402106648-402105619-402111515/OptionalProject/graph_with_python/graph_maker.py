import matplotlib.pyplot as plt
import numpy as np

# Data from the table
n = np.array([4, 8, 16, 32, 64, 128, 256])
plus = np.array([0.39, 2.1, 8.3, 66.2, 228.2, 1708, 13050])
normal = np.array([0.5, 3.5, 23.4, 101, 565, 4216, 34270])
c = np.array([0.28, 1.6, 11.3, 88, 711, 5600, 46100])

# Logarithmic transformation
log_n = np.log2(n)
log_plus = np.log10(plus)
log_normal = np.log10(normal)
log_c = np.log10(c)

# Linear regression in log space
coeff_plus = np.polyfit(log_n, log_plus, 1)  # Fit y = mx + b
coeff_normal = np.polyfit(log_n, log_normal, 1)
coeff_c = np.polyfit(log_n, log_c, 1)

# Generate best-fitting lines
fit_plus = coeff_plus[0] * log_n + coeff_plus[1]
fit_normal = coeff_normal[0] * log_n + coeff_normal[1]
fit_c = coeff_c[0] * log_n + coeff_c[1]

# Plot the data
plt.figure(figsize=(10, 6))
plt.scatter(n, plus, label='plus (data)', color='blue', marker='o')
plt.scatter(n, normal, label='normal (data)', color='green', marker='s')
plt.scatter(n, c, label='c (data)', color='red', marker='^')

# Plot the best-fitting lines
plt.plot(n, 10**fit_plus, label=f'plus (fit, slope={coeff_plus[0]:.2f})', color='blue', linestyle='--')
plt.plot(n, 10**fit_normal, label=f'normal (fit, slope={coeff_normal[0]:.2f})', color='green', linestyle='--')
plt.plot(n, 10**fit_c, label=f'c (fit, slope={coeff_c[0]:.2f})', color='red', linestyle='--')

# Logarithmic scale
plt.xscale('log', base=2)

# Labels and title
plt.xlabel('n (log scale)')
plt.ylabel('Values (log scale)')
plt.title('Logarithmic Plot with Best-Fitting Straight Lines')
plt.legend()
plt.grid(True, which="both", linestyle='--', linewidth=0.5)
plt.tight_layout()

# Show the plot
plt.show()

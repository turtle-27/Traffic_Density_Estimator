# Imports
import matplotlib.pyplot as plt
import numpy as np

runtime_array = []
error_array = []

runtime = 0.0
error = 0.0

plt.style.use('bmh')
# bmh, ggplot, seaborn, seaborn-darkgrid, seaborn-poster

fig1, ax1 = plt.subplots()
fig2, ax2 = plt.subplots()
fig3, (ax3, ax4) = plt.subplots(nrows = 2, ncols = 1)
fig4, (ax5, ax6) = plt.subplots(nrows = 2, ncols = 1)

f1 = open("utility_method1.dat", "r")
f2 = open("utility_method2.dat", "r")
f3 = open("utility_method3.dat", "r")
f4 = open("utility_method4.dat", "r")

# Method 1
for x in f1:
    runtime, error = map(float, x.replace('\n', '').split('\t'))
    runtime_array.append(runtime)
    error_array.append(error)

x_vals = runtime_array
y_vals = error_array

ax1.plot(x_vals, y_vals, label = 'Method 1')

ax1.set_xlabel('Runtime (in secs)')
ax1.set_ylabel('Error')
ax1.set_title('Error vs Runtime: Method 1')

# Method 2
runtime_array.clear()
error_array.clear()

for x in f2:
    runtime, error = map(float, x.replace('\n', '').split('\t'))
    runtime_array.append(runtime)
    error_array.append(error)

x_vals = runtime_array
y_vals = error_array

ax2.plot(x_vals, y_vals, label = 'Method 2')

ax2.set_xlabel('Runtime (in secs)')
ax2.set_ylabel('Utility')
ax2.set_title('Utility vs Runtime: Method 2')

# Method 3
runtime_array.clear()
error_array.clear()

for x in f3:
    runtime, error = map(float, x.replace('\n', '').split('\t'))
    runtime_array.append(runtime)
    error_array.append(error)

x_vals = runtime_array
y_vals = error_array

ax3.plot(x_vals, y_vals, label = 'Method 3: Utility vs Runtime')

ax3.set_xlabel('Runtime (in sec)')
ax3.set_ylabel('Utility')
ax3.set_title('Utility vs Runtime: Method 3')

x_vals = np.arange(1,11,1)
y_vals = runtime_array

ax4.plot(x_vals, y_vals, label = 'Method 3: Runtime vs No. of threads')

ax4.set_xlabel('No. of threads')
ax4.set_ylabel('Runtime (in sec)')
ax4.set_title('Runtime vs No. of threads: Method 3 ')

# Method 4
runtime_array.clear()
error_array.clear()

for x in f4:
    runtime, error = map(float, x.replace('\n', '').split('\t'))
    runtime_array.append(runtime)
    error_array.append(error)

x_vals = runtime_array
y_vals = error_array

ax5.plot(x_vals, y_vals, label = 'Method 4')

ax5.set_xlabel('Runtime (in sec)')
ax5.set_ylabel('Utility')
ax5.set_title('Utility vs Runtime: Method 4')

x_vals = np.arange(1,11,1)
y_vals = runtime_array

ax6.plot(x_vals, y_vals, label = 'Method 4: Runtime vs No. of threads')

ax6.set_xlabel('No. of threads')
ax6.set_ylabel('Runtime (in sec)')
ax6.set_title('Runtime vs No. of threads: Method 4')

plt.show()

f1.close()
f2.close()
f3.close()
f4.close()
# Imports
import matplotlib.pyplot as plt
import numpy as np

runtime_array = []
error_array = []

runtime = 0.0
error = 0.0

plt.style.use('bmh')
# bmh, ggplot, seaborn, seaborn-darkgrid, seaborn-poster

fig1, ax1 = plt.subplots() # Method 1: Utility v/s Runtime
fig2, (ax2, ax3) = plt.subplots(nrows = 2, ncols = 1, sharex = True) # Method 1: Utility and Runtime v/s parameter 

fig3, ax4 = plt.subplots() # Method 2: Utility v/s Runtime
fig4, (ax5, ax6) = plt.subplots(nrows = 2, ncols = 1, sharex = True) # Method 2: Utility and Runtime v/s parameter 

fig5, ax7 = plt.subplots() # Method 3: Utility v/s Runtime
fig6, (ax8, ax9) = plt.subplots(nrows = 2, ncols = 1, sharex = True) # Method 3: Utility and Runtime v/s parameter 

fig7, ax10 = plt.subplots() # Method 4: Utility v/s Runtime
fig8, (ax11, ax12) = plt.subplots(nrows = 2, ncols = 1, sharex = True) # Method 4: Utility and Runtime v/s parameter 

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

ax1.plot(x_vals, y_vals, label = 'Method 1: Utility vs Runtime')

ax1.set_xlabel('Runtime (in secs)')
ax1.set_ylabel('Utility')
ax1.set_title('Utility vs Runtime: Method 1')

x_vals = np.arange(1,11,1)

y_vals = error_array

ax2.plot(x_vals, y_vals, label = 'Method 1: Utility v/s Parameter')

ax2.set_ylabel('Utility')
ax2.set_title('Utility v/s Parameter: Method 1')

y_vals = runtime_array

ax3.plot(x_vals, y_vals, label = 'Method 1: Runtime v/s Parameter')

ax3.set_xlabel('Parameter: x')
ax3.set_ylabel('Runtime (in secs)')
ax3.set_title('Runtime v/s Parameter: Method 1')

# Method 2
runtime_array.clear()
error_array.clear()

for x in f2:
    runtime, error = map(float, x.replace('\n', '').split('\t'))
    runtime_array.append(runtime)
    error_array.append(error)

x_vals = runtime_array
y_vals = error_array

ax4.plot(x_vals, y_vals, label = 'Method 2')

ax4.set_xlabel('Runtime (in secs)')
ax4.set_ylabel('Utility')
ax4.set_title('Utility vs Runtime: Method 2')

x_vals = np.arange(1,8,1)

y_vals = error_array

ax5.plot(x_vals, y_vals, label = 'Method 2: Utility v/s Parameter')

ax5.set_ylabel('Utility')
ax5.set_title('Utility v/s Parameter: Method 2')

y_vals = runtime_array

ax6.plot(x_vals, y_vals, label = 'Method 2: Runtime v/s Parameter')

ax6.set_xlabel('Parameter: Resolution Factor')
ax6.set_ylabel('Runtime (in secs)')
ax6.set_title('Runtime v/s Parameter: Method 2')

# Method 3
runtime_array.clear()
error_array.clear()

for x in f3:
    runtime, error = map(float, x.replace('\n', '').split('\t'))
    runtime_array.append(runtime)
    error_array.append(error)

x_vals = runtime_array
y_vals = error_array

ax7.plot(x_vals, y_vals, label = 'Method 3: Utility vs Runtime')

ax7.set_xlabel('Runtime (in sec)')
ax7.set_ylabel('Utility')
ax7.set_title('Utility vs Runtime: Method 3')

x_vals = np.arange(1,11,1)

y_vals = error_array

ax8.plot(x_vals, y_vals, label = 'Method 3: Utility v/s Parameter')

ax8.set_ylabel('Utility')
ax8.set_title('Utility v/s Parameter: Method 3')

y_vals = runtime_array

ax9.plot(x_vals, y_vals, label = 'Method 3: Runtime v/s Parameter')

ax9.set_ylim([45, 55])
ax9.set_xlabel('Parameter: No. of threads')
ax9.set_ylabel('Runtime (in secs)')
ax9.set_title('Runtime v/s Parameter: Method 3')

# Method 4
runtime_array.clear()
error_array.clear()

for x in f4:
    runtime, error = map(float, x.replace('\n', '').split('\t'))
    runtime_array.append(runtime)
    error_array.append(error)

x_vals = runtime_array
y_vals = error_array

ax10.plot(x_vals, y_vals, label = 'Method 4')

ax10.set_ylim([0, 0.01])
ax10.set_xlabel('Runtime (in sec)')
ax10.set_ylabel('Utility')
ax10.set_title('Utility vs Runtime: Method 4')

x_vals = np.arange(1,11,1)

y_vals = error_array

ax11.plot(x_vals, y_vals, label = 'Method 4: Utility v/s Parameter')

ax11.set_ylim([0, 0.01])
ax11.set_ylabel('Utility')
ax11.set_title('Utility v/s Parameter: Method 4')

y_vals = runtime_array

ax12.plot(x_vals, y_vals, label = 'Method 4: Runtime v/s Parameter')

ax12.set_xlabel('Parameter: No. of threads')
ax12.set_ylabel('Runtime (in secs)')
ax12.set_title('Runtime v/s Parameter: Method 4')


plt.show()

fig1.savefig('Method1_util_runtime.png')
fig2.savefig('Method1_util_runtime_parameter.png')

fig3.savefig('Method2_util_runtime.png')
fig4.savefig('Method2_util_runtime_parameter.png')

fig5.savefig('Method3_util_runtime.png')
fig6.savefig('Method3_util_runtime_parameter.png')

fig7.savefig('Method4_util_runtime.png')
fig8.savefig('Method4_util_runtime_parameter.png')

f1.close()
f2.close()
f3.close()
f4.close()
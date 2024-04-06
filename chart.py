import serial
from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation
import datetime

# Initialize serial port - replace 'COM3' with your Arduino's serial port
ser = serial.Serial('COM3', 9600, timeout=1)
plt.style.use('seaborn-poster')  # for better graph aesthetics

# Create two subplots
fig, (ax1, ax2) = plt.subplots(2, 1)
fig.suptitle('Real-time data from Arduino')

x_data, temp_data, rad_data = [], [], []

def animate(i):
    line = ser.readline().decode('utf-8').strip()
    if line:  # If line isn't empty
        temp, rad = line.split(',')
        temp = float(temp)
        rad = float(rad)
        time_stamp = datetime.datetime.now()
        
        x_data.append(time_stamp)
        temp_data.append(temp)
        rad_data.append(rad)
        
        # Ensure we only show the last 50 data points to keep the graph readable
        x_data = x_data[-50:]
        temp_data = temp_data[-50:]
        rad_data = rad_data[-50:]

        ax1.clear()
        ax2.clear()

        ax1.plot(x_data, temp_data, label='Temperature')
        ax2.plot(x_data, rad_data, label='Radiation', color='r')

        ax1.set_xlabel('Time')
        ax1.set_ylabel('Temperature')
        ax2.set_xlabel('Time')
        ax2.set_ylabel('Radiation')

        ax1.legend()
        ax2.legend()

# Update the plot every 1000 ms
ani = FuncAnimation(fig, animate, interval=1000)

plt.tight_layout()
plt.show()

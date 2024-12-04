import serial
import time
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Function to initialize the serial connection
def initialize_serial(serial_port, baud_rate):
    try:
        ser = serial.Serial(serial_port, baud_rate, timeout=1)
        time.sleep(2)  # Wait for the serial connection to initialize
        return ser
    except serial.SerialException as e:
        print(f"Serial error: {e}")
        return None

# Function to send a command and receive the response
def send_command(ser, command):
    try:
        ser.write(command.encode('utf-8'))
        time.sleep(0.5)
        response = ser.readline().decode('utf-8').strip()
        response = response.replace(' ', '')  # Remove any spaces
        try:
            # Convert response to a float if possible
            return float(response)
        except ValueError:
            print(f"Non-numeric response received: {response}")
            return None
    except Exception as e:
        print(f"Error during send/receive: {e}")
        return None

# Function to update the plot
def update_plot(frame, ser, commands, x_data, y_data, lines, start_time, axes):
    for i, command in enumerate(commands):
        response = send_command(ser, command)
        if command == 'get_xadc\r' and response is not None:
            response -= 273
        if response is not None:
            print(f"Received for {command}: {response}")  # Print the received value for debugging
            x_data[i].append(time.time() - start_time)
            y_data[i].append(response)
            lines[i].set_data(x_data[i], y_data[i])
    for ax in axes:
        ax.relim()
        ax.autoscale_view()
    return lines

# Main function
def main(serial_port, baud_rate, commands, labels):
    ser = initialize_serial(serial_port, baud_rate)
    if ser is None:
        return

    # Initialize plot data for multiple series
    x_data = [[] for _ in commands]
    y_data = [[] for _ in commands]
    start_time = time.time()

    # Create subplots for different sensor groups
    global fig, axes  # Ensure these remain in scope
    fig, axes = plt.subplots(3, 1, figsize=(10, 15))

    # Acceleration plot (Accel X, Y, Z)
    accel_indices = [0, 1, 2]
    accel_lines = [axes[0].plot([], [], label=labels[i])[0] for i in accel_indices]
    axes[0].set_title('Acceleration Data')
    axes[0].set_xlabel('Time (s)')
    axes[0].set_ylabel('Acceleration (g)')
    axes[0].legend()

    # Temperature and Humidity plot (Humidity, Thermocouple Temp, XADC Temp)
    temp_humid_indices = [3, 4, 5]
    temp_humid_lines = [axes[1].plot([], [], label=labels[i])[0] for i in temp_humid_indices]
    axes[1].set_title('Temperature and Humidity Data')
    axes[1].set_xlabel('Time (s)')
    axes[1].set_ylabel('Temperature (°C)')
    ax2 = axes[1].twinx()  # Create a second y-axis for humidity
    ax2.set_ylabel('Humidity (%)')
    axes[1].legend()

    # Ambient Light plot (Ambient Light)
    light_indices = [6]
    light_lines = [axes[2].plot([], [], label=labels[i])[0] for i in light_indices]
    axes[2].set_title('Ambient Light Data')
    axes[2].set_xlabel('Time (s)')
    axes[2].set_ylabel('Light Intensity')
    axes[2].legend()

    # Combine all lines for updating
    lines = accel_lines + temp_humid_lines + light_lines

    # Set up plot to call update_plot function periodically
    global ani  # Ensure the animation object remains in scope
    ani = FuncAnimation(fig, update_plot, fargs=(ser, commands, x_data, y_data, lines, start_time, axes), interval=1000, blit=False, cache_frame_data=False)

    # Display the plot
    plt.tight_layout()
    plt.show()

    # Close the serial connection when the plot is closed
    ser.close()

if __name__ == "__main__":
    serial_port = 'COM59'  # Replace with your serial port
    baud_rate = 9600       # Replace with your baud rate
    commands = [
        'read_accel_x\r',
        'read_accel_y\r',
        'read_accel_z\r',
        'get_humidity\r',
        'get_temp\r',
        'get_xadc\r',
        'get_als\r'
    ]  # Commands to get data from the sensors
    labels = [
        'Accel X',
        'Accel Y',
        'Accel Z',
        'Humidity',
        'Thermocouple Temp',
        'XADC Temp',
        'Ambient Light'
    ]  # Labels for each data series
    try:
        main(serial_port, baud_rate, commands, labels)
    except KeyboardInterrupt:
        print("Interrupted!")

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
def update_plot(frame, ser, commands, x_data, y_data, lines, start_time):
    for i, command in enumerate(commands):
        response = send_command(ser, command)
        if response is not None:
            print(f"Received for {command}: {response}")  # Print the received value for debugging
            x_data[i].append(time.time() - start_time)
            y_data[i].append(response)
            lines[i].set_data(x_data[i], y_data[i])
    ax.relim()
    ax.autoscale_view()
    return lines

# Main function
def main(serial_port, baud_rate, commands):
    ser = initialize_serial(serial_port, baud_rate)
    if ser is None:
        return

    # Initialize plot data for multiple series
    x_data = [[] for _ in commands]
    y_data = [[] for _ in commands]
    start_time = time.time()

    # Create plot
    global fig, ax  # Ensure these remain in scope
    fig, ax = plt.subplots()
    lines = [ax.plot([], [], label=labels[i])[0] for i in range(len(commands))]
    ax.set_title('Real-Time Serial Data')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Received Value')
    ax.legend()

    # Set up plot to call update_plot function periodically
    global ani  # Ensure the animation object remains in scope
    ani = FuncAnimation(fig, update_plot, fargs=(ser, commands, x_data, y_data, lines, start_time), interval=1000, blit=False, cache_frame_data=False)

    # Display the plot
    plt.show()

    # Close the serial connection when the plot is closed
    ser.close()

if __name__ == "__main__":
    serial_port = 'COM18'  # Replace with your serial port
    baud_rate = 9600       # Replace with your baud rate
    commands = ['read_accel_x\r', 'read_accel_y\r', 'read_accel_z\r']  # Replace with the commands you want to send
    labels = ['Accel X', 'Accel Y', 'Accel Z']  # Labels for each data series
    try:
        main(serial_port, baud_rate, commands)
    except KeyboardInterrupt:
        print("Interrupted!")
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
        #print(f"Received: {response}")  # Print the received value for debugging
        if response.isdigit():
            response = int(response)
        elif response.replace('.', '', 1).isdigit():
            response = float(response)
        return float(response)
    except Exception as e:
        print(f"Error during send/receive: {e}")
        return None

# Function to update the plot
def update_plot(frame, ser, command, x_data, y_data, line, start_time):
    response = send_command(ser, command)
    #print("sending")
    if response is not None:
        #print(f"Received: {response}")  # Print the received value for debugging
        x_data.append(time.time() - start_time)
        y_data.append(response)
        line.set_data(x_data, y_data)
        ax.relim()
        ax.autoscale_view()
    return line,

# Main function
def main(serial_port, baud_rate, command):
    ser = initialize_serial(serial_port, baud_rate)
    if ser is None:
        return

    # Initialize plot data
    x_data, y_data = [], []
    start_time = time.time()

    # Create plot
    global fig, ax  # Ensure these remain in scope
    fig, ax = plt.subplots()
    line, = ax.plot([], [], 'r-')
    ax.set_title('Real-Time Serial Data')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Received Value')

    # Set up plot to call update_plot function periodically
    global ani  # Ensure the animation object remains in scope
    ani = FuncAnimation(fig, update_plot, fargs=(ser, command, x_data, y_data, line, start_time), interval=1000, blit=False, cache_frame_data=False)

    # Display the plot
    plt.show()

    # Close the serial connection when the plot is closed
    ser.close()

if __name__ == "__main__":
    serial_port = 'COM15'  # Replace with your serial port
    baud_rate = 9600       # Replace with your baud rate
    command = 'read_accel_z\r'  # Replace with the command you want to send

    try:
        main(serial_port, baud_rate, command)
    except KeyboardInterrupt:
        print("Interrupted!")

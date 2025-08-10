import serial
import time
import tkinter as tk
from tkinter import StringVar
from PIL import Image, ImageTk  # Import to handle images

# Setup Arduino connection (using RFC2217 for Wokwi)
arduino = serial.serial_for_url('rfc2217://localhost:4000', baudrate=9600)
time.sleep(2)

def send_expression_to_arduino(expression):
    """Send the expression to the Arduino simulator."""
    try:
        arduino.write(expression.encode())  # Send the expression
    except Exception as e:
        print(f"Error sending expression: {e}")

def update_from_arduino():
    """Read updates from Arduino and update the GUI."""
    try:
        if arduino.in_waiting:  # Check if there is data available from Arduino
            data = arduino.readline().decode().strip()
            if data.startswith("Expression:"):
                expression = data.split(":")[1].strip()
                expression_var.set(expression)
            elif data.startswith("Result:"):
                result = data.split(":")[1].strip()
                result_label.config(text=result)
    except Exception as e:
        print(f"Error reading from Arduino: {e}")
    app.after(100, update_from_arduino)  # Continuously check for updates

def update_expression():
    """Send the current expression to Arduino when Enter is pressed."""
    expression = expression_var.get()
    if expression:
        send_expression_to_arduino(expression + "\n")  # Send the expression followed by a newline

# Create the main application window
app = tk.Tk()
app.title("Binary Calculator")
app.geometry("517x700")
app.resizable(False, False)

# Load and set the background image
background_image_path = "CalculatorBG.png"
bg_image = Image.open(background_image_path)
bg_photo = ImageTk.PhotoImage(bg_image)

background_label = tk.Label(app, image=bg_photo)
background_label.place(relwidth=1, relheight=1)  # Make it cover the entire window

# Expression Variable and Entry
expression_var = StringVar()

# # Adjust layout to fit the calculator UI
# expression_label = tk.Label(
#     app, text="", font=("Arial", 14), bg="#111111", fg="#000000"
# )
# expression_label.place(x=400, y=100)  # Adjust based on your calculator image

expression_entry = tk.Entry(
    app, textvariable=expression_var, font=("Consolas", 18), width=17, bd=0, bg="#A0A180"
)
expression_entry.place(x=143, y=117)  # Adjust this position

expression_entry.bind("<Return>", lambda event: update_expression())

# Result Label
result_label = tk.Label(
    app, text="", font=("Consolas", 14), bg="#A0A180"
)
result_label.place(x=138, y=240)  # Adjust this position

# Start the process to read from Arduino
app.after(100, update_from_arduino)

# Run the application
app.mainloop()

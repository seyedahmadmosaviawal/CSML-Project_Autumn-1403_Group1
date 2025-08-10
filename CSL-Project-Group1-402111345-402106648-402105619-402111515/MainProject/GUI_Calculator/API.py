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
    root.after(100, update_from_arduino)  # Continuously check for updates

def update_expression():
    """Send the current expression to Arduino when Enter is pressed."""
    expression = expression_var.get()
    if expression:
        send_expression_to_arduino(expression + "\n")  # Send the expression followed by a newline

def button_click(value):
    entry_field.insert(tk.INSERT, value)
    update_expression()

def clear():
    entry_field.delete(0, tk.END)
    update_expression()

def move_cursor_left():
    pos = entry_field.index(tk.INSERT)
    if pos > 0:
        entry_field.icursor(pos - 1)
    update_expression()

def move_cursor_right():
    pos = entry_field.index(tk.INSERT)
    if pos < len(entry_field.get()):
        entry_field.icursor(pos + 1)
    update_expression()

def delete_character():
    pos = entry_field.index(tk.INSERT)
    if pos > 0:
        entry_field.delete(pos - 1)
    update_expression()

root = tk.Tk()
root.title("Binary Calculator")

# Expression Variable and Entry
expression_var = StringVar()

entry_field = tk.Entry(root, textvariable=expression_var, font=("Arial", 20), justify="right", bd=8, relief="ridge")
entry_field.grid(row=0, column=0, columnspan=4, ipadx=8, ipady=8)
entry_field.focus_set()

# Result Label
result_label = tk.Label(root, text="", font=("Arial", 20), justify="right", bd=8, relief="ridge", width=19)
result_label.grid(row=1, column=0, columnspan=4, ipadx=8, ipady=8)


buttons = [
    ('◀', 3, 0, "orange", "white"), ('▶', 3, 1, "orange", "white"), ('C', 3, 2, "red", "white"), ('⌫', 3, 3, "red", "white"),
    ('+', 4, 0, "blue", "white"), ('-', 4, 1, "blue", "white"), ('*', 4, 2, "blue", "white"), ('/', 4, 3, "blue", "white"),
    ('0', 5, 0, "grey", "black"), ('1', 5, 1, "grey", "black"), ('(', 5, 2, "black", "white"), (')', 5, 3, "black", "white"),
]

for (text, row, col, bg, fg) in buttons:
    if text == "◀":
        button = tk.Button(root, text=text, font=("Arial", 18), bg=bg, fg=fg,
                            width=5, height=2, command=move_cursor_left)
    elif text == "▶":
        button = tk.Button(root, text=text, font=("Arial", 18), bg=bg, fg=fg,
                            width=5, height=2, command=move_cursor_right)
    elif text == "◀":
        button = tk.Button(root, text=text, font=("Arial", 18), bg=bg, fg=fg,
                            width=5, height=2, command=move_cursor_left)
    elif text == "⌫":
        button = tk.Button(root, text=text, font=("Arial", 18), bg=bg, fg=fg,
                            width=5, height=2, command=delete_character)
    elif text == "C":
        button = tk.Button(root, text=text, font=("Arial", 18), bg=bg, fg=fg,
                            width=5, height=2, command=clear)
    else:
        button = tk.Button(root, text=text, font=("Arial", 18), bg=bg, fg=fg,
                            width=5, height=2, command=lambda t=text: button_click(t))

    button.grid(row=row, column=col, padx=5, pady=5)


root.after(100, update_from_arduino)

root.update_idletasks()
root.minsize(root.winfo_width(), root.winfo_height())
root.mainloop()

# Door Locker Security System – ATmega32 🔐
👋 Hello, everyone!

This repository contains the final project from my Embedded Software Basics Diploma at EDGES for Training.

📝 Project Summary
This project is a secure door locking system that uses password-based authentication, dual Electronic Control Units (ECUs), and an alarm mechanism for security enforcement.

🎯 The goal was to design a complete access control system using modular embedded architecture with real-time communication between ECUs.

🧰 Technologies & Components Used
Component/Feature	Purpose
🧠 MCU	ATmega32 Microcontroller
🧾 HMI ECU	Human-Machine Interface (Keypad + LCD)
⚙️ Control ECU	Handles logic, password validation, and control
🎛️ Keypad	User input for password entry
📺 LCD	Interface feedback
💾 EEPROM	Stores user credentials
🔊 Buzzer	Alarm for invalid access attempts
🚪 DC Motor	Controls door opening/closing
🔄 Protocols	UART (inter-ECU communication), I2C
⏱️ Timer	Delay and timeout handling

🔐 System Operation Overview
🔁 First-Time Setup
Enter a 5-digit password, then press ON/C to confirm.

Re-enter the same password for confirmation.

If the two passwords match:

The system saves the password to EEPROM

Displays: Password changed successfully

Stores 0xDEADBEEF as an initialization flag

🧭 Main Menu Options
Once a password is set, the system displays:

🔓 Open Door

🔄 Change Password

🧩 Operation Flow
User selects an option → System prompts for password

If correct:

🔓 Open Door: Motor opens the door, then automatically closes it

🔄 Change Password: Prompts for a new password and saves it

If incorrect password entered 3 times:

🚨 System locks

🔊 Alarm is triggered via buzzer

System delays input for a few seconds, then resets

🧪 All functionality is available in the attached Proteus simulation file.

📄 Documentation
📎 You can find the full requirements and simulation setup in the attached project files.

✅ Project Status
🎉 Completed
The system is fully functional and tested, with all features working as specified.

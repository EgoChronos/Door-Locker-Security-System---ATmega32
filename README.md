# Door-Locker-Security-System---ATmega32
Implemented a door locking mechanism using password authentication, 2 ECUs, and Alarm

Hallo, all.

This is a final project of my Embedded software basic diploma at "EDGES for training"

A summery of the project: it is a secureity door locking system with authentication protocol and alarm.

the system works with 2 ECUs: HMI which will handle all the interface and user requests and send it to CONTROL to check if valid or not and handle data.
  
  System operation overview:

    If it is your first time:
    
    Enter a 5-digit password and press the ON/c button to confirm.
    
    Retype the same password and press ON/c again.
    
    If the passwords don’t match, the system will reject it and ask you to re-enter the password.
    
    When the passwords match, the system will display "Password changed successfully," save it in the EEPROM, and store DEADBEAF for future reference.
    
    From this point onward, the app will offer two options:
    
    Open Door
    
    Change Password
    
    If you select either option, the app will ask for your password.
    
    If the password is correct, the app will proceed with your chosen action:
    
    Open Door: The motor will turn to open the door, then automatically close it after a few seconds.
    
    Change Password: You can set a new password.
    
    After completing either action, the app will return to the main screen with the same two options.
    
    If the password is entered incorrectly three times in a row, the system will lock and trigger an alarm.
    
    The system will remain locked for a few seconds before returning to its normal state.
    
    You can test all these features in the attached Proteus simulation!


o	Utilized: GPIO, Keypad, LCD, Timer, UART, I2C, EEPROM, Buzzer, and DC Motor.

o Microcontroller: ATmega32.

You can read the full requirements in the attached file.

Thank you.

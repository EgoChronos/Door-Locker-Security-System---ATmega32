# Door-Locker-Security-System---ATmega32
Implemented a door locking mechanism using password authentication, 2 ECUs, and Alarm

Hallo, all.

This is a final project of my Embedded software basic diploma at "EDGES for training"

A summery of the project: it is a secureity door locking system with authentication protocol and alarm.
the system works with 2 ECUs: HMI which will handle all the interface and user requests and send it to CONTROL to check if valid or not and handle data.
  
  The securtiy protocol is as follows:
  -the system checks if it is your first time using a DEADBEAF.
  -If it is your first time you shall enter a password of five digits then press the button (ON/c) for confirmation.
  -You shall retype the same password again and press (ON/c)
  -if the password did not match the retyped one system will not accept the password and will ask you for password again.
  -when it mtaches, it will show you password changed successfully and will save it in the EEBPROM and will save DEADBEAF for future reference.
  -the app from this point forward will give the user only 2 options: 1.to Open Door or 2.Change Password
  -if you chose any option the app will ask you for the password.
  -if the password is correct then you app will carry on with your previous choice.
  -the door will open by turning the motor then closes seconds after that automatically.
  -or, you can change the password.
  -if you are done you will be back to the orginial screen which gives you the same 2 options 1.to Open Door or 2.Change Password
  -if the password was entered incorrectly at any point of time for 3 consecutive times the system will enter an a locked state and an alarm will start.
  -The system will stay in the locked state for seconds before return back to normal state.
  -You can simply try all those options on the attached Proteus simulation!

o	Utilized: GPIO, Keypad, LCD, Timer, UART, I2C, EEPROM, Buzzer, and DC Motor.

o Microcontroller: ATmega32.

You can read the full requirments in the attached file.

Thank you.

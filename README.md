# Pulse-Width-Modulation
Use Port Interrupts with PWM to sweep a single bladed arm of a SG90 Servo Motor

This is a C code that configures a microcontroller to control a servo motor and a piezo buzzer. The code sets up the input pins and pull-up resistors, interrupts, PWM output pin for the servo, and initializes the LCD screen.

When an interrupt is triggered by pressing a button, a function is called that controls the servo to rotate for one cycle, play a tone on the buzzer, and display a message on the LCD screen. Another button triggers a function that repeatedly cycles the servo between two positions while playing a tone on the buzzer and displaying a message on the LCD screen, until a third button is pressed, which exits the loop.

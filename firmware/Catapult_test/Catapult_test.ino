/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
#include "Arduino.h"
#include "nokia5110.h"
#include <Servo.h> 
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 8;
int btn = 12;
int pot1 = A1;
int pot2 = A2;
int sensor = A3;

Servo servo1;     // create servo object to control a servo 
Servo servo2;     // create servo object to control a servo 
Servo servo3;     // create servo object to control a servo 

int power = 0;        // power to send to ESC, normilied to 0 - 100
int old_power=0;      // old value of power
int duration = 0;     // ESC power up cycle duration

int timeServoRemain = 0;
int timeButtonRemain = 0;

unsigned int updateData = 0;


// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  
  // initialize button pin
  pinMode(btn, INPUT);
  digitalWrite(btn, HIGH);
  
  // initialize sensor pin
  pinMode(sensor, INPUT);
  digitalWrite(sensor, HIGH);

  //output to LCD
  Nokia5110::Initialise();
  Nokia5110::Clear();
  Nokia5110::PrintString("Catapult");
  Nokia5110::gotoXY(0, 1);
  Nokia5110::PrintString("Test Program");

  Nokia5110::gotoXY(0, 3);
  Nokia5110::PrintString("Initializing ");
  
  //init servo objects
  servo1.attach(9);  // attaches the servo1 on pin 9 to the servo object 
  servo2.attach(10);  // attaches the servo2 on pin 10 to the servo object 
  servo3.attach(11);  // attaches the servo3 on pin 11 to the servo object 
  
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Catapult test!\n");
}

void runServo(int a_power)
{
    if(old_power != a_power) {
        old_power = a_power;
        int val = map(a_power, 0, 100, 90, 179);   // scale it to use it with the servo (value between 0 and 180) 

        servo1.write(val);  // sets the servo position according to the scaled value 
        servo2.write(val);  // sets the servo position according to the scaled value 
        servo3.write(val);  // sets the servo position according to the scaled value 
    }
}

void stopServo()
{
    runServo(0);
}

void readPots()
{
    int _val1 = analogRead(pot1);         // reads the value of the potentiometer (value between 0 and 1023) 
    int _val2 = analogRead(pot2);         // reads the value of the potentiometer (value between 0 and 1023) 
    
//    Serial.print("\n");
//    Serial.print("pot1 = ");
//    Serial.print(val1);
//    Serial.print("\n");
    
//    Serial.print("pot2 = ");
//    Serial.print(val2);
//    Serial.print("\n");
    
    if(_val1 > 270) {
        _val1 = 270;
    }
    _val1 = map(_val1, 0, 270, 0, 100);   // scale it to use it with the servo (value between 0 and 180) 
    
    if(((power - _val1)>2) || ((_val1 - power)>2)) {
        power = 5*((int)(_val1/5));
    }
    

    if(((duration - _val2)>5) || ((_val2 - duration)>5)) {
        duration = 10*(_val2/10);
    }
}

void displaySensorError()
{
    Nokia5110::gotoXY(0, 3);
    Nokia5110::PrintString("            ");
    Nokia5110::gotoXY(0, 4);
    Nokia5110::PrintString("   SENSOR   ");
}


void displayData()
{
    char buf[15];
    Nokia5110::gotoXY(0, 3);
    sprintf(buf, "Power: %3d%% ", power);
    Nokia5110::PrintString(buf);
    
    Nokia5110::gotoXY(0, 4);
    sprintf(buf, "Time: %4dms", duration);
    Nokia5110::PrintString(buf);
}

// the loop routine runs over and over again forever:
void loop() 
{
    boolean bSensor = (LOW == digitalRead(sensor));
    
    boolean bServoRun = false;
    int LEDStatus = LOW;
    
    if(timeServoRemain <= 0) {
        if(timeButtonRemain <=0) {
            if(0 == (updateData++)%100) {
                readPots();
                displayData();
            }
            LEDStatus = HIGH;
            
            //if button pressed - run a cycle again
            boolean bButton = (LOW == digitalRead(btn));
            if(bButton) {
                timeServoRemain = duration;
                timeButtonRemain = 500;
            }
            
        } else {
            timeButtonRemain--;
        }
    }
    
    if(bSensor) {
        stopServo();
        LEDStatus = LOW;
        bServoRun = false;
        timeServoRemain = 0;
        timeButtonRemain = 0;
        displaySensorError();
        delay(50);
        
    } else {
        if(timeServoRemain > 0) {
            timeServoRemain--;
            LEDStatus = LOW;
            bServoRun = true;
        }  
    }
    
    digitalWrite(led, LEDStatus);
    if(bServoRun) {
        runServo(power);
    } else {
        stopServo();
    }
    
    delay(1); // wait for a milisecond
}

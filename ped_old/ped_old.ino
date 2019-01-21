
/*
2018-09-30
Modified by V.Palleschi from the original by Salvatore Fancello,
http://www.progettiarduino.com
e-mail: salvatorefancello89@gmail.com
e-mail: vpalleschi@gmail.com
https://www.thingiverse.com/thing:3119066
*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SoftwareSerial.h>



#define SERVOMIN 180 //
#define SERVOMAX 420 //
#define SLOW 50 //
#define FAST 10 //

#define bluetooth Serial

SoftwareSerial mySerial(0, 1); // Pins for serial communication
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);

void setup()
{
pwm.begin();
pwm1.begin();
pwm1.setPWMFreq(60); // Analog servos run at ~60 Hz updates
pwm.setPWMFreq(60); // Analog servos run at ~60 Hz updates

bluetooth.begin(9600);
mySerial.begin(9600); // Start communication

iniz();
}

void servocontrol(int n, int a) // n= servo number a= angle
{

if (n > 8){
a = map(a, 0, 180, SERVOMIN, SERVOMAX);
pwm.setPWM(n-9, 0, a);}
else {
a = map(a, 180, 0, SERVOMIN, SERVOMAX);
pwm1.setPWM(n, 0, a);}
}

void moveleg(int n, int an, int f, int t, int w)
{
yield();
if (t >=0) {servocontrol(3*(n-1)+2, t);};
delay(w);
if (f >=0) {servocontrol(3*(n-1)+1, f);};
delay(w);
if (an >=0) {servocontrol(3*(n-1), an);};
delay(w);
}

void stp(int n, int a, int w)
{
yield();

 servocontrol(3*(n-1)+1, 140); 
 delay(w);
 servocontrol(3*(n-1)+2, 40);
 delay(w);
 servocontrol(3*(n-1), a);
 delay(w);
 servocontrol(3*(n-1)+2, 90);
 delay(w);
 servocontrol(3*(n-1)+1, 90);

}

void mov(int n, int a, int w)
{
moveleg(n,a,-1,-1, w);
}

void iniz()
{
moveleg(1,90,90,90, FAST);
moveleg(2,90,90,90, FAST);
moveleg(3,90,90,90, FAST);
moveleg(4,90,90,90, FAST);
moveleg(5,90,90,90, FAST);
moveleg(6,90,90,90, FAST);
}

void stand()
{
moveleg(1,-1,50,80, FAST);
moveleg(2,-1,50,80, FAST);
moveleg(3,-1,50,80, FAST);
moveleg(4,-1,35,80, FAST);
moveleg(5,-1,50,80, FAST);
moveleg(6,-1,50,80, FAST);
}

void squat()
{
moveleg(1,-1,130,100, FAST);
moveleg(2,-1,130,100, FAST);
moveleg(3,-1,130,100, FAST);
moveleg(4,-1,130,100, FAST);
moveleg(5,-1,130,100, FAST);
moveleg(6,-1,130,100, FAST);
}

void wave_r(){

for (int j=0; j< 3; j++){
moveleg(6,-1,160,0, SLOW);
for (int i=30; i <= 110; i++){
moveleg(6,i,-1,-1, FAST);
}
for (int i=110; i >= 20; i--){
moveleg(6,i,-1,-1, FAST);
}
}
moveleg(6,90,90,90, SLOW);
}

void wave_l(){
for (int j=0; j< 3; j++){
moveleg(1,-1,160,0, SLOW);
for (int i=30; i <= 110; i++){
moveleg(1,i,-1,-1, FAST);
}
for (int i=110; i >= 20; i--){
moveleg(1,i,-1,-1, FAST);
}
}
moveleg(1,90,90,90, SLOW);
}

void skew_r(){
moveleg(1,20,-1,-1, FAST);
moveleg(2,20,-1,-1, FAST);
moveleg(3,20,-1,-1, FAST);
moveleg(4,160,-1,-1, FAST);
moveleg(5,160,-1,-1, FAST);
moveleg(6,160,-1,-1, FAST);
}
void skew_l(){
moveleg(1,160,-1,-1, FAST);
moveleg(2,160,-1,-1, FAST);
moveleg(3,160,-1,-1, FAST);
moveleg(4,20,-1,-1, FAST);
moveleg(5,20,-1,-1, FAST);
moveleg(6,20,-1,-1, FAST);
}
void courtsy(){
moveleg(1,-1,140,80, FAST);
moveleg(6,-1,140,80, FAST);
moveleg(2,-1,-1,80, FAST);
moveleg(5,-1,-1,80, FAST);
moveleg(3,-1,40,80, FAST);
moveleg(4,-1,40,80, FAST);

}
void prepare_jump(){
moveleg(1,-1,40,80, FAST);
moveleg(6,-1,40,80, FAST);
moveleg(2,-1,-1,80, FAST);
moveleg(5,-1,-1,80, FAST);
moveleg(3,-1,140,80, FAST);
moveleg(4,-1,140,80, FAST);
}

void rotate_r(){
char dat='X';
while (dat !='E'){
dat = bluetooth.read();
stp(1,160, SLOW);
stp(3,160, SLOW);
stp(5,20, SLOW);
stp(2,160, SLOW);
stp(4,20, SLOW);
stp(6,20, SLOW);
delay(50);
mov(6,90, FAST);
mov(5,90, FAST);
mov(4,90, FAST);
mov(3,90, FAST);
mov(2,90, FAST);
mov(1,90, FAST);
}
}

void rotate_l(){
char dat='X';
while (dat !='E'){
dat = bluetooth.read();
stp(1,20, SLOW);
stp(3,20, SLOW);
stp(5,160, SLOW);
stp(2,20, SLOW);
stp(4,160, SLOW);
stp(6,160, SLOW);
delay(50);
mov(1,90, FAST);
mov(2,90, FAST);
mov(3,90, FAST);
mov(4,90, FAST);
mov(5,90, FAST);
mov(6,90, FAST);
}
}

void forward(){
yield();
char dat='X';
while (dat !='E'){
dat = bluetooth.read();
mov(6,50, FAST);
mov(1,50, FAST);
mov(2,50, FAST);
mov(5,50, FAST);
mov(4,50, FAST);
mov(3,50, FAST);
stp(1,130, SLOW);
stp(6,130, SLOW);
stp(5,130, SLOW);
stp(2,130, SLOW);
stp(3,130, SLOW);
stp(4,130, SLOW);
}
delay(SLOW);
iniz();
}

void backward(){
yield();
char dat='X';
while (dat !='E'){
dat = bluetooth.read();
mov(3,130, FAST);
mov(4,130, FAST);
mov(5,130, FAST);
mov(2,130, FAST);
mov(1,130, FAST);
mov(6,130, FAST);
stp(4,50, SLOW);
stp(3,50, SLOW);
stp(2,50, SLOW);
stp(5,50, SLOW);
stp(6,50, SLOW);
stp(1,50, SLOW);
}
delay(SLOW);
iniz();
}

void loop()
{
char olddato;
while (bluetooth.available())
{
char dato = bluetooth.read(); // read Bluetooth character

   switch(dato)
   {
       case 'A': 
     {
      if (olddato == 'E') stand();
      else forward();
      olddato=dato;
       break;
     }
      case 'B': 
     {
      if (olddato == 'E') squat();
      else backward();
      olddato=dato;
       break;
     }
      case 'C': 
     {
      if (olddato == 'E') courtsy();  
      else skew_r();
      olddato=dato;
       break;
     }
     case 'D': 
     {
      if (olddato == 'E') prepare_jump(); 
      else skew_l();
      olddato=dato;
       break;
     }  
      case 'G': 
     {
      if (olddato == 'E') wave_l();
      else rotate_l() ;
      olddato=dato;
      break;
     }
     case 'F': // 
     {
       if (olddato == 'E') wave_r();
       else rotate_r();
       olddato=dato;
       break;
     }  
     case 'E': 
     {
      if (olddato == 'E') {
        iniz();
        olddato = ' '; // Larry Change from "" to ' '
      }
      else olddato=dato;
       break;
     }  

    }
   }

}

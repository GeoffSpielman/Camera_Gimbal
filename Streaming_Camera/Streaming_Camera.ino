#include <Servo.h>
const int LED = 13;

Servo pitchServo;
const int MAX_PITCH = 2230; 
const int MIN_PITCH = 760;
const int REST_PITCH = 1495;
const int INTERVAL_PITCH = 10;
int curPitch = 1495;

int lastLEDSwitch = millis();
int lastPitchMove = millis();
boolean lightOn = true;
String recString = "";

int cameraMoveMode = 5;
/* Pan Modes:
 *  1 2 3           Up-Left     Up      Up-Right
 *  4 5 6     =     Left        Idle    Right
 *  7 8 9           Down-Left   Down    Down-Right
 */
void setup() {
  Serial.begin(9600);  
  pinMode(LED, OUTPUT);
  pitchServo.attach(9); 
  pitchServo.writeMicroseconds(REST_PITCH);     
}


void loop() {

    //LED heartbeat
   if ((millis() - lastLEDSwitch) > 1000){
     lastLEDSwitch = millis();
     if (lightOn){
       digitalWrite(LED, LOW);
     }
     else{
       digitalWrite(LED, HIGH);
     }
     lightOn = !lightOn;
   }

   //Camera pitch
   if ((cameraMoveMode != 5) &&(millis() - lastPitchMove > INTERVAL_PITCH)){
       if (cameraMoveMode == 2){
          curPitch -= 1;
          if (curPitch < MIN_PITCH){
            curPitch = MIN_PITCH;
          }
          pitchServo.writeMicroseconds(curPitch);
           
       }
       if (cameraMoveMode == 8){
          curPitch += 1;
          if (curPitch > MAX_PITCH){
            curPitch = MAX_PITCH;
          }
          pitchServo.writeMicroseconds(curPitch);
       }
       lastPitchMove = millis();
   }
   
}

void serialEvent()
{
  while (Serial.available())
  {  
    char inChar = (char)Serial.read();
    if (inChar != '~'){
      recString += inChar;
    }
    else{
      cameraMoveMode = recString.toInt();
      recString = "";
    }
  }
}


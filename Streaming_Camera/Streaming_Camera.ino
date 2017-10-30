#include <Servo.h>
const int LED = 13;
const int PITCH_SERVO = 9;
const int YAW_SERVO = 6;


const int MAX_PITCH = 2230; 
const int MIN_PITCH = 760;
const int REST_PITCH = 1495;
const int INTERVAL_PITCH = 20;
const int INCREMENT_PITCH = 1;
int curPitch = 1495;

//most constants are 'good to know' and for future use - not necessary right now
const int MAX_CCW_YAW = 1600;
const int MIN_CCW_YAW = 1508;
const int MAX_CW_YAW = 1380;
const int MIN_CW_YAW = 1455;
const int REST_YAW = 1480;
const int INCREMENT_YAW = 1;
int curYaw = 1480;


int lastLEDSwitch = millis();
unsigned long lastPitchMove = millis();
boolean lightOn = true;
String recString = "";
String cameraCommand = "";
String previousCameraCommand = "initialization";
bool valuesHaveChanged = false;

Servo pitchServo;
Servo yawServo;
void setup() {
  Serial.begin(9600);  
  pinMode(LED, OUTPUT);
  pitchServo.attach(PITCH_SERVO); 
  pitchServo.writeMicroseconds(REST_PITCH);
  yawServo.attach(YAW_SERVO);
  yawServo.writeMicroseconds(REST_YAW);     
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
   if ((cameraCommand != "Idle") && (millis() - lastPitchMove > INTERVAL_PITCH)){
       if (cameraCommand == "Up" || cameraCommand == "Up-Left" || cameraCommand == "Up-Right"){
          if (curPitch + INCREMENT_PITCH < MAX_PITCH){
             curPitch += INCREMENT_PITCH;
             valuesHaveChanged = true;
          }
       }
       else if (cameraCommand == "Down" || cameraCommand == "Down-Left" || cameraCommand == "Down-Right"){
          if(curPitch - INCREMENT_PITCH > MIN_PITCH){
            curPitch -= INCREMENT_PITCH;
            valuesHaveChanged = true;
          }       
       }
       pitchServo.writeMicroseconds(curPitch);
       lastPitchMove = millis();      
   }

   //Camera yaw
   if (cameraCommand != previousCameraCommand){
      if (cameraCommand == "Left" || cameraCommand == "Up-Left" || cameraCommand == "Down-Left"){
        curYaw = MIN_CCW_YAW + INCREMENT_YAW;
        valuesHaveChanged = true;
      }
      else if (cameraCommand == "Right" || cameraCommand == "Up-Right" || cameraCommand == "Down-Right"){
        curYaw = MIN_CW_YAW - INCREMENT_YAW;
        valuesHaveChanged = true;
      }
      else{
        curYaw = REST_YAW;
      }
      previousCameraCommand = cameraCommand;
      yawServo.writeMicroseconds(curYaw);
   }

   //provide feedback to UI
   if (valuesHaveChanged){
      Serial.println(String(curPitch) + "," + String(5));
      valuesHaveChanged = false;
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
      cameraCommand = recString;
      recString = "";
    }
  }
}


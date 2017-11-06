#include <Servo.h>
const int PITCH_SERVO = 9;
const int YAW_SERVO = 6;


const int MAX_PITCH = 2230; 
const int MIN_PITCH = 760;
const int REST_PITCH = 1430;
const int INTERVAL_PITCH = 20;
const int INCREMENT_PITCH = 1;
const double PITCH_DEGREES_PER_MICROSECOND = 0.11;
int curPitch = 1430;

//most constants are 'good to know' and for future use - not necessary right now
const int MAX_CCW_YAW = 1600;
const int MIN_CCW_YAW = 1508;
const int MAX_CW_YAW = 1380;
const int MIN_CW_YAW = 1455;
const int REST_YAW = 1480;
const int INCREMENT_YAW = 8;
const double YAW_DEGREES_PER_MICROSECOND = 0.005294;
int curYaw = 1480;
unsigned long yawMotionTimeStart;
double yawMotionValueStart;
bool yawCurrentlySpinning = false;

unsigned long lastPitchMove = millis();
String recString = "";
String cameraCommand = "";
String previousCameraCommand = "initialization";
bool valuesHaveChanged = false;

Servo pitchServo;
Servo yawServo;
void setup() {
  Serial.begin(9600);  
  pitchServo.attach(PITCH_SERVO); 
  pitchServo.writeMicroseconds(REST_PITCH);
  yawServo.attach(YAW_SERVO);
  yawServo.writeMicroseconds(REST_YAW);     
}


void loop() {


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
    if (previousCameraCommand != cameraCommand){
        if (cameraCommand == "Left" || cameraCommand == "Up-Left" || cameraCommand == "Down-Left"){
            curYaw = MIN_CCW_YAW + INCREMENT_YAW;
            yawMotionTimeStart = millis();
            yawCurrentlySpinning = true;
        }
        else if (cameraCommand == "Right" || cameraCommand == "Up-Right" || cameraCommand == "Down-Right"){
            curYaw = MIN_CW_YAW - INCREMENT_YAW;
            yawMotionTimeStart = millis();
            yawCurrentlySpinning = true;
        }
        else{
            curYaw = REST_YAW;
            yawCurrentlySpinning = false;
            //Yaw servo takes 34 seconds to complete 180° at the current speed (we'll need to recalibrate later)
            yawMotionValueStart = (millis() - yawMotionTimeStart)*180/34000.0;
        }
        yawServo.writeMicroseconds(curYaw);
        previousCameraCommand = cameraCommand;
    }
    
   //provide feedback to UI
   if (valuesHaveChanged || yawCurrentlySpinning){    
      //Right now 1430 seems level - I'll call that 0 degrees for now but we need to recalibrate later
      //Pitch servo: 0.079°/μsec according to data sheet
      Serial.print((curPitch - 1430)*PITCH_DEGREES_PER_MICROSECOND, 4);
      Serial.print(",");
      if (yawCurrentlySpinning){
          if(curYaw == MIN_CCW_YAW + INCREMENT_YAW){
            Serial.print(yawMotionValueStart + ((millis() - yawMotionTimeStart)*YAW_DEGREES_PER_MICROSECOND), 4);
          }
          else{
            Serial.print(yawMotionValueStart - ((millis() - yawMotionTimeStart)*YAW_DEGREES_PER_MICROSECOND), 4);
          }
      }
      else{
        Serial.print(yawMotionValueStart, 4); 
      }
      Serial.println("|");
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


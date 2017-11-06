#include <Servo.h>
const int PITCH_SERVO = 9;
const int YAW_SERVO = 6;


const int MAX_PITCH = 2230; 
const int MIN_PITCH = 760;
const int REST_PITCH = 1430;
const int INTERVAL_PITCH = 20;
const int INCREMENT_PITCH = 1;
int curPitch = 1430;

//most constants are 'good to know' and for future use - not necessary right now
const int MAX_CCW_YAW = 1600;
const int MIN_CCW_YAW = 1508;
const int MAX_CW_YAW = 1380;
const int MIN_CW_YAW = 1455;
const int REST_YAW = 1480;
const int INCREMENT_YAW = 2;
int curYaw = 1480;
unsigned long yawTimeLastUpdate;
bool yawCurrentlySpinning = false;
float yawCurrentHeading = 0;

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

   //Camera yaw -- //Yaw servo takes 47.47 seconds to complete 180° at the current speed (we'll need to recalibrate later)
    if (cameraCommand == "Left" || cameraCommand == "Up-Left" || cameraCommand == "Down-Left"){
      if(!yawCurrentlySpinning){
        curYaw = MIN_CCW_YAW + INCREMENT_YAW;
        yawTimeLastUpdate = millis();
        yawCurrentlySpinning = true;
      }
      valuesHaveChanged = true;
      yawCurrentHeading += (millis() - yawTimeLastUpdate)*180/47470.0;
      yawTimeLastUpdate = millis();
    }
    else if (cameraCommand == "Right" || cameraCommand == "Up-Right" || cameraCommand == "Down-Right"){
      if(!yawCurrentlySpinning){
            curYaw = MIN_CW_YAW - INCREMENT_YAW;
            yawTimeLastUpdate = millis();
            yawCurrentlySpinning = true;
      }
      valuesHaveChanged = true;
      yawCurrentHeading -= (millis() - yawTimeLastUpdate)*180/47470.0;
      yawTimeLastUpdate = millis();  
    }
    else{
      curYaw = REST_YAW;
      yawCurrentlySpinning = false;
    }
    
    if (previousCameraCommand != cameraCommand){
      yawServo.writeMicroseconds(curYaw);
    }
    
 


   //provide feedback to UI
   if (valuesHaveChanged || yawCurrentlySpinning){
      //Right now 1430 seems level - I'll call that 0 degrees for now but we need to recalibrate later
      //Pitch servo: 0.079°/μsec according to data sheet
      Serial.print((curPitch - 1430)*0.079, 4);
      Serial.print(",");
      Serial.print(yawCurrentHeading, 4);
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


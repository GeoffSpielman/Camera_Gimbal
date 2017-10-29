int led = 13;
int lastSwitch = millis();
boolean lightOn = true;
boolean stayOn = false;

String recString = "";


void setup() {
Serial.begin(9600);  
pinMode(led, OUTPUT);     
}

// the loop routine runs over and over again forever:
void loop() {
    
   if (((millis() - lastSwitch) > 1000) && !stayOn){
     lastSwitch = millis();
     if (lightOn){
       digitalWrite(led, LOW);
     }
     else{
       digitalWrite(led, HIGH);
     }
     lightOn = !lightOn;
   }
}

void serialEvent()
{
  while (Serial.available())
  {  
    char inChar = (char)Serial.read();
    recString += inChar;
    
    if (inChar == '~')
    {
      int endIndex = recString.indexOf("~");
      //start index = inclusive, end index = exlusive (this removes tilde)
      recString = recString.substring(0, endIndex);
     
      if (recString == "Left"){
        stayOn = true;
        digitalWrite(led, HIGH);
        recString = "";     
      }
      else{
        recString = "";
      }
    }
  }
}

#include <DS3232RTC.h>  // RTC library https://github.com/JChristensen/ Time library https://github.com/PaulStoffregen/Time
#include <string.h>
#define BUTTON 7
#define MOTOR 4
#define PROX 5

DS3232RTC myRTC;


/*
*Notes:
*It will stop if it is powered off after the alarm is triggered until next alarm is triggerd.
*Try not to perform alignment once the alarm is triggered.
*/

/*
******************** Press the push button when uploading the code to update the current time, alarm and other updated variables. *************************************
***********************************************************************************************************************************************************************
***********************************************************************************************************************************************************************
********************** Use 24 HR format to set the Alarm.**************************************************************************************************************
********************** Alarm_Hour ---> set alarm hour.*****************************************************************************************************************
********************** Alarm_Minute ---> set alarm minute.*************************************************************************************************************
********************** Alarm_Second ---> set alarm second. ************************************************************************************************************
********************** rotations ---> It will stop rotating after is has completed specified number of rotations until next alarm is triggered.************************
********************** interval (in milliseconds) ---> set the interval time. After each interval time trap will rotate. 1 HR = 3600 seconds = 3600000 milliseconds.***
**********************************************************************************************************************************************************************/
//=====================================================================================================================================================================
/*********************************/             int Alarm_Hour = 17, Alarm_Minute = 13, Alarm_Second = 0;                 //===========================================
/*********************************/             int rotations = 23;                                                       //===========================================
/*********************************/             unsigned long interval = 3600000;                                         //===========================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================

int n_rotations = 0;
bool trap = false;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  myRTC.begin();
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(PROX, INPUT);
  pinMode(MOTOR, OUTPUT);

  if(digitalRead(BUTTON) == LOW) {
    digitalWrite(13, HIGH);
    set_time();
    delay(3000);
    digitalWrite(13, LOW);
  }


}

void loop() {

  time_t t;
  unsigned long currentMillis = millis();


  t = myRTC.get();
  Serial.println("Time:"+String(hour(t)) + ":" + String(minute(t)) + ":" + String(second(t)) + " Date:" + String(day(t)) + "/" + String(month(t)) + "/" + String(year(t)));


  if(hour(t) == Alarm_Hour && minute(t) == Alarm_Minute && second(t) < 1){
    Serial.println("Alarm Triggered at: "+String(hour(t)) + ":" + String(minute(t)) + ":" + String(second(t)));
    digitalWrite(13, HIGH);
    trap = true;
    previousMillis = millis();
    delay(3000);
    digitalWrite(13, LOW);
  }

  if(digitalRead(BUTTON) == LOW){
    //rotate to adjust
    digitalWrite(MOTOR, HIGH);
    delay(100);
    digitalWrite(MOTOR, LOW);
  }

  if(trap==true){

    if(currentMillis - previousMillis > interval) {
      //rotate until next magnet hits prox
      previousMillis = millis();
      digitalWrite(MOTOR, HIGH);
      delay(1000);
      while(digitalRead(PROX)==LOW){
        delay(50);
      }
      digitalWrite(MOTOR, LOW);
      n_rotations++;
      if(n_rotations>=rotations) {trap=false; n_rotations=0;}

    }

  }


  delay(1000);

}

uint8_t mmmtoi(String mmm){

  String monthstrings[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", ""};

  for(int i=0; monthstrings[i]!=""; i++) if(strcmp(mmm.c_str(),monthstrings[i].c_str()) == 0) return i+1;

  return 1;

}

void set_time(){
  //set current time
  String time = __TIME__ ;
  String date = __DATE__ ;
  tmElements_t tm,t;
  
  t.Second = 9;
  tm.Hour = atoi(time.substring(0,2).c_str());
  tm.Minute = atoi(time.substring(3,5).c_str());
  tm.Second = atoi(time.substring(6,8).c_str());
  tm.Second = t.Second + tm.Second;
  tm.Month = mmmtoi(date.substring(0,3).c_str());
  tm.Day = atoi(date.substring(4,6).c_str());
  tm.Year = atoi(date.substring(7,11).c_str()) - 1970;

  myRTC.write(tm);
}

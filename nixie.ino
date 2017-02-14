#include <NixieClock.h>
#include <idDHTLib.h>
#include <Bounce2.h>
#include <iarduino_RTC.h>

//#define DHTPIN            13         // Pin which is connected to the DHT sensor.
//#define DHTTYPE           DHT11     // DHT 11

int idDHTLibPin = 2; //Digital pin for comunications
int idDHTLibIntNumber = 0; //interrupt number (must be the one that use the previus defined pin (see table above)

enum disp_mode {
  TIME,
  SEC,
  DATE,
  TEMP
};

//enum  disp_set {
//  TIME_
//}

int command = 0;

int  runDigit = 0;
bool runInprogress = false;
int runInterval = 660; //in seconds 600 - 10 minutes
int runLast = 0;
int seconds = 0;

int piezo_frequency = 3500;
bool isBeepGenerated = false;
int lastBeepTime = 0;
int beepInterval = 0;

int mode_timeout = 10;
int mode_changed = 0;
disp_mode current_mode = TIME;

//buttons
const int pin_mode = 4;
const int pin_set = 5;
const int pin_up = 6;
const int pin_down = A7;

//piezo
const int pin_piezo = 3;

//nixie pins
const int pin_A0 = A3;
const int pin_A1 = A1;
const int pin_A2 = A0;
const int pin_A3 = A2;

const int pin_anod_sec_indicator_1 = 11;
const int pin_anod_sec_indicator_2 = 12;
const int pin_anod_min_1 = 7;
const int pin_anod_min_2 = 8;
const int pin_anod_hour_1 = 9;
const int pin_anod_hour_2 = 10;

NixieClock nixie(4);

unsigned long lastTime;
unsigned long currentTime;

//unsigned long current_acquire_time = millis();
float temperature = 0.0;

iarduino_RTC time(RTC_DS3231);
Bounce bouncer = Bounce(); //создаем экземпляр класса Bounce

//declaration
void dhtLib_wrapper(); // must be declared before the lib initialization

// Lib instantiate
idDHTLib DHTLib(idDHTLibPin,idDHTLibIntNumber,dhtLib_wrapper);

int delayIn = 1000;
int digit = 0;
int digit1 = 0;
int clockStr[] = {0, 0, 0, 0};

// This wrapper is in charge of calling 
// mus be defined like this for the lib work
void dhtLib_wrapper() {
  //DHTLib.dht11Callback(); // Change dht11Callback() for a dht22Callback() if you have a DHT22 sensor
  DHTLib.dht22Callback();
}

int getDigitIndicator() {

  currentTime = micros();
  digit1 = (currentTime - lastTime) / 1000000;
  if (digit1 >= 1) {
    ++digit;
    lastTime = currentTime;
  }
  //  Serial.print("lastTime: ");
  //  Serial.println(lastTime);
  //  Serial.print("currentTime: ");
  //  Serial.println(currentTime);
  //  Serial.print("Time: ");
  //  Serial.println(digit);
  return digit;
}

void turnOff() {
  digitalWrite(pin_A0, HIGH);
  digitalWrite(pin_A1, HIGH);
  digitalWrite(pin_A2, HIGH);
  digitalWrite(pin_A3, HIGH);
  digitalWrite(pin_anod_min_1, LOW);
  digitalWrite(pin_anod_min_2, LOW);
  digitalWrite(pin_anod_hour_1, LOW);
  digitalWrite(pin_anod_hour_2, LOW);

  digitalWrite(pin_anod_sec_indicator_1, LOW);
  digitalWrite(pin_anod_sec_indicator_2, LOW);
}

void show_0() {
  digitalWrite(pin_A0, LOW);
  digitalWrite(pin_A1, LOW);
  digitalWrite(pin_A2, LOW);
  digitalWrite(pin_A3, LOW);
}

void show_1() {
  digitalWrite(pin_A0, HIGH);
  digitalWrite(pin_A1, LOW);
  digitalWrite(pin_A2, LOW);
  digitalWrite(pin_A3, LOW);
}


void show_2() {
  digitalWrite(pin_A0, LOW);
  digitalWrite(pin_A1, HIGH);
  digitalWrite(pin_A2, LOW);
  digitalWrite(pin_A3, LOW);
}

void show_3() {
  digitalWrite(pin_A0, HIGH);
  digitalWrite(pin_A1, HIGH);
  digitalWrite(pin_A2, LOW);
  digitalWrite(pin_A3, LOW);
}

void show_4() {
  digitalWrite(pin_A0, LOW);
  digitalWrite(pin_A1, LOW);
  digitalWrite(pin_A2, HIGH);
  digitalWrite(pin_A3, LOW);
}

void show_5() {
  digitalWrite(pin_A0, HIGH);
  digitalWrite(pin_A1, LOW);
  digitalWrite(pin_A2, HIGH);
  digitalWrite(pin_A3, LOW);
}

void show_6() {
  digitalWrite(pin_A0, LOW);
  digitalWrite(pin_A1, HIGH);
  digitalWrite(pin_A2, HIGH);
  digitalWrite(pin_A3, LOW);
}

void show_7() {
  digitalWrite(pin_A0, HIGH);
  digitalWrite(pin_A1, HIGH);
  digitalWrite(pin_A2, HIGH);
  digitalWrite(pin_A3, LOW);
}

void show_8() {
  digitalWrite(pin_A0, LOW);
  digitalWrite(pin_A1, LOW);
  digitalWrite(pin_A2, LOW);
  digitalWrite(pin_A3, HIGH);
}

void show_9() {
  digitalWrite(pin_A0, HIGH);
  digitalWrite(pin_A1, LOW);
  digitalWrite(pin_A2, LOW);
  digitalWrite(pin_A3, HIGH);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(pin_anod_sec_indicator_1, OUTPUT);
  digitalWrite(pin_anod_sec_indicator_1, LOW);

  pinMode(pin_anod_sec_indicator_2, OUTPUT);
  digitalWrite(pin_anod_sec_indicator_2, LOW);

  // Configure A0
  pinMode(pin_A0, OUTPUT);
  digitalWrite(pin_A0, HIGH);

  // Configure A1
  pinMode(pin_A1, OUTPUT);
  digitalWrite(pin_A1, HIGH);

  // Configure A2
  pinMode(pin_A2, OUTPUT);
  digitalWrite(pin_A2, HIGH);

  // Configure A3
  pinMode(pin_A3, OUTPUT);
  digitalWrite(pin_A3, HIGH);

  pinMode(pin_anod_min_1, OUTPUT);
  digitalWrite(pin_anod_min_1, HIGH);

  pinMode(pin_anod_min_2, OUTPUT);
  digitalWrite(pin_anod_min_2, HIGH);

  pinMode(pin_anod_hour_1, OUTPUT);
  digitalWrite(pin_anod_hour_1, HIGH);

  pinMode(pin_anod_hour_2, OUTPUT);
  digitalWrite(pin_anod_hour_2, HIGH);

  pinMode(pin_mode, INPUT);
  digitalWrite(pin_mode, HIGH);
  bouncer.attach(pin_mode);
  bouncer.interval(5);

  lastTime = micros();
  time.begin();
  //time.settime(15,44,19,8,1,17,7); //сек,мин,час,день,мес,год,день_недели
  time.gettime();
  time.period(1);

  //  dht.begin();
  //  sensor_t sensor;
  //  dht.temperature().getSensor(&sensor);
  //  Serial.println("------------------------------------");
  //  Serial.println("Temperature");
  //  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  //  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  //  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  //  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  //  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  //  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");
  //  Serial.println("------------------------------------");
  //  dht.humidity().getSensor(&sensor);
  //  Serial.println("------------------------------------");
  //  Serial.println("Humidity");
  //  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  //  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  //  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  //  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  //  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  //  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");
  //  Serial.println("------------------------------------");
  //  Serial.print  ("min_delay:       "); Serial.println(sensor.min_delay);
  //  DHT_delay = sensor.min_delay / 1000;

  Serial.println(IDDHTLIB_VERSION);
  delay(3000); // The sensor need like 2 sec to initialize, if you have some code before this that make a delay, you can eliminate this delay

  DHTLib.acquire();
  //current_acquire_time = millis();
  while (DHTLib.acquiring())
    ;
  int result = DHTLib.getStatus();
  switch (result)
  {
  case IDDHTLIB_OK: 
    Serial.println("OK"); 
    break;
  case IDDHTLIB_ERROR_CHECKSUM: 
    Serial.println("Error\n\r\tChecksum error"); 
    break;
  case IDDHTLIB_ERROR_TIMEOUT: 
    Serial.println("Error\n\r\tTime out error"); 
    break;
  case IDDHTLIB_ERROR_ACQUIRING: 
    Serial.println("Error\n\r\tAcquiring"); 
    break;
  case IDDHTLIB_ERROR_DELTA: 
    Serial.println("Error\n\r\tDelta time to small"); 
    break;
  case IDDHTLIB_ERROR_NOTSTARTED: 
    Serial.println("Error\n\r\tNot started"); 
    break;
  default: 
    Serial.println("Unknown error"); 
    break;
  }
  
  Serial.print("Humidity (%): ");
  Serial.println(DHTLib.getHumidity(), 2);

  Serial.print("Temperature (oC): ");
  Serial.println(DHTLib.getCelsius(), 2);

  Serial.println(time.gettime("d-m-Y, H:i:s, D"));
  runLast = time.seconds + time.minutes * 60 + time.Hours * 360;
}

void show_digit(int index, int digit) {

  switch (index) {
    case 0:
      digitalWrite(pin_anod_min_1, HIGH);
      break;
    case 1:
      digitalWrite(pin_anod_min_2, HIGH);
      break;
    case 2:
      digitalWrite(pin_anod_hour_1, HIGH);
      break;
    case 3:
      digitalWrite(pin_anod_hour_2, HIGH);
      break;
  }

  switch (digit) {
    case 0:
      show_0();
      break;
    case 1:
      show_1();
      break;
    case 2:
      show_2();
      break;
    case 3:
      show_3();
      break;
    case 4:
      show_4();
      break;
    case 5:
      show_5();
      break;
    case 6:
      show_6();
      break;
    case 7:
      show_7();
      break;
    case 8:
      show_8();
      break;
    case 9:
      show_9();
      break;
  }
  delayMicroseconds(delayIn);
  turnOff();
}

void show_indicator(int digit) {
  if (digit % 2 >= 1 ) {
    digitalWrite(pin_anod_sec_indicator_1, HIGH);
    digitalWrite(pin_anod_sec_indicator_2, HIGH);
  }
  delayMicroseconds(delayIn);
  turnOff();
}

int getD(int digit) {
  if (digit < 10) {
    return digit;
  }  else {
    return digit - 10;
  }
}

void beep(int seconds) {
  if (lastBeepTime != seconds) {
    if (seconds != 0) {
      tone(pin_piezo, piezo_frequency, 100);
    } else {
      tone(pin_piezo, piezo_frequency, 100 + time.Hours * 20);
    }
    lastBeepTime = time.seconds;
  }
}

void changeMode() {
  mode_changed = time.seconds;
  switch (current_mode) {
    case TIME:
      current_mode = SEC;
      break;
    case SEC:
      current_mode = DATE;
      break;
    case DATE:
      current_mode = TEMP;
      //delay(DHT_delay);
      //      dht.temperature().getEvent(&event);
      //      if (isnan(event.temperature)) {
      //        Serial.println("Error reading temperature!");
      //      }
      //      else {
      //        Serial.print("Temperature: ");
      //        Serial.print(event.temperature);
      //        Serial.println(" *C");
      //      }
      DHTLib.acquire();
    //current_acquire_time = millis(); 
      break;
    case TEMP:
      current_mode = TIME;
      break;
  }
}

void checkMode() {
  if(current_mode != SEC) {
    if (abs(time.seconds - mode_changed) >= mode_timeout) {
      current_mode = TIME;
    }
  }
}

void settime(int cmd) {
  int a = cmd >> 6;
  Serial.println(a);
  if(a == 1) {
    Serial.print("Set seconds: ");
    Serial.println(cmd & 0x3F);
    time.settime(cmd & 0x3F,time.minutes,time.Hours,time.day,time.month,time.year,time.midday); //сек,мин,час,день,мес,год,день_недели
  }else if(a == 2){
    Serial.print("Set minutes: ");
    Serial.println(cmd & 0x3F);
    time.settime(time.seconds,cmd & 0x3F,time.Hours,time.day,time.month,time.year,time.midday); //сек,мин,час,день,мес,год,день_недели
  } else if(a == 3) {
    Serial.print("Set hours: ");
    Serial.println(cmd & 0x3F);
    time.settime(time.seconds,time.minutes,cmd & 0x3F,time.day,time.month,time.year,time.midday); //сек,мин,час,день,мес,год,день_недели  
  }
  return;
}

void loop() {
  // put your main code here, to run repeatedly:
  //delay(delayIn);

  if(Serial.available()){
    command = Serial.parseInt();
  }

  if(command) {
    settime(command);
    command = 0;  
  }
  
  time.gettime();
  seconds = time.seconds + time.minutes * 60 + time.Hours * 360;

  //  if(!digitalRead(pin_mode)){
  //    changeMode();
  //  } else {
  //    checkMode();
  //  }

  if (bouncer.update())
  { //если произошло событие
    if (bouncer.read() == 0)
    { //если кнопка нажата
      Serial.println("pressed"); //вывод сообщения о нажатии
    } else {
      Serial.println("released"); //вывод сообщения об отпускании
      changeMode();
    }
  }

  checkMode();

  if (time.minutes == 59) {
    //if(time.minutes >= 0){
    if (time.seconds > 55) {
      beep(time.seconds);
    }
  }

  if (time.minutes == 0 && time.seconds == 0) {
    //if(time.seconds == 0) {
    beep(0);
  }

  if (abs(seconds - runLast) > runInterval) {
    if(time.minutes != 0) {
      runLast = seconds;
      runInprogress = true;
      Serial.println("Run");
    }
  }

  if (abs(seconds - runLast) >= 10) {
    runInprogress = false;
  }

//  if(abs(millis() - current_acquire_time) > 10000) {
//    DHTLib.acquire();
//    current_acquire_time = millis(); 
//  }

  if(!DHTLib.acquiring()) {
    int result = DHTLib.getStatus();
    if(result == IDDHTLIB_OK) {
            temperature = DHTLib.getCelsius(); 
          }
//          else {
//            Serial.print("Status in not OK: ");
//            Serial.println(result); 
//          }
  }

  if (!runInprogress) {
    switch (current_mode) {
      case TIME:
        clockStr[0] = time.minutes % 10;
        clockStr[1] = (time.minutes / 10) % 10;
        clockStr[2] = time.Hours % 10;
        clockStr[3] = (time.Hours / 10) % 10;
        break;
      case SEC:
        clockStr[0] = time.seconds % 10;
        clockStr[1] = (time.seconds / 10) % 10;
        clockStr[2] = time.minutes % 10;;
        clockStr[3] = (time.minutes / 10) % 10;;
        break;
      case DATE:
        if (abs(time.seconds - mode_changed) <= mode_timeout / 2) {
          clockStr[0] = time.month % 10;
          clockStr[1] = (time.month / 10) % 10;
          clockStr[2] = time.day % 10;
          clockStr[3] = (time.day / 10) % 10;
        } else {
          clockStr[0] = time.year % 10;
          clockStr[1] = (time.year / 10) % 10;
          clockStr[2] = 0;
          clockStr[3] = 2;
        }
        break;
      case TEMP:
        
          int intPart = int(temperature);
          int floatPart = int((temperature - intPart) * 100);

          clockStr[0] = floatPart % 10;
          clockStr[1] = (floatPart / 10) % 10;
          clockStr[2] = intPart % 10;
          clockStr[3] = (intPart / 10) % 10;
        break;
    }

  } else {
    runDigit = seconds - runLast;
    clockStr[0] = runDigit;
    clockStr[1] = runDigit;
    clockStr[2] = runDigit;
    clockStr[3] = runDigit;
  }



  show_digit(0, getD(clockStr[0]));
  show_digit(1, getD(clockStr[1]));
  show_digit(2, getD(clockStr[2]));
  show_digit(3, getD(clockStr[3]));
  show_indicator(time.seconds);
}

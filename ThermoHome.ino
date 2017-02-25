#include <LiquidCrystal.h>
#include <EEPROM.h>  
#include <math.h>
#include "CmdMessenger.h"


const int sensorPin = A7; //set the temperature sensor pin
const int relayPin = 8; //set the relay pin
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  //set the LCD pins

int Contrast=10;
String Degree = "C"; // "C"- celsius, "F"- fahrenheit
float TempSet;
String Status;
bool Power;
int TempEeprom = 1; //address in memory
int PowerEeprom = 0;//address in memory

float Thermistor(int RawADC) {
  //calculate raw temperature data from KY-013 sensor
  float Temp;
  Temp = log(10000.0*((1024.0/RawADC-1))); 
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  if (Degree == "C"){
    Temp = Temp - 273.15;            // Convert Kelvin to Celcius
  }else if (Degree == "F"){
    Temp = (Temp * 9.0)/ 5.0 + 32.0;
  }
  return Temp;
}


enum {
    curr_temp,
    curr_temp_is,
    default_temp,
    default_temp_is,
    set_temp,
    power,
    power_is,
    error,
};
CmdMessenger c = CmdMessenger(Serial,',',';','/');

//get temperature sensor
float GetTemp (void){
  int readVal=analogRead(sensorPin); //read raw data from the temperature sensor
  float temp =  Thermistor(readVal);
  return temp;
}


//get current temperature from sensor
void on_curr_temp(void){
  float temp2 = GetTemp();
  c.sendCmd(curr_temp_is,String(temp2));
}
//get set tempreture
void on_default_temp(void){
  c.sendCmd(default_temp_is,String(TempSet));
}
//turn power on\off
void on_power(void){
  Power = c.readBinArg<bool>();
  EEPROM.put(PowerEeprom, Power);
  c.sendCmd(power_is,String(Status));
}
//set temperature
void on_set_temp(void){
  TempSet = c.readBinArg<float>();
  EEPROM.update(TempEeprom, TempSet);
  c.sendCmd(default_temp_is,String(TempSet));
}
//unknown command
void on_unknown_command(void){
    c.sendCmd(error,"Command without callback.");
}
//CmdMessenger module
void attach_callbacks(void) { 
    c.attach(curr_temp,on_curr_temp);
    c.attach(default_temp,on_default_temp);
    c.attach(set_temp,on_set_temp);
    c.attach(power,on_power);
    c.attach(on_unknown_command);
}


void setup()
{ 
    Serial.begin(9600);
    attach_callbacks();
    analogWrite(6,Contrast); //set contrast for LCD
    pinMode(relayPin, OUTPUT);
    lcd.begin(16, 2);
    
    //check data in memory
    //if it's not empty, get data from memory
    int val = 0;
    val = EEPROM.read(PowerEeprom);
    if (val == 0xFF){
      Power = false;
      TempSet = -40.00;
      EEPROM.put(PowerEeprom, Power);
      EEPROM.put(TempEeprom, TempSet);
    } else {
      EEPROM.get(PowerEeprom, Power);
      EEPROM.get(TempEeprom, TempSet);
    }
    
}  

void loop()
{  
  
  c.feedinSerialData();
  float temp =  GetTemp();
  
  lcd.setCursor(0, 0);
  lcd.print(String("Temp: " + String(temp) + Degree)); //print on the LCD current temperature
  if (Power){
    if (temp < TempSet){
      digitalWrite(relayPin, LOW);
      Status = "RelayON";
    }else if(temp > (TempSet + 1.0)) {
      digitalWrite(relayPin, HIGH);
      Status = "RelayOFF";
    }
  }else {
    Status = "PowerOFF";
    digitalWrite(relayPin, HIGH);
  }
  lcd.setCursor(0, 1);
  lcd.println(String(Status + "|DT" + String(TempSet))); //print on the LCD current Relay Status
  delay(200);
  
}

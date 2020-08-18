#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <avr/io.h>
#include <util/delay.h>


//oled//
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
///////

//ina219-sensor//
#include <Adafruit_INA219.h>
Adafruit_INA219 ina219_1;
Adafruit_INA219 ina219_2(0x41);
////

float a =86; // PWM signal variable

void setup()
{
//  
 pinMode(3, OUTPUT);
  TCCR2A = _BV(COM2A0) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS21);
  OCR2A = 86;
  OCR2B = a;
//PWM initiation (23Khz)

//display initiation  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay(); 
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(27,30);
  display.print("getting ready ");
  display.setCursor(37,40);
  display.print(a);
  display.display();
  ina219_1.begin();
  ina219_2.begin();
  Serial.begin(115200);
  delay(500);
  //
}


//Ina219_1 initition
float shuntvoltage_1 = 0;
float busvoltage_1 = 0;
float current_mA_1 = 0;
float loadvoltage_1 = 0;
float energy_1 = 0;
float power_1 = 0;
int b=0;

//ina219_2 initiation
float shuntvoltage_2 = 0;
float busvoltage_2 = 0;
float current_mA_2 = 0;
float loadvoltage_2 = 0;
float power_2 = 0;
float energy_2 = 0;

//MPPT variable program
float powerhold= 0;
float volthold=0;
float deltaenergy= 0;
float deltavolt = 0;
float time_exe=0;
float disptime= 0 ;
float energytime=0;
float time_log=0;

void loop()
{

  unsigned long currentMillis1 = millis();

  ina219values();

  deltaenergy = power_2 - powerhold;
  deltavolt = loadvoltage_2 - volthold;

if (loadvoltage_1<4.2)
{
  if (loadvoltage_1>=3)
  {
    Serial.print("MPPT!!!");Serial.print(";");
    MPPT_ALG();
  }
  else if (loadvoltage_1<3)
  {
    OCR2B = a;
    b=0;
    while((b==0))
    {
    unsigned long currentMillis1 = millis();
    if(current_mA_1>=380)
    {
      if(a>=86)
           {
            a=a-1  ;                                                             
           }
           else
           {
            a=a+1;
           }
    }
        else
        {
        if(a>0)
        {
          a=a-1;
         }
       else
          {a=1;}
           }
      
           OCR2B = a;
            ina219values();    
            if (current_mA_1<=400)
            {
              if (loadvoltage_1>=3)
              {
              b=1;}       
            }
      Serial.print("trickling!!!");Serial.print(";");
      Serial.print(loadvoltage_1);Serial.print(";");
      Serial.print(current_mA_1);Serial.print(";");
      Serial.print(loadvoltage_2);Serial.print(";");
      Serial.print(current_mA_2);;Serial.print(";");
      Serial.print(time_exe);Serial.print(";");
      Serial.println(a);
      unsigned long currentMillis2 = millis();
      time_exe=currentMillis2-currentMillis1;
      disptime=disptime+time_exe;
     if (disptime>=1000)
     {     
      disptime=0;
      displaydata(); // this consume 340ms
      delay (50);
     }
      energy_1 = energy_1 + power_1 * time_exe / 3600000;
      energy_2 = energy_2 + power_2 * time_exe / 3600000;
      time_log=time_log+time_exe;
     }
  }   
}
   
  

else if(loadvoltage_1>4.2)
{
  if(current_mA_1>=84)
  {
  a=a+1;
  }
  else if (current_mA_1<84)
  {
    a=86;
  }
}

  
volthold = loadvoltage_2;
powerhold = power_2;
OCR2B = a;
  if (disptime>=1000)
  {     
    disptime=0;
    displaydata();
    delay (50);
  }
  unsigned long currentMillis2 = millis();
  time_exe=currentMillis2-currentMillis1;
  Serial.println(time_exe);
  disptime=disptime+time_exe;
  Serial.println(disptime);
    
  energy_1 = energy_1 + power_1 * time_exe / 3600000;
  energy_2 = energy_2 + power_2 * time_exe / 3600000;
  time_log=time_log+time_exe;
  
  Serial.print(loadvoltage_1);Serial.print(";");
  Serial.print(current_mA_1);Serial.print(";");
  Serial.print(loadvoltage_2);Serial.print(";");
  Serial.print(current_mA_2);;Serial.print(";");
  Serial.print(time_exe);Serial.print(";");
  Serial.println(a);
}

void displaydata() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(loadvoltage_1);
  display.setCursor(35, 0);
  display.println("V");
  display.setCursor(50, 0);
  display.println(current_mA_1);
  display.setCursor(95, 0);
  display.println("mA");
  display.setCursor(0, 10);
  display.println(power_1);
  display.setCursor(65, 10);
  display.println("mW");
  display.setCursor(0, 20);
  display.println(energy_1);
  display.setCursor(65, 20);
  display.println("mWh");
  
  display.setCursor(0, 30);
  display.println(loadvoltage_2);
  display.setCursor(35, 30);
  display.println("V");
  display.setCursor(50, 30);
  display.println(current_mA_2);
  display.setCursor(95, 30);
  display.println("mA");
  display.setCursor(0, 40);
  display.println(power_2);
  display.setCursor(65, 40);
  display.println("mW");
  display.setCursor(0, 50);
  display.println(energy_2);
  display.setCursor(65, 50);
  display.println("mWh");

  display.display();
}

void ina219values() {
  
  shuntvoltage_1 = ina219_1.getShuntVoltage_mV();
  busvoltage_1 = ina219_1.getBusVoltage_V();
  current_mA_1 = ina219_1.getCurrent_mA();
  loadvoltage_1 = busvoltage_1 + (shuntvoltage_1 / 1000);
  //energy_1 = energy_1 + loadvoltage_1 * current_mA_1 / 3600;
  power_1 = loadvoltage_1 * current_mA_1;

  shuntvoltage_2 = ina219_2.getShuntVoltage_mV();
  busvoltage_2 = ina219_2.getBusVoltage_V();
  current_mA_2 = ina219_2.getCurrent_mA();
  loadvoltage_2 = busvoltage_2 + (shuntvoltage_2 / 1000);
  //energy_2 = energy_2 + loadvoltage_2 * current_mA_2 / 3600;
  power_2 = loadvoltage_2 * current_mA_2;
}

void MPPT_ALG()
{
  if (deltaenergy > 0)                     
  {
    if (deltavolt > 0)
    {
      //pwm increase voltage
      if(a>0)
      {
           if(a>=86)
           {
            a=a-1   ;                                                            
           }
           else
           {
        a=a+1;
           }
      }
      else 
      {a=1;
        }                                                                  
    }
    else if (deltavolt < 0)
    {
      //pwm decrease voltage
      if(a>0)
      {
        a=a-1;
      }
      else
      {a=1;}
    }
  }
  
  else if (deltaenergy < 0) 
  {
    if (deltavolt > 0)                         
    {
      //pwm decrease voltage
        if(a>0)
      {
        a=a-1;
      }
      else 
      {a=1;}
    }
    else if (deltavolt < 0)
    {
      //pwm increase voltage
      if(a>0)
      {
        if(a>=86)
           {
            a=a-1  ;                                                             
           }
           else
           {
            a=a+1;
           }
      }
      else 
      {a=1;}
    }
  }  
}

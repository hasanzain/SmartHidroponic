#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <EEPROM.h>
#include "GravityTDS.h"
#include <OneWire.h> 
#include <DallasTemperature.h>
#include "DFRobot_ESP_PH.h"
TaskHandle_t Task1;
TaskHandle_t Task2;


char auth[] = "bS-gUY8aQg2fgRGMZWfRpIgQjI625wQt";
char ssid[] = "Sembarang";
char pass[] = "kanakfathan";
String serialResponse = "";
char sz[] = "Here; is some; sample;100;data;1.414;1020";
String dataTx[2];
//deklarasi ----------------- tds
#define ONE_WIRE_BUS 27 // Digitalpin where Temp sensor is connected
#define TdsSensorPin 32 // Where Analog pin of TDS sensor is connected to arduino
OneWire oneWire(ONE_WIRE_BUS); 
GravityTDS gravityTds;
DallasTemperature sensors(&oneWire);
float tdsValue = 0;

//deklarasi ph ----------------------
DFRobot_ESP_PH ph;
#define ESPADC 4096.0   //the esp Analog Digital Convertion value
#define ESPVOLTAGE 3300 //the esp voltage supply value
#define PH_PIN 35    //the esp gpio data pin number
float voltage, phValue, temperature = 30;

//deklarasi ultrasonic ------------------------
const int trigPin = 33;
const int echoPin = 25;
// defines variables
long duration;
int distance;

//Deklarasi sensor kekeruhan air---------------------------
const int pinsensor = 23;
float tegangan; //data untuk tegangan
float ntu; //data untuk nilai pembacaan satuan sensor kekeruhan

//Deklarasi sensor cahaya---------------------------
const int pinCahaya = 18;

//deklarasi pakan otomatis
const int pakan = 5;


 
void kekeruhan(){
    int val = analogRead(pinsensor);
    tegangan = val*(5.0/1024);
    ntu = 100.00-(tegangan/4.16)*100.00;
//    kirim data ke blynk
//    Blynk.virtualWrite(V5, ntu);
}

void cahaya(){
    int val = analogRead(pinsensor);
    float adc = val*(5.0/1024);
//    kirim data ke blynk
//    Blynk.virtualWrite(V5, adc);
}

void tds(){
sensors.requestTemperatures();
gravityTds.setTemperature(sensors.getTempCByIndex(0)); // grab the temperature from sensor and execute temperature compensation
gravityTds.update(); //calculation done here from gravity library
tdsValue = gravityTds.getTdsValue(); // then get the TDS value
//    kirim data ke blynk
//    Blynk.virtualWrite(V5, tdsValue);
//Serial.print("TDS value is:");
//Serial.print(tdsValue,0);
//Serial.println("ppm");
//Serial.print("Temperature is: "); 
//Serial.println(sensors.getTempCByIndex(0));
}

void ultrasonic()
{
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;

//// Prints the distance on the Serial Monitor
//Serial.print("Distance: ");
//Serial.println(distance);
//    kirim data ke blynk
//    Blynk.virtualWrite(V5, adc);
}

float phMeter()
{
    static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U) //time interval: 1s
  {
    timepoint = millis();
    //voltage = rawPinValue / esp32ADC * esp32Vin
    voltage = analogRead(PH_PIN) / ESPADC * ESPVOLTAGE; // read the voltage

    phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
//    Serial.print("pH:");
//    Serial.println(phValue, 4);
//    kirim data ke blynk
//    
    return phValue;
  }
  ph.calibration(voltage, temperature); // calibration process by Serail CMD
}

void setup() {
//Deklarasi kekeruhan
pinMode(pinsensor,INPUT);

//Deklarasi sensor cahaya
pinMode(pinCahaya,INPUT);

//  Deklarasi tds
sensors.begin();
gravityTds.setPin(TdsSensorPin);
gravityTds.setAref(3.3); //reference voltage on ADC, default 5.0V on Arduino UNO
gravityTds.setAdcRange(4096); //1024 for 10bit ADC;4096 for 12bit ADC
gravityTds.begin(); //initialization

//deklarasi ultrasonic
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input

//PH
EEPROM.begin(32);//needed to permit storage of calibration value in eeprom
ph.begin();

Blynk.begin(auth, ssid, pass);
Serial.begin(9600);
Serial2.begin(9600);

//create multitasking process
xTaskCreatePinnedToCore(Task1code,"Task1",10000,NULL,1,&Task1,0);                         
delay(500); 

xTaskCreatePinnedToCore(Task2code,"Task2",10000,NULL,1,&Task2,1);          
delay(500);

}

void Task1code( void * parameter ){
  for(;;){
    float dataPh = phMeter();
//  if(dataPh <= 6){
//  Urutan penerimaan data relay(relay1;relay2;...relay6)
//  Serial2.print(1;-;-;-;-;-)
//  Relay1 nyala
//    }
    Blynk.virtualWrite(V5, dataPh);
    delay(1000);
  } 
}


void Task2code( void * parameter ){
  for(;;){
    if ( Serial2.available()) {
    serialResponse = Serial2.readStringUntil('\r\n');

    // Convert from String Object to String.
    char buf[sizeof(sz)];
    serialResponse.toCharArray(buf, sizeof(buf));
    char *p = buf;
    char *str;
    int i=0;
    while ((str = strtok_r(p, ";", &p)) != NULL) {// delimiter is the semicolon
      dataTx[i] = str;
      i++;
    }
//    urutan penerimaan serial(utrasonic1;ultrasonic2)
//    Serial.print("US1 : ");
//    Serial.println(dataTx[0]);
//    Serial.print("US2 : ");
//    Serial.println(dataTx[1]);

//    kirim data ultrasonic ke blynk
      Blynk.virtualWrite(V0,dataTx[0]);
//      Blynk.virtualWrite(V1,dataTx[1]);
  }
  delay(1000);
  }
}

void loop() {
  

}

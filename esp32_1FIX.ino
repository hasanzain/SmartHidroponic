TaskHandle_t Task1;
TaskHandle_t Task2;

const int trigPin1 = 5;
const int echoPin1 = 18;
const int trigPin2 = 19;
const int echoPin2 = 21;
const int Relay1 = 13;
const int Relay2 = 12;
const int Relay3 = 14;
const int Relay4 = 27;
const int Relay5 = 26;
const int Relay6 = 25;
long duration1,duration2;
int distance1,distance2;
String serialResponse = "";
String dataTx[2];
char sz[] = "1;0";

void setup() {
pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin2, INPUT); // Sets the echoPin as an Input
pinMode(Relay1, OUTPUT);
pinMode(Relay2, OUTPUT);
pinMode(Relay3, OUTPUT);
pinMode(Relay4, OUTPUT);
pinMode(Relay5, OUTPUT);
pinMode(Relay6, OUTPUT);
Serial.begin(9600);
Serial2.begin(9600);
Serial.setTimeout(5);
Serial2.setTimeout(5);

//create multitasking process
xTaskCreatePinnedToCore(Task1code,"Task1",10000,NULL,1,&Task1,0);                         
delay(500); 
xTaskCreatePinnedToCore(Task2code,"Task2",10000,NULL,1,&Task2,1);          
delay(500);
}


void ultrasonic(){
digitalWrite(trigPin1, LOW);  
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin1, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin1, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration1 = pulseIn(echoPin1, HIGH);
// Calculating the distance
distance1= duration1*0.034/2;
// Prints the distance on the Serial Monitor
//Serial.print("Distance 1: ");
//Serial.println(distance1);

//Ultrasonic2
digitalWrite(trigPin2, LOW);  
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin2, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin2, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration2 = pulseIn(echoPin2, HIGH);
// Calculating the distance
distance2 = duration2*0.034/2;
// Prints the distance on the Serial Monitor
//Serial.print("Distance 2: ");
//Serial.println(distance2);

//Send Data to other Device
String dataSend = String(distance1) + ";" + String(distance2);
Serial.println(dataSend);
}

void Task1code( void * parameter ){
//  Serial.print("Task1 is running on core ");
//  Serial.println(xPortGetCoreID());

  for(;;){
ultrasonic();
delay(1000);
  } 
}


void Task2code( void * parameter ){
//  Serial.print("Task2 is running on core ");
//  Serial.println(xPortGetCoreID());

  for(;;){
    if ( Serial.available()) {
    serialResponse = Serial.readStringUntil('\r\n');

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
//    Urutan penerimaan data relay(relay1;relay2;...relay6)
    if (dataTx[0] == "1"){
      digitalWrite(Relay1,HIGH);
//      Serial.print("relay1 : ");
//      Serial.println(dataTx[0]);
    }else if(dataTx[0] == "-"){
      
    }
    else{
      digitalWrite(Relay1,LOW);
//      Serial.print("relay1 : ");
//      Serial.println(dataTx[0]);
    }
    
  }
}
}

void loop() {
  

}

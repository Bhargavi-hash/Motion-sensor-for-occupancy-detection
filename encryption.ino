#include <ThingSpeak.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "HTTPClient.h"
#include "time.h"
#include <tinyECC.h>
tinyECC ecc;

#include <Melopero_AMG8833.h>
Melopero_AMG8833 grid_sensor;

//*** time n date**********

#include <NTPClient.h>
#include <WiFiUdp.h>
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

//***** time n date



#define pirPin 19 // pirOutput pin
#define motionLed 18

const char* host = "api.thingspeak.com";
const char* WriteAPI = "T27SXD639SVCB0MX";

WiFiClient  client;

unsigned long myChannelNumber = 2;

// SCL_pin is GPIO22 in esp32
// SDA_pin is GPIO21 in esp32
#define timeSeconds 1 // interval at which to blink led(seconds)


int ledState = LOW; // ledState used to set the LED
int pirState = LOW;
// Timer: Auxiliar variables
long now = millis();
long lastTrigger = 0;
boolean startTimer = false;
int val = LOW;
int count=0;


/*Put your SSID & Password*/
//const char *ssid = "Galaxy M112694"; // Enter SSID here
//const char *pwd = "12345678";  //Enter Password here
const char *ssid = "Galaxy M511CCF"; // Enter SSID here
const char *pwd = "ramreddy@3";  //Enter Password here
//const char *ssid = "esw-m19@iiith"; // Enter iiith SSID here
//const char *pwd = "e5W-eMai@3!20hOct";  //Enter Password here

WebServer server(80);

//************** OM2M *****************
#define CSE_IP     "esw-onem2m.iiit.ac.in" //"esw-onem2m.iiit.ac.in"//nand-"192.168.171.221"//"127.0.0.1"////ruch -"192.168.36.221" //replace with system-ip  // //esw-onem2m.iiit.ac.in
//#define CSE_PORT  443 //5089 //443
#define HTTPS     true
#define OM2M_ORGIN   "zZ!#4s:m&Y#HL"//"zZ!#4s:m&Y#HL" //"admin:admin"
#define OM2M_MN     "/~/in-cse/in-name/"
#define OM2M_AE     "Team-28"//"Team-28"//"Od-TEST"//"OD-TEST" //"AE-TEST"
#define OM2M_DATA_CONT  "Node-1/Data"


//************** OM2M *****************


void setup()
{
  
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  delay(100);
  pinMode(pirPin, INPUT);
  pinMode(motionLed, OUTPUT);
  digitalWrite(motionLed, ledState);
  
 //********** GRID EYE CODE
  // initializing I2C to use default address AMG8833_I2C_ADDRESS_B and Wire (I2C-0):
  Wire.begin();
  grid_sensor.initI2C();
  // To use Wire1 (I2C-1):
  //Wire1.begin();
//  sensor.initI2C(AMG8833_I2C_ADDRESS_B, Wire1);
  Serial.println("Resetting grid_sensor ... ");  
  int statusCode = grid_sensor.resetFlagsAndSettings();
  Serial.println(grid_sensor.getErrorDescription(statusCode));

  Serial.println("Setting FPS ... ");
  statusCode = grid_sensor.setFPSMode(FPS_MODE::FPS_10);
  Serial.println(grid_sensor.getErrorDescription(statusCode));
//******** GRID EYE CODE
  
  // Serial.println("--------------- Welcome -----------------");
  Serial.println("Connecting to ");
  Serial.println(ssid);
  Serial.println();
 
  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.println("WiFi Connected!!");
  }
  Serial.print("http://");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("HTTP server started");
  ThingSpeak.begin(client); 
//******* time n date ******
  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(19800);// IST = GMT+ 5:30 hrs
//******* time n date ******
}


void om2m_createCI(int & pirState, String & row01,String & row23,String & row45,String & row67){


  HTTPClient http;
  //******* time n date ******
//     while(!timeClient.update()) {
//       timeClient.forceUpdate();
//     }
//     // The formattedDate comes with the following format:
//   // 2018-05-28T16:00:13Z
//   // We need to extract date and time
//   formattedDate = timeClient.getFormattedDate();
// //  Serial.println(formattedDate);

 
//   // Extract date
//   int splitT = formattedDate.indexOf("T");
//   dayStamp = formattedDate.substring(0, splitT);
// //  Serial.print("DATE: ");
// //  Serial.println(dayStamp);
//   // Extract time
//   timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
//   Serial.print("HOUR: ");
//   Serial.println(timeStamp);
    
  //******* time n date ******
 // String data="[" + String(epochTime)+","+ String(pirState) + ",[["+ row01 +"],[" +row23 +"],["+ row45 +"],["+ row67 +"]]]"; //String(occupancy) + " , " + String(distance)
  
  String data="[" + String(timeStamp)+","+ String(pirState) + ",[["+ row01 +"],[" +row23 +"],["+ row45 +"],["+ row67 +"]]]"; //String(occupancy) + " , " + String(distance)
  //String httpserver="http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String()+OM2M_MN;
  //String httpserver="https://" + String() + CSE_IP + ":" + String() /*+ CSE_PORT + String()*/+OM2M_MN;
  //======================Security==============================
  //Encryption
    ecc.plaintext= data;
    Serial.println("Plain Text: "+ecc.plaintext);
    ecc.encrypt();
    String encrypted = ecc.ciphertext;
    Serial.println("Cipher Text: "+ encrypted);
    
  //======================Security==============================
  //Serial.println(data);
  //http.begin(httpserver + String() +OM2M_AE + "/" + OM2M_DATA_CONT + "/");
  http.begin("https://esw-onem2m.iiit.ac.in/~/in-cse/in-name/Team-28/Node-1/Data/");
  Serial.println("https://esw-onem2m.iiit.ac.in/~/in-cse/in-name/Team-28/Node-1/Data/");
  http.addHeader("X-M2M-Origin", OM2M_ORGIN);
  http.addHeader("Content-Type", "application/json;ty=4");
  http.addHeader("Content-Length", "600");
  
  
  
 
  String label = "Node-1";
  
  String req_data = String() + "{\"m2m:cin\": {"
  
    + "\"con\": \"" + encrypted + "\","
  
//    + "\"rn\": \"" + "cin_"+String(i++) + "\","
  
    + "\"lbl\": \"" + label + "\","
  
    + "\"cnf\": \"text\""
  
    + "}}";
  int code = http.POST(req_data);
  
  Serial.println(code);
  if(code == 201 ){

    Serial.println("CREATED");
  }
  else if (code == -1) {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
//  //======================Security==============================
//  
//    //Decryption
//    ecc.plaintext = F("");
//    //decrypts ecc.ciphertext
//    ecc.decrypt();
//    ecc.ciphertext = F("");
//    Serial.println("Decoded: "+ecc.plaintext);
//  //======================Security==============================
  http.end();
}


void loop(){


  server.handleClient();
  
  val = digitalRead(pirPin);  // read pir value
  if (val == HIGH){
    
    digitalWrite(motionLed, HIGH);
    ledState = HIGH;
    startTimer = true;
    lastTrigger = millis();
    if(pirState == LOW){
      Serial.println("MOTION DETECTED!!!");
      pirState = HIGH;
    }
  }
  else if(val ==LOW){
      if(pirState == HIGH){
        Serial.println("Motion stopped...");
        pirState = LOW;
      }
  }

  now = millis();
  if(startTimer && ((now - lastTrigger) > (timeSeconds*1000))) {
   // Serial.println("Motion stopped...");
   
    digitalWrite(motionLed, LOW);
    ledState=LOW;
    //Serial.println(ledState);
    
    //pirState=LOW;
    startTimer = false;
  }
// if duration after "motion stopped"  and next line is >=10 sec . then it is considered as motion stopped really, otherwise motion is not really stopped.
  
  
  
  //***************GRID EYE CODE
  //Serial.print("Updating grid_eye thermistor temperature ... ");
  int statusCode = grid_sensor.updateThermistorTemperature();
  //Serial.println(grid_sensor.getErrorDescription(statusCode));

  //Serial.print("Updating pixel matrix ... ");
  statusCode = grid_sensor.updatePixelMatrix();
  //Serial.println(grid_sensor.getErrorDescription(statusCode));

  //Serial.print("Thermistor temp: ");
  int gridEyeTemperature = grid_sensor.thermistorTemperature;
  //Serial.print(gridEyeTemperature);
  //Serial.println("°C");
  
  //Serial.println("Temperature Matrix: ");
  String arr[8]={""};
  for (int x = 0; x < 8; x++){
    for (int y = 0; y < 8; y++){
      //Serial.print(grid_sensor.pixelMatrix[y][x]);
      arr[x] = String(arr[x]+ String(grid_sensor.pixelMatrix[y][x]));
      if(y<7)arr[x]= String(arr[x]+",");
      //Serial.print(" ");
    }
    Serial.println();
  }
  
  //***************GRID EYE CODE
//=1111111111111111111111111111
 //we are sending data to thingspeak & OM2M every 30 seconds 
 if(count == 30){ 
  //******* time n date ******
    while(!timeClient.update()) {
      timeClient.forceUpdate();
    }
    // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
//  Serial.println(formattedDate);

 
  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
//  Serial.print("DATE: ");
//  Serial.println(dayStamp);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);
    
  //******* time n date ******
  Serial.print("pir state: ");
  Serial.println(pirState);
  Serial.println("Temperature Matrix: ");
  String arr[8]={""};
  for (int x = 0; x < 8; x++){
    for (int y = 0; y < 8; y++){
      Serial.print(grid_sensor.pixelMatrix[y][x]);
      arr[x] = String(arr[x]+ String(grid_sensor.pixelMatrix[y][x]));
      if(y<7)arr[x]= String(arr[x]+",");
      Serial.print(" ");
    }
    Serial.println();
  }
  
    String row01 = String(arr[0]+","+arr[1]);
    String row23 = String(arr[2]+","+arr[3]);
    String row45 = String(arr[4]+","+arr[5]); //1116111
    String row67 = String(arr[6]+","+arr[7]);

    
//     //************** OM2M *****************
    om2m_createCI(pirState,row01,row23,row45,row67);
//
//    //************** OM2M *****************
    //********** Thingspeak **********
     ThingSpeak.setField(1, pirState);
     ThingSpeak.setField(2, gridEyeTemperature);
     ThingSpeak.setField(3, row01);
     ThingSpeak.setField(4, row23);
     ThingSpeak.setField(5, row45);
     ThingSpeak.setField(6, row67);

 
     int x = ThingSpeak.writeFields(myChannelNumber, WriteAPI);

     if(x == 200){
       Serial.println("Channel update successful.");
     }
     else{
       Serial.println("Problem updating channel. HTTP error code " + String(x));
     }
     //********** Thingspeak **********
     count = 0;
 }
   delay(1000);
   count++;
   //delay(30000);
}
 

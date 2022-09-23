#include <ThingSpeak.h>
//#include <esp32cam.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "HTTPClient.h"
#include "time.h"
#include <WiFiClientSecure.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "Base64.h"

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

//*******CAM codef
#include "esp_camera.h"
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
#include "camera_pins.h"
#define camPin1 13
#define camPin2 14
//*******CAM code

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
//unsigned long epochTime; 

/*Put your SSID & Password*/
//const char *ssid = "Galaxy M112694"; // Enter SSID here
//const char *pwd = "12345678";  //Enter Password here
const char *ssid = "Galaxy M511CCF"; // Enter SSID here
const char *pwd = "ramreddy@3";  //Enter Password here
//const char *ssid = "esw-m19@iiith"; // Enter SSID here
//const char *pwd = "e5W-eMai@3!20hOct";  //Enter Password here

// https://script.google.com/macros/s/AKfycbxUyS-RZ-7dKZ9NswutjR8q9kdm1xOP9nMO6eGxNntD4jjzup1Z1rjpYMR0WNUPTrSi/exec
String myScript = "/macros/s/AKfycbxUyS-RZ-7dKZ9NswutjR8q9kdm1xOP9nMO6eGxNntD4jjzup1Z1rjpYMR0WNUPTrSi/exec";    //Create your Google Apps Script and replace the "myScript" path.
String myLineNotifyToken = "myToken=**********";    //Line Notify Token. You can set the value of xxxxxxxxxx empty if you don't want to send picture to Linenotify.
String myFoldername = "&myFoldername=ESP32-CAM";
String myFilename = "&myFilename=ESP32-CAM.jpg";
String myImage = "&myFile=";

// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled

//CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

WebServer server(80);
//************** OM2M *****************
//#define CSE_IP     "192.168.36.221"// "127.0.0.1"//"esw-onem2m.iiit.ac.in"//"192.168.171.221"//////192.168.36.221 //replace with system-ip  // //esw-onem2m.iiit.ac.in
//#define CSE_PORT   5089// 443 //5089 //443
//#define HTTPS     false
//#define OM2M_ORGIN   "admin:admin"//"zZ!#4s:m&Y#HL" //"admin:admin"// //"zZ!#4s:m&Y#HL" 
//#define OM2M_MN     "/~/in-cse/in-name/"
//#define OM2M_AE     "Od-TEST"//"Team-28"//"Od-TEST"//// //"AE-TEST" //"Team-28"
//#define OM2M_DATA_CONT  "Node-1/Data"


//************** OM2M *****************

void setup()
{
  
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  delay(100);
//  pinMode(pirPin, INPUT);
//  pinMode(motionLed, OUTPUT);
//  digitalWrite(motionLed, ledState);
  
// //********** GRID EYE CODE
//  // initializing I2C to use default address AMG8833_I2C_ADDRESS_B and Wire (I2C-0):
//  Wire.begin();
//  grid_sensor.initI2C();
//  // To use Wire1 (I2C-1):
//  //Wire1.begin();
////  sensor.initI2C(AMG8833_I2C_ADDRESS_B, Wire1);
//  Serial.println("Resetting grid_sensor ... ");  
//  int statusCode = grid_sensor.resetFlagsAndSettings();
//  Serial.println(grid_sensor.getErrorDescription(statusCode));
//
//  Serial.println("Setting FPS ... ");
//  statusCode = grid_sensor.setFPSMode(FPS_MODE::FPS_10);
//  Serial.println(grid_sensor.getErrorDescription(statusCode));
////******** GRID EYE CODE
  
  Serial.println("--------------- Welcome -----------------");
  Serial.println("Connecting to ");
  Serial.println(ssid);
  Serial.println();

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.println("WiFi Connected!!");
  }
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  
camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }
  
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    //ESP.restart();
  }
  else {
  //drop down frame size for higher initial frame rate
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_VGA);  // UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA
  }
  server.begin();
  Serial.println("HTTP server started");
//   ThingSpeak.begin(client); 
//******* time n date ******
  // Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(19800);
}



void loop(){
  
   server.handleClient();
  
//   val = digitalRead(pirPin);  // read pir value
//  if (val == HIGH){
//    
//    digitalWrite(motionLed, HIGH);
//    ledState = HIGH;
//    startTimer = true;
//    lastTrigger = millis();
//    if(pirState == LOW){
//      pirState = HIGH;
//    }
//  }
//  else if(val ==LOW){
//      if(pirState == HIGH){
//        pirState = LOW;
//      }
//  }
//
//  now = millis();
//  if(startTimer && ((now - lastTrigger) > (timeSeconds*1000))) {
//   
//    digitalWrite(motionLed, LOW);
//    ledState=LOW;
//    
//    //pirState=LOW;
//    startTimer = false;
//  }
// if duration after "motion stopped"  and next line is >=10 sec . then it is considered as motion stopped really, otherwise motion is not really stopped.
  
  
  
//  //***************GRID EYE CODE
//  //Serial.print("Updating grid_eye thermistor temperature ... ");
//  int statusCode = grid_sensor.updateThermistorTemperature();
//  //Serial.println(grid_sensor.getErrorDescription(statusCode));
//
//  //Serial.print("Updating pixel matrix ... ");
//  statusCode = grid_sensor.updatePixelMatrix();
//  //Serial.println(grid_sensor.getErrorDescription(statusCode));
//
//  //Serial.print("Thermistor temp: ");
//  int gridEyeTemperature = grid_sensor.thermistorTemperature;
//
//    String arr[8]={""};
//  for (int x = 0; x < 8; x++){
//    for (int y = 0; y < 8; y++){
//      //Serial.print(grid_sensor.pixelMatrix[y][x]);
//      arr[x] = String(arr[x]+ String(grid_sensor.pixelMatrix[y][x]));
//      if(y<7)arr[x]= String(arr[x]+",");
//      //Serial.print(" ");
//    }
//    Serial.println();
//  }
 //we are sending data to thingspeak & OM2M every 30 seconds 
 if(count == 30){ 

    
//    String row01 = String(arr[0]+","+arr[1]);
//    String row23 = String(arr[2]+","+arr[3]);
//    String row45 = String(arr[4]+","+arr[5]);
//    String row67 = String(arr[6]+","+arr[7]);

    
//     //************** OM2M *****************
     //om2m_createCI(pirState,row01,row23,row45,row67);
    //************** OM2M *****************
    
    //********** Thingspeak **********

     SendCapturedImage();
//     ThingSpeak.setField(1, pirState);
//     ThingSpeak.setField(2, gridEyeTemperature);
//     ThingSpeak.setField(3, row01);
//     ThingSpeak.setField(4, row23);
//     ThingSpeak.setField(5, row45);
//     ThingSpeak.setField(6, row67);
//
//     
//     int x = ThingSpeak.writeFields(myChannelNumber, WriteAPI);
//
//     if(x == 200){
//       Serial.println("Channel update successful.");
//     }
//     else{
//       Serial.println("Problem updating channel. HTTP error code " + String(x));
//     }
//     //********** Thingspeak **********
     count = 0;
 }
   delay(1000);
   count++;
}

String SendCapturedImage() {
  const char* myDomain = "script.google.com";
  String getAll="", getBody = "";
  
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    //delay(1000);
    ESP.restart();
    return "Camera capture failed";
  }  
  
  Serial.println("Connect to " + String(myDomain));
  WiFiClientSecure client_tcp;
  client_tcp.setInsecure();   //run version 1.0.5 or above
  
  if (client_tcp.connect(myDomain, 443)) {
    Serial.println("Connection successful");
    
    char *input = (char *)fb->buf;
    char output[base64_enc_len(3)];
    String imageFile = "data:image/jpeg;base64,";
    for (int i=0;i<fb->len;i++) {
      base64_encode(output, (input++), 3);
      if (i%3==0) imageFile += urlencode(String(output));
    }
    String Data = myLineNotifyToken+myFoldername+myFilename+myImage;
    
    client_tcp.println("POST " + myScript + " HTTP/1.1");
    client_tcp.println("Host: " + String(myDomain));
    client_tcp.println("Content-Length: " + String(Data.length()+imageFile.length()));
    client_tcp.println("Content-Type: application/x-www-form-urlencoded");
    client_tcp.println("Connection: keep-alive");
    client_tcp.println();
    
    client_tcp.print(Data);
    int Index;
    for (Index = 0; Index < imageFile.length(); Index = Index+1000) {
      client_tcp.print(imageFile.substring(Index, Index+1000));
    }
    esp_camera_fb_return(fb);
    
    int waitTime = 10000;   // timeout 10 seconds
    long startTime = millis();
    boolean state = false;
    
    while ((startTime + waitTime) > millis())
    {
      Serial.print(".");
      delay(100);      
      while (client_tcp.available()) 
      {
          char c = client_tcp.read();
          if (state==true) getBody += String(c);        
          if (c == '\n') 
          {
            if (getAll.length()==0) state=true; 
            getAll = "";
          } 
          else if (c != '\r')
            getAll += String(c);
          startTime = millis();
       }
       if (getBody.length()>0) break;
    }
    client_tcp.stop();
    Serial.println(getBody);
  }
  else {
    getBody="Connected to " + String(myDomain) + " failed.";
    Serial.println("Connected to " + String(myDomain) + " failed.");
  }
  
  return getBody;
}

//https://github.com/zenmanenergy/ESP8266-Arduino-Examples/
String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
}
 

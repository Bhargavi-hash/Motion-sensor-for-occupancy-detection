#include <ThingSpeak.h>
//#include <esp32cam.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "HTTPClient.h"
#include "time.h"

#include <Melopero_AMG8833.h>
Melopero_AMG8833 grid_sensor;

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
unsigned long epochTime; 

/*Put your SSID & Password*/
//const char *ssid = "Galaxy M112694"; // Enter SSID here
//const char *pwd = "12345678";  //Enter Password here
const char *ssid = "Galaxy M511CCF"; // Enter SSID here
const char *pwd = "ramreddy@3";  //Enter Password here

WebServer server(80);
//void startCameraServer();

//static auto loRes = esp32cam::Resolution::find(320, 240);
//static auto midRes = esp32cam::Resolution::find(350, 530);
//static auto hiRes = esp32cam::Resolution::find(800, 600);
//************** OM2M *****************
#define CSE_IP      "esw-onem2m.iiit.ac.in"//"192.168.171.221"//////192.168.36.221 //replace with system-ip  //127.0.0.1 //esw-onem2m.iiit.ac.in
#define CSE_PORT    443 //5089 //443
#define HTTPS     false
#define OM2M_ORGIN   "zZ!#4s:m&Y#HL" //"admin:admin"// //"zZ!#4s:m&Y#HL" 
#define OM2M_MN     "/~/in-cse/in-name/"
#define OM2M_AE     "Team-28"//"Od-TEST"//// //"AE-TEST" //"Team-28"
#define OM2M_DATA_CONT  "Node-1/Data"

//************** OM2M *****************

//void serveJpg()
//{
//  auto frame = esp32cam::capture();
//  if (frame == nullptr) {
//    Serial.println("CAPTURE FAIL");
//    server.send(503, "", "");
//    return;
//  }
//  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
//                static_cast<int>(frame->size()));
// 
//  server.setContentLength(frame->size());
//  server.send(200, "image/jpeg");
//  WiFiClient client = server.client();
//  frame->writeTo(client);
//}
 
//void handleJpgLo()
//{
//  if (!esp32cam::Camera.changeResolution(loRes)) {
//    Serial.println("SET-LO-RES FAIL");
//  }
//  serveJpg();
//}
// 
//void handleJpgHi()
//{
//  if (!esp32cam::Camera.changeResolution(hiRes)) {
//    Serial.println("SET-HI-RES FAIL");
//  }
//  serveJpg();
//}
// 
//void handleJpgMid()
//{
//  if (!esp32cam::Camera.changeResolution(midRes)) {
//    Serial.println("SET-MID-RES FAIL");
//  }
//  serveJpg();
//  // capturing
//}



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
 // {
//    using namespace esp32cam;
//    Config cfg;
//    cfg.setPins(pins::AiThinker);
//    cfg.setResolution(hiRes);
//    cfg.setBufferCount(2);
//    cfg.setJpeg(80);
// 
//    bool ok = Camera.begin(cfg);
//    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
//  }
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.println("WiFi Connected!!");
  }
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.println("  /cam-lo.jpg");
  Serial.println("  /cam-hi.jpg");
  Serial.println("  /cam-mid.jpg");
 
//  server.on("/cam-lo.jpg", handleJpgLo);
//  server.on("/cam-hi.jpg", handleJpgHi);
//  server.on("/cam-mid.jpg", handleJpgMid);
 
  server.begin();
  Serial.println("HTTP server started");
  ThingSpeak.begin(client); 
}
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void om2m_createCI(int & pirState, String & row01,String & row23,String & row45,String & row67){

  epochTime = getTime();
  HTTPClient http;
  
  String data="[" + String(epochTime)+","+ String(pirState) + ",[["+ row01 +"],[" +row23 +"],["+ row45 +"],["+ row67 +"]]]"; //String(occupancy) + " , " + String(distance)

  String httpserver="http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String()+OM2M_MN;
  
  Serial.println(data);
  http.begin(httpserver + String() +OM2M_AE + "/" + OM2M_DATA_CONT + "/");
  
  http.addHeader("X-M2M-Origin", OM2M_ORGIN);
  http.addHeader("Content-Type", "application/json;ty=4");
  http.addHeader("Content-Length", "600");
  
  
  
  static int i=0;
  String label = "Node-1";
  
  String req_data = String() + "{\"m2m:cin\": {"
  
    + "\"con\": \"" + data + "\","
  
    + "\"rn\": \"" + "cin_"+String(i++) + "\","
  
    + "\"lbl\": \"" + label + "\","
  
    + "\"cnf\": \"text\""
  
    + "}}";
  int code = http.POST(req_data);
  Serial.println(code);
  if (code == -1) {
  Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
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
  Serial.print("Updating grid_eye thermistor temperature ... ");
  int statusCode = grid_sensor.updateThermistorTemperature();
  Serial.println(grid_sensor.getErrorDescription(statusCode));

  Serial.print("Updating pixel matrix ... ");
  statusCode = grid_sensor.updatePixelMatrix();
  Serial.println(grid_sensor.getErrorDescription(statusCode));

  Serial.print("Thermistor temp: ");
  int gridEyeTemperature = grid_sensor.thermistorTemperature;
  Serial.print(gridEyeTemperature);
  Serial.println("°C");
  
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
  String row45 = String(arr[4]+","+arr[5]);
  String row67 = String(arr[6]+","+arr[7]);

  //***************GRID EYE CODE
 
 //we are sending data to thingspeak & OM2M every 30 seconds 
 if(count == 30){ 
     //************** OM2M *****************
     om2m_createCI(pirState,row01,row23,row45,row67);

    //************** OM2M *****************
     ThingSpeak.setField(1, pirState);
     ThingSpeak.setField(2, gridEyeTemperature);
     ThingSpeak.setField(3, row01);
     ThingSpeak.setField(4, row23);
     ThingSpeak.setField(5, row45);
     ThingSpeak.setField(6, row67);

 //    Serial.println();
 //    Serial.println("Pixel matrix rows converted to strings by values being separated by commas");
 //    Serial.println();
 //    Serial.print("row 0 & 1: ");
 //    Serial.println(row01);
 //    Serial.print("row 2 & 3: ");
 //    Serial.println(row23);
 //    Serial.print("row 4 & 5: ");
 //    Serial.println(row45);
 //    Serial.print("row 6 & 7: ");
 //    Serial.println(row67);
 //    Serial.println();
     // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
     // pieces of information in a channel. 

     int x = ThingSpeak.writeFields(myChannelNumber, WriteAPI);

     if(x == 200){
       Serial.println("Channel update successful.");
     }
     else{
       Serial.println("Problem updating channel. HTTP error code " + String(x));
     }
     count = 0;
 }
   delay(1000);
   count++;
   //delay(30000);
}
 

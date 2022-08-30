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


/*Put your SSID & Password*/
const char *ssid = "Galaxy M112694"; // Enter SSID here
const char *pwd = "12345678";  //Enter Password here

//WebServer server(80);
void startCameraServer();



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
  // Wire1.begin();
  // sensor.initI2C(AMG8833_I2C_ADDRESS_B, Wire1);
  Serial.print("Resetting grid_sensor ... ");  
  int statusCode = grid_sensor.resetFlagsAndSettings();
  Serial.println(grid_sensor.getErrorDescription(statusCode));

  Serial.print("Setting FPS ... ");
  statusCode = grid_sensor.setFPSMode(FPS_MODE::FPS_10);
  Serial.println(grid_sensor.getErrorDescription(statusCode));
//******** GRID EYE CODE
  

  
  //**********CAM code
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
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(camPin1, INPUT_PULLUP);
  pinMode(camPin2, INPUT_PULLUP);
#endif
  // camera init
  esp_err_t cam_err = esp_camera_init(&config);
  if (cam_err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", cam_err);
    return;
  }
  
  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);
  
#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif 
  //***********CAM code
  
  // Serial.println("--------------- Welcome -----------------");
  Serial.println("Connecting to ");
  Serial.println(ssid);
  //connect to your local wi-fi network
  WiFi.begin(ssid, pwd);
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());
  
//*******CAM code
  startCameraServer();
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
//*******CAM code
  
 // server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  //server.handleClient();
  
  
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
  else{
    pirState=LOW;
    now = millis();
    if(startTimer==true) {
      if((now - lastTrigger) > (timeSeconds*1000)){
        Serial.println("Motion stopped...");
        digitalWrite(motionLed, LOW);
        ledState = LOW;
        startTimer = false;
      }
      else {
        digitalWrite(motionLed, HIGH);
        ledState = HIGH;
      }
    }
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
  Serial.print(grid_sensor.thermistorTemperature);
  Serial.println("°C");

  Serial.println("Temperature Matrix: ");
  for (int x = 0; x < 8; x++){
    for (int y = 0; y < 8; y++){
      Serial.print(grid_sensor.pixelMatrix[y][x]);
      Serial.print(" ");
    }
    Serial.println();
  }

  delay(1000);
  //***************GRID EYE CODE
}
 

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "HTTPClient.h"
#include "time.h"
#include <Melopero_AMG8833.h>
Melopero_AMG8833 grid_sensor;
#include "esp_camera.h"
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
#include "camera_pins.h"
#define camPin1 13
#define camPin2 14
#define pirPin 19
#define motionLed 18
// SCL_pin is GPIO22 in esp32
// SDA_pin is GPIO21 in esp32

bool isDetected;
float t;
String  motion;

//String distance = "200";
/*Put your SSID & Password*/
const char *ssid = "redmi"; // Enter SSID here
const char *pwd = "09262525";  //Enter Password here

void startCameraServer();

String cse_ip = "192.168.1.7"; // Do ifconfig to get your ip.
String cse_port = "8080";
String Server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";

String ae1 = "eswPIR";
String cnt1 = "node1";
void CreateCImotion(String &val)
{
  HTTPClient http;
  http.begin(Server + ae1 + "/" + cnt1 + "/");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");

  int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");

  Serial.println(code);
  if (code == -1)
  {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
  http.end();
}

WebServer server(80);
void send_event(const char *event);
void setup()
{
  //Serial.begin(9600);
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  delay(100);
  pinMode(pirPin, INPUT);
  pinMode(motionLed, OUTPUT);
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
  digitalWrite(motionLed, LOW);
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
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
  
  digitalWrite(motionLed, LOW);
  
  int m = 0;
  for (int i = 0; i < 10; i++)
  {
    isDetected = digitalRead(pirPin);
    String motion_val = (String)isDetected;
    CreateCImotion(motion_val);
    motion = String(isDetected);
    if (isDetected)
    {
      m = 1;
      Serial.println("Motion detected");
      digitalWrite(motionLed, HIGH);
      
      delay(1000);
    }
  }
  
  if (m == 0)
  {
    Serial.println("No Motion Detected");
  }
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
 

// float t = dht.readTemperature();
// String temp_val = (String)t;
// CreateCItemp(temp_val);
//   t = 51;
// Serial.print("Temperature: ");
// Serial.println(t);
// temperature = String(t);
// //t = 51;
// if (t > 50)
// {
//   digitalWrite(fireLed, HIGH);
//   redled_status = String("ON");
//   digitalWrite(buzzer, HIGH);
//   buzzer_status = String("ON");
//   delay(1000);
// }

// while(authentication == 1)
// {
//   digitalWrite(fireLed, LOW);
//   digitalWrite(buzzer, LOW);
//   isDetected = digitalRead(pirPin);
//   motion = String(isDetected);
//   if(isDetected)
//   {
//     Serial.println("Motion detected");
//   }
//   float t = dht.readTemperature();
//   temperature = String(t);

//   if (t > 50)
//   {
//     digitalWrite(fireLed, HIGH);
//     redled_status = String("ON");
//     buzzer_status = String("ON");
//     digitalWrite(buzzer, HIGH);
//     Serial.print("Temperature: ");
//     Serial.println(t);
//     delay(1000);
//   }

//   String exiting;
//   if(Serial.available() > 0)
//   {
//     exiting = Serial.readString();
//     if(exiting == "q\n")
//     {
//       authentication = 0;
//     }
//     if(exiting == "exit\n")
//     {
//       exit(0);
//     }
//   }
//   delay(1000);

// }
// delay(500);
// }

// void send_event(const char *event)
// {
//   Serial.print("Connecting to ");
//   Serial.println(host);
//   WiFiClient client;
//   const int httpPort = 80;
//   if (!client.connect(host, httpPort))
//   {
//     Serial.println("Connection failed");
//     return;
//   }
//   String url = "/trigger/";
//   url += event;
//   url += "/with/key/";
//   url += privateKey;
//   Serial.print("Requesting URL: ");
//   Serial.println(url);
//   client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

//   while (client.connected())
//   {
//     if (client.available())
//     {
//       // sends msg to server

//       String line = client.readStringUntil('\r');
//       Serial.print(line);
//     }
//     else
//     {
//       delay(50);
//     }
//   }
//   Serial.println();
//   Serial.println("Closing Connection");
//   client.stop();
// }
//**************SendHTML FUNCTIONS START*************************
// const char HOME_page[] PROGMEM = R"=====(
// <!DOCTYPE html>
// <html>
// <head>
//     <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
//     <title>Home</title>
// <style>
// * {
//    box-sizing: border-box;
// }
// body {
//    margin: 0;
//    font-family: Arial;
//    font-size: 17px;
// }
// #myVideo {
//    position: fixed;
//    right: 0;
//    bottom: 0;
//    min-width: 100%;
//    min-height: 100%;
// }
// .content {
//    position: fixed;
//    bottom: 0;
//    background: rgba(0, 0, 0, 0.5);
//    color: #f1f1f1;
//    width: 100%;
//    padding: 20px;
// }
// #myBtn {
//    width: 200px;
//    font-size: 18px;
//    padding: 10px;
//    border: none;
//    background: #ddd;
//    color: midnightblue;
//    cursor: pointer;
// }
// #myBtn:hover {
//    background: black;
//    color: white;
// }
// </style>

// <script >
//     function pageRedirect() {

//     setTimeout("window.location.href = '/login'", 0);
// }
// var video = document.getElementById("myVideo");
// var btn = document.getElementById("myBtn");
// function myFunction() {
//     if (video.paused) {
//         video.play();
//         btn.innerHTML = "Pause";
//     } else {
//         video.pause();
//         btn.innerHTML = "Play";
//     }
// }</script>

// </head>
// <body>
//     <video autoplay muted loop id="myVideo">
//         <source src="https://st4.depositphotos.com/11486624/24816/v/600/depositphotos_248163784-stock-video-iot-internet-things-devices-symbols.mp4" type="video/mp4">
//         Your browser does not support HTML5 video.
//     </video>
//     <div class="content">
//         <h1>BigFoot</h1>
//         <h3>Anti Theft and Fire alarm</h3>
//         <p>IoT project Monson 2021</p>
//         <p>- Bhargavi</p>
//         <p>- Ruchita</p>
//         <p>- Nandini</p>
//         <p>- Girija</p>
//         <button id="myBtn" onclick="pageRedirect()">Sign in</button>
//     </div>
// </body>
// </html>
// )=====";

// const char LOGIN_page[] PROGMEM = R"=====(
// <!DOCTYPE html>
// <html>
// <head>
//     <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
//     <title>Login</title>
//     <script >
//     var UserName = "Bhargavi"
// var PassWord = "1234"
// var maxChances = 5
// var cur_chance = 0
// function authentication() {
//     var cust_Name = document.getElementById("myID").value;
//     var cust_Password = document.getElementById("myPassword").value;
//     if(cust_Name == UserName)
//     {
//         if(cust_Password == PassWord)
//         {

//             setTimeout("window.location.replace('/monitor'), 0")
//         }
//     }
//     else{
//         cur_chance = cur_chance + 1
//         alert("Invalid Authenticaton !! Please try again. ");
//     }

// }
// </script>
// <style>
// body {
//     background-color: skyblue;
// }
// .login {
//     position: absolute;
//     left: 100px;
//     top: 40px;
//     border-radius: 100%;
// }
// #myVideo {
//     position: fixed;
//     right: 0;
//     bottom: 0;
//     min-width: 100%;
//     min-height: 100%;
//  }

// .container {
//     border-radius: 20px;
//     padding-top: 400px;
//     padding-bottom: 20px;
//     padding-left: 25px;
//     padding-right: 25px;
//     background-color: skyblue;
//     opacity: 0.9;
// }
// form {
//     color: midnightblue;
//     font-size: 25px;
//     font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;
//     position: absolute;
//     left: 600px;
//     top: 100px;
//     width: 550px;
//     height: 700px;
//     border-radius: 30px;
// }
// input[type=text], input[type=password] {
//     width: 100%;
//     margin: 8px 0;
//     padding: 12px 20px;
//     display: inline-block;
//     border: 3px solid midnightblue;
//     box-sizing: border-box;
//     border-radius: 10px;
//     outline: none;
// }
// button {
//     font-size: 20px;
//     background-color: midnightblue;
//     width: 100%;
//     color: white;
//     padding: 15px;
//     margin: 10px 0px;
//     border: none;
//     cursor: pointer;
//     border-radius: 10px;
// }
// button:hover {
//     transform: scale(1.02);
//     opacity: 0.7;
// }
// </style>

//     <meta charset="utf-8">
// </head>
// <body>
//     <video autoplay muted loop id="myVideo">
//         <source src="https://st4.depositphotos.com/11486624/24816/v/600/depositphotos_248163784-stock-video-iot-internet-things-devices-symbols.mp4" type="video/mp4">
//         Your browser does not support HTML5 video.
//     </video>
//     <form>
//         <div class="container">
//             <div class="login">
//                 <img src="https://static.vecteezy.com/system/resources/previews/002/640/730/original/default-avatar-placeholder-profile-icon-male-vector.jpg" style="width: 360px; height:360px; border-radius: 100%; object-fit: cover; ">
//             </div>
//             <form>
//                 <label>Username : </label>
//                 <input type="text" id="myID" placeholder="Enter Username" name="username" required>
//                 <label>Password : </label>
//                 <input type="password" id="myPassword" placeholder="Enter Password" name="password" required>
//                 <button type="submit" onclick="authentication()">Login</button>
//         </div>
//     </form>
// </body>
// </html>
// )=====";

// String SendHTML_home(){
//   String s = HOME_page;
//   return s;
// }
// String SendHTML_login(){
//    String s = LOGIN_page;
//   return s;
// }
// String SendHTML_monitor(String temperature,String humidity,/*String pressure,*/String redled_status,String buzzer_status,String greenled_status,String motion){

// String ptr = "<!DOCTYPE html> <html>\n";
//   ptr +="<head><meta charset=\"utf-8\">\n";
//   ptr +="<title>Monitor</title>\n";

//   ptr +="<style>body {background-image: url('https://clockwise.software/img/blog/best-iot-startups-to-follow/header-background.jpg');background-repeat: no-repeat;background-attachment: fixed;background-size: cover;}\n";
//   ptr +=".container {margin: 150px 250px 80px 250px;display: flex;justify-content: space-between;}\n";
//   ptr +=".safety_status {margin-left: 10px;margin-top: 10px;}\n";
//   ptr +=".box {display: flex;}\n";
//   ptr +=".hide {position: relative;z-index: 2;background-color: white;color: #444;margin-top: 40px;margin-left: -30px;padding: 30px;padding-bottom: 18px;padding-left: 55px;border-top-right-radius: 50px;border-bottom-right-radius: 50px;height: 50px;border: none;text-align: center;outline: none;font-size: 30px;font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}\n";
//   ptr +=".slidebtn {z-index: 3;border: none;outline: none;background: none;cursor: pointer;transition: 0.5s;}\n";
//   ptr +=".slidebtn:hover{transform: scale(1.3);}\n";
//   ptr +=".control-btns-panel {display: flex;justify-content: center;}\n";
//   ptr +=".control-btn {margin: 30px;outline: none;border: none;padding: 30px;text-align: center;vertical-align: middle;height: 70px;font-size: 30px;font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;background-color: lightgreen;border-radius: 10px;cursor: pointer;transition: 0.3s;}\n";
//   ptr +=".control-btn:hover {background-color: lightseagreen;color: white;transform: scale(1.1);}\n";
//   ptr +="</style>\n";

//   ptr +="<script>\n";
// ptr +="setInterval(loadDoc,1000);\n";
// ptr +="function loadDoc() {\n";
// ptr +="var xhttp = new XMLHttpRequest();\n";
// ptr +="xhttp.onreadystatechange = function() {\n";
// ptr +="if (this.readyState == 4 && this.status == 200) {\n";
// ptr +="document.body.innerHTML =this.responseText}\n";
// ptr +="};\n";
// ptr +="xhttp.open(\"GET\", \"/monitor\", true);\n";
// ptr +="xhttp.send();\n";
// ptr +="}\n";
// ptr +="</script>\n";

//   ptr +="</head>\n";

//   ptr +="<body>\n";
//   ptr +="<div class=\"container\">\n";
//   ptr +="<div class=\"safety_status\">\n";
//   ptr +="<div class=\"box\">\n";
//   ptr +="<button class=\"slidebtn\"><img src=\"https://image.shutterstock.com/image-vector/led-light-bulb-icon-on-260nw-709931749.jpg\" style=\"width: 150px; height:150px; border-radius: 100%; object-fit: cover;\"></button>\n";
//   ptr +="<div class=\"hide\">\n";
//   ptr +=redled_status;
//   ptr += "</div>\n";
//   ptr += "</div>\n";
//   ptr += "<br>\n";
//   ptr += "<br>\n";
//   ptr +="<div class=\"box\">\n";
//   ptr +="<button class=\"slidebtn\"><img src=\"https://st.depositphotos.com/1310390/4858/v/950/depositphotos_48584945-stock-illustration-temperature-icon-with-a-thermometer.jpg\" style=\"width: 150px; height:150px; border-radius: 100%; object-fit: cover;\"></button>\n";
//   ptr +="<div class=\"hide\">\n";
//   ptr += temperature;
//   ptr += "</div>\n";
//   ptr += "</div>\n";
//   ptr += "<br>\n";
//   ptr += "<br>\n";
//   ptr += "<div class=\"box\">\n";
//   ptr += "<button class=\"slidebtn\"><img src=\"https://media.istockphoto.com/vectors/motion-sensor-icon-in-comic-style-sensor-waves-with-man-vector-vector-id1093505818\" style=\"width: 150px; height:150px; border-radius: 100%; object-fit: cover;\"></button>\n";
//   ptr += "<div class=\"hide\">\n";
//   ptr += motion;
//   ptr += "</div>\n";
//   ptr += "</div>\n";
//   ptr += "</div>\n";

//   ptr += "<div class=\"safety_status\">\n";
//   ptr += "<div class=\"box\">\n";
//   ptr += "<button class=\"slidebtn\"><img src=\"https://media.istockphoto.com/vectors/emergency-siren-icon-in-flat-style-police-alarm-vector-illustration-vector-id1144204158?k=20&m=1144204158&s=170667a&w=0&h=vWFXabMbVd42h0XOVN9OCnFotB41Ul4thJNVos_BuWg=\" style=\"width: 150px; height:150px; border-radius: 100%; object-fit: cover;\"></button>\n";
//   ptr += "<div class=\"hide\">\n";
//   ptr += buzzer_status;
//   ptr += "</div>\n";
//   ptr += "</div>\n";
//   ptr += "<br>\n";
//   ptr += "<br>\n";
//   ptr += "<div class=\"box\">\n";
//   ptr += "<button class=\"slidebtn\"><img src=\"https://previews.123rf.com/images/sanek13744/sanek137441908/sanek13744190800082/128009902-humidity-icon-in-comic-style-climate-vector-cartoon-illustration-on-white-isolated-background-temper.jpg\" style=\"width: 150px; height:150px; border-radius: 100%; object-fit: cover;\"></button>\n";
//   ptr += "<div class=\"hide\">\n";
//   ptr += humidity;
//   ptr += "</div>\n";
//   ptr += "</div>\n";
//   ptr += "<br>\n";
//   ptr += "<br>\n";
//   ptr += "<div class=\"box\">\n";
//   ptr += "<button class=\"slidebtn\"><img src=\"https://thumb7.shutterstock.com/image-photo/stock-vector-vector-illustration-of-red-bulb-icon-250nw-575638942.jpg\" style=\"width: 150px; height:150px; border-radius: 100%; object-fit: cover;\"></button>\n";
//   ptr += "<div class=\"hide\">\n";
//   ptr += greenled_status;
//   ptr += "</div>\n";
//   ptr += "</div>\n";
//   ptr += "</div>\n";
//   ptr += "</div>\n";

//   ptr += "<div class=\"control-btns-panel\">\n";

//    /*if(led1stat)
//   {ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
//   else
//   {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}
//   if(led2stat)
//   {ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
//   else
//   {ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}
// */
//   /*ptr += "<button class=\"control-btn\">LED</button>\n";
//   ptr += "<button class=\"control-btn\">Buzzer</button>\n";*/
//     ptr +="<a href=\"/monitorled\"><button class=\"control-btn\">LED</button></a>\n";
// ptr +="<a href=\"/monitorbuzzer\"><button class=\"control-btn\">Buzzer</button></a>\n";
//   ptr += "</div>\n";

//   ptr +="</body>\n";

//   ptr +="</html>\n";
//   return ptr;
// }
// //**************SendHTML FUNCTIONS END*************************

// //**************SENSOR FUNCTIONS START*************************
// //**************SENSOR FUNCTIONS END*************************
// //**************HANDLE FUNCTIONS START*************************
// void handle_OnConnect() {

//   server.send(200, "text/html", SendHTML_home());
// }
// void handle_login(){
//   server.send(200, "text/html", SendHTML_login()); }

// void handle_monitor(){

//   server.send(200, "text/html", SendHTML_monitor(temperature,humidity,/* pressure,*/redled_status, buzzer_status, greenled_status, motion));
//   }

// void handle_NotFound(){
//   server.send(404, "text/plain", "Not found");
// }
// ////////extra
// void handle_monitorled(){
//     if(redled_status=="ON"){
//           digitalWrite(motionLed, LOW);
//           //digitalWrite(fireLed, LOW);
//           redled_status=="OFF";
//       }
//       else {
//           digitalWrite(motionLed, HIGH);
//           redled_status=="ON";
//       }
//   server.send(200, "text/html", SendHTML_monitor(temperature,humidity,redled_status, buzzer_status, greenled_status, motion));
//   }
//   void handle_monitorbuzzer(){
//       if(buzzer_status=="ON"){
//           digitalWrite(buzzer, LOW);
//           buzzer_status=="OFF";
//       }
//       else {
//           buzzer_status=="ON";
//           digitalWrite(buzzer, HIGH);
//           //delay(50);
//           //digitalWrite(buzzer,LOW);

//       }
//   server.send(200, "text/html", SendHTML_monitor(temperature,humidity,redled_status, buzzer_status, greenled_status, motion));
//   }
//   ////////////////////
// //**************HANDLE FUNCTIONS END*************************

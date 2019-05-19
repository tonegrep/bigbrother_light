#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include "bigbrother_common.h"
#include <Timer.h>

const char* CONTROLLER_UUID = "2XadeeI7rJ3QsbeOHAiJ";
int redPin = D5;
int greenPin = D6;
int bluePin = D7;
int CONTROLLER_PORT = 301;
int CURRENT_BRIGHTNESS;
const char* SSID = "aaaaaaaaaaaaaaa";
const char* PASSWORD = "89153510369";
const char* GLOBAL_IP;
const char* LOCAL_IP;
int STATUS_CODE;
MDNSResponder mdns;
ESP8266WebServer server(CONTROLLER_PORT);

Timer timer_send_data;

//utilitary function to store public ip address
// const char* getExternalIP()
// {
//   HTTPClient http;
//   http.begin("http://api.ipify.org/");
//   int httpCode = http.GET(); 
//   String payload;
//   if (httpCode > 0) {
//     payload = http.getString();
//     Serial.println(payload);
//   }
//   http.end();
//   return payload.c_str();
// }

void setup() {
  Serial.begin(115200);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  // Connect to WiFi network
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  // Setup mdns
  if (mdns.begin("esp8266WebForm", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  // Bound functions to requests
  server.on("/", page);
  server.on("/SET", set_brightness);
  server.on("/GET", get_brightness);

  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());

//  timer_send_data.setInterval(10000);
//  timer_send_data.setCallback(send_data);
//  timer_send_data.start();
  timer_send_data.every(10000, send_data, 0);

  change_status(STATUS_READY);
}

void loop() {
  server.handleClient();
  timer_send_data.update();
}

void page() {
   String s = "<!DOCTYPE HTML>\r\n<html>\r\n";
   s += "<h3>current brightness is ";
   s += CURRENT_BRIGHTNESS;
   s += "</h3>";
   s += "<form action=\"SET\" method=\"post\">";
   s += "<h5> Set new brightness: </h5>";
   s += "<input name=\"brightness\">";
   s += "<button type=\"submit\" value=\"SET\"> Set </button>";
   s += "</html>\n";
  respond_ok("text/html", s);
}

void set_brightness() {
  change_status(STATUS_BUSY);
  if (server.hasArg("plain")== false) { //Check if body received
    server.send(403, "text/plain", "Body not received");
    return;
  }
  String message = server.arg("plain");
  message = message.substring(message.indexOf('=') + 1, message.length());
  Serial.println(message);
  int brightness = message.toInt();
  analogWrite(redPin, brightness);
  analogWrite(greenPin, brightness);
//  analogWrite(bluePin, brightness);
  CURRENT_BRIGHTNESS = brightness;
  change_status(STATUS_READY);
  send_data(0);
  respond_ok("text/plain", "set complete");
}

void get_brightness() {
  String response = String(CURRENT_BRIGHTNESS);
  respond_ok("text/plain", response);
}

void send_data(void* context) {
  HTTPClient http;
  http.begin(HOST_TRANSMIT);
  http.addHeader("Content-Type", "text/json");
  int httpCode = http.POST(create_response(JOB_CLEAR, CURRENT_BRIGHTNESS));
  String payload = http.getString();
  http.end();
}

void change_status(int status) {
  STATUS_CODE = status;
  HTTPClient http;
  http.begin(HOST_TRANSMIT);
  http.addHeader("Content-Type", "text/json");
  int httpCode = http.POST(create_response(STATUS_CHANGED, STATUS_CODE));
  String payload = http.getString();
  http.end();
}

void respond_ok(String response_type, String response){
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, response_type, response);
}

String create_response(int response_code, int data) {
  const int capacity = JSON_OBJECT_SIZE(4);
  StaticJsonDocument<capacity> doc;
  doc["type"] = CONTROLLER_TYPE_LIGHT;
  doc["UUID"] = CONTROLLER_UUID;
  doc["status"] = STATUS_CODE;
  doc["response_code"] = response_code;
  doc["data"] = data;
  String output;
  serializeJson(doc, output);
  return output;
}

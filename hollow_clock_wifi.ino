#include <ESP8266WiFiMulti.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <AccelStepper.h>

#include "dashboard.h"
#include "config.h"

#define IN1 14
#define IN2 12
#define IN3 13
#define IN4 15

#define ACCELSTEPPER_MODE 0

Dashboard* dashboard;
AsyncWebServer server(80);

bool test = false;

const int stepsPerRotation = 30720; // minute hand

#if ACCELSTEPPER_MODE
AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);
#endif

const long utcOffsetInSeconds = 19800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

ESP8266WiFiMulti wifiMulti;
const uint32_t connectTimeoutMs = 5000;

int target,current_pos;

int delaytime = 2;
int port[4] = {14,12,13,15};
int total_steps = 0;

// sequence of stepper motor control
int seq[8][4] = {
  {  LOW, HIGH, HIGH,  LOW},
  {  LOW,  LOW, HIGH,  LOW},
  {  LOW,  LOW, HIGH, HIGH},
  {  LOW,  LOW,  LOW, HIGH},
  { HIGH,  LOW,  LOW, HIGH},
  { HIGH,  LOW,  LOW,  LOW},
  { HIGH, HIGH,  LOW,  LOW},
  {  LOW, HIGH,  LOW,  LOW}
};


void rotate(int step) {
  static int phase = 0;
  int i, j;
  int delta = (step > 0) ? 1 : 7;
  int dt = 20;

  step = (step > 0) ? step : -step;
  for(j = 0; j < step; j++) {
    phase = (phase + delta) % 8;
    for(i = 0; i < 4; i++) {
      digitalWrite(port[i], seq[phase][i]);
    }
    delay(dt);
    if(dt > delaytime) dt--;
  }
  // power cut
  for(i = 0; i < 4; i++) {
    digitalWrite(port[i], LOW);
  }
}

int get_current_time_steps() {
  timeClient.update();
  int total_steps = 0;
  
  total_steps+= (timeClient.getHours()%12)*stepsPerRotation;
  total_steps+= (timeClient.getMinutes()*stepsPerRotation)/60;
  return total_steps;
}

void connect_wifi() {
  WiFi.mode(WIFI_STA);

  // Register multi WiFi networks
  wifiMulti.addAP(WIFI1_SSID, WIFI1_PASSWD);
  wifiMulti.addAP(WIFI2_SSID, WIFI2_PASSWD);

  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
  } else {
    Serial.println("WiFi not connected!");
  }
}

int get_target() {
  return target;
}

int get_current_pos() {
  return current_pos;
}



void setup() {
  Serial.begin(115200);
  connect_wifi();

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("esp8266")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  Serial.println("Starting server...");
  dashboard = new Dashboard(&server);

  dashboard->Add<uint32_t>("Uptime", millis, 5000);
  dashboard->Add<float>(
      "Current Pos", []() { return current_pos; }, 2000);
  dashboard->Add<float>(
      "Target", []() { return target; }, 2000);
  server.begin();
  Serial.println("Started server.");

  timeClient.begin();
  

#if ACCELSTEPPER_MODE
  stepper.setMaxSpeed(1100);
  stepper.moveTo(target);
  stepper.setSpeed(1000);
#else
  pinMode(port[0], OUTPUT);
  pinMode(port[1], OUTPUT);
  pinMode(port[2], OUTPUT);
  pinMode(port[3], OUTPUT);
  rotate(-20);
  rotate(20);
  target = get_current_time_steps();
#endif
}

void loop() {
  MDNS.update();
  test = !test;
  
#if ACCELSTEPPER_MODE
  current_pos = stepper.currentPosition();
  if (target - current_pos >= 512) { //more than a minute
    stepper.runSpeedToPosition();
    return;
  }
  Serial.println("Reached");
  target = get_current_time_steps();
  if (target < current_pos) {
    stepper.setCurrentPosition(current_pos - stepsPerRotation*12);
  }
  stepper.moveTo(target);
  stepper.setSpeed(1000);

#else 
  
  if (target-current_pos >= 512) { //more than a minute
    rotate(512);
    current_pos+=512;
    return;
  }
  
  target = get_current_time_steps();
  if (target < current_pos) { // 0 hours after 23 hours
    current_pos = current_pos - stepsPerRotation*12;
  }
#endif
}

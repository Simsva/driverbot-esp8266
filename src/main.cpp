#define MOTOR_POW 4
#define MOTOR_DIR 2
#define SERVO_PIN 14 // D5

// Defines WIFI_SSID, WIFI_PASS and MQTT_SERVER
#include "secret.h"

#include <cstring>
#include <cstdlib>

#include <math.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

Servo servo;
WiFiClient net;

void callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic, "speed") == 0) {
    int speed = atoi((char*)payload);
    digitalWrite(MOTOR_DIR, speed < 0);
    analogWrite(MOTOR_POW, abs(speed));
  } else if(strcmp(topic, "direction") == 0) {
    int direction = atoi((char*)payload);
    servo.write(direction);
  }
}

PubSubClient client(MQTT_SERVER, 1883, callback, net);

void setup() {
  pinMode(MOTOR_POW, OUTPUT);
  pinMode(MOTOR_DIR, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println(WiFi.localIP());

  if(client.connect("driverbot")) {
    Serial.println("Connected");
    client.publish("hello", "Hello");
    client.subscribe("speed");
    client.subscribe("direction");
  } else {
    Serial.print("Ded");
  }

  servo.attach(SERVO_PIN);
}

void loop() {
  client.loop();
}

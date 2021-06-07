#define MOTOR_POW 4
#define MOTOR_DIR 2
#define SERVO_PIN 14 // D5

// Defines WIFI_SSID, WIFI_PASS and MQTT_SERVER
#include "secret.h"

#include <math.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

Servo servo;
WiFiClient net;

void callback(char* topic, byte* payload, unsigned int length) {
  /*
   * Payload structure
   *  - 2 byte : Signed motor speed, int
   *  - 1 byte : Unsigned servo rotation, int
   */
  int16_t speed = *(int16_t*)payload;
  uint8_t direction = *(uint8_t*)(payload + sizeof(int16_t));

  digitalWrite(MOTOR_DIR, speed < 0);
  analogWrite(MOTOR_POW, abs(speed));

  servo.write(direction);
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

  while(!client.connected()) {
    if(client.connect("driverbot")) {
      Serial.println("Connected");

      client.publish("hello", "Hello");
      client.subscribe("data");
    } else {
      Serial.print("Failed to connect: ");
      Serial.println(client.state());
      delay(500);
    }
  }

  servo.attach(SERVO_PIN);
}

void loop() {
  client.loop();
}
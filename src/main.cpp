#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("hello world 0");
  Serial1.println("hello world 1");
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(2000);
}
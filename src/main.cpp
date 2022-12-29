#include <Arduino.h>
#include <WXT520.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Password.h>

/******************************************************************************/
/********************  Définition des variables *******************************/
/******************************************************************************/

String chaine;
int cptr =0;

//déclaration d'un objet capteur méteo
WXT520 My_WXT520;

WiFiClient espClient;
PubSubClient client(espClient);

// led embarquée ESP32
const int On_Board_LED    = LED_BUILTIN; // Arduino pin connected to LED's pin
int ledState = LOW;     // the current state of LED

char Var_MQTT[10]; //tableau pour la conversion des valeurs

/******************************************************************************/
/********************  Définition des fonctions *******************************/
/******************************************************************************/

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // publication sur le topic pour annoncer la connexion
      //client.publish("Maison/Leds", "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


/******************************************************************************/
/************************  Code principal  ************************************/
/******************************************************************************/


void setup() {
  Serial.begin(9600);
  Serial2.begin(19200);
  Serial.println("Début programme:");
  Serial.println(ssid);
  Serial.println(password);
  pinMode(On_Board_LED,OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // toggle state of LED
    ledState = !ledState;
    // control LED arccoding to the toggled state
    digitalWrite(On_Board_LED, ledState); 
    Serial.print(".");
  }

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  
  while (Serial2.available()>0){
    char caractere =  Serial2.read();
    //Serial.print("caractère boucle: ");
    chaine.concat(caractere);
    //Serial.println(chaine);
    delay(15);
  }
  
  if(chaine != ""){
    //Serial.print("Chaine reçue: ");
    //Serial.println(chaine);
    My_WXT520.Conversion(chaine);
    My_WXT520.Afficher_valeurs();
    chaine ="";
    // envoi datas en MQTT
    dtostrf(My_WXT520.Temperature(), 1, 2, Var_MQTT);
    client.publish("WXT520/Temperature",Var_MQTT);
    Serial.println("message_publié");
    ledState = !ledState;
    // control LED arccoding to the toggled state
    digitalWrite(On_Board_LED, ledState); 
  }
}  
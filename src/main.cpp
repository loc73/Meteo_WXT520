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

WiFiClient espClient; // cration du client Wifi
PubSubClient client(espClient);

// led embarquée ESP32
const int On_Board_LED    = LED_BUILTIN; // Arduino pin connected to LED's pin
int ledState = LOW;     // the current state of LED

char Var_MQTT[10]; //tableau pour la conversion des valeurs

long lastReconnectAttempt = 0; // variable permettant d'avoir un delay avant une reconnexion.

/******************************************************************************/
/********************  Définition des fonctions *******************************/
/******************************************************************************/

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "WXT520";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // publication sur le topic pour annoncer la connexion
      //client.publish("Maison/Leds", "hello world");
      client.subscribe("WXT520/Cmd"); // souscription au topic "WXT520/Cmd" permettant de renvoyer des cmd à la station.
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// fonction permettant de gérer les infos arrivant du topic auquel on a souscrit
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial2.print((char)payload[i]);
  }
  Serial2.println();
}


/******************************************************************************/
/************************  Code principal  ************************************/
/******************************************************************************/


void setup() {
  Serial.begin(9600);
  Serial2.begin(19200); // liaison série vers la station WXT520.
  Serial.println("Début programme:");
  pinMode(On_Board_LED,OUTPUT); // déclaration de la led embarquée sur la carte.

  WiFi.begin(ssid, password); // connexion au wifi avec SSID et password définit dans le fichier password.h

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // toggle state of LED
    ledState = !ledState;
    // control LED arccoding to the toggled state
    digitalWrite(On_Board_LED, ledState);   // clignotement de la led pour afficher une tentative de connexion. 
    Serial.print(".");
  }
  // utilisé pour le debug => affichage de l'addresse IP sur le réseau.
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883); // connexion au serveur MQTT: 'mqtt_server' définit dans le fichier password.h
  client.setCallback(callback);
  
}

void loop() {

  // si on est déconnecté on tente une reconnexion au serveur toutes les 5sec
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
     reconnect();
    }
  }
  else
  {
    client.loop();
  }
  
  // récupération des données sur la liaison série de la station.
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
    //My_WXT520.Afficher_valeurs();
    chaine ="";

    // envoi datas en MQTT
    dtostrf(My_WXT520.Direction_Vent_moy(), 1, 2, Var_MQTT);
    client.publish("WXT520/Vent_moy",Var_MQTT);
    ledState = !ledState;
    digitalWrite(On_Board_LED, ledState); 
    delay(500);

    dtostrf(My_WXT520.Temperature(), 1, 2, Var_MQTT);
    client.publish("WXT520/Temp",Var_MQTT);
    ledState = !ledState;
    digitalWrite(On_Board_LED, ledState); 
    delay(500);
    
    dtostrf(My_WXT520.Humidite(), 1, 2, Var_MQTT);
    client.publish("WXT520/Humidite",Var_MQTT);
    ledState = !ledState;
    digitalWrite(On_Board_LED, ledState); 
    delay(500);

    dtostrf(My_WXT520.Pression(), 1, 2, Var_MQTT);
    client.publish("WXT520/Pression",Var_MQTT);
    ledState = !ledState;
    digitalWrite(On_Board_LED, ledState); 
    delay(500);

    dtostrf(My_WXT520.Duree_precipitation(), 1, 2, Var_MQTT);
    client.publish("WXT520/Duree_preci",Var_MQTT);
    ledState = !ledState;
    digitalWrite(On_Board_LED, ledState); 
    delay(500);

    dtostrf(My_WXT520.Intensite_precipitation(), 1, 2, Var_MQTT);
    client.publish("WXT520/Intensite_preci",Var_MQTT);
    ledState = !ledState;
    digitalWrite(On_Board_LED, ledState); 
    delay(500);
    //Serial.println("message_publié");
    // control LED arccoding to the toggled state
    ledState = LOW;
    digitalWrite(On_Board_LED, ledState); 
  }
}  
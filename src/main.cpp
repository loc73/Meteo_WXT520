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

char Dmd_vent[] = "0R1";
char Dmd_temp[] = "0R2";
char Dmd_preci[] = "0R3";

unsigned long currentTime=0;

unsigned long previousTime_temp=0;
unsigned long previousTime_vent=0;
unsigned long Interval_temp_15min = 900000; //-> 15min, interval de demande et d'envoi de l'humidité et de la pression 
//unsigned long Interval_temp_5min = 300000; //-> 15min, interval de demande et d'envoi de l'humidité et de la pression 
int cptr_1h = 0;

bool Tps_15min_ok = false;
bool Tps_1h_ok = false;
bool infos_completes = true;
bool envoie_temp = false;
bool envoie_vent = false;
bool envoie_preci = false;


unsigned int Cptr_temp = 99;
unsigned int retard_envoi = 0;

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
  if (!strncmp((char *)payload, "infos", length)){
    infos_completes = true;
  }
  else if (!strncmp((char *)payload, "temp", length)){ // on force l'envoie de la température
    envoie_temp = true;
  }
  else if (!strncmp((char *)payload, "vent", length)){ // on force l'envoie du vent
    envoie_vent = true;
  }
  else if (!strncmp((char *)payload, "preci", length)){ // on force l'envoi des précipitation.
    envoie_preci = true;
  }
  // sinon on envoie la commande sur la liaison série cela peux être utile pour reconfigurer la station
  else{
    for (int i=0;i<length;i++) {
      Serial2.print((char)payload[i]);
    }
    Serial2.println();
  }
}

void recup_tri_donnees(){
  String chaine ="";
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
  }
}

// 1=temp, 2=vent, 3=preci
void dmd_manuelle(int choix){
    int length = 0;

    if (choix == 1){
      length = sizeof(Dmd_temp);  
      for (int i=0;i<length;i++) {
        Serial2.print((char)Dmd_temp[i]);
      }
      Serial2.println();
      delay(150);
      envoie_temp = true;      
    }

    else if (choix == 2){
      length = sizeof(Dmd_vent);  
      for (int i=0;i<length;i++) {
        Serial2.print((char)Dmd_vent[i]);
      }
      Serial2.println();
      delay(150);
      envoie_vent = true;
    }

    else if (choix == 3){
      length = sizeof(Dmd_preci);  
      for (int i=0;i<length;i++) {
        Serial2.print((char)Dmd_preci[i]);
      }
      Serial2.println();
      delay(150);
      envoie_preci = true;
    }
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
  
  currentTime=millis(); // mise à jour du temps actuel

    // test sur boucle de 15min
  if((currentTime-previousTime_temp) > Interval_temp_15min){
    previousTime_temp=currentTime;
    Tps_15min_ok = true;
    cptr_1h++;
    if (cptr_1h > 3){
      envoie_vent = true;
    }
  }


  // si on est déconnecté on tente une reconnexion au serveur toutes les 5sec
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
     reconnect();
    }
  }
  else{
    client.loop();
  }

  // si on fait une demande de toutes les infos alors on interroge les différents buffer.
  if(infos_completes){
    dmd_manuelle(1);
    delay(150);
    recup_tri_donnees();
    dmd_manuelle(2);
    delay(150);
    recup_tri_donnees();
    dmd_manuelle(3);
    delay(150);
    recup_tri_donnees();
    infos_completes = false;
  }
  // sinon on regarde si il y a qqch sur la liaison série
  // on doit au minimum avoir des données toutes les minutes.
  else{
    recup_tri_donnees();
    // si le vent est supérieur à 1 m/s ou 1km/h alors on envoie toutes les valeurs correspondants au vent
    if (My_WXT520.Vitesse_Vent_moy() > 1){
      dmd_manuelle(2);
      delay(150);
      envoie_vent = true;
    }
    // si il y a des précipitations alors on envoie plus souvent toutes les valeurs
    if (My_WXT520.Intensite_precipitation() > 0){
      dmd_manuelle(3);
      delay(150);
      recup_tri_donnees();
      envoie_preci = true;
    }

  }



  // si intervalle de temps > 15min ou demande d'envoie des données
  if (Tps_15min_ok || envoie_temp){
    // envoie de la température
    dtostrf(My_WXT520.Temperature(), 1, 2, Var_MQTT);
    client.publish("WXT520/Temp",Var_MQTT);
    ledState = !ledState;
    digitalWrite(On_Board_LED, ledState);
    delay(500);
    retard_envoi++;// on incrémente le compteur de délai
    // on envoie une fois sur 2 l'himidité et la pression
    if (retard_envoi >1){ 
      retard_envoi = 0;
      dmd_manuelle(1);
      delay(150);
      recup_tri_donnees();
      // envoie de la humidité
      dtostrf(My_WXT520.Humidite(), 1, 2, Var_MQTT);
      client.publish("WXT520/Humidite",Var_MQTT);
      ledState = !ledState;
      digitalWrite(On_Board_LED, ledState); 
      delay(500);
      // envoie de la pression
      dtostrf(My_WXT520.Pression(), 1, 2, Var_MQTT);
      client.publish("WXT520/Pression",Var_MQTT);
      ledState = !ledState;
      digitalWrite(On_Board_LED, ledState); 
      delay(500);
    }
    Tps_15min_ok = false;
    envoie_temp = false;
  }

  // publication des infos de vent si supérieur à 1km/h
  if (envoie_vent){
    dtostrf(My_WXT520.Direction_Vent_moy(), 1, 2, Var_MQTT);
    client.publish("WXT520/Vent_moy",Var_MQTT);
    ledState = !ledState;
    digitalWrite(On_Board_LED, ledState); 
    delay(500);
    envoie_vent = false;
    cptr_1h = 0; //raz durée une heure
  }

  // publication des infos de précipitation si > à 0mm/h
  if(envoie_preci){
    dtostrf(My_WXT520.Accumulation_precipitation(), 1, 2, Var_MQTT);
    client.publish("WXT520/Accu_preci",Var_MQTT);
    ledState = !ledState;
    digitalWrite(On_Board_LED, ledState); 
    delay(500);
    dtostrf(My_WXT520.Duree_precipitation(), 1, 2, Var_MQTT);
    client.publish("WXT520/Duree_preci",Var_MQTT);
    ledState = !ledState;
    digitalWrite(On_Board_LED, ledState); 
    delay(500);
    envoie_preci = false;
  } 
}  
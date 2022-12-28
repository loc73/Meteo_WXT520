#include <Arduino.h>
#include <WXT520.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Password.h>

String chaine;
int cptr =0;

//déclaration d'un objet capteur méteo
WXT520 My_WXT520;



void setup() {
  Serial.begin(9600);
  Serial2.begin(19200);
  Serial.println("Début programme:");
  Serial.println(ssid);
  Serial.println(password);
  
}

void loop() {
  
  
  while (Serial2.available()>0){
    char caractere =  Serial2.read();
    //Serial.print("caractère boucle: ");
    chaine.concat(caractere);
    //Serial.println(chaine);
    delay(15);
  }
  
  if(chaine != ""){
    Serial.print("Chaine reçue: ");
    Serial.println(chaine);
    My_WXT520.Conversion(chaine);
    My_WXT520.Afficher_valeurs();
    chaine ="";
  }
}  
#include <Arduino.h>

String chaine;
int cptr =0;
void setup() {
  Serial.begin(9600);
  Serial2.begin(19200);
  
}

void loop() {
  
  
  while (Serial2.available()>0){
    char caractere =  Serial2.read();
    //Serial.print("caractère boucle: ");
    chaine.concat(caractere);
    //Serial.println(chaine);
    delay(15);
  }
  
  if( chaine != ""){
    Serial.println(chaine);
    chaine ="";
    cptr++;
    delay(1000);
    Serial2.println("0R1");
    cptr =0;
  }
}  
class WXT520
{
    private:
    // définition de toutes les variables envoyées par la station. 
    int _addr;
    int _Dn;
    int _Dm;
    int _Dx;
    float _Sn;
    float _Sm;
    float _Sx;
    float _Ta;
    float _Ua;
    float _Pa;
    float _Rc;
    float _Rd;
    float _Ri;
    float _Hc;
    int _Hd;
    float _Hi;
    float _Rp;
    float _Hp;


    public:
    WXT520();
    int Get_addr();
    char *Reset_intensite_precipitation();
    void Conversion(String chaine);
    void Afficher_valeurs();

    int Direction_Vent_min(); 
    int Direction_Vent_moy();
    int Direction_Vent_max();
    float Vitesse_Vent_min();
    float Vitesse_Vent_moy();
    float Vitesse_Vent_max();
    float Temperature();
    float Humidite();
    float Pression();
    float Accumulation_precipitation();
    float Duree_precipitation();
    float Intensite_precipitation();
    float Accumulation_grele();
    int Duree_grele();
    float Intensite_grele();
    float Intensite_max_precipitation();
    float Intensite_max_grele();

    
};

WXT520::WXT520(){
_addr = 0;
_Dm = 0;
_Ta = 0;
}

void WXT520::Afficher_valeurs(){
    Serial.print("Dn: ");
    Serial.println(_Dn);
    Serial.print("Dm: ");
    Serial.println(_Dm);
    Serial.print("Dx: ");
    Serial.println(_Dx);
    Serial.print("Sn: ");
    Serial.println(_Sn);
    Serial.print("Sm: ");
    Serial.println(_Sm);
    Serial.print("Sx: ");
    Serial.println(_Sx);
    Serial.print("Ta: ");
    Serial.println(_Ta);
    Serial.print("Ua: ");
    Serial.println(_Ua);
    Serial.print("Pa: ");
    Serial.println(_Pa);
    Serial.print("Rc: ");
    Serial.println(_Rc);
    Serial.print("Rd: ");
    Serial.println(_Rd);
    Serial.print("Ri: ");
    Serial.println(_Ri);
    Serial.print("Hc: ");
    Serial.println(_Hc);
    Serial.print("Hd: ");
    Serial.println(_Hd);
    Serial.print("Hi: ");
    Serial.println(_Hi);
    Serial.print("Rp: ");
    Serial.println(_Rp);
    Serial.print("Hp: ");
    Serial.println(_Hp);
}

int WXT520::Get_addr(){
    return _addr;
}


char *WXT520::Reset_intensite_precipitation(){
    /* cette fonction permet de retourner la commande pour pour réinitialiser
       les valeurs Ri, Rp, Hi, Hp.
    */
    static char buf[6] = "0XZRU";
    return buf;
}

void WXT520::Conversion(String chaine){
    
    int idx_1 = chaine.indexOf(',');
    while (idx_1 != -1)
    {
        String Parse_datas = chaine.substring(0,idx_1); // on coupe le morceau de chaine entre les virgules
        int idx_2 = Parse_datas.indexOf('=');
        if (idx_2 != -1){
            String Label = Parse_datas.substring(0,idx_2);
            Serial.print("Label: ");
            Serial.println(Label);
            Parse_datas = Parse_datas.substring(idx_2+1, Parse_datas.length());
            Serial.print("Datas: ");
            Serial.println(Parse_datas);

            if(String(Label) == "Dm"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                Serial.print("Data dans if Dm: ");
                Serial.println(Parse_datas);
                _Dm = Parse_datas.toInt();
                Serial.print("Data enregistrees Dm: ");
                Serial.println(_Dm);
            }
            else if (String(Label) == "Dn"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Dn = Parse_datas.toInt();
            }
            else if (String(Label) == "Dx"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Dx = Parse_datas.toInt();
            }
            else if (String(Label) == "Sn"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Sn = Parse_datas.toFloat();
            }
            else if (String(Label) == "Sm"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Sm = Parse_datas.toFloat();
            }
            else if (String(Label) == "Sx"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Sx = Parse_datas.toFloat();
            }
            else if (String(Label) == "Ta"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Ta = Parse_datas.toFloat();
            }
            else if (String(Label) == "Ua"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Ua = Parse_datas.toFloat();
            }
            else if (String(Label) == "Pa"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Pa = Parse_datas.toFloat();
            }
            else if (String(Label) == "Rc"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Rc = Parse_datas.toFloat();
            }
            else if (String(Label) == "Rd"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Rd = Parse_datas.toFloat();
            }
            else if (String(Label) == "Ri"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Ri = Parse_datas.toFloat();
            }
            else if (String(Label) == "Hc"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Hc = Parse_datas.toFloat();
            }
            else if (String(Label) == "Hd"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Hd = Parse_datas.toInt();
            }
            else if (String(Label) == "Hi"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Hi = Parse_datas.toFloat();
            }
            else if (String(Label) == "Rp"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Rp = Parse_datas.toFloat();
            }
            else if (String(Label) == "Hp"){
                Parse_datas = Parse_datas.substring(0,Parse_datas.length()-1); // on supprime le dernier caractère
                _Hp = Parse_datas.toFloat();
            }
        }   
    
      chaine = chaine.substring(idx_1+1, chaine.length()); // on supprime le morceau qu'on vient de tester
      idx_1 = chaine.indexOf(',');
    }

}

int WXT520::Direction_Vent_min(){
    return _Dn;
}

int WXT520::Direction_Vent_moy(){
    return _Dm;
}

int WXT520::Direction_Vent_max(){
    return _Dx;
}

float WXT520::Vitesse_Vent_min(){
    return _Sn;
}

float WXT520::Vitesse_Vent_moy(){
    return _Sm;
}

float WXT520::Vitesse_Vent_max(){
    return _Sx;
}

float WXT520::Temperature(){
    return _Ta;
}

float WXT520::Humidite(){
    return _Ua;
}

float WXT520::Pression(){
    return _Pa;
}

float WXT520::Accumulation_precipitation(){
    return _Rc;
}

float WXT520::Duree_precipitation(){
    return _Rd;
}

float WXT520::Intensite_precipitation(){
    return _Ri;
}

float WXT520::Accumulation_grele(){
    return _Hc;
}

int WXT520::Duree_grele(){
    return _Hd;
}

float WXT520::Intensite_grele(){
    return _Hi;
}

float WXT520::Intensite_max_precipitation(){
    return _Rp;
}

float WXT520::Intensite_max_grele(){
return _Hp;
}

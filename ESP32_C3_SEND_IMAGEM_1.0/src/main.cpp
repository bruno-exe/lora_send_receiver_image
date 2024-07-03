#include <Arduino.h>
#include "SPIFFS.h"

//VARIAVEIS GLOBAIS 
const int packetSize = 230; // Tamanho do pacote

void sendStringInPackets(String str);
void initSPIFFS();

void setup() {
Serial.begin(115200);
Serial1.begin(9600, SERIAL_8N1, 6, 7); 
 initSPIFFS();
  
}

void loop() {

String imagem;// VARIAVEIS PARA DADOS

File rFile = SPIFFS.open("/zelda.jpg", "r");
  
  if(!rFile)
  {
    Serial.println(" falha ao abrir arquivo");
  }
  else 
  {
    while (rFile.available()) 
    {
      imagem += rFile.read();
    }
    Serial.println("log lidos com susesso: ");
  }
  
  rFile.close();
  
   Serial.println(imagem.length());
  
   sendStringInPackets(imagem);

  delay(10000);  
}


void sendStringInPackets(String str) {
  int strLength = str.length();
  int numPackets = strLength / packetSize;

  //Serial1.println(String(numPackets));// envia a quantidade de pacotes 
  
  Serial.println("--------------------------"+String(numPackets)+"---------------------------");
  Serial1.println(String(numPackets));// envia a quantidade de pacotes 
  delay(2400);


  int remainingChars = strLength % packetSize;

  for (int i = 0; i < numPackets; i++) {
    String packet = str.substring(i * packetSize, (i + 1) * packetSize);
    Serial1.print(packet);
    Serial.print(packet);
    delay(3500); // Pequeno atraso para evitar sobrecarga

    while(true){}
//  criar um pin digital imput no axiliar 

   // }

  }

  // Enviar os caracteres restantes
  if (remainingChars > 0) {
    String packet = str.substring(numPackets * packetSize);
    Serial1.print(packet);
    Serial.print(packet);
  }

  Serial.println("--------------------------finalizado---------------------------");
  
}


void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}
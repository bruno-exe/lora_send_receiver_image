#include "SPIFFS.h"
 String hexString = ""; // variavel para guardar dados em exa


//VARIAVEIS GLOBAIS 
const int packetSize = 200; // Tamanho do pacote

void sendStringInPackets(String str);
void initSPIFFS();
void encoders();
void decoders();

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 6, 7); 
  initSPIFFS();
}

void loop() {

  encoders();
  sendStringInPackets(hexString);
  delay(50000);

}


void encoders(){

File file = SPIFFS.open("/zelda.bmp", FILE_READ);
  if (!file) {
    Serial.println("falha não existe arquivo");
    return;
  }

  hexString = "";
  while (file.available()) {
    uint8_t byte = file.read();
    char hex[3];
    sprintf(hex, "%02X", byte);
    hexString += hex;
  }
  file.close();

  // Envia a string hexadecimal via serial
   Serial.println("imagem codificada");


}


void decoders(){
  
   
    File file = SPIFFS.open("/zelda.bmp", FILE_WRITE);
    if (!file) {
      Serial.println("falha, não existe arquivo");
      return;
    }

    for (int i = 0; i < hexString.length(); i += 2) {
      String hexByte = hexString.substring(i, i + 2);
      char byte = strtol(hexByte.c_str(), NULL, 16);
      file.write(byte);
    }
    file.close();
    Serial.println("Image decodificada");
  
}


void sendStringInPackets(String str) {
  int strLength = str.length();
  int numPackets = strLength / packetSize;

  //Serial1.println(String(numPackets));// envia a quantidade de pacotes 
  
  Serial.println("--------------------------"+String(numPackets)+"---------------------------");
  Serial1.println(String(numPackets-2));// envia a quantidade de pacotes 
  delay(1500);


  int remainingChars = strLength % packetSize;

  for (int i = 0; i < numPackets; i++) {
    String packet = str.substring(i * packetSize, (i + 1) * packetSize);
    Serial1.print(packet);
    Serial.print(packet);
    delay(1600); // Pequeno atraso para evitar sobrecarga
   //  criar um pin digital imput no axiliar 
  }
  delay(100);
  // Enviar os caracteres restantes
  if (remainingChars > 0) {
    String packet = str.substring(numPackets * packetSize);
    Serial1.print(packet);
    Serial.print(packet);
  }

  Serial.println("--------------------------finalizado---------------------------");
   //while(true){} // não executa
}

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <ArduinoJson.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define RXD2 20
#define TXD2 21

//notas da musica
#define NOTE_E2  82
#define NOTE_AS2 117
#define NOTE_C3  131
#define NOTE_D3  147
#define NOTE_E3  165
#define REST      0

int musica = 0;
int tempo = 225;
int buzzer = 10;


int melody[] = {  // doom
  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,
};

// DADOS PARA CONTROLE DE FLUXO
 String filename = "/img.bmp";
 int packtes = 0; // quantidade de paquetes que ira receber
 int recebido = 0;// quando recebe algum pacote 
 String hexString = ""; // variavel para guardar dados em exa

 int soma =0;
 int imgs = 0;
// Replace with your network credentials
const char* ssid = "ZOE";//"server";
const char* password = "43633151b";//"12345678";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 3000; // 3 segundos


int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String ajustarString(String input);
void initSPIFFS();
void tocar();
void display_init();
void writeFile(String path, String message, const char* modo);
void serialcomando();
String getSensorReadings();
void initWebSocket();
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void initWiFi();
void notifyClients(String sensorReadings);
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void confRouter();
void display_init();
void writeFile(String path, String message, const char* modo);
String ajustarString(String input);
void atualizar();
// funçoes de teste abaixo
void encoders();
void decoders(String hexString);




void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  initWiFi();
  initSPIFFS();
  initWebSocket();
  confRouter();
  display_init();

   Serial.println("iniciado...");

   Serial1.readString();
}

void loop() {

  // tesde de encoder e decoder 
  //--------------------------------------------------------

//delay(5000);
//encoders();
//delay(1000);
//decoders();

//if ((millis() - lastTime) > timerDelay) {
    //String sensorReadings = getSensorReadings(); // atualiza a imagem
   // notifyClients(sensorReadings);
   // lastTime = millis(); 

 // }
 // ws.cleanupClients();
  


  //--------------------------------------------------------

 String loraDados;
 if(Serial1.available()>0)
  { 
   
    delay(300);
    String tamanhos;
    while(Serial1.available())
    {
      char c = Serial1.read();

      if(c == '0')
      tamanhos += c;
      if(c == '1')
      tamanhos += c;
      if(c == '2')
      tamanhos += c;
      if(c == '3')
      tamanhos += c;
      if(c == '4')
      tamanhos += c;
      if(c == '5')
      tamanhos += c;
      if(c == '6')
      tamanhos += c;
      if(c == '7')
      tamanhos += c;
      if(c == '8')
      tamanhos += c;
      if(c == '9')
      tamanhos += c;
      if(c == ' ')
      tamanhos += c;
       if(c == '\r')
      tamanhos += c;
       if(c == '\n')
      tamanhos += c;

    }

   
    Serial.println("recebido tamanho: "+tamanhos );
    packtes = tamanhos.toInt();

    display.clearDisplay();
    display.setTextSize(1); 
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(ajustarString("recebido> " + String(recebido) + " de " + String(packtes))); // ajusta
    display.display();// mosta o texto ajustado 
    Serial.println("recebido tamanho e foi resetado");
    recebido++;

   
    tone(10,440,100); //LA
    
    
    long time_out = millis();
    loraDados = "";
  
    while(millis() - time_out < 3000)
    {
        if(Serial1.available()>0)
        {
          loraDados += Serial1.readString();
          time_out = millis();
          recebido++;
          atualizar();
          //Serial.println("reveiver");
          Serial.println(recebido);

        }
      
    }
     packtes = 0;
     recebido = 0;
    //writeFile(filename, loraDados , "w");// sobrescreve
    decoders(loraDados);
    Serial.println("escrito na memoria");
    tocar();

  }



  if ((millis() - lastTime) > timerDelay) {
    String sensorReadings = getSensorReadings();
    notifyClients(sensorReadings);
    lastTime = millis(); 

  }
  ws.cleanupClients();
}




// Get Sensor Readings and return JSON object
String getSensorReadings(){


String images;// VARIAVEIS PARA DADOS
File rFile = SPIFFS.open("/zelda2.bmp", "r");
  
  if(!rFile)
  {
    Serial.println(" falha ao abrir arquivo");
  }
  else 
  {
    while (rFile.available()) 
    {
      images += rFile.read();
    }
    Serial.println("log lidos com susesso: ");
  }
  
  rFile.close();
 
 

  DynamicJsonDocument doc(1024);
  doc["tamanho"] = images;
  doc["falta"] =  "decodificado";
  doc["img"] = "zelda2.bmp";
  
  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}


void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}


void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
 }

 void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    String message = (char*)data;
    if (message == "getReadings") {
      String sensorReadings = getSensorReadings();
      notifyClients(sensorReadings);
    }
  }
 }

void confRouter(){

 // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");
  
  Serial.println("atualizado....2");
  // Start server
  server.begin();

}

void display_init(){

if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    }

    display.clearDisplay();
    display.display();
    tone(10,440,100); 
    display.clearDisplay();
    display.setTextSize(1); 
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(ajustarString("aguardando primeiro contato")); // ajusta
    display.display();// mosta o texto ajustado 

}

void writeFile(String path, String message, const char* modo) {
  
  File file = SPIFFS.open(path, modo); // w sobrescreve

  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
   // Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

String ajustarString(String input) {
 
  if(input.substring(21) != " " && input.length() > 21 )
    {
      int x = 21;
      int v = 0;
      while(input.substring(x-1,x) != " ")
      {
        x--;
        v++;
      }
      //adicionar espaço
      String addspaco = "";
      while(v > 0)
      {
          addspaco += " ";
          v--;
      }
      input = input.substring(0, x) + addspaco + input.substring(x,input.length());
    }
  //-------------------------------------------------------------------

  if(input.substring(42) != " " && input.length() > 42 )
    {
      int x = 42;
      int v = 0;
      while(input.substring(x-1,x) != " ")
      {
        x--;
        v++;
      }
      String addspaco = "";
      while(v > 0)
      {
          addspaco += " ";
          v--;
      }
      input = input.substring(0, x) + addspaco + input.substring(x,input.length());
    }

  //---------------------------------------------------------------------
  if(input.substring(63) != " " && input.length() >63 )
    {
      int x = 63;
      int v = 0;
      while(input.substring(x-1,x) != " ")
      {
        x--;
        v++;
      }
      //adicionar espaço
      String addspaco = "";
      while(v > 0)
      {
          addspaco += " ";
          v--;
      }
      input = input.substring(0, x) + addspaco + input.substring(x,input.length());
    }

  if (input.length() > 80)
  Serial1.println("caracters exedido");
  return input;
 

}

void tocar(){ 
  // Calcula o tempo da nota 
  notes = sizeof(melody) / sizeof(melody[0]) / 2;
  wholenote = (60000 * 4) / tempo;
  divider = 0, noteDuration = 0;
  //executa o toque

for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    divider = pgm_read_word_near(melody+thisNote + 1);
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }
    tone(buzzer, pgm_read_word_near(melody+thisNote), noteDuration * 0.9);
    delay(noteDuration);
    noTone(buzzer);
  }

}


void atualizar(){

    display.clearDisplay();
    display.setTextSize(1); 
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(ajustarString("recebido> " + String(recebido) + " de " + String(packtes))); // ajusta
    display.display();// mosta o texto ajustado 
   // Serial.println("recebido tamanho e foi resetado");



}



void encoders(){

File file = SPIFFS.open("/zelda.bmp", FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
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
  //Serial.println(hexString);

    display.clearDisplay();
    display.setTextSize(1); 
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(ajustarString("foi codificado...")); // ajusta
    display.display();// mosta o texto ajustado 

}


void decoders(String hexString){
  
   
    File file = SPIFFS.open("/zelda2.bmp", FILE_WRITE);
    if (!file) {
      Serial.println("Failed to open file for writing");
      return;
    }

    for (int i = 0; i < hexString.length(); i += 2) {
      String hexByte = hexString.substring(i, i + 2);
      char byte = strtol(hexByte.c_str(), NULL, 16);
      file.write(byte);
    }
    file.close();
    Serial.println("Image received and saved.");
  
}


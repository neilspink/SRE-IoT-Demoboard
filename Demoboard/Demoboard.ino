#define LED1  23
#define LED2  22
#define LED3  26
#define LED4  4
#define LED5  15

#define BTN1  T9
#define BTN2  T8
#define BTN3  T7
#define BTN4  T6
#define BTN5  T5

#include "WiFi.h"
#include "ESPAsyncWebServer.h"

const char* ssid = "Moody";
const char* password = "firstclass";

AsyncWebServer server(80);

int score;
int mood; 

String readScore() {
  return String(score);
}

String readMood() {
  if (score < 37)
    mood = 5; // Angry
  else if (score < 62)
    mood = 4; // Sad
  else if (score < 87)
    mood = 3; // OK
  else if (score < 112)    
    mood = 2; // Happy
  else
    mood = 1; // Super Happy
      
  return String(mood);
}

bool button(int id)
{
  for(int i=0; i<5; i++)
  {
    int value = touchRead(id);
    if(value <= 40)
    {
      Serial.println(value);
      delay(1);
    }
    else
    {
      if(i > 0)
      {
        Serial.print("False touch on input ");
        Serial.print(id);
        Serial.print(", iteration ");
        Serial.println(i);
      }
      return false;
    }
  }
  
  Serial.print("Detected touch on input ");
  Serial.println(id);

  Serial.print("Score ");
  Serial.println(score);
  return true;
}

void setup()
{
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor
  Serial.println("ESP32 Touch Test");

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);

  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  digitalWrite(LED5, HIGH);

  score = 50;
  delay(500);

  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/score", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readScore().c_str());
  });

  server.on("/mood", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readMood().c_str());
  });

  server.begin();
}

void updateSecore(int emotiScore)
{
  if (score < emotiScore)
    score = score + 5;
  else if (emotiScore > mood)
    score = score - 5;  
}

void loop()
{
  if (button(BTN1)) {
    updateSecore(125);
    digitalWrite(LED1, HIGH); 
  } else {
    digitalWrite(LED1, LOW); 
  }
  
  if (button(BTN2)) {
    updateSecore(100);
    digitalWrite(LED2, HIGH); 
  } else {
    digitalWrite(LED2, LOW); 
  }
  
  if (button(BTN3)) {
    updateSecore(75);
    digitalWrite(LED3, HIGH); 
  } else {
    digitalWrite(LED3, LOW); 
  }
  
  if (button(BTN4)) {
    updateSecore(50);
    digitalWrite(LED4, HIGH); 
  } else {
    digitalWrite(LED4, LOW); 
  }
  
  if (button(BTN5)) {
    updateSecore(25);
    digitalWrite(LED5, HIGH); 
  } else {
    digitalWrite(LED5, LOW); 
  }
  
  delay(100);
}

#define LED1  23
#define LED2  22
#define LED3  26
#define LED4  4
#define LED5  15

#define BTN1  T8
#define BTN2  T9
#define BTN3  T7
#define BTN4  T6
#define BTN5  T5


#include <PubSubClient.h>   // by Nick O'Leary
#include <WiFi.h>           // by Arduino

// WiFi
WiFiClient espClient;
const char* ssid = "";
const char* password =  "";


// MQTT
PubSubClient client(espClient);
const char* mqttPublishTopic = "test/demoboardbtn";
const char* mqttSubscribeTopic = "test/demoboardled";
const char* mqttServer = "broker.tec2020.fun";
const unsigned int mqttPort = 1883;
const char* mqttClientId = "test";
const char* mqttUser = "sammy";
const char* mqttPassword = "1234";
const unsigned int ReconnectEveryMillis = 5000;
unsigned long timeLastMsg = 0;


bool button(int id)
{
  for(int i=0; i<5; i++)
  {
    int value = touchRead(id);
    if(value <= 40)
    {
//      Serial.println(value);
      delay(1);
    }
    else
    {
      if(i > 0)
      {
//        Serial.print("False touch on input ");
//        Serial.print(id);
//        Serial.print(", iteration ");
//        Serial.println(i);
      }
      return false;
    }
  }
  
  Serial.print("Detected touch on input ");
  Serial.println(id);
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

  delay(500);

  SetupWifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(Callback);

  Serial.println("setup complete");
}

void loop()
{
  if (button(BTN1))
    digitalWrite(LED1, HIGH); 
  else
    digitalWrite(LED1, LOW); 

  if (button(BTN2))
    digitalWrite(LED2, HIGH); 
  else
    digitalWrite(LED2, LOW); 

  if (button(BTN3))
    digitalWrite(LED3, HIGH); 
  else
    digitalWrite(LED3, LOW); 

  if (button(BTN4))
    digitalWrite(LED4, HIGH); 
  else
    digitalWrite(LED4, LOW); 

  if (button(BTN5))
    digitalWrite(LED5, HIGH); 
  else
    digitalWrite(LED5, LOW); 
  
  
  delay(100);

  unsigned long now = millis();
  if (now - timeLastMsg >= ReconnectEveryMillis) {
    timeLastMsg = now;

    if (!client.connected()) {
      Reconnect();
    }
    client.loop();

    PublishCurrentMood();
  }

  
}

void PublishCurrentMood() {
  unsigned int currentPos = 0;
  
  if (digitalRead(LED1) == 1)
    currentPos = 1;

  if (digitalRead(LED2) == 1)
    currentPos = 2;

  if (digitalRead(LED3) == 1)
    currentPos = 3;
    
  if (digitalRead(LED4) == 1)
    currentPos = 4;

  if (digitalRead(LED5) == 1)
    currentPos = 5;

  String currentMood = "Current Mood "+(String)currentPos;
  char charBuf[15];
  currentMood.toCharArray(charBuf, 15);

  Serial.println(currentMood);
  client.publish(mqttPublishTopic, charBuf);
}


void SetupWifi() {
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void SetPos(const unsigned int target) {

  if (target == 1)
    digitalWrite(LED1, HIGH); 
  else
    digitalWrite(LED1, LOW); 

  if (target == 2)
    digitalWrite(LED2, HIGH); 
  else
    digitalWrite(LED2, LOW); 

  if (target == 3)
    digitalWrite(LED3, HIGH); 
  else
    digitalWrite(LED3, LOW); 

  if (target == 4)
    digitalWrite(LED4, HIGH); 
  else
    digitalWrite(LED4, LOW); 

  if (target == 5)
    digitalWrite(LED5, HIGH); 
  else
    digitalWrite(LED5, LOW); 
    
  
}


void Callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message of " + (String)length + " chars arrived on topic: ");
  Serial.print(topic);

  if (length == 1) {
    int mood = atoi(reinterpret_cast<char *>(message));  // if non integer number a zero is returned

    if (mood == 0) {
      Serial.print(" (message = " + (String)static_cast<char>(message[0]) + ")");
    }

    Serial.print(". Set mood to " + (String)mood);
    Serial.println();
    SetPos(mood);
  } else {
    Serial.print(". Invalid message: ");
    String messageTemp;

    for (int i = 0; i < length; i++) {
      messageTemp += static_cast<char>(message[i]);
    }
    Serial.print(messageTemp);
    Serial.println();
  }
}


void Reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(mqttClientId, mqttUser, mqttPassword)) {
      Serial.println("connected");
      Serial.println("publishing mood to topic " + (String)mqttPublishTopic);
      Serial.println("subscribing to topic " + (String)mqttSubscribeTopic);
      client.subscribe(mqttSubscribeTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);  // Wait before retrying
    }
  }
}

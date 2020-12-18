#include <Arduino.h>
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

// Fingerprint for demo URL, expires on July 31, 2020, needs to be updated well before this date
const uint8_t fingerprint[20] = {0xF6, 0xE2, 0xDB, 0x5C, 0xB7, 0xE2, 0xA8, 0x70, 0x62, 0x95, 0xCB, 0xF5, 0x47, 0x24, 0xE3, 0xA6, 0xC5, 0x9F, 0x6B, 0x2F};

// network SSID (name) and network key     
char ssid[] = "WIFI_SSID";
char password[] = "WIFI_PASSWORD";  

// OneSignal Authentication
char BasicAuth[] = "Basic XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

// App Id
char hashAppId[]="xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";

ESP8266WiFiMulti WiFiMulti;

// PIR Sensor
int pinPir = 4; // D2
int moved;

// Sound Sensor 
int pinSound = 2; // D4
int sound;

void setup() {

  Serial.begin(74880);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  // Define PIR and Sound sensor as INPUT
  pinMode(pinPir, INPUT);
  pinMode(pinSound, INPUT); 
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    // read PIR sensor
    // moved = digitalRead(pinPir);
    // Move detected!
    if(moved == HIGH) {
      //postMessage("Movimento dectectado!");
      Serial.println("Movimento dectectado!");
      Serial.println("Wait 120s before next round...");
      delay(120000);
    }

    // read sound sensor
    sound = digitalRead(pinSound);
    // Move detected!
    if(sound) {
      //postMessage("Campainha tocou!");
      Serial.println("Campainha tocou!");
      Serial.println("Wait 20s before next round...");
      delay(20000);
    } 
    
    //postMessage("Campainha tocou!");
  }
}

void postMessage(String message)
{  
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setFingerprint(fingerprint);

  const int capacity = JSON_OBJECT_SIZE(11);
  StaticJsonDocument<capacity> doc;

  //JsonObject root = doc.as<JsonObject>(); 
  doc["app_id"] = hashAppId;
  JsonArray jas = doc.createNestedArray("included_segments");
  jas.add("All"); //Add value to array

  JsonObject joh = doc.createNestedObject("headings");
  joh["en"]="DING DONG!";

  JsonObject joc = doc.createNestedObject("contents");
  joc["en"]=message;
  
  doc["android_accent_color"] = "790e8b";
  doc["android_sound"] = "dingdong";

  char jsonBuffer[300];
  serializeJson(doc, jsonBuffer);
  Serial.println(jsonBuffer);

  HTTPClient https;

  // HTTPS begin
  if (https.begin(*client, "https://onesignal.com/api/v1/notifications")) {

    //Specify content-type header  
    https.addHeader("Authorization", BasicAuth);
    https.addHeader("Content-Type", "application/json");

    //Send the request
    int httpCode = https.POST(jsonBuffer);

    // httpCode will be negative on error
    if (httpCode > 0) {

      //Get the response payload
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = https.getString();
        Serial.println(payload);

        Serial.println("Wait 10s before next round...");
        delay(10000);
      }
    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
    
  } else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }
  
}

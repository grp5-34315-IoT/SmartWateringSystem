#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "SSID";
const char* password = "pw";

const char* serverName = "http://api.thingspeak.com/update";

String apiKey = "";  // Replace it with your ThingSpeak channel API key


void setup() {
Serial.begin(115200);
WiFi.begin(ssid, password);

while(WiFi.status() != WL_CONNECTED) {
  delay(750);
  Serial.print(".");
}
Serial.println("Arduino is now connected to the WiFi network);
}

void loop() {
  if (Serial.available() > 0) {
    String request = Serial.readStringUntil('\n'); //Read incoming HTTP requests
    request.trim();  // Remove any whitespace. Maybe remove this code if redundant

    if (request == "POST") {
      doPostRequest(24.5);  // Example temperature value
    } else if (request == "GET") {
      doGetRequest();
    }
  }
  delay(1000);  // Reduce CPU usage
}

void doPostRequest(int pumpNumber) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String httpRequestData = "api_key=" + apiKey + "&field1=" + String(pumpNumber);
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(httpRequestData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
      //TODO: Call the pump function and activate Pump 'X'
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  }
}

//Todo: call getHumidity(), getTemperature() etc. depending on the GET request path
void doGetRequest() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://api.thingspeak.com/channels/YOUR_CHANNEL_ID/feeds.json?api_key=YOUR_READ_API_KEY&results=2");
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }
}

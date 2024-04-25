#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);

void handleRoot();
void handleNotFound();
void openweather();

void setup() {
    Serial.begin(9600);
    delay(10);

    // Connect to WiFi network
    //wifiMulti.addAP("Nishtman", "Nishtman20");
    //wifiMulti.addAP("Jakobs_iphone", "LarsDrip");
    //wifiMulti.addAP("SRK", "12345678");

    Serial.println();
    Serial.print("Connecting ...");

    while (wifiMulti.run() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected to ");
    Serial.println(WiFi.SSID());
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("iot")) {
        Serial.println("mDNS responder started");
    } else {
        Serial.println("Error setting up MDNS responder!");
    }

    server.on("/", HTTP_GET, handleRoot);
    server.on("/weather", HTTP_GET, openweather);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("Server started");
}

void loop() {
    server.handleClient();
}

void handleRoot() {
    String page = "<html><head><title>Internet of Things - Demonstration</title><meta charset=\"utf-8\"></head><body>";
    page += "<h1>Welcome!</h1>";
    page += "<p>Click the following button to get current weather information:</p>";
    page += "<form action=\"/weather\" method=\"GET\"><input type=\"submit\" value=\"Get Weather\"></form>";
    page += "</body></html>";
    server.send(200, "text/html", page);
}

void openweather() {
    const char* endpoint = "http://api.openweathermap.org/data/2.5/weather?q=Kongens%20Lyngby,dk&APPID=2bd047f4071440aff1b938b1fb2942e4&units=metric";
    HTTPClient http;
    WiFiClient client;

    http.begin(client, endpoint);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        DynamicJsonDocument doc(2000);
        deserializeJson(doc, payload);

        JsonObject main = doc["main"];
        float main_temp = main["temp"];
        float main_hum = main["humidity"];

        char html[500];
        sprintf(html, "The temperature is %.1f Celsius degrees and the humidity is %.0f .", main_temp, main_hum);
        server.send(200, "text/html", html);
    } else {
        server.send(500, "text/plain", "Failed to retrieve weather data");
    }

    http.end();
}

void handleNotFound() {
    server.send(404, "text/plain", "404 Not Found");
}

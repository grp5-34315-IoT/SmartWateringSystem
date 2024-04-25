// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

// DHT sensor 
#define DHTTYPE DHT11   // DHT 11


// network details
//const char* ssid = "Nishtman";
//const char* password = "Nishtman20";
//const char* ssid = "SRK";
//const char* password = "12345678";
const char* ssid = "recherche...";
const char* password = "mpsi2areuhere";

WiFiClient client;

const int DHTPin = D3;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];


unsigned long channelID = 2519764; // channel
const char * myWriteAPIKey = "LLIBJC6Q2HMOE826"; // WRITE API key
const char* server = "api.thingspeak.com";

const int postingInterval = 20 * 1000; // post data every 20 seconds

// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);

  dht.begin();
  
  // Connecting to WiFi network
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
  
}

// runs over and over again
void loop() {
  ThingSpeak.begin(client);
  if (client.connect(server, 80)) {
    
    // Measure Signal Strength (RSSI) of Wi-Fi connection
    long rssi = WiFi.RSSI();

    Serial.print("RSSI: ");
    Serial.println(rssi); 


    ThingSpeak.setField(4,rssi);

// **** This part reads  sensors and calculates
    float h = dht.readHumidity();
            // Read humidity
    float t = dht.readTemperature();
            // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);
            // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      strcpy(celsiusTemp,"Failed");
      strcpy(fahrenheitTemp, "Failed");
      strcpy(humidityTemp, "Failed");         
      }
    else{
              // Computes temperature values in Celsius + Fahrenheit and Humidity
      float hic = dht.computeHeatIndex(t, h, false);       
      dtostrf(hic, 6, 2, celsiusTemp);             
      float hif = dht.computeHeatIndex(f, h);
      dtostrf(hif, 6, 2, fahrenheitTemp);         
      dtostrf(h, 6, 2, humidityTemp);

              Serial.print("Humidity: ");
              Serial.print(h);
              Serial.print(" %\t Temperature: ");
              Serial.print(t);
              Serial.print(" *C ");
              Serial.print(f);
              Serial.print(" *F\t Heat index: ");
              Serial.print(hic);
              Serial.print(" *C ");
              Serial.print(hif);
              Serial.print(" *F");
              Serial.print("Humidity: ");
              Serial.print(h);
              Serial.print(" %\t Temperature: ");
              Serial.print(t);
              Serial.print(" *C ");
              Serial.print(f);
              Serial.print(" *F\t Heat index: ");
              Serial.print(hic);
              Serial.print(" *C ");
              Serial.print(hif);
              Serial.println(" *F");
        //end of sensor readings
      ThingSpeak.setField(1,t);
      ThingSpeak.setField(2,f);
      ThingSpeak.setField(3,h);
      }
            
      ThingSpeak.writeFields(channelID, myWriteAPIKey);
  }
    client.stop();

  // wait and then post again
  delay(postingInterval);
}

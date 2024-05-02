
#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>
#include <ThingSpeak.h>
#include <ArduinoJson.h>
MKRIoTCarrier carrier;
// Temporary variables
//const int  = 0; 
//const int D1 = 1;
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];

unsigned long channelID = 2519764; // channel
const char * myWriteAPIKey = "LLIBJC6Q2HMOE826"; // WRITE API key
const char* thingSpeakServer = "api.thingspeak.com";

const int postingInterval = 20 * 1000; // post data every 20 seconds



// Arduino SAMD Boards
// WiFiNINA
// Arduino IOT MKR
WiFiClient thingSpeakClient;
unsigned long previoustime = 0;   // Stores the time when the sensor data was last read
const long interval = 20000//5 * 60 * 1000;  // Interval between senso 2min


//char ssid[] = "iPhone 13 Pro";        // your network SSID (name)
//char pass[] = "malloy96";
//char ssid[] = "Nishtman";        // your network SSID (name)
//char pass[] = "Nishtman20";
char ssid[] = "SRK";        // your network SSID (name)
char pass[] = "12345678";
int status = WL_IDLE_STATUS;       // the Wifi radio's status

WiFiServer server(23); // TCP server on port 23 (Typically used for Telnet, change as needed)

void setup() {
    Serial.begin(9600);
    carrier.noCase();
    carrier.begin();
    pinMode(A5, INPUT);
    pinMode(A6, INPUT);
    while (!Serial);

    // Attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to network: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(10000);
    }

    server.begin(); // Start the server
    Serial.print("Server started, IP address: ");
    IPAddress ip = WiFi.localIP();
    Serial.println(ip);

    // Format the IPAddress type to a string so we can display the ip address on the screen
    carrier.display.fillScreen(0xFFFF); // Clear the screen
    carrier.display.setTextSize(3); // Set text size
    carrier.display.setTextColor(0x001F); // Set text color
    carrier.display.setCursor(0, 0); // Set cursor position
    String ipStr = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]) + ":23";
    displayData(ipStr);


            // Read humidity
    float humidity = carrier.Env.readHumidity();
    String humidityStr = String(humidity);
    displayData("Humidity: " + humidityStr + "%");

        // Read temperature
    float temp = carrier.Env.readTemperature();
    String temperatureStr = String(temp);
    displayData("Temp: " + temperatureStr + "C");
    //updateDisplay("IP Address:\n", ipStr);

    //read the moisture
    int moisture1 = analogRead(A5);
    int moisture2 = analogRead(A6);
    int map_moisture1 = map(moisture1, 0, 1023, 100, 0);
    int map_moisture2 = map(moisture2, 0, 1023, 100, 0);
    String moist1Str = String(map_moisture1);
    String moist2Str = String(map_moisture2);
    displayData("Moist 1: " + moist1Str + "%");
    displayData("Moist 2: " + moist2Str + "%");
}

void loop() {
    pump1OFF();
    pump2OFF();
    WiFiClient client = server.available();
    readsensor();
    thingspeak();
    //If there is an incoming client request, process it
    if (client) {
        processClient(client);
        client.stop();
    }
  
}

void readsensor(){
    unsigned long currenttime = millis();
    char command = Serial.read();

    // Check if time to read sensor data and update display
    if (currenttime - previoustime >= interval) {
        // Update the time for the next sensor reading
        previoustime = currenttime;
        carrier.display.fillScreen(0xFFFF); // Clear the screen
        carrier.display.setCursor(0, 0);
        // Read humidity
        float humidity = carrier.Env.readHumidity();
        String humidityStr = String(humidity);
        displayData("Hum: " + humidityStr + "%");

        // Read temperature
        float temp = carrier.Env.readTemperature();
        String temperatureStr = String(temp);
        displayData("Temp: " + temperatureStr + "C");

        //read the moisture
        int moisture1 = analogRead(A5);
        int moisture2 = analogRead(A6);
        int map_moisture1 = map(moisture1, 0, 1023, 100, 0);
        int map_moisture2 = map(moisture2, 0, 1023, 100, 0);
        String moist1Str = String(map_moisture1);
        String moist2Str = String(map_moisture2);
        displayData("Moist 1: " + moist1Str + "%");
        displayData("Moist 2: " + moist2Str + "%");
        if (map_moisture1<34){
          pump1ON();
        }
        if (map_moisture2<34){
          pump2ON();
        }
    }
}

void displayData(String data) {
    carrier.display.println(data);
}

//this function handles if incoming client requests are POST or GET
void processClient(WiFiClient &client) {
    String currentLine = "";
    boolean isRequestLine = true;
    String method, url;

    while (client.connected()) {
        if (client.available()) {
            char c = client.read();
            Serial.write(c);

            if (c == '\n') {
                if (currentLine.length() == 0) {
                    if (method == "GET") {
                        handleGetRequest(client, url);
                    } else if (method == "POST") {
                        handlePostRequest(client, url);
                    }
                    break; // End of the request
                }

                if (isRequestLine) {
                    int firstSpace = currentLine.indexOf(' ');
                    int secondSpace = currentLine.indexOf(' ', firstSpace + 1);
                    method = currentLine.substring(0, firstSpace);
                    url = currentLine.substring(firstSpace + 1, secondSpace);
                    isRequestLine = false;
                }

                currentLine = "";
            } else if (c != '\r') {
                currentLine += c;
            }
        }
    }
}

//if we receive a GET request from a client app, then we call the relevant request function and return the relevant data
void handleGetRequest(WiFiClient &client, const String &url) {
  if (url.startsWith("/humidity")) {
    //float humidity = 65.0; //Todo: Read real humidity
    float humidity = carrier.Env.readHumidity();
    sendHttpResponse(client, 200, "{\"humidity\": " + String(humidity) + "}");
    String humidityStr = String(humidity);
    displayData("Humidity: " + humidityStr + "%");
  }
  if (url.startsWith("/moisture")) {
    float moisture = 20.1;  //Todo: Read real soil moisture
    sendHttpResponse(client, 200, "{\"moisture\": " + String(moisture) + "}");
  }
  if (url.startsWith("/temperature")) {
    //float temp = 20.1; //Todo: Read real soil moisture
    float temp = carrier.Env.readTemperature();
    sendHttpResponse(client, 200, "{\"temp\": " + String(temp) + "}");
    String temperatureStr = String(temp);
    displayData("Temp: " + temperatureStr + "°C");
  } else {
    sendHttpResponse(client, 404, "{\"error\": \"Not found\"}");
  }
}

void handlePostRequest(WiFiClient &client, const String &url) {
  // Handle POST requests here
  if (url.startsWith("/pump1")) {
    pump1ON();
    sendHttpResponse(client, 200, "{}");
  }
  if (url.startsWith("/pump2")) {
    pump2ON();
    sendHttpResponse(client, 200, "{}");
  }
}

void pump1ON() {
  carrier.Relay1.close();
  delay(2000);
  pump1OFF();
}
void pump1OFF() {
  carrier.Relay1.open();
}

void pump2ON() {
  carrier.Relay2.close();
  delay(2000);
  pump2OFF();
}
void pump2OFF() {
  carrier.Relay2.open();
}

void sendHttpResponse(WiFiClient &client, int statusCode, const String &content) {
    client.println("HTTP/1.1 " + String(statusCode) + " " + (statusCode == 200 ? "OK" : "Not Found"));
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.println(content);
}

void thingspeak(){
  ThingSpeak.begin(thingSpeakClient);
  if (thingSpeakClient.connect(thingSpeakServer, 80)) {

// **** This part reads  sensors and calculates
    float h = carrier.Env.readHumidity();
            // Read humidity
    float t = carrier.Env.readTemperature();

    int moisture1 = analogRead(A5);

    int moisture2 = analogRead(A6);
    float m1 = map(moisture1, 0, 1023, 100, 0);
    float m2 = map(moisture2, 0, 1023, 100, 0);

    //float map_moisture1 = 100 * max(0.0f, min(1.0f, static_cast<float>(moisture1 - 880) / (1023 - 880)));
    //float map_moisture2 = 100 * max(0.0f, min(1.0f, static_cast<float>(moisture2 - 880) / (1023 - 880)));
            // Read temperature
    //float f = dht.readTemperature(true);
            // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      strcpy(celsiusTemp,"Failed");
      strcpy(humidityTemp, "Failed");
      }
    else{
        //end of sensor readings
      ThingSpeak.setField(1,t);
      //ThingSpeak.setField(2,f);
      ThingSpeak.setField(2,h);

      ThingSpeak.setField(3,m1);
      ThingSpeak.setField(4,m2);
      }

      ThingSpeak.writeFields(channelID, myWriteAPIKey);
  }
    thingSpeakClient.stop();
 // wait and then post again
  //delay(postingInterval);
}



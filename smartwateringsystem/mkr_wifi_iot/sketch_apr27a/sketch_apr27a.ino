
#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>
MKRIoTCarrier carrier;

// Arduino SAMD Boards
// WiFiNINA
// Arduino IOT MKR
MKRIoTCarrier carrier;
unsigned long previoustime = 0;   // Stores the time when the sensor data was last read
const long interval = 1 * 60 * 1000;  // Interval between senso 2min

//char ssid[] = "iPhone 13 Pro";        // your network SSID (name)
// char pass[] = "malloy96";          // your network password (use for WPA, or use as key for WEP)
char ssid[] = "Nishtman";        // your network SSID (name)
char pass[] = "Nishtman20";
int status = WL_IDLE_STATUS;       // the Wifi radio's status

WiFiServer server(23); // TCP server on port 23 (Typically used for Telnet, change as needed)

void setup() {
    Serial.begin(9600);
    carrier.noCase();
    carrier.begin();
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
}

void loop() {
    WiFiClient client = server.available();
    unsigned long currenttime = millis();

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
    }
    //If there is an incoming client request, process it
    if (client) {
        processClient(client);
        client.stop();
    }
    //Otherwise do the automated watering system functions (implement the rest of your code)

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
        float moisture = 20.1; //Todo: Read real soil moisture
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
    if (url.startsWith("/pump3")) {
        pump3ON();
        sendHttpResponse(client, 200, "{}");
    }
}

void pump1ON(){
carrier.Relay1.on();
}
void pump1OFF(){
carrier.Relay1.off();
}

void pump2ON(){
carrier.Relay2.on();
}
void pump2OFF(){
carrier.Relay2.off();
}


/*
void handlePostRequestOff(WiFiClient &client, const String &url) {
    // Handle POST requests here
    if (url.startsWith("/pump1")) {
        pump1OFF();
        sendHttpResponse(client, 200, "{}");
    }
    if (url.startsWith("/pump2")) {
        pump2OFF();
        sendHttpResponse(client, 200, "{}");
    }
    if (url.startsWith("/pump3")) {
        pump3OFF();
        sendHttpResponse(client, 200, "{}");
    }
}
*/

void sendHttpResponse(WiFiClient &client, int statusCode, const String &content) {
    client.println("HTTP/1.1 " + String(statusCode) + " " + (statusCode == 200 ? "OK" : "Not Found"));
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.println(content);
}

void displayData(String data) {
    carrier.display.println(data);
}
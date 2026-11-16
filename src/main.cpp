#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "secrets.hpp"

const char *ssid = SECRET_SSID;
const char *password = SECRET_PASS;

// DHT Sensor Setup
#define DHTPIN 4 // GPIO number where the  sensor is connected
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

AsyncWebServer server(80);

float temperature = NAN;
float humidity = NAN;
float heatIndex = NAN;

unsigned long lastRead = 0;

void readSensor(){
  float newHumidity = dht.readHumidity();
  float newTemperature = dht.readTemperature();

  if (isnan(newHumidity) || isnan(newTemperature))
  {
    Serial.println("Reading from sensor failed!!!");
    return;
  }

  humidity = newHumidity;
  temperature = newTemperature;

  heatIndex = dht.computeHeatIndex(temperature, humidity, false);

  Serial.printf("Temperature: %.2f °C, Humidity: %.2f %%, Heat Index: %.2f °C\n",
                temperature, humidity, heatIndex); // Welcome to C ig bruh
}

// the webpage
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang ="en">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Weather Station</title>

  <style>
    body {
        background-color: #ffffff ;
        font-family: Arial, Helvetica, sans-serif;
        margin: 0;
        padding-bottom: 60px;
    }

    h1 {
        text-align: center;
    }

    .subtitle {
        text-align: center;
    }

    .footer {
        position: fixed;
        bottom: 0;
        width: 100%;
        text-align: center;
    }

    table {
        width: 80%;
        border-collapse:separate;
        border: 1px solid black;
        border-spacing: 0;
        border-radius: 15px;
        overflow: hidden;
        margin: 0 auto;
    }

    th {
        background-color: rgb(5, 203, 5);
    }
    th , td {
        padding: 10px;
        text-align: center;
        border-right: 1px solid black;
        border-bottom: 1px solid black;
    }

    th:last-child, td:last-child {
        border-right: none;
    }

    tr:last-child td {
        border-bottom: none;
    }

  </style>

</head>

<body>

  <div class="container">
    
    <h1>ESP32 Weather Station</h1>
    <p class = "subtitle">Live Monitoring With ESP32 Micro controller</p>

    <table>
        <tr>
            <th>🌡️ Temperature</th>
            <th>💦 Humidity</th>
            <th>🔥 Heat Index</th>
        </tr>
        <tr>
              <td id="temperature">-- °C</td>
              <td id="humidity">-- %</td>
              <td id="heatIndex">-- °C</td>
          </tr>
      </table>
      
      <div class="footer">
        <hr style= "width: 85%;">
        <p class = "footnote"><em>Accuracy: ± 2°C Temperature and ± 5% Relative Humidity </em></p>
        <p class = "attribution"><em> Made by Leouigi Kyle R. Placido for Project S.H.I.N.E (English Project) </em> </p>
      </div>
      
    </div>

    <script>
    async function updateData(){

      try {
      const response = await fetch("/api");

      if (!response.ok) {
            throw new Error("HTTP error: " + response.status);
      }

      const json = await response.json();

      // Get all the data

      document.getElementById("temperature").innerHTML= json.temperature + " °C";
      document.getElementById("humidity").innerHTML=json.humidity + " %";
      document.getElementById("heatIndex").innerHTML=json.heatIndex + " °C";
      

      }
      catch (error){
      console.error("Error fetching data from sensor", error)
      }
    }

    updateData();
    setInterval(updateData, 2000);
    </script>
</body>

</html>
)rawliteral";

void setup(){
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wifi : ");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("~");
  }
  Serial.println("Connected to Wifi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  readSensor();
  lastRead = millis();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  server.on("/api", HTTP_GET, [](AsyncWebServerRequest *request){

    if (isnan(temperature) || isnan(humidity) || isnan(heatIndex)){
      request->send(503,
      "application/json",
      "{\"error\": \"Data cannot be accessed or does not exist\"}");
      return;
    }

    String json = "{";
    json += "\"temperature\": " + String(temperature,2) + ",";
    json += "\"humidity\": " + String(humidity,2) + ",";
    json += "\"heatIndex\": " + String(heatIndex,2);
    json += "}";
    request->send(200, "application/json", json); });

  server.begin();

  Serial.println("Server has been started (HTTP)");
}

void loop(){

  if (millis() - lastRead >= 2000){
    readSensor();
    lastRead = millis();
  }
}

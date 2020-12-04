#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

#define DHTTYPE DHT11
#define DHTPIN 2

const char* ssid     = "<ssid>";
const char* password = "<password>";

WiFiServer server(80);
DHT dht(DHTPIN, DHTTYPE);

//IPAddress local_IP(192, 168, 0, 200);
//IPAddress gateway(192, 168, 0, 1);
//IPAddress subnet(255, 255, 255, 0);

void setup() {
  Serial.begin(9600);
  delay(10);
  Serial.println();

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

//  if (!WiFi.config(local_IP, gateway, subnet)) {
//    Serial.println("STA Failed to configure");
//  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  dht.begin();
  Serial.println("DHT Module started");
  delay(500);
}

void loop() {
  delay(5000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print("temperature:");
  Serial.print(t);
  Serial.print(" humidity:");
  Serial.print(h);
  Serial.println();

  WiFiClient client = server.available();
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");  // the connection will be closed after completion of the response
  client.println("Refresh: 5");  // refresh the page automatically every 5 sec
  client.println();

  String html = "<!DOCTYPE html><html lang=\"en\"><head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>Document</title> <link rel=\"preconnect\" href=\"https://fonts.gstatic.com\"> <link href=\"https://fonts.googleapis.com/css2?family=Open+Sans:wght@400;700&display=swap\" rel=\"stylesheet\"> <style>html, body{width: 100%; height: 100%; margin: 0; padding: 0;}body{display: flex; flex-direction: column; font-family: 'Open Sans', sans-serif;}.temperatureContainer, .humidityContainer{display: flex; flex-direction: column; align-items: center; justify-content: center;}.temperatureContainer>.label, .humidityContainer>.label{font-size: 20px;}.temperatureContainer>.value, .humidityContainer>.value{font-weight: bold;}.temperatureContainer{flex: 2; font-size: 100px; background-color: var(--temperature-bg); color: var(--temperature-fg); border-bottom: solid 1px white;}.humidityContainer{flex: 1; font-size: 50px; background-color: var(--humidity-bg); color: var(--humidity-fg);}</style> <script>var red=\"#f44336\"; var blue=\"#2196F3\"; var green=\"#4CAF50\"; var fgColor=\"#212121\"; function temperatureBgCalculator(value){var result=green; if (value < 20){result=blue;}else if (value > 34){result=red;}return result;}function temperatureFgCalculator(value){return fgColor;}function humidityBgCalculator(value){var result=blue; if (value < 50){result=green;}else if (value > 80){result=red;}return result;}function humidityFgCalculator(value){return fgColor;}function setValues(temperature, humidity){document.documentElement.style.setProperty(\"--temperature-bg\", temperatureBgCalculator(temperature)); document.documentElement.style.setProperty(\"--temperature-fg\", temperatureFgCalculator(fgColor)); document.querySelector(\".temperatureContainer span.value\").innerHTML=`${temperature}°`; document.documentElement.style.setProperty(\"--humidity-bg\", humidityBgCalculator(humidity)); document.documentElement.style.setProperty(\"--humidity-fg\", humidityFgCalculator(fgColor)); document.querySelector(\".humidityContainer span.value\").innerHTML=`${humidity}%`;}</script></head><body> <div class=\"temperatureContainer\"> <span class=\"label\">Sıcaklık</span> <span class=\"value\"></span> </div><div class=\"humidityContainer\"> <span class=\"label\">Nem</span> <span class=\"value\"></span> </div><script>setValues({{temperature}},{{humidity}}); </script></body></html>";
  html.replace("{{temperature}}", String(t));
  html.replace("{{humidity}}", String(h));
  client.println(html);
}
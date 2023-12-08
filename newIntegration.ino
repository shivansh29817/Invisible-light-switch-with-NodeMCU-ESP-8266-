#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <TimeLib.h>

#define LASER_SENSOR_PIN D8
#define LDR_SENSOR_PIN D0
#define RELAY_PIN D4

const char *ssid = "NotHuman";
const char *password = "nothuman";

#define BOTtoken "6570310938:AAH4X2ugi-JnKQ-SbrRkwZ_23kzANpnwg4s"
#define CHAT_ID "1177426775"

int prevLdrSensorValue = HIGH;
int bulbState = LOW;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  pinMode(LASER_SENSOR_PIN, OUTPUT);
  pinMode(LDR_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, bulbState);
  digitalWrite(LASER_SENSOR_PIN, HIGH);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    handleClient(client);
  }

  int ldrSensorValue = digitalRead(LDR_SENSOR_PIN);

  if (prevLdrSensorValue == LOW && ldrSensorValue == HIGH) {
    bulbState = !bulbState;
    digitalWrite(RELAY_PIN, bulbState);
  }

  // Get current time
  time_t now = time(nullptr);
  struct tm *timeInfo;
  timeInfo = localtime(&now);

  // Set the scheduler time (in 24-hour format)
  int turnOnHour = 6;
  int turnOffHour = 18;

  // Check if the current time is within the scheduled range
  if (timeInfo->tm_hour >= turnOnHour && timeInfo->tm_hour < turnOffHour) {
    // Turn on the bulb
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    // Turn off the bulb
    digitalWrite(RELAY_PIN, LOW);
  }

  prevLdrSensorValue = ldrSensorValue;
  delay(10);
}

void handleClient(WiFiClient client) {
  if (client.available()) {
    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("/toggle") != -1) {
      bulbState = !bulbState;
      digitalWrite(RELAY_PIN, bulbState);
    }

    String response = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>IoT Project</title><style>";
    response += "body{ background-color: #000000; color: aliceblue; font-family: 'Monsterrat',sans-serif; display: flex; flex-direction: column; padding: 50px; margin: 0%; }";
    response += ".mainHead{ font-size: 40px; display: flex; justify-content: center; align-items: center; height: 150px; }";
    response += ".leftMain{ width: 50%; display: flex; justify-content: center; align-items: center; flex-direction: column; }";
    response += ".rightMain{ width: 50%; display: flex; justify-content: flex-start; align-items:flex-start; flex-direction: column; }";
    response += ".det{ margin: auto; }";
    response += ".container{ display: flex; justify-content: space-around; }";
    response += "span{ color: aqua; }";
    response += ".bpic{ width: 120px; height:200px; }";
    response += ".btnbox{ width: 180px;}";
    response += ".button{ padding: 10px 15px; border: none; text-decoration: none; border-radius: 8px; color: aqua; background-color: #252525; font-size: 20px; letter-spacing: 0.5px; transition: all 0.5s ease; cursor: pointer; }";
    response += ".button:hover{ background-color: #141414; border-radius: 10px; }";
    response += "@media only screen and (max-width: 767px) { .hide-on-phone { display: none; } .mediumtext{ font-size:30px; height: 40px; margin-bottom: 40px; }  .text{ font-size: large; } .down{ display: flex; flex-direction: column; align-items: center; gap: 50px; }.rightMain{width:auto;} .leftMain{width:auto; gap:30px;} }";
    response += "li{ margin: 5px 0; }";
    response += ".member{ display: flex; justify-content: left; width: 300px; }";
    response += "</style></head><body>";
    response += "<div class=\"main\"><h1 class=\"mainHead mediumtext\">Invisible Light Switch using <span>NodeMCU</span>.</h1></div>";
    response += "<div class=\"container down\"><div class=\"leftMain\"><img class=\"bpic\" src=\"https://static.vecteezy.com/system/resources/previews/009/385/277/original/colored-light-bulb-clipart-design-illustration-free-png.png\" /><h1 class=\"leftHead text\">Control Bulb <span>Remotely</span></h1><div class=\"btnBox\"><a href='/toggle' class='button'>Turn On/Off</a></div></div>";
    response += "<div class=\"rightMain \"><h1 class=\"det hide-on-phone\">Project Details :</h1>";
    response += "<p class=\"hide-on-phone\">This project utilizes a NodeMCU ESP8266 microcontroller to control a bulb wirelessly over WiFi. The system incorporates a laser sensor and an LDR sensor to detect changes in light conditions. When the laser beam is interrupted, the bulb state toggles, providing a simple mechanism for turning the bulb on and off. The user interface is a web page served by the ESP8266, featuring a single \"Turn ON/OFF\" button for convenient control.</p>";
    response += "<div class=\"member\"><h3>Team Members:</h3><ul><li>Shivansh Mishra</li><li>Parth Awasthi</li><li>Ashumendra Pratap Singh</li></ul></div></div></div>";
    response += "<script src=\"https://unpkg.com/@dotlottie/player-component@latest/dist/dotlottie-player.mjs\" type=\"module\"></script></body></html>";

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println("");
    client.println(response);
  }

  delay(1);
 

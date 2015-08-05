#include <DNSServer2.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include "WebPages.h"

/* Settings you may want to configure for your LogoBot */
#define LOGOBOT_SSID "LogobotBlue"
#define LOGOBOT_PWD "logobot1"
#define LOGOBOT_URL "logo.bot"
static const IPAddress apIP(192, 168, 4, 1);
static const IPAddress mask(255, 255, 255, 0);

/* Settings you should leave alone */
#define DNS_PORT 53
#define MAX_PARAM_LENGTH 50
#define led 2
#define LOGOBOT_RESPONSE_TIME 200

DNSServer dnsServer;
ESP8266WebServer server(80);

static void urldecode2(char *dst, const char *src)
{
  char a, b;
  while (*src) {
    if ((*src == '%') &&
      ((a = src[1]) && (b = src[2])) &&
      (isxdigit(a) && isxdigit(b))) {
      if (a >= 'a')
        a -= 'a' - 'A';
      if (a >= 'A')
        a -= ('A' - 10);
      else
        a -= '0';
      if (b >= 'a')
        b -= 'a' - 'A';
      if (b >= 'A')
        b -= ('A' - 10);
      else
        b -= '0';
      *dst++ = 16 * a + b;
      src += 3;
    } else {
        *dst++ = *src++;
    }
  }
  *dst++ = '\0';
}

static void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, F("text/html"), mainPage);
  digitalWrite(led, 0);
}

static void handleStatus() {
  digitalWrite(led, 1);
  server.send(200, F("text/html"), statusPage);
  digitalWrite(led, 0);
}

static void handleBatch() {
  digitalWrite(led, 1);
  server.send(200, F("text/html"), batchPage);
  digitalWrite(led, 0);
}

static void handleCommand()
{
  digitalWrite(led, 1);
  
  if (server.hasArg("action")) {
    // Parse args
    String cmd = server.arg("action");
    char arg[MAX_PARAM_LENGTH];
    char temp[MAX_PARAM_LENGTH];
    if (server.hasArg("p1")) {
      server.arg("p1").toCharArray(arg, MAX_PARAM_LENGTH);
      urldecode2(temp, arg);
      cmd += " " + String(temp);
    }
    if (server.hasArg("p2")) {
      server.arg("p2").toCharArray(arg, MAX_PARAM_LENGTH);
      urldecode2(temp, arg);
      cmd += " " + String(temp);
    }
    
    // Clear input in anticipation of a response.
    while(Serial.available())
      Serial.read();

    // Send command
    Serial.println(cmd);

    // Wait for a limited time for a reply
    Serial.setTimeout(LOGOBOT_RESPONSE_TIME);
    String response = Serial.readStringUntil('\n');

    server.send(200, F("text/plain"), response);

  } else {
    server.send(200, F("text/plain"), F("err"));
  }
  
  digitalWrite(led, 0);
}

static void handleStat()
{
  digitalWrite(led, 1);
  
  // Clear input in anticipation of a response.
  while(Serial.available())
    Serial.read();
  
  // Request status from Logobot
  Serial.println("STAT");
  
  // Wait for a limited time for a reply
  Serial.setTimeout(LOGOBOT_RESPONSE_TIME);
  String response = Serial.readStringUntil('\n');
  
  if (response.length() == 0) {
    server.send(401, F("application/json"), F("{\"response\": \"noreply\"}"));
  } else {
    // Split on space to get 4 parameters "x y ang qSize"
    response.replace('\r', ' ');
    String params[4];
    int a = 0;
    int b = 0;
    for (int i = 0; i < 4; i++) {
      b = response.indexOf(' ', a);
      params[i] = response.substring(a, b);
      a = b + 1;
    }
    
    int space = response.indexOf(' ');
    String json = "{\"response\": \"ok\"";
    json += ", \"x\": \"" + params[0] + "\"";
    json += ", \"y\": \"" + params[1] + "\"";
    json += ", \"ang\": \"" + params[2] + "\"";
    json += ", \"qSize\": \"" + params[3] + "\"";
    json += "}";
    
    server.send(200, F("application/json"), json);
  }
  
  digitalWrite(led, 0);
}

static void handleNotFound()
{
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, F("text/plain"), message);
  digitalWrite(led, 0);
}
 
void setup(void){
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("Logobot Wifi Interface"));
  
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, mask);
  WiFi.softAP(LOGOBOT_SSID, LOGOBOT_PWD);
  
  dnsServer.setTTL(300);
  dnsServer.setErrorReplyCode(DNSReplyCode::NonExistentDomain);
  dnsServer.start(DNS_PORT, LOGOBOT_URL, apIP);

  // Most frequently accesed pages first for speed.  
  server.on("/stat", handleStat);
  server.on("/cmd", handleCommand);
  server.on("/status", handleStatus);
  server.on("/", handleRoot);
  server.on("/batch", handleBatch);
  server.onNotFound(handleNotFound);
  
  server.begin();
}
 
void loop(void){
  dnsServer.processNextRequest();
  server.handleClient();
}

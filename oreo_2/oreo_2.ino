#include <Adafruit_CC3000.h>
#include <Adafruit_CC3000_Server.h>
#include <ccspi.h>
#include <CC3000_MDNS.h>
#include <SPI.h>
#include "utility/debug.h"
#include "utility/socket.h"
#include <PubSubClient.h>

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11

#define WLAN_SSID       "ATT121" 
#define WLAN_PASS       "9804522874"
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define LISTEN_PORT           80
#define MAX_ACTION            10
#define MAX_PATH              64
#define BUFFER_SIZE           MAX_ACTION + MAX_PATH + 20 
#define TIMEOUT_MS            500

IPAddress mqtt_ip(192, 168, 1, 82);

Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed

Adafruit_CC3000_Server httpClient(LISTEN_PORT);

// MQTT
Adafruit_CC3000_Server mqttServer(1883);
Adafruit_CC3000_Client mqttClient = cc3000.connectTCP(mqtt_ip,1883); 

uint8_t buffer[BUFFER_SIZE+1];
int bufindex = 0;
char action[MAX_ACTION+1];
char path[MAX_PATH+1];

void blinkLED(){
  digitalWrite(9,HIGH);
  delay(200);
  digitalWrite(9,HIGH);
  delay(200); 
}

void Anim_MQTT_isUp(){
  Serial.println("MQTT is running");
  digitalWrite(9,HIGH);
  delay(100);
  digitalWrite(9,LOW);
  delay(100);
  digitalWrite(9,HIGH);
  delay(1000);
  digitalWrite(9,LOW);
}

void Anim_HTTP_isUp(){
  Serial.println("HTTP is running");
  digitalWrite(9,HIGH);
  delay(100);
  digitalWrite(9,LOW);
  delay(100);
  digitalWrite(9,HIGH);
  delay(1000);
  digitalWrite(9,LOW);
  delay(100);
  digitalWrite(9,LOW);
  delay(100);
  digitalWrite(9,HIGH);
  delay(1000);
  digitalWrite(9,LOW);
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  blinkLED();
}

PubSubClient mqtt_client(mqtt_ip, 1883 , {}, mqttClient);

void setup(void)
{

//  mqtt_client.setClient(mqttClient);
//  mqtt_client.setServer(mqttServer, 1883);
//  mqtt_client.setCallback(callback);
  
  pinMode(13,OUTPUT);
  digitalWrite(9,HIGH);
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!\n")); 

  Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);
  
  // Initialise the module
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  
  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
  Serial.println(F("Connected!"));
  
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    // ToDo: Insert a DHCP timeout!
  }  

  // Display the IP address DNS, Gateway, etc.
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  // ******************************************************
  // You can safely remove this to save some flash memory!
  // ******************************************************
  Serial.println(F("\r\nNOTE: This sketch may cause problems with other sketches"));
  Serial.println(F("since the .disconnect() function is never called, so the"));
  Serial.println(F("AP may refuse connection requests from the CC3000 until a"));
  Serial.println(F("timeout period passes.  This is normal behaviour since"));
  Serial.println(F("there isn't an obvious moment to disconnect with a server.\r\n"));
  
  // Start listening for connections
  digitalWrite(9,HIGH);
  delay(100);
  digitalWrite(9,LOW);
  delay(100); 
  httpClient.begin();
  
  Serial.println(F("Listening for connections..."));
}

void loop(void)
{
  // Try to get a client which is connected.
  Adafruit_CC3000_ClientRef http_client = httpClient.available();
//  int mqtt_up = mqttServer.available();
//
//  if(mqtt_up){
//    Anim_MQTT_isUp();
//  }
  
  if (http_client) {
    Anim_HTTP_isUp();
    Serial.println(F("Client connected."));
    // Process this request until it completes or times out.
    // Note that this is explicitly limited to handling one request at a time!

    // Clear the incoming data buffer and point to the beginning of it.
    bufindex = 0;
    memset(&buffer, 0, sizeof(buffer));
    
    // Clear action and path strings.
    memset(&action, 0, sizeof(action));
    memset(&path,   0, sizeof(path));

    // Set a timeout for reading all the incoming data.
    unsigned long endtime = millis() + TIMEOUT_MS;
    
    // Read all the incoming data until it can be parsed or the timeout expires.
    bool parsed = false;
    while (!parsed && (millis() < endtime) && (bufindex < BUFFER_SIZE)) {
      if (http_client.available()) {
        buffer[bufindex++] = http_client.read();
      }
      parsed = parseRequest(buffer, bufindex, action, path);
    }

    // Handle the request if it was parsed.
    if (parsed) {
      Serial.println(F("Processing request"));
      Serial.print(F("Action: ")); Serial.println(action);
      Serial.print(F("Path: ")); Serial.println(path);
      // Check the action to see if it was a GET request.
      if (strcmp(action, "GET") == 0) {
        // Respond with the path that was accessed.
        // First send the success response code.
        http_client.fastrprintln(F("HTTP/1.1 200 OK"));
        // Then send a few headers to identify the type of data returned and that
        // the connection will not be held open.
        http_client.fastrprintln(F("Content-Type: text/plain"));
        http_client.fastrprintln(F("Connection: close"));
        http_client.fastrprintln(F("Server: Adafruit CC3000"));
        // Send an empty line to signal start of body.
        http_client.fastrprintln(F(""));
        // Now send the response data.
        http_client.fastrprintln(F("Hello world!"));
        http_client.fastrprint(F("You accessed path: ")); http_client.fastrprintln(path);
      }
      else {
        // Unsupported action, respond with an HTTP 405 method not allowed error.
        http_client.fastrprintln(F("HTTP/1.1 405 Method Not Allowed"));
        http_client.fastrprintln(F(""));
      }
    }

    // Wait a short period to make sure the response had time to send before
    // the connection is closed (the CC3000 sends data asyncronously).
    delay(100);

    // Close the connection when done.
    Serial.println(F("http_client disconnected"));
    http_client.close();
    
  }
}

// Return true if the buffer contains an HTTP request.  Also returns the request
// path and action strings if the request was parsed.  This does not attempt to
// parse any HTTP headers because there really isn't enough memory to process
// them all.
// HTTP request looks like:
//  [method] [path] [version] \r\n
//  Header_key_1: Header_value_1 \r\n
//  ...
//  Header_key_n: Header_value_n \r\n
//  \r\n
bool parseRequest(uint8_t* buf, int bufSize, char* action, char* path) {
  // Check if the request ends with \r\n to signal end of first line.
  if (bufSize < 2)
    return false;
  if (buf[bufSize-2] == '\r' && buf[bufSize-1] == '\n') {
    parseFirstLine((char*)buf, action, path);
    return true;
  }
  return false;
}

// Parse the action and path from the first line of an HTTP request.
void parseFirstLine(char* line, char* action, char* path) {
  // Parse first word up to whitespace as action.
  char* lineaction = strtok(line, " ");
  if (lineaction != NULL)
    strncpy(action, lineaction, MAX_ACTION);
  // Parse second word up to whitespace as path.
  char* linepath = strtok(NULL, " ");
  if (linepath != NULL)
    strncpy(path, linepath, MAX_PATH);
}

// Tries to read the IP address and other connection details
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}




#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

const char* ssid = "zavod";
const char* password = "xxxxxxxxx";

WiFiUDP udp_client;
unsigned int client_port = 12345;  // local port to listen on
unsigned int server_port = 4210;
const char * server_ip = "192.168.145.1";
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "client 1 got it";  // a reply string to send back

bool result;
unsigned long start_millis;
unsigned long end_millis;

void setup()
{
  Serial.begin(9600);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" connected");
  delay(100);

  udp_client.begin(client_port);
  Serial.print("listening on port:");
  Serial.println(client_port);

}


void loop()
{

  start_millis = millis();
  udp_client.beginPacket(server_ip, server_port);
  udp_client.write(replyPacket);
  udp_client.endPacket();
  Serial.print("sent ... ");

  int packetSize = udp_client.parsePacket();

  if (packetSize) {
    Serial.println(millis());
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d", packetSize, udp_client.remoteIP().toString().c_str(), udp_client.remotePort());
    Serial.println();
    int len = udp_client.read(incomingPacket, 255);

    if (len > 0) {
      incomingPacket[len] = 0;
    }

    Serial.printf("UDP packet contents: %s", incomingPacket);
    Serial.println();
    Serial.print("UDP remote ip:");
    Serial.print(udp_client.remoteIP());
    Serial.println();
    Serial.print("UDP remote port:");
    Serial.print(udp_client.remotePort());
    Serial.println();
    end_millis = millis();
    Serial.print("roundtrip:");
    Serial.println(end_millis - start_millis);

  }
}


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#define BUTTON_1 15

//wifi

const char *ssid = "zavod";
const char *password = "xxxxxxxxx";

WiFiUDP udp_client;
unsigned int client_port = 12345; // local port to listen on
unsigned int server_port = 4210;
const char *server_ip = "192.168.145.1";
char incomingPacket[255]; // buffer for incoming packets
char ReplyPacket[2];      // a reply string to send back
bool result;
unsigned long start_millis;
unsigned long end_millis;
int race; //state of race 0: stop 1:start 2:cil
int switch_status;
int switch_status_last;

void comm_info()
{
  Serial.print("UDP remote ip:");
  Serial.print(udp_client.remoteIP());
  Serial.println();
  Serial.print("UDP remote port:");
  Serial.print(udp_client.remotePort());
  Serial.println();
}

void initialize_pins()
{
  pinMode(15, INPUT_PULLUP); //D8
  pinMode(12, INPUT_PULLUP); //D6
  pinMode(LED_BUILTIN, OUTPUT);
}

void process_packet(int pckt_size)
{
  int res;

  Serial.println(millis());
  // receive incoming UDP packets
  Serial.printf("Received %d bytes from %s, port %d", pckt_size, udp_client.remoteIP().toString().c_str(), udp_client.remotePort());
  Serial.println();
  int len = udp_client.read(incomingPacket, 255);

  if (len > 0)  {
    incomingPacket[len] = 0;
  }

  Serial.printf("UDP packet contents: %s", incomingPacket);
  Serial.println();
  comm_info();

  if (!strcmp(incomingPacket, "start"))  {
    //start race
    strcpy(ReplyPacket, "11");
  }  else if (!strcmp(incomingPacket, "stop"))  {
    //end race
    strcpy(ReplyPacket, "00");
  }

  end_millis = millis();
  Serial.print("roundtrip:");
  Serial.println(end_millis - start_millis);
}

void check_keys()
{
  switch_status = digitalRead(D6);

  if (switch_status)
  {
    Serial.println("switch pressed");
  }
  else
  {
    Serial.println("switch off");
  }

  if (switch_status != switch_status_last)  {
    if (switch_status == HIGH && switch_status_last == LOW)    {
      Serial.println("switch press end");

      if (!race)      {
        //start_race();
      } else {
        //abort_race();
      }
    }

    if (switch_status == 0 && switch_status_last == 1)    {
      Serial.println("switch press start");
    }
  }

  switch_status_last = switch_status;
}

void setup()
{

  Serial.begin(9600);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" connected");
  delay(100);

  initialize_pins();

  udp_client.begin(client_port);
  Serial.print("listening on port:");
  Serial.println(client_port);

  strcpy(ReplyPacket, "00");
}

void send_info_packet()
{
  udp_client.beginPacket(server_ip, server_port);
  udp_client.write(ReplyPacket);
  udp_client.endPacket();
  Serial.print("sent ... ");
}

void loop()
{
  int packet_size;

  //tady se nekde nadetekuje aktivace cile
  //if (konec) {strcpy(ReplyPacket,"22");}
  check_keys();

  start_millis = millis();

  packet_size = udp_client.parsePacket();
  if (packet_size)
  {
    process_packet(packet_size);
  }

  //send_info_packet();
}

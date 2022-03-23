#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#define BUTTON_1 D6

#define PROTO_SPECIAL_CONNECTED "01" 
#define PROTO_SPECIAL_PONG "02" 
//wifi

#define STATE_SETTING 1234
#define STATE_NOTHING 0
#define STATE_RACE 1235

//#define SERIAL_DEBUG 
//#define KEYBOARD_DEBUG
//#define LED_DEBUG

const char *ssid = "zavod";
const char *password = "xxxxxxxxx";
const char *lane_id = "1";

WiFiUDP udp_client;
unsigned int client_port = 12345; // local port to listen on
unsigned int server_port = 4210;
const char *server_ip = "192.168.145.1";
char incomingPacket[255]; // buffer for incoming packets
char ReplyPacket[30];      // a reply string to send back
bool result;
unsigned long start_millis;
unsigned long end_millis;
unsigned long press_start;
int press_counter = 0;
unsigned long press_time = 0;
int hold_long = 0;
int hold_short = 0;

int race; //state of race 0: stop 1:start 2:cil
int switch_status;
int switch_status_last;
int machine_state = STATE_NOTHING;

int get_comm_channel                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ()
{
  return 145;
}

void set_ipv4()
{
  int x = get_comm_channel();
  const char *server_ip = "192.168.145.1";
  //ip_addr = new IPAddress(192,168,x,1);  //server ip address
  //gateway = new IPAddress(192,168,x,1);
  //subnet = new IPAddress(255,255,255,0);
}

void send_info_packet()
{
  if (! strlen(ReplyPacket) > 0) return;
  
  #ifdef SERIAL_DEBUG
  Serial.print("sending packet ");
  Serial.print(server_ip);
  Serial.println(server_port);
  #endif
  //udp_client.beginPacket(server_ip, server_port);
  udp_client.beginPacket("192.168.145.1", 4210);
  udp_client.write(ReplyPacket);
  udp_client.endPacket();
  #ifdef SERIAL_DEBUG
  Serial.printf("%s sent ... \n", ReplyPacket);
  #endif
}

void led_blink()
{
   digitalWrite(LED_BUILTIN, LOW);
   delay(100);
   digitalWrite(LED_BUILTIN, HIGH);
}

void comm_info()
{
  #ifdef SERIAL_DEBUG
  Serial.print("UDP remote ip:");
  Serial.print(udp_client.remoteIP());
  Serial.print(" ");
  Serial.print("UDP remote port:");
  Serial.print(udp_client.remotePort());
  Serial.println();
  #endif
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

  //Serial.println(millis());
  // receive incoming UDP packets
  //Serial.printf("Received %d bytes from %s, port %d", pckt_size, udp_client.remoteIP().toString().c_str(), udp_client.remotePort());
  //Serial.println();
  int len = udp_client.read(incomingPacket, 255);

  if (len > 0)  {
    incomingPacket[len] = 0;
  }

  //Serial.printf("Contents: %s", incomingPacket);
  //Serial.println();
  //comm_info();

  if (!strcmp(incomingPacket, "00"))  {
    strcpy(ReplyPacket, lane_id);
    strcat(ReplyPacket, "2");  
  }

  if (!strcmp(incomingPacket, "start"))  {
    //start race
    strcpy(ReplyPacket, "13");
  }  else if (!strcmp(incomingPacket, "stop"))  {
    //end race
    strcpy(ReplyPacket, "10");
  }

  end_millis = millis();
  //Serial.print("roundtrip:");
  //Serial.println(end_millis - start_millis);
}

void check_keys()
{
  switch_status = digitalRead(D6);

  if (!switch_status)  {
    //Serial.print("*");
    if (press_start) {
      press_time = millis() - press_start;
    }
  }  else  {
    press_time = 0;
    //Serial.println("switch off");
  }

  if (switch_status != switch_status_last)  {

    //DEPRESS
    if (switch_status == HIGH && switch_status_last == LOW)    {
      Serial.println(".");
      Serial.println("switch press end");
      strcpy(ReplyPacket, "11");
      send_info_packet();
      led_blink();
      //check press length

      unsigned long press_length = millis() - press_start;

      Serial.println(press_counter);
      Serial.println(press_length);

      if (press_length > 700 && press_length < 1000) {
        Serial.println("long press");
        //reset counter
        press_counter = 0;
      }

      if (press_length > 300 && press_length < 700) {
        Serial.println("short press");
        //reset counter
        press_counter = 0;
      }

      /*if (press_length > 100 && press_length < 300 && press_counter > 2) {
        Serial.println("double press");
        //reset counter
        press_counter = 0;
        //set device
      }*/

      press_start = 0;
    }

    //PRESS
    if (switch_status == LOW && switch_status_last == HIGH)    {
      Serial.println("switch press start");
      press_start = millis();
      press_counter++;
    }
    
    switch_status_last = switch_status;
  }

  //long hold
  if (press_time > 1000) {
    press_counter = 0;
    Serial.println("long hold");
    hold_long = 1;
  }
  
  if (press_time > 300) {
    press_counter = 0;
    //Serial.println("double click cleared");

  }

}

void heartbeat()
{
  udp_client.beginPacket("192.168.145.1", 4210);
  udp_client.write("19");
  udp_client.endPacket();
}


void setup()
{
int len;

  Serial.begin(9600);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println(" connected");
  delay(100);

  initialize_pins();

  udp_client.begin(client_port);
  Serial.print("listening on port:");
  Serial.println(client_port);

//send login packet

  strcpy(ReplyPacket, lane_id);
  strcat(ReplyPacket, "0");
  
  send_info_packet();

  //wait for response

  Serial.println("waiting ...");
  for(;;) {
    len = udp_client.read(incomingPacket, 255);
    if (len > 0)  {
      incomingPacket[len] = 0;
      Serial.println("incoming");
      Serial.println(incomingPacket);
      
      if (!strcmp(incomingPacket, PROTO_SPECIAL_CONNECTED))  {
      //continue to loop
      Serial.println("done");
      return;
      }
    }
    delay(10);
    Serial.print("#");

  }
  Serial.println("connected");
  switch_status_last = switch_status = 0;
}


unsigned long prev_millis = millis();

void loop()
{
  int packet_size;

  strcpy(ReplyPacket,"");

  unsigned long curr_millis = millis();
  int interval = 1000;

  if ((unsigned long)(curr_millis - prev_millis) < interval) {
    check_keys();
  } else  {
    //heartbeat();
    prev_millis = millis();
  }
  
  check_keys();

  start_millis = millis();

  packet_size = udp_client.parsePacket();
  if (packet_size)  {
    process_packet(packet_size);
  }

  send_info_packet();
}

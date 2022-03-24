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
#define STATE_SETTING_NETWORK 12
#define STATE_SETTING_DEVICE  13

#define LED_STATUS D4
#define LED_BIN_1 D2
#define LED_BIN_2 D5
#define LED_BIN_3 D8

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
int network_identification = 0;
int device_identification = 0;
unsigned long led_setup_interval;
unsigned long led_prev_millis;
int led_status_light = 0;

int race; //state of race 0: stop 1:start 2:cil
int switch_status;
int switch_status_last;
int machine_state = STATE_NOTHING;
int setting_state = STATE_SETTING_NETWORK;

bool is_bit(int num, int pos)
{
  return (num & (1 << pos));
}

int get_comm_channel()
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
  if (! (strlen(ReplyPacket) > 0)) return;

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
  digitalWrite(LED_STATUS, LOW);
  delay(100);
  digitalWrite(LED_STATUS, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
}

void led_status_lights()
{
  switch  (machine_state) {
    case STATE_SETTING:
      switch  (setting_state) {
        case STATE_SETTING_NETWORK:
          led_setup_interval = 200;
          break;

        case STATE_SETTING_DEVICE:
          led_setup_interval = 600;
          break;
      }
  }

  if ((unsigned long)(millis() - led_prev_millis) < led_setup_interval) {
    digitalWrite(LED_STATUS, led_status_light);
  } else  {

    led_prev_millis = millis();
    led_status_light = !led_status_light;
  }
}

void led_bin_lights(int x)
{
#ifdef LED_DEBUG
  Serial.print(is_bit(x,2));
  Serial.print(is_bit(x,1));
  Serial.print(is_bit(x,0));
  Serial.println();
#endif

  digitalWrite(LED_BIN_1, is_bit(x,2) ? HIGH : LOW );
  digitalWrite(LED_BIN_2, is_bit(x,1) ? HIGH : LOW );
  digitalWrite(LED_BIN_3, is_bit(x,0) ? HIGH : LOW );

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

  pinMode(LED_STATUS, OUTPUT);

  pinMode(LED_BIN_1, OUTPUT);
  pinMode(LED_BIN_2, OUTPUT);
  pinMode(LED_BIN_3, OUTPUT);
}

void initialize_leds()
{
  digitalWrite(LED_BIN_1, LOW);
  digitalWrite(LED_BIN_2, LOW);
  digitalWrite(LED_BIN_3, LOW);
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

void press_short ()
{
  //reset counter
  press_counter = 0;

  switch  (machine_state) {
    case STATE_SETTING:
      switch  (setting_state) {
        case STATE_SETTING_NETWORK:
          if (++network_identification > 7) {
            network_identification = 0;
          }

          led_blink();
#ifdef LED_DEBUG
          Serial.print("network_identification:");
          Serial.println(network_identification);
#endif
          led_bin_lights(network_identification);
          break;

        case STATE_SETTING_DEVICE:
          if (++device_identification > 7) {
            device_identification = 0;
          }

          led_blink();
#ifdef LED_DEBUG
          Serial.print("device_identification:");
          Serial.println(device_identification);
#endif
          led_bin_lights(device_identification);
          break;

        default:
          break;
      }

      break;

    default:
#ifdef LED_DEBUG
      Serial.println("no known machine state");
#endif
      break;
  }

}

void check_keys()
{
  switch_status = digitalRead(D6);

  if (!switch_status)  {
    //pressed
    if (press_start) {
      press_time = millis() - press_start;
    }
  }  else  {
    //depressed
    press_time = 0;
    hold_long = 0;
  }

  if (switch_status != switch_status_last)  {

    //DEPRESS
    if (switch_status == HIGH && switch_status_last == LOW) {
#ifdef KEYBOARD_DEBUG
      Serial.println("switch press end");
      Serial.print("machine_state:");
      Serial.println(machine_state);
#endif
      strcpy(ReplyPacket, "11");
      send_info_packet();
      led_blink();

      unsigned long press_length = millis() - press_start;

      if (press_length > 700 && press_length < 1500) {
        //long press
        //reset counter
        press_counter = 0;

        if (machine_state == STATE_SETTING && setting_state == STATE_SETTING_NETWORK) {
          setting_state = STATE_SETTING_DEVICE;
        }
      }

      if (press_length > 100 && press_length < 700) {
        //short press
        press_short();
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
    if (switch_status == LOW && switch_status_last == HIGH) {
      //Serial.println("switch press start");
      press_start = millis();
      press_counter++;
    }

    switch_status_last = switch_status;
  }

//long hold
  if (press_time > 3000 and !hold_long) {
    press_counter = 0;
    //Serial.println("long hold");
    hold_long = 1;

    if (machine_state == STATE_SETTING) {
      machine_state = STATE_RACE;
      hold_long = 0;
    } else {
      machine_state = STATE_SETTING;
      setting_state = STATE_SETTING_NETWORK;
    }
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

  //Serial.begin(9600);
  //Serial.println();

  //Serial.printf("Connecting to %s ", ssid);
  initialize_leds();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    //Serial.print(".");
  }

  //erial.println("");
  //Serial.println(" connected");
  delay(100);

  initialize_pins();

  udp_client.begin(client_port);
  //Serial.print("listening on port:");
  //Serial.println(client_port);

//send login packet

  strcpy(ReplyPacket, lane_id);
  strcat(ReplyPacket, "0");

  send_info_packet();

  //wait for response

  //Serial.println("waiting ...");

  for (;;) {
    len = udp_client.read(incomingPacket, 255);

    if (len > 0)  {
      incomingPacket[len] = 0;
      //Serial.println("incoming");
      //Serial.println(incomingPacket);

      if (!strcmp(incomingPacket, PROTO_SPECIAL_CONNECTED))  {
        //continue to loop
        //Serial.println("done");
        return;
      }
    }

    delay(10);
    //Serial.print("#");

  }

  //Serial.println("connected");
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
    led_status_lights();
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

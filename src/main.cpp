#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>

//GPIO
#define BUTTON_1 D6
#define RELAY_1 D1
#define EXTERNAL_1 D7
#define LED_STATUS D4
#define LED_BIN_1 D2
#define LED_BIN_2 D5
#define LED_BIN_3 D8

#define PROTO_SPECIAL_CONNECTED "01"
#define PROTO_SPECIAL_PONG "02"
//wifi

#define STATE_NOTHING 0
#define STATE_WIFI 1231
#define STATE_CHECK 1232
#define STATE_LOGIN 1233
#define STATE_SETTING 1234
#define STATE_RACE 1235
#define STATE_IDLE 1236
#define STATE_SETTING_NETWORK 12
#define STATE_SETTING_DEVICE  13

#define SERIAL_DEBUG
#define KEYBOARD_DEBUG
#undef LED_DEBUG

char ssid[20];
const char *password = "xxxxxxxxx";
const char *lane_id = "1";

WiFiUDP udp_client;
unsigned int client_port = 12345; // local port to listen on
unsigned int server_port = 4210;
char server_ip[30];
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
int external_status;
int external_status_last;
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
  return network_identification + 140;
  //return 145;
}

void set_ipv4()
{
  int x = get_comm_channel();
  sprintf(server_ip, "192.168.%i.1", x);
  Serial.print("Server_ip:");
  Serial.println(server_ip);
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
  Serial.print(":");
  Serial.println(server_port);
#endif

  udp_client.beginPacket(server_ip, server_port);
  udp_client.write(ReplyPacket);
  udp_client.endPacket();
#ifdef SERIAL_DEBUG
  Serial.printf("-%s- sent ... \n", ReplyPacket);
#endif
}

void led_blink(int delayt)
{

  digitalWrite(LED_STATUS, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(delayt);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LED_STATUS, LOW);
}

void led_bin_lights(int x)
{
#ifdef LED_DEBUG
  Serial.printf("bin_lights val %i\n",x);
  Serial.print(is_bit(x,2));
  Serial.print(is_bit(x,1));
  Serial.print(is_bit(x,0));
  Serial.println();
#endif

  digitalWrite(LED_BIN_1, is_bit(x,2) ? HIGH : LOW );
  digitalWrite(LED_BIN_2, is_bit(x,1) ? HIGH : LOW );
  digitalWrite(LED_BIN_3, is_bit(x,0) ? HIGH : LOW );

}

void led_status_lights()
{
  switch  (machine_state) {
    case STATE_SETTING:
      switch  (setting_state) {
        case STATE_SETTING_NETWORK:
          led_bin_lights(network_identification);
          led_setup_interval = 200;
          break;

        case STATE_SETTING_DEVICE:
          led_bin_lights(device_identification);
          led_setup_interval = 600;
          break;
      }

      if ((unsigned long)(millis() - led_prev_millis) < led_setup_interval) {
        digitalWrite(LED_STATUS, led_status_light);
      } else  {

        led_prev_millis = millis();
        led_status_light = !led_status_light;
      }

      break;

    case STATE_WIFI:
      break;

    case STATE_RACE:
      digitalWrite(LED_STATUS, HIGH);
      break;

    case STATE_IDLE:
      led_bin_lights(0);
      led_setup_interval = 80;

      if ((unsigned long)(millis() - led_prev_millis) < led_setup_interval) {
        digitalWrite(LED_STATUS, led_status_light);
      } else  {

        led_prev_millis = millis();
        led_status_light = !led_status_light;
      }

      break;
  }
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

void  initialize_pins()
{
  pinMode(15, INPUT_PULLUP); //D8
  pinMode(12, INPUT_PULLUP); //D6

  pinMode(EXTERNAL_1, INPUT_PULLUP);

  pinMode(RELAY_1, OUTPUT);

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
    sprintf(ReplyPacket, "%i2", device_identification);
  }

  /*if (!strcmp(incomingPacket, "start"))  {
    //start race
    strcpy(ReplyPacket, "13");
  }  else if (!strcmp(incomingPacket, "stop"))  {
    //end race
    strcpy(ReplyPacket, "10");
  }*/

  end_millis = millis();
  //Serial.print("roundtrip:");
  //Serial.println(end_millis - start_millis);
}

int settings_get()
{
  File f;

  f = LittleFS.open("/data.txt", "r");

  if (!f) {
    return 0;
  }

  Serial.println("settings netvork device");

  if (f.available()) {
    String line = f.readStringUntil('\n');
    Serial.println(line);
    int x = atoi(line.c_str());
    network_identification = x;
  } else {
    network_identification = 0;
  }

  if (f.available()) {
    String line = f.readStringUntil('\n');
    Serial.println(line);
    int x = atoi(line.c_str());
    device_identification = x;
  } else {
    device_identification = 0;
  }

  f.close();

  return 1;
}

int settings_set()
{
  File f;

  f = LittleFS.open("/data.txt", "w");

  if (!f) {
    return 0;
  }

  f.println(network_identification);
  f.println(device_identification);

  f.close();
  return 1;
}

void press_short ()
{
  //reset counter
  press_counter = 0;

  switch  (machine_state) {
    case STATE_IDLE:
      Serial.println("press_short(): Idle, waiting for start");
      break;

    case STATE_RACE:
      Serial.println("press_short(): Race");
      break;

    case STATE_SETTING:
      switch  (setting_state) {
        case STATE_SETTING_NETWORK:
          if (++network_identification > 7) {
            network_identification = 0;
          }


          Serial.print("network_identification:");
          Serial.println(network_identification);

          led_bin_lights(network_identification);
          break;

        case STATE_SETTING_DEVICE:
          if (++device_identification > 7) {
            device_identification = 0;
          }

          Serial.print("device_identification:");
          Serial.println(device_identification);
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


void press_long()
{
  //long press
  //reset counter
  press_counter = 0;
#ifdef KEYBOARD_DEBUG
  Serial.println("long press");
#endif

  if (machine_state == STATE_SETTING && setting_state == STATE_SETTING_NETWORK) {
    setting_state = STATE_SETTING_DEVICE;
  }
}

void press_and_hold()
{
  press_counter = 0;
#ifdef KEYBOARD_DEBUG
  Serial.println("long hold");
  Serial.print("machine_state:");
  Serial.println(machine_state);
  Serial.print("setting_state:");
  Serial.println(setting_state);
#endif
  hold_long = 1;

  switch (machine_state) {
    case STATE_SETTING:
      machine_state = STATE_WIFI;
      settings_set();
      hold_long = 0;
      break;

    case STATE_NOTHING:
      machine_state = STATE_SETTING;
      setting_state = STATE_SETTING_NETWORK;
      break;

    case STATE_RACE:
      Serial.println("long during race");
      break;

    case STATE_IDLE:
      Serial.println("long during idle");
      machine_state = STATE_NOTHING;
      hold_long = 0;
      break;
  }
}

void check_keys()
{
  switch_status = digitalRead(D6);
  external_status = digitalRead(EXTERNAL_1);

  if (!external_status)  {
    //pressed
  }  else  {
    //depressed
  }

  if (external_status != external_status_last)  {
    //DEPRESS
    if (external_status == HIGH && external_status_last == LOW) {
      Serial.println("external press end");
      sprintf(ReplyPacket, "%i1", device_identification);
      send_info_packet();
    }

    //PRESS
    if (external_status == LOW && external_status_last == HIGH) {
      Serial.println("external press start");
    }

    external_status_last = external_status;
  }

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
      //Serial.println("switch press end");
      Serial.print("machine_state:");
      Serial.println(machine_state);
#endif


      unsigned long press_length = millis() - press_start;
      Serial.println(press_length);

      //LONG PRESS
      if (press_length > 700 && press_length < 2500) {
        press_long();
      }

      ////// SHORT PRESS
      if (press_length > 30 && press_length < 700) {
        press_short();
      }

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
    press_and_hold();
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


void blink_for(int sec, int interval)
{
  unsigned long start = millis();
  unsigned long led_prev_millis = start;
  int led_status_light = 1;

  Serial.println("blink start");

  while (millis() < start + sec * 1000) {
    if ((millis() - led_prev_millis) < interval) {
      delay(1);
      digitalWrite(LED_STATUS, led_status_light);
    } else {
      delay(1);
      led_prev_millis = millis();
      led_status_light = !led_status_light;
    }
  }

  Serial.println("blink done");
}

void settings_show()
{
  Serial.println("settings show (n d)");

  Serial.println(network_identification);
  Serial.println(device_identification);

  led_bin_lights(network_identification);
  blink_for(3, 300);
  //device setting
  led_bin_lights(device_identification);
  blink_for(3, 600);

  delay(5000);
  //light up state_led
  digitalWrite(LED_STATUS, HIGH);
  Serial.println("settings done");
}

void loop_idle()
{
  int packet_size;
  packet_size = udp_client.parsePacket();

  if (packet_size)  {
    strcpy(ReplyPacket,"");
    process_packet(packet_size);
    send_info_packet();
  }
}

void loop_race()
{
  int packet_size;
  packet_size = udp_client.parsePacket();

  if (packet_size)  {
    strcpy(ReplyPacket,"");
    process_packet(packet_size);
    send_info_packet();
  }

}


void wifi_connection()
{
  int channel;

  channel = get_comm_channel();
  channel = 142;
  sprintf(ssid, "zavod%i", channel);

  Serial.println("Connecting to wifi ...");


  Serial.printf("Connecting to %s:", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_STATUS, LOW);
    delay(200);
    Serial.print(".");
    digitalWrite(LED_STATUS, HIGH);
  }

  Serial.println("");   //lf after dots
  Serial.print("Connected, IP ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP client");
  delay(100);

  udp_client.begin(client_port);
  Serial.print("listening on port:");
  Serial.println(client_port);

  machine_state = STATE_LOGIN;
}

void loop_wifi_login()
{
  unsigned int interval = 200;
  unsigned long prev_millis = millis();
  int led_status_light = 0;
  int len;

  set_ipv4();

  Serial.print("Server:");
  Serial.print(server_ip);
  Serial.print(":");
  Serial.println(server_port);

//send login packet

  sprintf(ReplyPacket, "%i0", device_identification);

  send_info_packet();

  Serial.println("Waiting for response! ...");

  for (int ii = 0; ii < 2000; ii++) {
    len = udp_client.read(incomingPacket, 255);

    if (len > 0)  {
      incomingPacket[len] = 0;
      Serial.println("incoming packet");
      Serial.println(incomingPacket);

      if (!strcmp(incomingPacket, PROTO_SPECIAL_CONNECTED))  {
        //continue to loop
        Serial.println("login done");
        machine_state = STATE_IDLE;
        return;
      }
    }

    unsigned long curr_millis = millis();

    if ((unsigned long)(curr_millis - prev_millis) < interval) {
      digitalWrite(LED_STATUS, led_status_light);
    } else  {
      prev_millis = millis();
      led_status_light = !led_status_light;
      Serial.print("#");
    }

    delay(10);

  }

  Serial.println("!");
}

void setup()
{

  digitalWrite(RELAY_1, LOW);

  Serial.begin(9600);
  Serial.println();  //to solve garbage after reset
  Serial.println("HW Start:");
  initialize_pins();
  initialize_leds();

  Serial.print("FS:");

  if (!LittleFS.begin()) {
    Serial.println("fs error");
  } else {
    Serial.println("[ok]");
  }

  Serial.println("Settings:");

  //network setting
  if (settings_get()) {
    Serial.println("settings get");
  } else {
    Serial.println("settings not");
  }

  settings_show();

  machine_state = STATE_SETTING;

//return !!! rest is done by loop()

  switch_status_last = switch_status = 0;
  external_status = external_status_last = digitalRead(EXTERNAL_1);

}


unsigned long prev_millis = millis();

//

//       ///////  ///////  ///////
//       //   //  //   //  //   //
//       //   //  //   //  ///////
//       //   //  //   //  //
///////  ///////  ///////  //

//

void loop()
{
  unsigned long curr_millis = millis();
  int interval = 1000;

  if ((unsigned long)(curr_millis - prev_millis) < interval) {
    //check_keys();
    led_status_lights();
  } else  {
    //once per second stuff
    //heartbeat();
    prev_millis = millis();
  }

  check_keys();

  switch (machine_state) {
    case STATE_WIFI:
      wifi_connection();
      break;

    case STATE_LOGIN:
      loop_wifi_login();
      break;

    case STATE_NOTHING:
      //start
      break;

    case STATE_SETTING:
      break;

    case STATE_IDLE:
      loop_idle();
      break;

    case STATE_RACE:
      loop_race();
      break;
  }

  //start_millis = millis();


}

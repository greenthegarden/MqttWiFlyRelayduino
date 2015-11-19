// WiFly libraries
#include <SPI.h>
#include <WiFly.h>
#include <SoftwareSerial.h>


// RS-485 library
//#include <ICSC.h>

#include "config.h"

#if USE_FREEMEM
#include <MemoryFree.h>
#endif


#if DEBUG
void debug(const __FlashStringHelper * console_text)
{
  Serial.println(console_text);
}
#endif


#include <Time.h>

// write time and date to char_buffer (19 chars) in format
// 0000000000111111111
// 0123456789012345678
// HH:MM:SS DD-MM-YYYY
void date_string()
{
  char_buffer[0] = '\0';
  sprintf(char_buffer,
          "%02d:%02d:%02d %02d-%02d-%02d",
          hour(),
          minute(),
          second(),
          day(),
          month(),
          year()
          );
   if (daylight_summer_time)
     strcat(char_buffer, " DST");
}


#include <TimeAlarms.h>
byte current_timer_ref = 255;


// callback function definition required here as client needs to be defined before
// including relay.h
void callback(char* topic, uint8_t* payload, unsigned int length);


PubSubClient   mqtt_client(mqtt_server_addr, MQTT_PORT, callback, wifly_client);


void publish_connected()
{
  prog_buffer[0] = '\0';
  strcpy_P(prog_buffer, (char*)pgm_read_word(&(STATUS_TOPICS[0])));
  mqtt_client.publish(prog_buffer, "connected");
}

void publish_date()
{
  prog_buffer[0] = '\0';
  strcpy_P(prog_buffer, (char*)pgm_read_word(&(STATUS_TOPICS[1])));
  date_string();  // date is stored in char_buffer
  mqtt_client.publish(prog_buffer, char_buffer);
}

void publish_alarm_id(byte ref = 255)
{
  if (ref == 255)
    ref = Alarm.getTriggeredAlarmId();
  prog_buffer[0] = '\0';
  strcpy_P(prog_buffer, (char*)pgm_read_word(&(STATUS_TOPICS[5])));
  char str[4];
  mqtt_client.publish(prog_buffer, itoa(ref, str, 10));
}

void publish_alarms(bool enabled)
{
  prog_buffer[0] = '\0';
  strcpy_P(prog_buffer, (char*)pgm_read_word(&(STATUS_TOPICS[6])));
  if ( enabled )
    mqtt_client.publish(prog_buffer, "enabled");
  else
    mqtt_client.publish(prog_buffer, "disabled");
}

#if USE_FREEMEM
void publish_memory()
{
  prog_buffer[0] = '\0';
  strcpy_P(prog_buffer, (char*)pgm_read_word(&(STATUS_TOPICS[2])));
  char_buffer[0] = '\0';
  itoa(freeMemory(), char_buffer, 10);
  mqtt_client.publish(prog_buffer, char_buffer);
}
#endif


void disable_alarms()
{
  for (byte idx = 0; idx < ARRAY_SIZE(alarm_refs); idx++) {
    Alarm.disable(alarm_refs[idx]);
  }
  publish_alarms(false);
}

void enable_alarms()
{
  for (byte idx = 0; idx < alarm_refs_cnt; idx++) {
    Alarm.enable(alarm_refs[idx]);
  }
  publish_alarms(true);
}


#include "relayduinoConfig.h"

const byte RELAY_PINS_USED[] = {RELAY_1, RELAY_2, RELAY_3, RELAY_4};
int        relay_durations[] = {relay_1_duration, relay_2_duration, relay_3_duration, relay_4_duration};

void get_relay_durations()
{
//  for ( byte idx=0; idx<sizeof(relay_durations)/sizeof(int); idx++ ) {
  for (byte idx = 0; idx < ARRAY_SIZE(relay_durations); idx++) {
    prog_buffer[0] = '\0';
    strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(STATUS_TOPICS[7])));
    char str[4];
    mqtt_client.publish(prog_buffer, itoa(relay_durations[idx], str, 10));
  }
}

#if USE_MASTER_RELAY
// returns 1 if relay is currently on and switched off, else returns 0
byte master_relay_off()
{
  // only switch relay off if it is currently on
  if (digitalRead(RELAY_MASTER)) {
    digitalWrite(RELAY_MASTER, LOW);
#if DEBUG
    debug(F("master off"));
#endif
    prog_buffer[0] = '\0';
    strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(STATUS_TOPICS[4])));
    mqtt_client.publish(prog_buffer, "M");
    return 1;
  }
  return 0;
}

// returns 1 if relay is currently on and switched off, else returns 0
byte master_relay_on()
{
  // only switch relay on if it is currently off
  if(!digitalRead(RELAY_MASTER)) {
    digitalWrite(RELAY_MASTER, HIGH);
#if DEBUG
    debug(F("master on"));
#endif
    prog_buffer[0] = '\0';
    strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(STATUS_TOPICS[3])));
    mqtt_client.publish(prog_buffer, "M");
    return 1;
  }
  return 0;
}
#endif

// returns 1 if relay connected to given pin is on, else returns 0
byte relay_state(byte idx)
{
  return(digitalRead(RELAY_PINS_USED[idx]));
}

void relays_state()
{
  for (byte idx = 0; idx < ARRAY_SIZE(RELAY_PINS_USED); idx++) {
    if (digitalRead(RELAY_PINS_USED[idx])) {
#if DEBUG
      debug(F("relay on"));
#endif
      prog_buffer[0] = '\0';
      strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(STATUS_TOPICS[3])));
      char str[2];
      mqtt_client.publish(prog_buffer, itoa(idx+1, str, 10));
    } else {
#if DEBUG
      debug(F("relay off"));
#endif
      prog_buffer[0] = '\0';
      strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(STATUS_TOPICS[4])));
      char str[2];
      mqtt_client.publish(prog_buffer, itoa(idx+1, str, 10));
    }
  }
}

// returns 1 if relay is currently on and switched off, else returns 0
byte relay_switch_off(byte idx)
{
  // only switch relay off if it is currently on
  if (relay_state(idx)) {
    digitalWrite(RELAY_PINS_USED[idx], LOW);
#if DEBUG
    debug(F("relay off"));
#endif
    prog_buffer[0] = '\0';
    strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(STATUS_TOPICS[4])));
    char str[2];
    mqtt_client.publish(prog_buffer, itoa(idx+1, str, 10));
#if USE_MASTER_RELAY
    master_relay_off();
#endif
#if USE_LED
    digitalWrite(LED_PIN, LOW);
#endif
    return 1;
  }
  return 0;
}

// used by callback as a void function to switch off relay which is currenlty on
// and then switches off master
void relays_switch_off()
{
  for (byte idx = 0; idx < ARRAY_SIZE(RELAY_PINS_USED); idx++) {
    if (relay_state(idx))
      relay_switch_off(idx);
  }
}

byte timer_stop()
{
  return (Alarm.timerOnce(1, relays_switch_off));
}

// returns 1 if relay is currently off and switched on, else returns 0
byte relay_switch_on(byte idx)
{
  if (!relay_state(idx)) {
    digitalWrite(RELAY_PINS_USED[idx], HIGH);
#if DEBUG
    debug(F("relay on"));
#endif
    prog_buffer[0] = '\0';
    strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(STATUS_TOPICS[3])));
    char str[2];
    mqtt_client.publish(prog_buffer, itoa(idx+1, str, 10));
#if USE_MASTER_RELAY
    master_relay_on();
#endif
#if USE_LED
    digitalWrite(LED_PIN, HIGH);
#endif
    return 1;
  }
  return 0;
}

byte relay_switch_on_with_timer(byte idx, int duration)
{
  if (relay_switch_on(idx)) {
    current_timer_ref = Alarm.timerOnce(duration * SECS_PER_MIN, relays_switch_off);
    publish_alarm_id(current_timer_ref);
    return 1;
  }
  return 0;
}

// define void functions for each irrigation zone required as alarm callbacks
void turn_on_relay_1()
{
  byte idx = 0;
  relay_switch_on_with_timer(idx, relay_durations[idx]);
}

void turn_on_relay_2()
{
  byte idx = 1;
  relay_switch_on_with_timer(idx, relay_durations[idx]);
}

void turn_on_relay_3()
{
  byte idx = 2;
  relay_switch_on_with_timer(idx, relay_durations[idx]);
}

void turn_on_relay_4()
{
  byte idx = 3;
  relay_switch_on_with_timer(idx, relay_durations[idx]);
}


void callback(char* topic, uint8_t* payload, unsigned int payload_length)
{
  // handle message arrived
  /* topic = part of the variable header:has topic name of the topic where the publish received
   	 * NOTE: variable header does not contain the 2 bytes with the 
   	 *       publish msg ID
   	 * payload = pointer to the first item of the buffer array that
   	 *           contains the message tha was published
   	 *          EXAMPLE of payload: lights,1
   	 * length = the length of the payload, until which index of payload
   	 */

#if DEBUG
  debug(F("Payload length is"));
  Serial.println(payload_length);
#endif

  // Copy the payload to the new buffer
  char* message = (char*)malloc((sizeof(char)*payload_length)+1); // get the size of the bytes and store in memory 
  memcpy(message, payload, payload_length*sizeof(char));          // copy the memory
  message[payload_length*sizeof(char)] = '\0';                    // add terminating character

#if DEBUG
  debug(F("Message with topic"));
  Serial.println(topic);
  debug(F("arrived with payload"));
  Serial.println(message);
#endif

  byte topic_idx = 0;
  // find if topic is matched
  for (byte i = 0; i < ARRAY_SIZE(CONTROL_TOPICS); i++) {
    prog_buffer[0] = '\0';
    strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(CONTROL_TOPICS[i])));
    if (strcmp(topic, prog_buffer) == 0) {
      topic_idx = i;
      break;
    }
  }
#if DEBUG
  debug(F("Control topic index"));
  Serial.println(topic_idx);
#endif

// topic is dst_set
  if (topic_idx == 0) {
    byte integer = atoi(message);    // parse to int (will return 0 if not a valid int)
    if (integer == 1 && !daylight_summer_time) {
      adjustTime(SECS_PER_HOUR);     // move time forward one hour
      daylight_summer_time = true;
    } else if (integer == 0 && daylight_summer_time) { 
      adjustTime(-SECS_PER_HOUR);    // move time backward one hour
      daylight_summer_time = false;
    }
    publish_date();
#if DEBUG
    debug(F("dst set topic arrived"));
    date_string();
    Serial.println(char_buffer);
#endif
  } else if (topic_idx == 1) {  // topic is time_request
    publish_date();
#if DEBUG
    debug(F("time request topic arrived"));
    date_string();
    Serial.println(char_buffer);
#endif
  } else if (topic_idx == 2) {  // topic is state_request
    relays_state();
  } else if (topic_idx == 3) {  // topic is duration_request
    get_relay_durations();
  } else if (topic_idx == 4) {  // topic is timer_stop
    timer_stop();
  } else if (topic_idx == 5) {  // topic is alarms_control
    // enable or disable alarms
    int integer = atoi(message);  //parse to int will return 0 if fails
    if (integer == 0)
      disable_alarms();
    else
      enable_alarms();
  } else if (topic_idx == 6) {  // topic is relay_control
    // message should be of format relay,duration
    char *separated_message = strchr(message, COMMAND_SEPARATOR);
    // separated_message is of format ",duration"
    if (separated_message != 0) {
      byte relay_ref = atoi(message)-1;
      ++separated_message;
      int duration = atoi(separated_message);
    
      if (duration == 0 ) {
        if (relay_ref == 0 )
          turn_on_relay_1();
        else if (relay_ref == 1)
          turn_on_relay_2();
        else if (relay_ref == 2)
          turn_on_relay_3();
        else if (relay_ref == 3)
          turn_on_relay_4();
      } else { 
        relay_switch_on_with_timer(relay_ref, duration);
      }
    }  
  } else if (topic_idx == 7) {  // topic is duration_control
    // message should be of format relay,duration
    char *separated_message = strchr(message, COMMAND_SEPARATOR);
    // separated_message is of format ",duration"
    if (separated_message != 0) {
      byte relay_ref = atoi(message)-1;
      ++separated_message;
      int duration = atoi(separated_message);
      if (duration > 0)
        relay_durations[relay_ref] = duration;
      get_relay_durations();
    }
  } else {  // unknown topic has arrived - ignore!!
#if DEBUG
    debug(F("Unknown topic arrived"));
#endif
  }

  // free memory assigned to message
  free(message);
}

void wifly_connect()
{
#if DEBUG
  debug(F("initialising wifly"));
#endif

  WiFly.begin();
  delay(5000);  // allow time to WiFly to initialise

#if DEBUG
  debug(F("joining network"));
#endif

//  if (!WiFly.join(MY_SSID, MY_PASSPHRASE, mode)) {
  if (!WiFly.join(MY_SSID)) {   // needs to be fixed to allow a passphrase if secure
    wifly_connected = false;
#if DEBUG
    debug(F("  failed"));
#endif
    delay(AFTER_ERROR_DELAY);
  } else {
    wifly_connected = true;
#if DEBUG
    debug(F("  connected"));
#endif
  }
}

void mqtt_connect()
{
  if (!wifly_connected)
    wifly_connect();

  if (wifly_connected) {
    // MQTT client setup
    //    mqttClient.disconnect();
#if DEBUG
    debug(F("connecting to broker"));
#endif
    if (mqtt_client.connect(mqtt_client_id)) {
#if DEBUG
      debug(F("  connected"));
#endif
      publish_connected();
      publish_date();
#if USE_FREEMEM
      publish_memory();
#endif

      // subscribe to topics 
      mqtt_client.subscribe("all/#");
      mqtt_client.subscribe("relayduino/control/#");
      mqtt_client.subscribe("relayduino/request/#");
    } else {
#if DEBUG
      debug(F("  failed"));
#endif
      delay(AFTER_ERROR_DELAY);
    }
  }
}

void reset_connection()
{
  if (mqtt_client.connected())
    mqtt_client.disconnect();
  wifly_connect();
  mqtt_connect();
}

void time_set()
{
  if (!wifly_connected)
    wifly_connect();

  if (wifly_connected) {
#if DEBUG
    debug(F("setting time"));
#endif
    // set time on Arduino
    setTime(WiFly.getTime());
    // adjust time to timezone
    adjustTime(TZ_OFFSET_HOURS * SECS_PER_HOUR + TZ_OFFSET_MINUTES * SECS_PER_MIN);
  }
}


/*--------------------------------------------------------------------------------------
 setup()
 Called by the Arduino framework once, before the main loop begins
 --------------------------------------------------------------------------------------*/
void setup()
{
  // configure relay pins as outputs and set to LOW
#if USE_MASTER_RELAY  
  pinMode(RELAY_MASTER, OUTPUT);
  digitalWrite(RELAY_MASTER, LOW);
#endif
  for (byte idx = 0; idx < ARRAY_SIZE(RELAY_PINS_USED); idx++) {
    pinMode(RELAY_PINS_USED[idx], OUTPUT);
    digitalWrite(RELAY_PINS_USED[idx], LOW);
  }
  
#if USE_LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
#endif

#if USE_HARDWARE_WATCHDOG
    ResetWatchdog1();
#endif
  
  // Configure WiFly
  Serial.begin(BAUD_RATE);

  wifly_serial.begin(BAUD_RATE);
  WiFly.setUart(&wifly_serial);
  
  wifly_connect();
  
#if USE_HARDWARE_WATCHDOG
    ResetWatchdog1();
#endif
  
  if (timeStatus() != timeSet)
    time_set();

#if USE_HARDWARE_WATCHDOG
    ResetWatchdog1();
#endif
  
#if DEBUG
  Serial.println(WiFly.ip());
  //  Serial.println(WiFly.getMAC());
  date_string();
  Serial.println(char_buffer);
  debug(F("Number of relays is "));
  Serial.println(ARRAY_SIZE(RELAY_PINS_USED));
#endif

  // Define hourly 'I am here' alarm
  Alarm.timerRepeat(60*60, publish_connected);
  
  // reset connection to client every three hours
  Alarm.timerRepeat(3*60*60, reset_connection);

  // Define default irrigation alarms
  // start times are fixed and days are fixed
  // durations are defined by relayDurations[]

  // Irrigation Zone 1: Front grass
  // Set to Sunday and Wednesday evenings @ 1800
  alarm_refs[alarm_refs_cnt] = Alarm.alarmRepeat(dowSunday, 18, 0, 0, turn_on_relay_1);
  alarm_refs_cnt++;
  alarm_refs[alarm_refs_cnt] = Alarm.alarmRepeat(dowWednesday, 18, 0, 0, turn_on_relay_1);
  alarm_refs_cnt++;
    
  // Irrigation Zone 2: Flower beds
  // Set to Sunday and Wednesday evenings @ 1900
  alarm_refs[alarm_refs_cnt] = Alarm.alarmRepeat(dowSunday, 19, 0, 0, turn_on_relay_2);
  alarm_refs_cnt++;
  alarm_refs[alarm_refs_cnt] = Alarm.alarmRepeat(dowWednesday, 19, 0, 0, turn_on_relay_2);
  alarm_refs_cnt++;
  
  // Irrigation Zone 3: Fruit trees
  // Set to Saturday morning @ 0730
  alarm_refs[alarm_refs_cnt] = Alarm.alarmRepeat(dowSaturday, 7, 30, 0, turn_on_relay_3);
  alarm_refs_cnt++;

  // Relay 4: Vegetable beds
  // Set to everyday @ 0700
  alarm_refs[alarm_refs_cnt] = Alarm.alarmRepeat(7, 0, 0, turn_on_relay_4); //alarm_refs_cnt++;  

#if USE_HARDWARE_WATCHDOG
    ResetWatchdog1();
#endif
}


/*--------------------------------------------------------------------------------------
 loop()
 Arduino main loop
 --------------------------------------------------------------------------------------*/
void loop()
{
  // require an Alarm.delay in order to allow alarms to work
  Alarm.delay(0);

  // require a client.loop in order to receive subscriptions
  //  mqttClient.loop();

  if (!mqtt_client.loop()) {
//    wifly_connect();
    mqtt_connect();
  }

  #if USE_HARDWARE_WATCHDOG
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= watchdog_interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    ResetWatchdog1();
  }
  #endif
}


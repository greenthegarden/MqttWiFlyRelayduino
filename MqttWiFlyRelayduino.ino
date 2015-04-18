/* RN-XV WiFly Module - Wire Antenna
 MAC: 00:06:66:50:71:6f
 IP: 192.168.1.52
 
 RN-XV WiFly Module – SMA
 MAC: 00:06:66:71:68:d5
 IP: 192.168.1.51
 */

// Note itoa function details
// char* itoa (	int val, char *buf, int radix)
// where radix is the number base, ie. 10

/* WiFly configuration
reboot	
$$$	
factory RESET	
	
set wlan join 0    // Stop device connecting while we setup

set ip dhcp 3
set wlan ssid xxx
set wlan phrase xxx
set wlan join 1

set time address 203.0.178.191
set time zone 0
set time enable 1

save
reboot
*/


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


#if USE_WATCHDOG
// see the following for information about watchdog timer
// http://forum.arduino.cc/index.php?topic=63651.0
#include <avr/wdt.h>

void watchdogSetup(void)
{
#if 0
  noInterrupts();  // disable interupts to ensure configuration is fully processed
  wdt_reset();

  /*
   WDTCSR configuration:
   WDIE = 1: Interrupt Enable
   WDE = 1 : Reset Enable
   See table for time-out variations:
   WDP3 = 0 : For 2000ms Time-out
   WDP2 = 1 : For 2000ms Time-out
   WDP1 = 1 : For 2000ms Time-out
   WDP0 = 1 : For 2000ms Time-out
  */
  // Enter Watchdog Configuration mode:
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  
  // Set Watchdog settings:
  WDTCSR = (1<<WDIE) | (1<<WDE) | (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);

  interrupts();  // re-enable interrupts
#else
  // alternative method
  // see https://tushev.org/articles/arduino/item/46-arduino-and-watchdog-timer
  /*
   Threshold value	Constant name	Supported on
   15 ms	        WDTO_15MS	ATMega 8, 168, 328, 1280, 2560
   30 ms	        WDTO_30MS	ATMega 8, 168, 328, 1280, 2560
   60 ms	        WDTO_60MS	ATMega 8, 168, 328, 1280, 2560
   120 ms	        WDTO_120MS	ATMega 8, 168, 328, 1280, 2560
   250 ms	        WDTO_250MS	ATMega 8, 168, 328, 1280, 2560
   500 ms	        WDTO_500MS	ATMega 8, 168, 328, 1280, 2560
   1 s	                WDTO_1S	        ATMega 8, 168, 328, 1280, 2560
   2 s	                WDTO_2S	        ATMega 8, 168, 328, 1280, 2560
   4 s	                WDTO_4S	        ATMega 168, 328, 1280, 2560
   8 s	                WDTO_8S	        ATMega 168, 328, 1280, 2560
  */
  wdt_enable(WDT_TIME);
#endif
}
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
void dateString()
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
   if (DST)
     strcat(char_buffer, " DST");
}

// write ip address to char_buffer in format
// 0000000000111111111
// 0123456789012345678
// xxx.xxx.xxx.xxx
//void ipString()
//{
//  char_buffer[0] = '\0';
//  strcat(char_buffer, WiFly.ip());
//}


#include <TimeAlarms.h>
//const unsigned int TIMER_INTERVAL_MINS = 5;  // measurement interval in minutes
byte currentTimerRef = 255;


// callback function definition required here as client needs to be defined before
// including relay.h
void callback(char* topic, uint8_t* payload, unsigned int length);


WiFlyClient wiflyClient;
SoftwareSerial wiflySerial(wiflySerialRx, wiflySerialTx);
PubSubClient mqttClient(mqtt_server_addr, mqtt_port, callback, wiflyClient);

void publishConnected()
{
  prog_buffer[0] = '\0';
  strcpy_P(prog_buffer, (char*)pgm_read_word(&(status_topics[0])));
  mqttClient.publish(prog_buffer, "Connected");
}

//void publishIp()
//{
//   mqttClient.publish(wifly_topic, "Local IP:");
//   ipToBuf();
//   mqttClient.publish(wifly_topic, char_buffer);
//}

void publishDate()
{
  prog_buffer[0] = '\0';
  strcpy_P(prog_buffer, (char*)pgm_read_word(&(status_topics[1])));
  dateString();  // date is stored in char_buffer
  mqttClient.publish(prog_buffer, char_buffer);
}

void publishAlarmId(byte ref = 255)
{
  if ( ref == 255 )
  {
    ref = Alarm.getTriggeredAlarmId();
  }
  prog_buffer[0] = '\0';
  strcpy_P(prog_buffer, (char*)pgm_read_word(&(status_topics[5])));
  char str[4];
  mqttClient.publish(prog_buffer, itoa(ref, str, 10));
}

void publishAlarms(bool enabled)
{
  prog_buffer[0] = '\0';
  strcpy_P(prog_buffer, (char*)pgm_read_word(&(status_topics[6])));
  if ( enabled )
    mqttClient.publish(prog_buffer, "enabled");
  else
    mqttClient.publish(prog_buffer, "disabled");
}

#if USE_FREEMEM
void publishMemory()
{
  prog_buffer[0] = '\0';
  strcpy_P(prog_buffer, (char*)pgm_read_word(&(status_topics[2])));
  char_buffer[0] = '\0';
  itoa(freeMemory(), char_buffer, 10);
  mqttClient.publish(prog_buffer, char_buffer);
}
#endif

void disableAlarms()
{
  for ( byte idx=0; idx<sizeof(alarm_refs); idx++ )
  {
    Alarm.disable(alarm_refs[idx]);
  }
  publishAlarms(false);
}

void enableAlarms()
{
  for ( byte idx=0; idx<alarm_refs_cnt; idx++ )
  {
    Alarm.enable(alarm_refs[idx]);
  }
  publishAlarms(true);
}


#include "relayduinoConfig.h"

const byte relayPinsUsed[]  = {RELAY_1, RELAY_2, RELAY_3, RELAY_4};
int        relayDurations[] = {R1_DURATION, R2_DURATION, R3_DURATION, R4_DURATION};

void getRelayDurations()
{
  //sizeof(myInts)/sizeof(int)
  for ( byte idx=0; idx<sizeof(relayDurations)/sizeof(int); idx++ )
  {
    prog_buffer[0] = '\0';
    strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(status_topics[7])));
    char str[4];
    mqttClient.publish(prog_buffer, itoa(relayDurations[idx], str, 10));
  }
}

#if USE_MASTER_RELAY
// returns 1 if relay is currently on and switched off, else returns 0
byte masterRelayOff()
{
  // only switch relay off if it is currently on
  if (digitalRead(RELAY_MASTER))
  {
    digitalWrite(RELAY_MASTER, LOW);
#if DEBUG
    debug(F("master off"));
#endif
    prog_buffer[0] = '\0';
    strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(status_topics[4])));
    mqttClient.publish(prog_buffer, "M");
    return 1;
  }
  return 0;
}

// returns 1 if relay is currently on and switched off, else returns 0
byte masterRelayOn()
{
  // only switch relay on if it is currently off
  if(!digitalRead(RELAY_MASTER))
  {
    digitalWrite(RELAY_MASTER, HIGH);
#if DEBUG
    debug(F("master on"));
#endif
    prog_buffer[0] = '\0';
    strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(status_topics[3])));
    mqttClient.publish(prog_buffer, "M");
    return 1;
  }
  return 0;
}
#endif

// returns 1 if relay connected to given pin is on, else returns 0
byte relayState(byte idx)
{
  return(digitalRead(relayPinsUsed[idx]));
}

void relaysState()
{
  for ( byte idx=0; idx<sizeof(relayPinsUsed); idx++ )
  {
    if ( digitalRead(relayPinsUsed[idx]) )
    {
#if DEBUG
      debug(F("relay on"));
#endif
      prog_buffer[0] = '\0';
      strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(status_topics[3])));
      char str[2];
      mqttClient.publish(prog_buffer, itoa(idx+1, str, 10));
    }
    else
    {
#if DEBUG
      debug(F("relay off"));
#endif
      prog_buffer[0] = '\0';
      strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(status_topics[4])));
      char str[2];
      mqttClient.publish(prog_buffer, itoa(idx+1, str, 10));
    }
  }
}

// returns 1 if relay is currently on and switched off, else returns 0
byte relaySwitchOff(byte idx)
{
  // only switch relay off if it is currently on
  if (relayState(idx))
  {
    digitalWrite(relayPinsUsed[idx], LOW);
#if DEBUG
    debug(F("relay off"));
#endif
    prog_buffer[0] = '\0';
    strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(status_topics[4])));
    char str[2];
    mqttClient.publish(prog_buffer, itoa(idx+1, str, 10));
#if USE_MASTER_RELAY
    masterRelayOff();
#endif
#if USE_LED
    digitalWrite(ledPin, LOW);
#endif
    return 1;
  }
  return 0;
}

// used by callback as a void function to switch off relay which is currenlty on
// and then switches off master
void relaysSwitchOff()
{
  for ( byte idx=0; idx<sizeof(relayPinsUsed); idx++ )
  {
    if (relayState(idx))
    {
      relaySwitchOff(idx);
    }
  }
}

byte timerStop()
{
  return (Alarm.timerOnce(1, relaysSwitchOff));
}

// returns 1 if relay is currently off and switched on, else returns 0
byte relaySwitchOn(byte idx)
{
  if(!relayState(idx))
  {
  digitalWrite(relayPinsUsed[idx], HIGH);
#if DEBUG
    debug(F("relay on"));
#endif
    prog_buffer[0] = '\0';
    strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(status_topics[3])));
    char str[2];
    mqttClient.publish(prog_buffer, itoa(idx+1, str, 10));
#if USE_MASTER_RELAY
    masterRelayOn();
#endif
#if USE_LED
    digitalWrite(ledPin, HIGH);
#endif
    return 1;
  }
  return 0;
}

//byte relayAndMasterSwitchOn(int pin)
//{
//  if ( relaySwitchOn(pin) )
//  {
//    if ( masterRelayOn() )
//      return 1;
//  }
//  return 0;
//}

byte relaySwitchOnWithTimer(byte idx, int duration)
{
  if (relaySwitchOn(idx))
  {
    currentTimerRef = Alarm.timerOnce(duration * SECS_PER_MIN, relaysSwitchOff);
    publishAlarmId(currentTimerRef);
    return 1;
  }
  return 0;
}

//byte relayAndMasterSwitchOnWithTimer(byte idx, int duration)
//{
//  if (relayAndMasterSwitchOn(idx))
//    return ( Alarm.timerOnce(duration * SECS_PER_MIN, relaysSwitchOff) );
//}

// define void functions for each irrigation zone required as alarm callbacks
void turnOnRelay1()
{
  byte idx = 0;
  relaySwitchOnWithTimer(idx, relayDurations[idx]);
//  relayAndMasterSwitchOnWithTimer(tr1.pin, tr1.duration);
}

void turnOnRelay2()
{
  byte idx = 1;
  relaySwitchOnWithTimer(idx, relayDurations[idx]);
//  relayAndMasterSwitchOnWithTimer(tr2.pin, tr2.duration);
}

void turnOnRelay3()
{
  byte idx = 2;
  relaySwitchOnWithTimer(idx, relayDurations[idx]);
//  relayAndMasterSwitchOnWithTimer(tr3.pin, tr3.duration);
}

void turnOnRelay4()
{
  byte idx = 3;
  relaySwitchOnWithTimer(idx, relayDurations[idx]);
//  relayAndMasterSwitchOnWithTimer(tr4.pin, tr4.duration);
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

 // strcpy(message, (char *) payload);
  
#if DEBUG
  debug(F("Message with topic"));
  Serial.println(topic);
  debug(F("arrived with payload"));
  Serial.println(message);
#endif

  byte topic_idx = 0;
  // find if topic is matched
  for ( byte i = 0; i < sizeof(control_topics); i++ )
  {
    prog_buffer[0] = '\0';
    strcpy_P(prog_buffer, (PGM_P)pgm_read_word(&(control_topics[i])));
    if (strcmp(topic, prog_buffer) == 0)
    {
      topic_idx = i;
      break;
    }
  }
#if DEBUG
  debug(F("Control topic index"));
  Serial.println(topic_idx);
#endif

//  if (strcmp(topic, prog_buffer) == 0)
// topic is dst_set
  if (topic_idx == 0)
  {
    byte rMsg = atoi(message); // parse to int (will return 0 if not a valid int)
    if ( rMsg == 1 && !DST)
    {
      adjustTime(SECS_PER_HOUR); 
      DST = true;
    }  // move forward one hour
    else if ( rMsg == 0 && DST)
    { 
      adjustTime(-SECS_PER_HOUR);
      DST = false;
    }  // move back one hour
    publishDate();
#if DEBUG
    debug(F("dst set topic arrived"));
    dateString();
    Serial.println(char_buffer);
#endif
  }
// topic is time_request
  else if (topic_idx == 1)
  {
    publishDate();
#if DEBUG
    debug(F("time request topic arrived"));
    dateString();
    Serial.println(char_buffer);
#endif
  }
// topic is state_request
  else if (topic_idx == 2)
  {
    relaysState();
  }
  else if (topic_idx == 3)
  // topic is duration_request
  {
    getRelayDurations();
  }
  else if (topic_idx == 4)
  // topic is timer_stop
  {
    timerStop();
  }
  else if (topic_idx == 5)
  // topic is alarms_control
  {
    // enable or disable alarms
    int rMsg = atoi(message);             //parse to int will return 0 if fails
    if ( rMsg == 0 ) { 
      disableAlarms();
    }
    else { 
      enableAlarms();
    }   
  }
  else if (topic_idx == 6)
  // topic is relay_control
  {
    // message should be of format relay,duration
    char *separated_message = strchr(message, COMMAND_SEPARATOR);
    // separated_message is of format ",duration"
    if (separated_message != 0)
    {
      byte relay_ref = atoi(message)-1;
      ++separated_message;
      int duration = atoi(separated_message);
    
      if ( duration == 0 )
      {
        if ( relay_ref == 0 )
          turnOnRelay1();
        else if ( relay_ref == 1 )
          turnOnRelay2();
        else if ( relay_ref == 2 )
          turnOnRelay3();
        else if ( relay_ref == 3 )
          turnOnRelay4();
      }
      else { 
        relaySwitchOnWithTimer(relay_ref, duration);
      }
    }  
  }
  else if (topic_idx == 7)
  // topic is duration_control
  {
    // message should be of format relay,duration
    char *separated_message = strchr(message, COMMAND_SEPARATOR);
    // separated_message is of format ",duration"
    if (separated_message != 0)
    {
      byte relay_ref = atoi(message)-1;
      ++separated_message;
      int duration = atoi(separated_message);
    
      if ( duration > 0 )
      {
        relayDurations[relay_ref] = duration;
      }
      getRelayDurations();
    }
  }
  else
  // unknown topic has arrived - ignore!!
  {
#if DEBUG
    debug(F("Unknown topic arrived"));
#endif
  }

  // ensure memory assigned to message is freed
  free(message);
}

void connect_wifly()
{
#if USE_WATCHDOG
  wdt_disable();
#endif

#if DEBUG
  debug(F("Initialising"));
#endif

  WiFly.begin();

#if DEBUG
  debug(F("Joining"));
#endif

  if (!WiFly.join(ssid, passphrase, mode))
  {
    wifly_connected = false;
#if DEBUG
    debug(F("Failed"));
#endif
    delay(AFTER_ERROR_DELAY);
  } 
  else {
    wifly_connected = true;
  }

#if USE_WATCHDOG
#if DEBUG 
  debug(F("Setting watchdog"));
#endif
  watchdogSetup();
#endif
}

void connect_mqtt()
{
  if (!wifly_connected)
    connect_wifly();

  if (wifly_connected)
  {
    // MQTT client setup
    //    mqttClient.disconnect();
#if DEBUG
    debug(F("Connecting"));
#endif
    if (mqttClient.connect(mqtt_client_id))
    {
#if DEBUG
      debug(F("Connected"));
#endif
      publishConnected();
//      publishIp();
      publishDate();
#if USE_FREEMEM
      publishMemory();
#endif

      // subscribe to topics 
      mqttClient.subscribe("all/#");
      mqttClient.subscribe("relayduino/control/#");
      mqttClient.subscribe("relayduino/request/#");
    } 
    else
    {
#if DEBUG
      debug(F("Failed"));
#endif
      delay(AFTER_ERROR_DELAY);
    }
  }
}

void set_time()
{
  if (!wifly_connected)
    connect_wifly();

  if (wifly_connected)
  {
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
#if USE_WATCHDOG
  wdt_disable();  // ensure watchdog timer is disabled during initial connection
#endif

  // configure relay pins as outputs and set to LOW
#if USE_MASTER_RELAY  
  pinMode(RELAY_MASTER, OUTPUT);
  digitalWrite(RELAY_MASTER, LOW);
#endif
  for ( byte idx=0; idx<sizeof(relayPinsUsed); idx++ )
  {
    pinMode(relayPinsUsed[idx], OUTPUT);
    digitalWrite(relayPinsUsed[idx], LOW);
  }
  
  // Configure WiFly
  Serial.begin(BAUD_RATE);

#if USE_LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
#endif
  
#if DEBUG
  debug(F("Setup"));
#endif

  wiflySerial.begin(BAUD_RATE);
  WiFly.setUart(&wiflySerial);

  if (timeStatus() != timeSet)
    set_time();

#if DEBUG
  Serial.println(WiFly.ip());
  //  Serial.println(WiFly.getMAC());
  dateString();
  Serial.println(char_buffer);
#endif

#if DEBUG
  Serial.print("Number of relays is ");
  Serial.println(sizeof(relayPinsUsed));
#endif

  // Define hourly 'I am here' alarm
  Alarm.timerRepeat(60*60, publishConnected);

  // Define default irrigation alarms
  // start times are fixed and days are fixed
  // durations are defined by relayDurations[]

  // Irrigation Zone 1: Front grass
  // Set to Sunday and Wednesday evenings @ 1800
  alarm_refs[alarm_refs_cnt] = Alarm.alarmRepeat(dowSunday,18,0,0,turnOnRelay1); alarm_refs_cnt++;
  alarm_refs[alarm_refs_cnt] = Alarm.alarmRepeat(dowWednesday,18,0,0,turnOnRelay1); alarm_refs_cnt++;
    
  // Irrigation Zone 2: Flower beds
  // Set to Sunday and Wednesday evenings @ 1900
  alarm_refs[alarm_refs_cnt] = Alarm.alarmRepeat(dowSunday,19,0,0,turnOnRelay2); alarm_refs_cnt++;
  alarm_refs[alarm_refs_cnt] = Alarm.alarmRepeat(dowWednesday,19,0,0,turnOnRelay2); alarm_refs_cnt++;
  
  // Irrigation Zone 3: Fruit trees
  // Set to Saturday morning @ 0730
  alarm_refs[alarm_refs_cnt] = Alarm.alarmRepeat(dowSaturday,7,30,0,turnOnRelay3); alarm_refs_cnt++;

  // Relay 4: Vegetable beds
  // Set to everyday @ 0700
  alarm_refs[alarm_refs_cnt] = Alarm.alarmRepeat(7,0,0,turnOnRelay4); //alarm_refs_cnt++;
  
//  byte alarm_refs[] = { Alarm.alarmRepeat(dowSunday,18,0,0,turnOnRelay1),
//                        Alarm.alarmRepeat(dowWednesday,18,0,0,turnOnRelay1),
//                      };
  
#if USE_WATCHDOG
  // configure watchdog timer  
  watchdogSetup();
#endif

}


/*--------------------------------------------------------------------------------------
 loop()
 Arduino main loop
 --------------------------------------------------------------------------------------*/
void loop()
{
#if USE_WATCHDOG  
  wdt_disable();
#endif

  // require an Alarm.delay in order to allow alarms to work
  Alarm.delay(0);

  // require a client.loop in order to receive subscriptions
  //  mqttClient.loop();

  if (!mqttClient.loop())
  {
    connect_mqtt();
  }

#if USE_WATCHDOG  
  watchdogSetup();
#endif

#if USE_WATCHDOG
  // reset watchdog timer
  wdt_reset();
#endif
}

#if USE_WATCHDOG
ISR(WDT_vect)
{
// Ensure all relays are off if reset will occur
  for (byte idx=0; idx<sizeof(relayPinsUsed); idx++)
  {
    digitalWrite(relayPinsUsed[idx], LOW);
  }
}
#endif


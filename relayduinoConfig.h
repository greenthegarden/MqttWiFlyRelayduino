#ifndef MQTTWIFLYRELAYDUINO_RELAYDUINO_CONFIG_H_
#define MQTTWIFLYRELAYDUINO_RELAYDUINO_CONFIG_H_


// pin definitions for RelayDuino
#define ANALOG_IN_1 A6  // Analog 1 is connected to Arduino Analog In 6
#define ANALOG_IN_2 A7  // Analog 2 is connected to Arduino Analog In 7
#define ANALOG_IN_3 A0  // Analog 3 is connected to Arduino Analog In 0

//int Analogs[3] = { ANALOG_IN_1, ANALOG_IN_2, ANALOG_IN_3 };

#define RELAY_1      2  // Relay 1 is connected to Arduino Digital 2
#define RELAY_2      3  // Relay 2 is connected to Arduino Digital 3 PWM
#define RELAY_3      4  // Relay 3 is connected to Arduino Digital 4
#define RELAY_4      5  // Relay 4 is connected to Arduino Digital 5 PWM
#define RELAY_5      6  // Relay 5 is connected to Arduino Digital 6 PWM
#define RELAY_6      7  // Relay 6 is connected to Arduino Digital 7
#define RELAY_7      8  // Relay 7 is connected to Arduino Digital 8
#define RELAY_MASTER 9  // Relay 8 is connected to Arduino Digital 9 PWM

//int Relays[8] = { RELAY_1, RELAY_2, RELAY_3, RELAY_4, RELAY_5, RELAY_6, RELAY_7, RELAY_MASTER };
  
//int PWM_PINS[4] = { 2, 4, 5, 8 };

#define OPTO_INPUT_1 15 // Opto-Isolated Input 1 is connected to Arduino Analog 1 which is Digital 15
#define OPTO_INPUT_2 16 // Opto-Isolated Input 2 is connected to Arduino Analog 2 which is Digital 16
#define OPTO_INPUT_3 17 // Opto-Isolated Input 3 is connected to Arduino Analog 3 which is Digital 17
#define OPTO_INPUT_4 18 // Opto-Isolated Input 4 is connected to Arduino Analog 4 which is Digital 18

//int Optos[4] = { OPTO_INPUT_1, OPTO_INPUT_2, OPTO_INPUT_3, OPTO_INPUT_4 };


#endif  /* MQTTWIFLYRELAYDUINO_RELAYDUINO_CONFIG_H_ */

#if !defined(_GLOBALS_)
#define _GLOBALS_

#define PIN_BUTTON 13
#define PIN_NEOPIX 15
#define PIN_BATTERY 36       // Used to read the battery level

#define BATTERY_MIN_VAL 1900 // ~3.3v
#define BATTERY_MAX_VAL 2500 // ~4.2v

const char *FILE_SSID = "/ssid.txt";
const char *FILE_PASSWORD = "/pass.txt";
const char *FILE_IP = "/ip.txt";
const char *FILE_IP_OUT = "/outip.txt";
const char *FILE_GATEWAY = "/gateway.txt";

#endif
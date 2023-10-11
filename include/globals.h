#if !defined(_GLOBALS_)
#define _GLOBALS_

const char *FILE_SSID = "/ssid.txt";
const char *FILE_PASSWORD = "/pass.txt";
const char *FILE_IP = "/ip.txt";
const char *FILE_IP_OUT = "/outip.txt";
const char *FILE_GATEWAY = "/gateway.txt";

// Wifi config > change WifiManager static variables organization (move to globals.h?)
String MOVUINO_WIFI_SSID = "";
String MOVUINO_WIFI_PASS = "";
String MOVUINO_WIFI_IP = "";
String MOVUINO_OSC_OUTIP = "";
/*
    + MOVUINO_OSC_PORT_IN
    + MOVUINO_OSC_PORT_OUT
*/

#define PIN_BUTTON 13
#define PIN_NEOPIX 15
#define PIN_BATTERY 36       // Used to read the battery level

#define BATTERY_MIN_VAL 1900 // ~3.3v
#define BATTERY_MAX_VAL 2500 // ~4.2v

#define WHITE255 ((255 << 16) | (255 << 8) | 255)
#define RED ((255 << 16) | (0 << 8) | 0)
#define GREEN ((0 << 16) | (250 << 8) | 0)
#define YELLOW ((200 << 16) | (175 << 8) | 0)
#define BLUE ((0 << 16) | (0 << 8) | 255)
#define MAGENTA ((255 << 16) | (0 << 8) | 255)

#endif
#if !defined(_ESP32_WIFI_)
#define _ESP32_WIFI_

#include <WiFi.h>

const IPAddress defaultIP;
const int delayConnection = 10000; // delay to wait for Wi-Fi connection (milliseconds)

// Mode: Wifi Station
// static variable for utily function (start / stop / get / etc.)
static String ssid;                            // name of the network to connect
static String password;                        // password of the network to connect
static IPAddress localIP;                      // ESP32 ip address (default = 0.0.0.0)
static IPAddress localGateway(192, 168, 1, 1); // ESP32 gateway
static IPAddress subnet(255, 255, 255, 0);

// Mode: Access Point
static String apSSID = "MOVUINO";
static char *apPassword = NULL;
static IPAddress apLocalIP;
static IPAddress apLocalGateway(192, 168, 0, 1);
static IPAddress apSubnet(255, 255, 255, 0);

static bool connectWiFi(String ssid_ = "", String password_ = "", String ip_ = "", String gateway_ = "")
{
    // Update config
    ssid = ssid_ != "" ? ssid_ : ssid;
    password = password_ != "" ? password_ : password;
    if (ip_ != "")
        localIP.fromString(ip_.c_str());
    if (gateway_ != "")
        localGateway.fromString(gateway_.c_str());

    // Basic check
    if (ssid == "")
    {
        Serial.println("Undefined SSID or IP address.");
        return false;
    }

    // Start connection
    WiFi.mode(WIFI_STA);
    if (localIP != IPAddress())
    {
        if (!WiFi.config(localIP, localGateway, subnet))
        {
            Serial.println("STA Failed to configure");
            return false;
        }
    }
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.println("Connecting to WiFi...");

    // Wait for result
    unsigned long time0_ = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - time0_ >= delayConnection)
        {
            Serial.println("Failed to connect.");
            return false;
        }
    }

    Serial.print("WiFi.localIP() = ");
    Serial.println(WiFi.localIP());
    return true;
}

static bool isWifiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

static bool startAccessPoint(String ssid_ = "", String password_ = "", String ip_ = "")
{
    // Update config
    apSSID = ssid_ != "" ? ssid_ : apSSID;
    password = password_ != "" ? password_ : password;
    if (ip_ != "")
        apLocalIP.fromString(ip_.c_str());

    // Init Access point
    WiFi.mode(WIFI_AP);
    if (apLocalIP != IPAddress())
    {
        if (!WiFi.softAPConfig(apLocalIP, apLocalGateway, apSubnet))
        {
            Serial.println("AP Failed to configure");
            return false;
        }
    }

    // Config (optionnal)
    int channel_ = 10;      // WiFi Channel number between 1 and 13: https://fr.wikipedia.org/wiki/Liste_des_canaux_Wi-Fi
    bool hideSSID_ = false; // To disable SSID broadcast -> SSID will not appear in a basic WiFi scan
    int maxConnection_ = 2; // Maximum simultaneous connected clients on the AP

    // Start AP
    WiFi.softAP(apSSID, apPassword, channel_, hideSSID_, maxConnection_);
    Serial.println("---------------");
    Serial.print("Access router: ");
    Serial.println(apSSID);
    Serial.print("Access Point IP address: ");
    Serial.println(WiFi.softAPIP());
    Serial.println("---------------");
    return true;
}

static IPAddress getIP() {
    if(WiFi.getMode() == WIFI_STA)
        return WiFi.localIP();
    if(WiFi.getMode() == WIFI_AP)
        return WiFi.softAPIP();
    return IPAddress();
}

// void display_connected_devices()
// {
//   wifi_sta_list_t wifi_sta_list;
//   tcpip_adapter_sta_list_t adapter_sta_list;
// //   esp_wifi_ap_get_sta_list(&wifi_sta_list);
//   tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);

//   Serial.println("-----------");
//   if (adapter_sta_list.num > 0)
//   {
//     for (uint8_t i = 0; i < adapter_sta_list.num; i++)
//     {
//       tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
//       Serial.print((String) "[+] Device " + i + " | MAC : ");
//       Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X", station.mac[0], station.mac[1], station.mac[2], station.mac[3], station.mac[4], station.mac[5]);
//       // Serial.println((String) " | IP " + ip4addr_ntoa(&(station.ip)));
//       // Serial.println((String) " | IP " + ip4addr_ntoa(esp_ip4_addr_t()));
//       Serial.print("\n");
//     }
//   }
//   else {
//     Serial.println("No devices connected.");
//   }
// }

#endif
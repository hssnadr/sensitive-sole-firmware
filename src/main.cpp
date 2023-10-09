#include <Arduino.h>

#include "globals.h"
#include "ESP32/FilesManager.h"
#include "ESP32/WifiManager.h"
#include "Movuino/BatteryManager.h"
#include "Movuino/MPU9250.h"
#include "tools/Button.h"
#include "tools/OSCWifi.h"
#include "tools/Recorder.h"
#include "tools/SingleNeopixAnimator.h"
#include "tools/WifiConfigServer.h"

#define WHITE255 ((255 << 16) | (255 << 8) | 255)
#define RED ((255 << 16) | (0 << 8) | 0)
#define GREEN ((0 << 16) | (250 << 8) | 0)
#define YELLOW ((200 << 16) | (175 << 8) | 0)
#define BLUE ((0 << 16) | (0 << 8) | 255)
#define MAGENTA ((255 << 16) | (0 << 8) | 255)

OSCWifi osc;
BatteryManager battery = BatteryManager();
Button button = Button(PIN_BUTTON);
MovuinoMPU9250 mpu = MovuinoMPU9250();
Recorder recorder = Recorder();
SingleNeopixAnimator led = SingleNeopixAnimator(PIN_NEOPIX);

// Wifi config > change WifiManager static variables organization (move to globals.h?)
String ssid_ = "";     // MOVUINO_WIFI_SSID
String password_ = ""; // MOVUINO_WIFI_PASS
String ip_ = "";       // MOVUINO_WIFI_IP
String outip_ = "";    // MOVUINO_OSC_OUTIP
/*
    + MOVUINO_OSC_PORT_IN
    + MOVUINO_OSC_PORT_OUT
*/

void setup()
{
    // Serial
    Serial.begin(115200);
    Serial.println("Welcome and bienvenue sur Movuino");

    // Neopixel
    led.begin();
    led.setBrightness(5);
    led.setColor(BLUE);
    led.forceUpdate();
    for (int i = 0; i < 3; i++)
    {
        delay(100);
        led.turnOff();
        led.forceUpdate();
        delay(100);
        led.turnOn();
        led.forceUpdate();
    }

    // Button
    button.begin();
    button.setMaxReClick(3); // less = less validation delay

    // IMU
    mpu.begin();

    // Recorder example
    recorder.begin();
    recorder.newRecord("imu_data");          // create file and start record
    recorder.defineColumns({"X", "Y", "Z"}); // define data columns
    recorder.pushData<float>({0, 1, 2});
    recorder.newRow();
    recorder.pushData<float>({3, 4, 5});
    recorder.stop();
    recorder.listRecords();
    recorder.readAllRecords();
    recorder.deleteAllRecords(); // remove example

    // Battery
    battery.begin();
    Serial.printf("Battery level = %i%%\n", battery.getLevel());

    // SPIFFS + Read wifi config on flash memory
    startSPIFFS();
    if (isSPIFFS)
    {
        // Read files for wifi connection setup
        printDirectories();
        ssid_ = readFile(FILE_SSID);
        password_ = readFile(FILE_PASSWORD);
        ip_ = readFile(FILE_IP);
        outip_ = readFile(FILE_IP_OUT);
    }

    // Wifi connection
    if (!connectWiFi(ssid_, password_, ip_))
    {
        // Connection failed -> start access point and wifi config server
        Serial.println("Failed to connect to Wifi. Starting access point...");
        if (startAccessPoint("Sensitive-Sole", "", "192.168.4.1"))
        {
            startWifiConfigServer();

            // Neopixel animation
            led.rainbowOn();
            led.breathOn(500);
        };
    }
    else
    {
        // Connection success
        Serial.print("Movuino successfully connected to ");
        Serial.println(ssid_);
        Serial.print("IP: ");
        Serial.println(ip_);
        Serial.println("---------");

        // Start OSC Wifi
        osc = OSCWifi(outip_, 3400);
        osc.begin();
    }
}

void loop()
{
    button.update();
    led.update();

    if (osc.isConnected())
    {
        // ON CLICK = send name
        if (button.isClicked())
        {
            osc.send("/name", "movuino");
            led.setColor(GREEN);
            if (button.isOnClickRelease())
                led.blinkOn(button.timeHoldOnRelease(), 1);
        }

        // ON DOUBLE CLICK = send ip
        if (button.isDoubleClicked())
        {
            osc.send("/ip", {getIP()[0], getIP()[1], getIP()[2], getIP()[3]});

            led.setColor(BLUE);
            if (button.isOnDoubleClickRelease())
                led.blinkOn(button.timeHoldOnRelease(), 2);
        }

        mpu.update();
        osc.send("/acc", {mpu.ax, mpu.ay, mpu.az});
        osc.send("/gyr", {mpu.gx, mpu.gy, mpu.gz});
        delay(1);
    }

    // ON TRIPLE CLICK = set access point + wifi config page
    if (button.isTripleClicked())
    {
        led.setColor(RED);
        if (button.isOnTripleClickRelease())
            led.blinkOn(button.timeHoldOnRelease(), 3);

        if (startAccessPoint("Sensitive-Sole", "", "192.168.4.1"))
        {
            startWifiConfigServer();
        }
    }
}
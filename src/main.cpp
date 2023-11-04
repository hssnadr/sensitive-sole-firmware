#include <Arduino.h>

#include "globals.h"
#include "ESP32/FilesManager.h"
#include "ESP32/WifiManager.h"
#include "Shields/ResistiveMatrix.h"
// #include "tools/SingleNeopixAnimator.h"
#include "tools/WifiConfigServer.h"
#include "Movuino/MPU9250.h"

// Base
// SingleNeopixAnimator led = SingleNeopixAnimator(PIN_NEOPIX);
MovuinoMPU9250 mpu = MovuinoMPU9250();

// Shield
#define COLS 7
#define ROWS 15
MovuinoResistiveMatrix sole = MovuinoResistiveMatrix(COLS, ROWS);

// Websockets
AsyncWebServer server(443);
AsyncWebSocket ws("/ws");

int latestClientId = -1;

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    Serial.println((char *)data); // incoming message

    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        ws.textAll("Message from Movuino");
        if (latestClientId != -1)
            ws.text(latestClientId, "It's you");
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    Serial.println(type);
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        client->text("Hello from Movuino");
        latestClientId = client->id();
        break;
    case WS_EVT_DISCONNECT:
        client->text("Bye from Movuino");
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
        break;
    case WS_EVT_ERROR:
        break;
    }
}

void startAPWifiConfig()
{
    // led.setColor(RED);
    // led.blinkOn(200, 5);
    // led.asyncBlinkOn(2000, 400);
    if (startAccessPoint("Sensitive-Sole", "", "192.168.4.1"))
    {
        startWifiConfigServer();
    };
}

void setup()
{
    // Serial
    Serial.begin(115200);
    Serial.println("Welcome and bienvenue sur Movuino");

    // Sensitive sole
    sole.begin();

    mpu.begin();

    // Neopixel
    // led.begin();
    // led.setBrightness(5);
    // led.setColor(BLUE);
    // led.forceUpdate();

    // SPIFFS (read stored config)
    startSPIFFS();
    if (isSPIFFS)
    {
        // Read files for wifi connection setup
        printDirectories();
        MOVUINO_WIFI_SSID = readFile(FILE_SSID);
        MOVUINO_WIFI_PASS = readFile(FILE_PASSWORD);
        MOVUINO_WIFI_IP = readFile(FILE_IP);
    }

    // Wifi connection
    if (!connectWiFi(MOVUINO_WIFI_SSID, MOVUINO_WIFI_PASS, MOVUINO_WIFI_IP))
    {
        // Connection failed: start access point and wifi config server
        Serial.println("Failed to connect to Wifi. Starting access point...");
        startAPWifiConfig();
    }
    else
    {
        // Connection success
        Serial.print("Movuino successfully connected to ");
        Serial.println(MOVUINO_WIFI_SSID);
        Serial.print("IP: ");
        Serial.println(MOVUINO_WIFI_IP);
        Serial.println("---------");

        // Start websocket on server
        ws.onEvent(onEvent);
        server.addHandler(&ws);
        server.begin();

        // led.setColor(GREEN);
        // led.breathOn(700, 0.5);
    }
}

void loop()
{
    // led.update();

    if (isWifiConnected())
    {
        // Websocket test
        // String message_ = String("message");
        // ws.textAll(message_);
        // delay(3);
        // ws.cleanupClients(); // to check: https://m1cr0lab-esp32.github.io/remote-control-with-websocket/websocket-setup/

        // Time
        String data_ = "t";
        data_ += String(millis());
        data_ += "\n";

        // IMU
        mpu.update();
        data_ += "a";
        data_ += String(mpu.ax);
        data_ += ",";
        data_ += String(mpu.ay);
        data_ += ",";
        data_ += String(mpu.az);
        data_ += "\n";

        // Sole
        sole.update();
        for (int i = 0; i < sole.rows(); i++)
        {
            data_ += "z";
            data_ += sole.printRow(i);
            if (i < sole.rows() - 1)
            {
                data_ += "\n";
            }
        }
        Serial.println(data_); // DEBUG
        ws.textAll(data_);
        delay(15);
    }
}
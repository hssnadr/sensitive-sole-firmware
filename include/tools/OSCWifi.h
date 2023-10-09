#if !defined(_MOVUINOESP32_WIFIOSC_)
#define _MOVUINOESP32_WIFIOSC_

#include <WiFi.h>
#include <OSCMessage.h>

class OSCWifi
{
private:
    WiFiUDP udp;
    IPAddress outIp;
    unsigned int port;
    bool isUdpBegin = false;

    // void receiveMessage();
    // static void callbackOSC(OSCMessage &msg);

public:
    OSCWifi();
    OSCWifi(String outIp_, unsigned int port_);
    ~OSCWifi();

    void begin();
    // void update();
    
    void setIpOut(String outIp_) { this->outIp.fromString(outIp_.c_str()); };
    void setPortOut(unsigned int port_) { this->port = port_; };
    
    template <typename DataType>
    void send(const char *addr_, DataType data_);
    
    template <class T>
    void send(const char *addr_, std::initializer_list<T> array_);

    bool isConnected() { return this->isUdpBegin; };
};

OSCWifi::OSCWifi() {}

OSCWifi::OSCWifi(String outIp_, unsigned int port_)
{
    this->setIpOut(outIp_);
    this->setPortOut(port_);
}
OSCWifi::~OSCWifi() {}

void OSCWifi::begin()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        this->udp.begin(this->port); // once wifi connected
        this->isUdpBegin = true;
    }
}

// void OSCWifi::update()
// {}

template <typename DataType>
void OSCWifi::send(const char *addr_, DataType data_)
{
    // create message
    OSCMessage msg_(addr_);
    msg_.add(data_);

    // send message
    this->udp.beginPacket(this->outIp, this->port);
    msg_.send(this->udp);
    this->udp.endPacket();
    msg_.empty();
}

template <class T>
void OSCWifi::send(const char *addr_, std::initializer_list<T> array_)
{
    // create message
    OSCMessage msg_(addr_);
    for (auto data : array_)
    {
        msg_.add(data);
    }

    // send message
    this->udp.beginPacket(this->outIp, this->port);
    msg_.send(this->udp);
    this->udp.endPacket();
    msg_.empty();
}

// >>> VOIR CLASS TIMER (Firmware Joseph) + CLASSES ENFANTS DANS SENSORS = callback every specified time
// void OSCWifi::receiveMessage() {
//   // Need to be called in the main loop()
//   OSCMessage msg_;
//   int size = udp.parsePacket();

//   if (size > 0) {
//     while (size--) {
//       msg_.fill(udp.read());
//     }
//     if (!msg_.hasError()) {
//      msg_.dispatch("/printInt", this->callbackOSC);
//     }
//   }
// }

// static void OSCWifi::callbackOSC(OSCMessage &msg) {
//   Serial.print("Receive message: ");
//   Serial.println(msg.getInt(0));
// }

#endif // _MOVUINOESP32_WIFIOSC_
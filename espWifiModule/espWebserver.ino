// This gets data via UART from the K66F and sends it to the Python Socket
#include "WiFi.h"
#include "AsyncUDP.h"

const char * ssid = "Linksys00321";
const char * password = "fxjz26padu";

AsyncUDP udp;

void setup()
{
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while(1) {
            delay(1000);
        }
    }
    if(udp.listen(5000)) {
        Serial.print("Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {
            Serial.write(packet.data(), packet.length());
            Serial.println();
        });
    }
}


void loop() {
    static String buffer;  

    while (Serial.available() > 0) {
        char c = Serial.read();  
        if (c == '\n') {  // end of message
            // Serial.print("Received full message: ");
            // Serial.println(buffer);
            udp.broadcastTo(buffer.c_str(), 6000);
            buffer = "";  // clear buffer
        } else if (c >= 32 && c <= 126) {
            buffer += c;  // getting rid of invalid data because of noise
        }
    }
}
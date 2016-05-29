#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet2.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp2.h>
#include <Twitter.h>
#include <util.h>


#include <Ethernet2.h>
#include <SPI.h>
    #include <stdio.h>
    
    byte     mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x02};
    byte     ip[] = {192, 168, 1, 177};
    byte     server[] = {192, 168, 1, 20};
    
    Client   client(server, 80);
    
    void setup()
    {
       Ethernet.begin(mac, ip);
       Serial.begin(9600);
       delay(500);
    }
    
    void loop()
    {
       long     count = 0;
       char     buf[128];
    
       Serial.println("connecting...");
    
       if (client.connect()) {
          sprintf(buf, "GET /arduino.php?n=%ld HTTP/1.0", count++);
          client.println(buf);
          client.println();
       } else {
          Serial.println("connection failed");
       }
    
       Serial.print("client.status = ");
       Serial.println(client.status(), DEC);
    
       while (client.available() && client.connected()) {
          char     c = client.read();
          Serial.print(c);
       }
    
       client.stop();
    }

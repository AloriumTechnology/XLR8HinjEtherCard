// Ping a remote server, also uses DHCP and DNS.
// 2011-06-12 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
// 2018-02-08 Bryan Craker of Alorium Technology (info@aloriumtech.com)

#include <XLR8HinjEtherCard.h>

byte XLR8Ethernet::buffer[700];
static uint32_t timer;

// called when a ping comes in (replies to it are automatic)
static void gotPinged (byte* ptr) {
  XLR8Ether.printIp(">>> ping from: ", ptr);
}

void setup () {
  Serial.begin(9600);
  Serial.println("Starting Pings");
  
  if (XLR8Ether.begin(sizeof XLR8Ethernet::buffer, mymac) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  if (!XLR8Ether.dhcpSetup())
    Serial.println(F("DHCP failed"));

  XLR8Ether.printIp("IP:  ", XLR8Ether.myip);
  XLR8Ether.printIp("GW:  ", XLR8Ether.gwip);

#if 1
  // use DNS to locate the IP address we want to ping
  if (!XLR8Ether.dnsLookup(PSTR("www.google.com")))
    Serial.println("DNS failed");
#else
  XLR8ether.parseIp(XLR8ether.hisip, "74.125.77.99");
#endif
  XLR8Ether.printIp("SRV: ", XLR8Ether.hisip);
    
  // call this to report others pinging us
  XLR8Ether.registerPingCallback(gotPinged);
  
  timer = -9999999; // start timing out right away
  Serial.println();
}

void loop () {
  word len = XLR8Ether.packetReceive(); // go receive new packets
  word pos = XLR8Ether.packetLoop(len); // respond to incoming pings
  
  // report whenever a reply to our outgoing ping comes back
  if (len > 0 && XLR8Ether.packetLoopIcmpCheckReply(XLR8Ether.hisip)) {
    Serial.print("  ");
    Serial.print((micros() - timer) * 0.001, 3);
    Serial.println(" ms");
  }
  
  // ping a remote server once every few seconds
  if (micros() - timer >= 5000000) {
    XLR8Ether.printIp("Pinging: ", XLR8Ether.hisip);
    timer = micros();
    XLR8Ether.clientIcmpRequest(XLR8Ether.hisip);
  }
}


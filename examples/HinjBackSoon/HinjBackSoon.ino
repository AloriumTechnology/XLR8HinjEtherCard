// Present a "Will be back soon web page", as stand-in webserver.
// 2011-01-30 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
// 2018-02-08 Bryan Craker of Alorium Technology (info@aloriumtech.com)
 
#include <XLR8HinjEtherCard.h>

#define STATIC 0  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = { 192,168,1,200 };
// gateway ip address
static byte gwip[] = { 192,168,1,1 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte XLR8Ethernet::buffer[500]; // tcp/ip send and receive buffer

const char page[] PROGMEM =
"HTTP/1.0 503 Service Unavailable\r\n"
"Content-Type: text/html\r\n"
"Retry-After: 600\r\n"
"\r\n"
"<html>"
  "<head><title>"
    "Hey There, I'm Hinj"
  "</title></head>"
  "<body>"
    "<h3>Hey There, I'm Hinj</h3>"
    "<p><em>"
      "The webpage you're looking for will be back soon."
    "</em></p>"
  "</body>"
"</html>"
;

void setup(){
  Serial.begin(9600);
  Serial.println("Starting Back Soon");
  
  if (XLR8Ether.begin(sizeof XLR8Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");
#if STATIC
  XLR8Ether.staticSetup(myip, gwip);
#else
  if (!XLR8Ether.dhcpSetup())
    Serial.println("DHCP failed");
#endif

  XLR8Ether.printIp("IP:  ", XLR8Ether.myip);
  XLR8Ether.printIp("GW:  ", XLR8Ether.gwip);  
  XLR8Ether.printIp("DNS: ", XLR8Ether.dnsip);  
}

void loop(){
  // wait for an incoming TCP packet, but ignore its contents
  if (XLR8Ether.packetLoop(XLR8Ether.packetReceive())) {
    memcpy_P(XLR8Ether.tcpOffset(), page, sizeof page);
    XLR8Ether.httpServerReply(sizeof page - 1);
  }
}

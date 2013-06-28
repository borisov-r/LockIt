/*
 * LockIt -- automated door knob for initlab.org
 * 
 * This currently only does simple UDP echo.
 * 
 * Based on udpServer example by             
 *  2013-4-7 Brian Lee <cybexsoft@hotmail.com>
 */ 

/* XXX:
 * 
 * Maybe we should look into xPL, though "official" software 
 * is Windows-only, but the protocol is pretty simple, ASCII-based
 * and everything is opensource.
 * 
 * xPL website:
 * http://en.wikipedia.org/wiki/XPL_Protocol
 * 
 * Arduino libraries:
 * https://code.google.com/p/xpl-arduino/
 * https://github.com/mgth/arduixpl
 */

#include <EtherCard.h>
#include <IPAddress.h>
#include <avr/wdt.h>

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x70,0x69,0x69,0x2D,0x30,0x31 };
byte Ethernet::buffer[500]; // tcp/ip send and receive buffer

// set to 1 to disable DHCP (adjust myip/gwip values below)
#define STATIC 0

#if STATIC
static byte myip[] = { 192,168,0,116 }; // local ip address
static byte gwip[] = { 192,168,0,1 };	// gateway address
// TODO: put those in eeprom, as well as 
//       - server endpoint configuration
// 		 - local port to listen on
//		 - some sort of limitation if not authentication
//       - implement remote configuration, xPL?
// TODO: manual dns?
#endif


// callback that prints received packets to the serial port
void handleUDP(word port, byte ip[4], const char *data, word len) {
  IPAddress src(ip[0], ip[1], ip[2], ip[3]);
  Serial.println(src);
  Serial.println(port);
  Serial.println(data);
  Serial.println(len);
  
  /*
  static byte destIp[] = { 239,192,74,66 }; // UDP multicast address 
  static void sendUdp(char *data,
                      uint8_t len,
                      uint16_t sport,
                      uint8_t *dip, 
                      uint16_t dport); */
  
  // XXX: it's probably a very bad idea to do this from a callback!!!
  //      this is just a test of udp ping-pongs
  ether.sendUdp((char *)data, len, 1337, ip, 1337);
}

void setup(){
  Serial.begin(57600);
  Serial.println("\n[LockIt/0.1]");
  
  // activate watchdog
  wdt_enable(WDTO_4S);

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) {
    Serial.println("Failed to access Ethernet controller!");
    while (1); // let the watchdog fire
  }
  
#if STATIC
  Serial.println("Setting static IP address configuration");
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup()) {
    Serial.println("DHCP failed, fallback to static");
	while (1); // let the watchdog fire
  } else {
	Serial.println("Got DHCP lease");
  }
#endif

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  //register handleUDP() to port 1337
  ether.udpServerListenOnPort(&handleUDP, 1337);
  Serial.println("Listening on port 1337 ...");
  wdt_reset();
}

void loop() {
  // this must be called for ethercard functions to work
  ether.packetLoop(ether.packetReceive());
  wdt_reset();
  delay(1); // otherwise the atmega hangs
}

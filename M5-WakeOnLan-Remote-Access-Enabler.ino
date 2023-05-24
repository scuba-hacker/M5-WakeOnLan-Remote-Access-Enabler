// Mark B Jones 2022

#include <M5StickCPlus.h>

// Send Wake On LAN packet to 27 inch iMac Ethernet
// Wake it up for remote access to iMac using Parallels Access software.
// M5 Stick C Plus must be plugged into a smart plug which can be turned on/off over the internet.
// When the M5 Stick sees power on the USB-C connector it auto-boots and runs for 10 minutes sending WOL packets.
// The iMac has weird behaviour regarding wakeups, so the code wasn't intuitive to sort out.

// Copy the secrets_template.c file to secrets.c and populate the required fields for your local network and iMac network attributes.

#include "secrets.c"

// https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFi.h
#include <WiFi.h>

// https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFiUdp.h
#include <WiFiUdp.h>
WiFiUDP UDP;

// https://github.com/a7md0/WakeOnLan
#include <WakeOnLan.h>
WakeOnLan WOL(UDP); // Pass WiFiUDP class

// https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFiClient.h
//#include <WiFiClient.h>
// the esp products use the standard POSIX time library functions.
// time(), gmtime(), localtime(), mktime(), gettimeofday() etc.
//#include "time.h" 
//#include <ESP32Ping.h>

// Press button A - big button - to quit

int screenBrightness = 8; // start at 2nd lowest brightness

bool firstSetupDone=false;

void setup() {
  // put your setup code here, to run once:

  WOL.setRepeat(100, 100); // 10 seconds of send - Set Wake On LAN To Repeat the packet 100 times with 100ms delay between
  // The mac will only wake up for 1 minute in response to the first WOL packets it sees.
  // Mac ignores all subsequent WOL packets during that minute, so repeat the WOL send after 55 seconds.
  // Repeat the 10 seconds send WOL, 55 seconds wait cycle 10 times to keep the Mac awake for 10 minutes.
  
  if (!firstSetupDone)
  {
    M5.begin();
    M5.Axp.ScreenBreath(screenBrightness);
    firstSetupDone=true;
  }
  
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextSize(2);

  Serial.begin(115200);
  
  //connect to WiFi
  M5.Lcd.println("Wake iMac 10 mins");
  M5.Lcd.println("Btn A to stop");
 
  WiFi.begin(ssid, password);
  int maxAttempts=50;
  while (WiFi.status() != WL_CONNECTED && --maxAttempts) {
      delay(500);
      M5.Lcd.print(".");
  }
  
  if (maxAttempts)
  {
    M5.Lcd.println(" CONNECTED");

    // set manually broadcast address
    // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/IPAddress.h
    WOL.setBroadcastAddress(IPAddress(networkBroadcastIPAddressOctet1,
										networkBroadcastIPAddressOctet2,
										networkBroadcastIPAddressOctet3,
										networkBroadcastIPAddressOctet4));

    // send 10 seconds of WOL packets followed by 55 seconds of delay, and repeat 10 times
    M5.Lcd.printf("V: %.3fv  I: %.0fma\r\n", M5.Axp.GetBatVoltage(),
                   M5.Axp.GetBatCurrent());
    M5.Lcd.println("Send WoL for 10 mins");
    int n=10;
    while (n--)
    {
      M5.Lcd.print("+");
      WOL.sendMagicPacket(Target_iMac_Ethernet_MACAddress); // UDP uses default port 9
      M5.update(); if (M5.BtnA.isPressed()) break; delay (5000);
      M5.update(); if (M5.BtnA.isPressed()) break; delay (5000);
      M5.update(); if (M5.BtnA.isPressed()) break; delay (5000);
      M5.update(); if (M5.BtnA.isPressed()) break; delay (5000);
      M5.update(); if (M5.BtnA.isPressed()) break; delay (5000);
      M5.update(); if (M5.BtnA.isPressed()) break; delay (5000);
      M5.update(); if (M5.BtnA.isPressed()) break; delay (5000);
      M5.update(); if (M5.BtnA.isPressed()) break; delay (5000);
      M5.update(); if (M5.BtnA.isPressed()) break; delay (5000);
      M5.update(); if (M5.BtnA.isPressed()) break; delay (5000);
    }
  }
  else
  {
    M5.Lcd.println(" FAILED");    
  }
}

void loop() {
    M5.Lcd.println("");
    M5.Lcd.println("Off in 5s...");
    delay(5000);
    M5.Axp.PowerOff();
}

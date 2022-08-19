
/* 
 *  Author: Pius Onyema Ndukwu
 *  License: MIT License
 *  GitHub:https://github.com/Pius171/esp8266-wifi-extender
 *  
 */

#include "WM.h"
// variables
bool RepeaterIsWorking= true;
int ledState = LOW; 
unsigned long previousMillis = 0;
long delay_time=0; // interval between blinks
// blink every 200ms if connected to router
// blink every 1sec if web server is active
// led is off is there is an error with the repeater
//led is on when trying to connect to router.


/* Set these to your desired credentials. */

#if LWIP_FEATURES && !LWIP_IPV6

#define HAVE_NETDUMP 0
#include <lwip/napt.h>
#include <lwip/dns.h>
#include <LwipDhcpServer.h>

#define NAPT 1000
#define NAPT_PORT 10

#if HAVE_NETDUMP

#include <NetDump.h>


void dump(int netif_idx, const char* data, size_t len, int out, int success) {
  (void)success;
  Serial.print(out ? F("out ") : F(" in "));
  Serial.printf("%d ", netif_idx);

  // optional filter example: if (netDump_is_ARP(data))
  {
    netDump(Serial, data, len);
    //netDumpHex(Serial, data, len);
  }
}
#endif






WM my_wifi;


void setup() {
  delay(1000);
 pinMode(0,INPUT_PULLUP);
 pinMode(LED_BUILTIN,OUTPUT);
 digitalWrite(LED_BUILTIN,1); //active low
  Serial.begin(115200);

  Serial.println();

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }

  Serial.printf("\n\nNAPT Range extender\n");
  Serial.printf("Heap on start: %d\n", ESP.getFreeHeap());

#if HAVE_NETDUMP
  phy_capture = dump;
#endif

  // first, connect to STA so we can get a proper local DNS server


  String ssid = my_wifi.get_credentials(0); // if the file does not exist the function will always return null
  String pass = my_wifi.get_credentials(1);
  String ap= my_wifi.get_credentials(2);

  if (ssid == "null") { // if the file does not exist ssid will be null
start_webserver:
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    WiFi.softAP("Pius_Electronics_extender0001");
    Serial.printf("AP: %s\n", WiFi.softAPIP().toString().c_str());
    my_wifi.create_server();
    //server.begin();
    my_wifi.begin_server();
    Serial.println("HTTP server started");
    delay_time=1000; // blink every sec if webserver is active
  }
  else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass); // check function to understand
    int timeout_counter=0;
    while (WiFi.status() != WL_CONNECTED) {
      if(timeout_counter>=120){
        goto start_webserver; // if it fails to connect start_webserver
      }

      Serial.print('.');
      timeout_counter++;
      digitalWrite(LED_BUILTIN,0);// leave led on when trying to connect
      delay(500);
    }



    Serial.printf("\nSTA: %s (dns: %s / %s)\n",
                  WiFi.localIP().toString().c_str(),
                  WiFi.dnsIP(0).toString().c_str(),
                  WiFi.dnsIP(1).toString().c_str());

    // give DNS servers to AP side
    dhcpSoftAP.dhcps_set_dns(0, WiFi.dnsIP(0));
    dhcpSoftAP.dhcps_set_dns(1, WiFi.dnsIP(1));

    WiFi.softAPConfig(  // enable AP, with android-compatible google domain
      IPAddress(172, 217, 28, 254),
      IPAddress(172, 217, 28, 254),
      IPAddress(255, 255, 255, 0));
    WiFi.softAP(ap, pass);
    Serial.printf("AP: %s\n", WiFi.softAPIP().toString().c_str());

    Serial.printf("Heap before: %d\n", ESP.getFreeHeap());
    err_t ret = ip_napt_init(NAPT, NAPT_PORT);
    Serial.printf("ip_napt_init(%d,%d): ret=%d (OK=%d)\n", NAPT, NAPT_PORT, (int)ret, (int)ERR_OK);
    if (ret == ERR_OK) {
      ret = ip_napt_enable_no(SOFTAP_IF, 1);
      Serial.printf("ip_napt_enable_no(SOFTAP_IF): ret=%d (OK=%d)\n", (int)ret, (int)ERR_OK);
      if (ret == ERR_OK) {
        Serial.printf("Successfully NATed to WiFi Network '%s' with the same password", ssid.c_str());
      }
    }
    Serial.printf("Heap after napt init: %d\n", ESP.getFreeHeap());
    if (ret != ERR_OK) {
      Serial.printf("NAPT initialization failed\n");
    }
    delay_time=200; // blink every half second if connection was succesfull
  }
  RepeaterIsWorking=true;
}

#else

void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nNAPT not supported in this configuration\n");
  RepeaterIsWorking= false;
  digitalWrite(LED_BUILTIN,1); // led stays off
}

#endif

void loop() {
  if(digitalRead(0)==LOW){
    LittleFS.format();
    ESP.restart();
  }

  while(RepeaterIsWorking){
 unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= delay_time) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(LED_BUILTIN, ledState);
  }
  break;
}
}

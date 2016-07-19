/*
    Taipei HackerSpace IOT online check-in system
    Modified by Rudi. Repo at http://github.com/todocono/ESPRedirect
    Date: 2016-07-18

    HTTPS with follow-redirect example
    Created by Sujay S. Phadke, 2016

    Based on the WifiClientSecure example by
    Ivan Grokhotkov
 *  *
    This example is in public domain.
*/

#include <Adafruit_NeoPixel.h>

#define D0 16     //onboard LED 
#define D1 5
#define D2 4
#define D3 0
#define D4 2      //onboard LED
#define D5 14
#define D6 12
#define D7 13
#define D8 15   //needs to be open when flashing

#define PIN D2
#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);

// Replace with your network credentials
const char* ssid = "TPE-Hackerspace";
const char* password = "HN74874666";

static boolean newState;
static boolean oldState;
const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";
const char *GScriptId = "AKfycbwIoZSoplXOlg67FiBS1zdNO5APHmOVMpM8nWJ2Pwa3ny1cswc";



// Google sheet hosted at  https://docs.google.com/spreadsheets/d/1EjDXjxsU1T6WPc_2eDv9oyeZjQBobPMp4C9s1Qw9QYs/edit?usp=sharing
// Google Script hosted at https://script.google.com/macros/s/AKfycbwIoZSoplXOlg67FiBS1zdNO5APHmOVMpM8nWJ2Pwa3ny1cswc/exec
// Note that the script gets a value that is the ID of the user

const int httpsPort = 443;

// http://askubuntu.com/questions/156620/how-to-verify-the-ssl-fingerprint-by-command-line-wget-curl/
// echo | openssl s_client -connect script.google.com:443 |& openssl x509 -fingerprint -noout

// SHA1 fingerprint of the certificate
//these are fingerprints from other locations/MACs:
//const char* fingerprint = "94 2F 19 F7 A8 B4 5B 09 90 34 36 B2 2A C4 7F 17 06 AC 6A 2E";
//const char* fingerprint = "F0 5C 74 77 3F 6B 25 D7 3B 66 4D 43 2F 7E BC 5B E9 28 86 AD";
//const char* fingerprint2 = "94 64 D8 75 DE 5D 3A E6 3B A7 B6 15 52 72 CC 51 7A BA 2B BE";

//this is the fingerprint obtained on 2016-07-19
const char* fingerprint = "66 53 78 0C FF 9B F4 FB FE 3F 6F 0B E2 05 EC E4 8F C9 C4 55";

//userID is the ID that refers to who opened the door
int userID = 0;

// Write to Google Spreadsheet to open the door
String urlOpen = String("/macros/s/") + GScriptId + "/exec?value=" + userID;
// Write to Google Calendar that nobody is in there
String urlClose = String("/macros/s/") + GScriptId + "/exec?value=0";

// Read from Google Spreadsheet  (we could display with LEDs if somebody is in there or not
//String url3 = String("/macros/s/") + GScriptId + "/exec?read";



void setup() {

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  // flush() is needed to print the above (connecting...) message reliably,
  // in case the wireless connection doesn't go through
  Serial.flush();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create TLS connection
  HTTPSRedirect client(httpsPort);
  Serial.print("Connecting to ");
  Serial.println(host);

  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  Serial.flush();
  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }

  Serial.flush();
  if (client.verify(fingerprint, host)) {
    Serial.println("Certificate match.");
  } else {
    Serial.println("Certificate mis-match");
  }

  // Note: setup() must finish within approx. 1s, or the the watchdog timer
  // will reset the chip. Hence don't put too many requests in setup()
  // ref: https://github.com/esp8266/Arduino/issues/34

  Serial.print("Requesting URL: ");
  Serial.println(urlClose);

  client.printRedir(urlClose, host, googleRedirHost);
  Serial.println("========== Taipei Hackerspace Closed =============");

  //Serial.print("Requesting URL: ");
  //Serial.println(urlOpen);

  //client.printRedir(url2, host, googleRedirHost);
  //Serial.println("==============================================================================");

  pinMode(D1, INPUT); // a switch
  pinMode(D0, OUTPUT); // an onboard LED
}

void loop() {
  HTTPSRedirect client(httpsPort);

  if (!client.connected())           // to keep the connection alive
    client.connect(host, httpsPort); // if it's disconnected, re-connect

  //the bug detected was that no actual post action was being done in here

  newState = digitalRead(D1);
  Serial.print( "read a: " );
  Serial.println( newState );

  // We use D1 as an input to read wether a switch has been flipped or not
  // Here could be the right spot to expand into RFID or other behaviors
  if (newState != oldState ) {
    oldState = newState;            // changes the state
    digitalWrite( D0, LOW); // switches an LED
    Serial.println( "state changed" );
    userID = newState;              //here we can input the ID from the user

    String url = String("/macros/s/") + GScriptId + "/exec?value=" + userID;
    client.printRedir(url, host, googleRedirHost);

  
  }
  else {
    digitalWrite( D0, HIGH); // switches OFF the LED
    Serial.println( "nothing" );
    userID = 0;
    //client.printRedir(urlClose, host, googleRedirHost);
  }

  if ( newState ) {
      rainbow(20);
    }
    else{
      colorWipe(strip.Color(0, 0, 0), 50); // turns them off
    }
  // this is note from Sujay, the original author. We didn't register any reboots on NodeMCU:
  // In my testing on a ESP-01, a delay of less than 1500 resulted
  // in a crash and reboot after about 50 loop runs.
  delay(1000);
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

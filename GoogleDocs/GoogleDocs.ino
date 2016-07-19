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


#define D0 16     //onboard LED 
#define D1 5
#define D2 4
#define D3 0
#define D4 2      //onboard LED
#define D5 14
#define D6 12
#define D7 13
#define D8 15   //needs to be open when flashing

#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"

// Replace with your network credentials
const char* ssid = "TPE-Hackerspace";
const char* password = "HN74874666";

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

  static boolean state;
  // We use D1 as an input to read wether a switch has been flipped or not
  // Here could be the right spot to expand into RFID or other behaviors
  if (digitalRead(D1) == HIGH && !state ) {
    state = true;            // changes the state
    digitalWrite( D0, HIGH); // switches an LED
    Serial.println( "state changed to TRUE" );
    userID = 1;
    client.printRedir(urlOpen, host, googleRedirHost);
  }
  else {
    state = false;            // changes the state
    digitalWrite( D0, LOW); // switches OFF the LED
    Serial.println( "state changed to FALSE" );
    userID = 0;
    client.printRedir(urlClose, host, googleRedirHost);
  }


  // this is note from Sujay, the original author. We didn't register any reboots on NodeMCU:
  // In my testing on a ESP-01, a delay of less than 1500 resulted
  // in a crash and reboot after about 50 loop runs.
  delay(1500);
}

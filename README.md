

# Project Description
This is the working version for the Taipei Hackerspace Door. It is based on an ESP8266 with a Google Script that updates an online spreadsheet. The HTML website pulls a local JS that updates a div container.

#1. Website
Their website is at https://taipeihack.org/#open .

 ## 1.1.1 Website Repo
 This is the repo of the website https://github.com/taipeihackerspace/webtemp/

 ## 1.1.2 Website JS to update the AJAX at the HTML
This is the script:
https://github.com/taipeihackerspace/webtemp/blob/gh-pages/js/hackerspace.js

#2. Google Spreadsheet

This is the link for the spreadsheet:
https://docs.google.com/spreadsheets/d/1EjDXjxsU1T6WPc_2eDv9oyeZjQBobPMp4C9s1Qw9QYs/edit?ts=578c6274#gid=1720119013

#3 Firmware

##3.1
At the repo it is published the working version http://github.todocono.com/ESPRedirect/GoogleDocs

##3.2 Development Environment
We used the Arduino IDE 1.6.8 with the ESP8266 board installed manually on the board manager (v2.3.0 stable)

#4. Hardware

## 4.1 NodeMCU
 The main chipset used that was used was ESP8266 with the setup of a NodeMCU.

## 4.2 Additional components
  We used an swtich on D1 and an USB power adaptor.


#5. Google Script

Google Script hosted at https://script.google.com/macros/s/AKfycbwIoZSoplXOlg67FiBS1zdNO5APHmOVMpM8nWJ2Pwa3ny1cswc/exec
Note that the script has to be published with authorization to be ran as the publisher, with access to anyone, including anonymous users. And that each time the script is updated, it is a requirement to update the version published.


#6. Contributors to this setup
Rudi, Kai, Raymond and Foucalt  

#5. Project References

- We forked from https://github.com/electronicsguy/ESP8266/tree/master/HTTPSRedirect . It's an extension of the work by Ivan Grokhotkov ([Ivan-github](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClientSecure.h)).
*HTTPSRedirect*
- We used intensively the post reference at https://mashe.hawksey.info/2014/07/google-sheets-as-a-database-insert-with-apps-script-using-postget-methods-with-ajax-example/
- Samples of Google Spreadsheet API: https://developers.google.com/gdata/samples/spreadsheet_sample




# HTTPS Redirect for ESP8266

This library extends the *WiFiClientSecure* library, which is an amazing piece of work by Ivan Grokhotkov ([Ivan-github](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClientSecure.h)).
*HTTPSRedirect* uses the header information in the server's reply to a *GET* request, and follows the re-direction URL by making another suitable *GET* request. In many cases, it is this re-directed URL which would present the final data that was required.
 This is, for example, used by a number of Google services.

## Working Example (Using Google Docs)
With Google Apps Script, you can publish your Google Scripts on the web. These scripts can access a variety of Google services,
like Gmail, Google Docs, Calendar, Drive, etc. Google requires all access to their services use SSL/TLS. Hence the regular *WiFiClient* is not suitable and *WiFiClientSecure* must be used. Thanks to Ivan's work, we can now access Google services, using TLS 1.0/1.1 and
with large certificates. However, *WiFiClientSecure* does not follow redirects.

When you publish your Google scripts, it'll be in the domain: **https://script.google.com/**. When you make a **GET** request to this
URL, the data will be processed by your Google script, as long as you have a *doGet()* function in there. This is what a **GET** request to
Google script will look like:

```
GET https://script.google.com/macros/s/<script-id>/exec  
```

However, any data returned by *doGet()* is not passed on in the body of the response. This is what the initial response header
looks like:
```
Access-Control-Allow-Origin: *
Alt-Svc: quic=":443"; ma=2592000; v="32,31,30,29,28,27,26,25"
Alternate-Protocol: 443:quic
Cache-Control: no-cache, no-store, max-age=0, must-revalidate
Content-Encoding: gzip
Content-Type: text/html; charset=UTF-8
Date: Mon, 25 Apr 2016 22:15:44 GMT
Expires: Mon, 01 Jan 1990 00:00:00 GMT
Location: https://script.googleusercontent.com/macros/<random url>
Pragma: no-cache
Transfer-Encoding: chunked
```

The HTTP error code will be **302 Moved Temporarily**.
Notice the field called **Location** in the response header. Even though we hit the script URL correctly (and passed on any parameters
 through the **GET** request), we will now need to follow this second link in order to get the return data. The second **GET**
 request has to be made to the domain: **https://script.googleusercontent.com/**.

 *HTTPSRedirect* does precisely this, in a seamless fashion. It'll make both the **GET** requests and return the final data from the server
  in the response body.

  The main class is **HTTPSRedirect**, which has a method called **printRedir**, which does most of the stuff.

  Please check the **GoogleDocs** Arduino example included above, on how to use this library. The *Extra* folder contains the Google Apps script that you can use for your own spreadsheet. It also has an image of the test calendar whose entries are fetched by the above example. The spreadsheet can be found at: [spreadsheet](http://bit.ly/1Ql4qrN).

  The Arduino example does 3 things:
  1. Makes a request to the script attached to the Google Spreadsheet, and write a value in the cell 'A1'.

  2. Fetches entries for the next 1 week from my Google calendar's test calendar.

  3. Keep repeating a request to read from the cell 'A1' of the spreadsheet. In this way, if you manually type something in the cell, you can *chat* with the ESP8266 :smile:

  Comments and suggestions welcome.

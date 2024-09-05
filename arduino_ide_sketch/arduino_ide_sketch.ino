/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-date-time-ntp-client-server-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>     // Replace with WebServer.h for ESP32
#include <AutoConnect.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

ESP8266WebServer Server;          // Replace with WebServer for ESP32
AutoConnect      Portal(Server);
AutoConnectAux aux("/mqtt_setting", "Waktu Azan");
ACText(header,"Waktu Azan mengikut waktu JAKIM");
ACText(caption,"Sistem ini dibina & dijaga oleh Fara Farizul. Hubungi saya melalui 0137944467");

// Replace with your network credentials
const char *ssid     = "FaraNurazrin";
const char *password = "cxgue23844g";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void rootPage() {
  char content[] = "Hello, world";
  Server.send(200, "text/plain", content);
}
char* getCurrentDdate(){
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  //Serial.print("Epoch Time: ");
  //Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  //Serial.print("Formatted Time: ");
  //Serial.println(formattedTime);  

  int currentHour = timeClient.getHours();
  //Serial.print("Hour: ");
  //Serial.println(currentHour);  

  int currentMinute = timeClient.getMinutes();
  ////Serial.print("Minutes: ");
  //Serial.println(currentMinute); 
   
  int currentSecond = timeClient.getSeconds();
  //Serial.print("Seconds: ");
  //Serial.println(currentSecond);  
  

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
  //Serial.print("Month day: ");
  //Serial.println(monthDay);

  int currentMonth = ptm->tm_mon+1;
  //Serial.print("Month: ");
  //Serial.println(currentMonth);


  int currentYear = ptm->tm_year+1900;
  //Serial.print("Year: ");
  //Serial.println(currentYear);

  //Print complete date:
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  //Serial.print("Current date: ");
  //Serial.println(currentDate);

  static char json_year_month_filename[16];
  //Serial.print("Current date: ");
  
  sprintf(json_year_month_filename, "%04d-%02d-%02d", currentYear, currentMonth, monthDay);
  //Serial.println(json_year_month_filename);
  
  //Serial.println("");

  return json_year_month_filename;
}

char* getCurrentTime(){
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  //Serial.print("Epoch Time: ");
  //Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  //Serial.print("Formatted Time: ");
  //Serial.println(formattedTime);  

  int currentHour = timeClient.getHours();
  //Serial.print("Hour: ");
  //Serial.println(currentHour);  

  int currentMinute = timeClient.getMinutes();
  ////Serial.print("Minutes: ");
  //Serial.println(currentMinute); 


  static char json_current_time[16];
  //Serial.print("Current date: ");
  
  sprintf(json_current_time, "%02d:%02d", currentHour, currentMinute);
  //Serial.println(json_year_month_filename);
  
  //Serial.println("");

  return json_current_time;
}

void setup() {
  
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  aux.add({header, caption});
  Portal.join(aux);
  Server.on("/", rootPage);
  if (Portal.begin(ssid,password)) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(28800);
}

void loop() {
  Portal.handleClient();
  

  String json_date_filename = String(getCurrentDdate());
  Serial.print("Current date: ");
  Serial.println(json_date_filename);

  String json_current_time = String(getCurrentTime());
  Serial.print("Current time: ");
  Serial.println(json_current_time);

  delay(2000);
}
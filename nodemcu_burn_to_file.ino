/*
  Fara Farizul
  Complete project details at https://github.com/farxpeace/nodemcu-waktu-azan
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>  // Replace with WebServer.h for ESP32
#include <WiFiManager.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>

SoftwareSerial mp3Serial(D4, D5); // RX, TX
DFPlayerMini_Fast mp3;

WiFiManager wifiManager;

//Your Domain name with URL path or IP address with path
const char *serverName = "https://pusara.tpirs.net/json/";

WiFiClientSecure wifi;
HTTPClient http;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

JsonDocument jsonDoc; // JSON document to store parsed data
String temporary_date = "1111";
String  json_dttm = "0";
String  json_imsak = "0";
String  json_fajr = "0";
String  json_dhuhr = "0";
String  json_asar = "0";
String  json_maghrib = "0";
String  json_isha = "0";

String adhan_is_playing = "no";
String zikr_is_playing = "no";

String *getCurrentDttm() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();

  String formattedTime = timeClient.getFormattedTime();
  //Get a time structure
  struct tm *ptm = gmtime((time_t *)&epochTime);

  int currentYear = ptm->tm_year + 1900;
  int currentMonth = ptm->tm_mon + 1;
  int monthDay = ptm->tm_mday;
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();

  static char json_current_date[16];
  static char json_current_time[16];
  static String json_current_dttm[2];

  sprintf(json_current_date, "%04d-%02d-%02d", currentYear, currentMonth, monthDay);
  sprintf(json_current_time, "%02d:%02d", currentHour, currentMinute);

  json_current_dttm[0] = json_current_date;
  json_current_dttm[1] = json_current_time;



  return json_current_dttm;
}

bool loadDailyJsonFile(const char *fileName) {
  if(temporary_date == json_dttm){
    Serial.print("JSON Already Loaded for : ");
    Serial.println(temporary_date);
    return true;
  }else{
    //get new json for today
    String fullPath = String(serverName) + fileName + String(".json");  // Create the full path for the JSON file
    String getpath = String("/json/") + fileName + String(".json");
    Serial.print("Fullpath is : ");
    Serial.println(fullPath);
    Serial.print("GET Path is : ");
    Serial.println(getpath);

    wifi.setInsecure();
    http.useHTTP10(true);
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    Serial.print("[HTTPS] begin...\n");

    if (http.begin(wifi, fullPath)) { 
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = http.getString();
            //String payload = https.getString(1024);  // optionally pre-reserve string to avoid reallocations in chunk mode
            //Serial.println(payload);
            DeserializationError error = deserializeJson(jsonDoc, payload);
            if (error) {
              Serial.println("Failed to parse JSON");
              return false;
            }
            serializeJsonPretty(jsonDoc, Serial);
            json_dttm = String(jsonDoc["dttm"]);
            json_imsak = String(jsonDoc["imsak"]);
            json_fajr = String(jsonDoc["fajr"]);
            json_dhuhr = String(jsonDoc["dhuhr"]);
            json_asar = String(jsonDoc["asar"]);
            json_maghrib = String(jsonDoc["maghrib"]);
            json_isha = String(jsonDoc["isha"]);

            //SET JSON DATA
            Serial.print("JSON DATA imsak : ");
            Serial.println(json_imsak);
            Serial.print("JSON DATA fajr : ");
            Serial.println(json_fajr);
            Serial.print("JSON DATA dhuhr : ");
            Serial.println(json_dhuhr);
            Serial.print("JSON DATA asar : ");
            Serial.println(json_asar);
            Serial.print("JSON DATA maghrib : ");
            Serial.println(json_maghrib);
            Serial.print("JSON DATA isha : ");
            Serial.println(json_isha);
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    }else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
  

  
  

  return true;
}
bool isTimeForAction(String adhanTime, int offsetMinutes = 0) {
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();

  int adhanHour = adhanTime.substring(0, 2).toInt();
  int adhanMinute = adhanTime.substring(3, 5).toInt();

  int actionHour = adhanHour;
  int actionMinute = adhanMinute - offsetMinutes;

  if (actionMinute < 0) {
    actionMinute += 60;
    actionHour -= 1;
    if (actionHour < 0) {
      actionHour += 24;
    }
  }

  return (currentHour == actionHour && currentMinute == actionMinute);
}
void playAdhan(const char* filename, int trackNumber, int playType) {
  String fullPath = String("/mp3/") + filename; // Create the full path for the MP3 file
  delay(1000);
  mp3.volume(30);
  delay(1000);
  mp3.stop();
  mp3.stopRepeatPlay();
  delay(1000);
  
  mp3.playFromMP3Folder(trackNumber); // Play MP3 file from mp3 folder
  Serial.print("Playing: ");
  Serial.println(fullPath);
  //Set var adhan_is_playing
  adhan_is_playing = "yes";
  zikr_is_playing = "no";
  if(playType == 1){
    delay(1000*60*5); //5 minutes
  } else if(playType == 2){
    delay(30000); //5 30 seconds
  }
  
  adhan_is_playing = "no";
}


void setup() {
  WiFi.mode(WIFI_STA);  // explicitly set mode, esp defaults to STA+AP
  // Initialize Serial Monitor
  Serial.begin(115200);
  mp3Serial.begin(9600);
  mp3.begin(mp3Serial);
  delay(1000);
  mp3.volume(30);
  mp3.playFromMP3Folder(13); //0012_wifi_error_while_connecting.mp3

  wifiManager.setConfigPortalTimeout(60);
  bool res;
  res = wifiManager.autoConnect("Waktu Azan", "12345678");
  if (!res) {
    
    mp3.playFromMP3Folder(12); //0012_wifi_error_while_connecting.mp3
    delay(15000);
    Serial.println("Failed to connect");
    ESP.restart();
  } else {
    //if you get here you have connected to the WiFi
    mp3.playFromMP3Folder(11); //0011_wifi_berjaya_disambungkan.mp3
    Serial.println("connected...yeey :)");
  }

  // Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(28800);
}


void loop() {
  

  String *json_current_dttm = getCurrentDttm();
  String current_date = String(json_current_dttm[0]);
  String current_time = String(json_current_dttm[1]);
  Serial.print("Array Current date: ");
  Serial.println(current_date);
  Serial.print("Array Current time: ");
  Serial.println(current_time);

  temporary_date = current_date; //uncomment to just get daily json and save
  if (!loadDailyJsonFile(current_date.c_str())) {
    delay(60000); // Wait a minute before trying again
    return;
  }


  if(adhan_is_playing == "yes"){
    Serial.println("Adhan is playing");
    Serial.print("Volume was set to : ");
    Serial.println(mp3.currentVolume());
  }else{
    if(temporary_date == json_dttm){
      Serial.println("Date match. Check for adhan time");
      if (isTimeForAction(json_fajr, 15)) {
        Serial.println("Reminder Adhan time match for Fajr");
        playAdhan("reminder_fajr.mp3", 1, 2);
      } else if(isTimeForAction(json_fajr)){
        Serial.println("Reminder Adhan time match for Fajr");
        playAdhan("fajr.mp3", 2, 1);
      } else if(isTimeForAction(json_dhuhr, 15)){ // reminder Dhuhr
        Serial.println("Reminder Adhan time match for Dhuhr");
        playAdhan("dhuhr.mp3", 3, 2);
      } else if(isTimeForAction(json_dhuhr)){ // reminder Dhuhr
        Serial.println("Adhan time match for Dhuhr");
        playAdhan("dhuhr.mp3", 4, 1);
      } else if(isTimeForAction(json_asar, 15)){ // reminder asr
        Serial.println("Reminder Adhan time match for Asr");
        playAdhan("asr.mp3", 5, 2);
      } else if(isTimeForAction(json_asar)){ // asr
        Serial.println("Adhan time match for Asr");
        playAdhan("asr.mp3", 6, 1);
      } else if(isTimeForAction(json_maghrib, 15)){ // reminder maghrib
        Serial.println("Reminder Adhan time match for Maghrib");
        playAdhan("maghrib.mp3", 7, 2);
      } else if(isTimeForAction(json_maghrib)){ // maghrib
        Serial.println("Adhan time match for Maghrib");
        playAdhan("maghrib.mp3", 8, 1);
      } else if(isTimeForAction(json_isha, 15)){ // reminder Isha
        Serial.println("Reminder Adhan time match for Isha");
        playAdhan("isha.mp3", 9, 2);
      } else if(isTimeForAction(json_isha)){ // Isha
        Serial.println("Adhan time match for Isha");
        playAdhan("isha.mp3", 10, 1);
      } else{
        Serial.println("No Adhan match.");
      }
    }else{
      Serial.println("No Adhan times for today.");
    }
    delay(10000);
  }

  
}
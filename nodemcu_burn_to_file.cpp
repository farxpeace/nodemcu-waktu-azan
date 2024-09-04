#include <ESP8266WiFi.h>
#include <DFPlayerMini_Fast.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Replace with your network credentials
const char* ssid     = "your_SSID";
const char* password = "your_PASSWORD";

// Define the pin connections
#define SD_CS_PIN D8

// Relay pin definitions
#define RELAY_FAJR D3
#define RELAY_DHUHR D4
#define RELAY_ASR D5
#define RELAY_MAGHRIB D6
#define RELAY_ISHA D7

// LED pin definitions
#define LED_WIFI_STATUS D0 // Blue LED
#define LED_POWER_STATUS D1 // Green LED
#define LED_MP3_PLAYING D2 // Red LED

SoftwareSerial mp3Serial(D4, D5); // RX, TX
DFPlayerMini_Fast mp3;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // NTP server, UTC offset in seconds, update interval

const unsigned long RELAY_ON_DURATION = 30000; // Relay on for 30 seconds

StaticJsonDocument<2048> jsonDoc; // JSON document to store parsed data

// Function to play Adhan based on filename and control relay
void playAdhan(const char* filename, int relayPin) {
  if (SD.exists(filename)) {
    digitalWrite(LED_MP3_PLAYING, HIGH); // Turn on MP3 playing LED
    mp3.playMP3Folder(filename); // Play MP3 file
    Serial.print("Playing: ");
    Serial.println(filename);
    digitalWrite(relayPin, HIGH); // Turn relay ON
    delay(RELAY_ON_DURATION); // Keep relay on for a certain duration
    digitalWrite(relayPin, LOW); // Turn relay OFF
    digitalWrite(LED_MP3_PLAYING, LOW); // Turn off MP3 playing LED
  } else {
    Serial.print("File not found: ");
    Serial.println(filename);
  }
}

// Function to compare current time with Adhan time
bool isTimeForAdhan(String adhanTime) {
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();

  int adhanHour = adhanTime.substring(0, 2).toInt();
  int adhanMinute = adhanTime.substring(3, 5).toInt();

  return (currentHour == adhanHour && currentMinute == adhanMinute);
}

// Function to load JSON file for the current month
bool loadMonthlyJsonFile(const char* fileName) {
  File file = SD.open(fileName, "r");

  if (!file) {
    Serial.print("Failed to open JSON file: ");
    Serial.println(fileName);
    return false;
  }

  DeserializationError error = deserializeJson(jsonDoc, file);
  if (error) {
    Serial.println("Failed to parse JSON");
    file.close();
    return false;
  }

  file.close();
  return true;
}

void setup() {
  Serial.begin(115200);
  mp3Serial.begin(9600);
  mp3.begin(mp3Serial);

  WiFi.begin(ssid, password);

  // Initialize LED pins
  pinMode(LED_WIFI_STATUS, OUTPUT);
  pinMode(LED_POWER_STATUS, OUTPUT);
  pinMode(LED_MP3_PLAYING, OUTPUT);

  // Initialize relay pins
  pinMode(RELAY_FAJR, OUTPUT);
  pinMode(RELAY_DHUHR, OUTPUT);
  pinMode(RELAY_ASR, OUTPUT);
  pinMode(RELAY_MAGHRIB, OUTPUT);
  pinMode(RELAY_ISHA, OUTPUT);

  // Ensure all relays and LEDs are initially OFF
  digitalWrite(RELAY_FAJR, LOW);
  digitalWrite(RELAY_DHUHR, LOW);
  digitalWrite(RELAY_ASR, LOW);
  digitalWrite(RELAY_MAGHRIB, LOW);
  digitalWrite(RELAY_ISHA, LOW);
  digitalWrite(LED_WIFI_STATUS, LOW);
  digitalWrite(LED_MP3_PLAYING, LOW);

  // Indicate NodeMCU power is ON
  digitalWrite(LED_POWER_STATUS, HIGH);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  digitalWrite(LED_WIFI_STATUS, HIGH); // Indicate WiFi connected
  timeClient.begin();

  // Initialize SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card initialization failed!");
    return;
  }

  Serial.println("SD Card initialized.");
}

void loop() {
  timeClient.update();

  // Get current date
  String currentDate = timeClient.getFormattedDate().substring(0, 10); // Format: YYYY-MM-DD
  String currentMonthYear = timeClient.getFormattedDate().substring(0, 7); // Format: YYYY-MM

  // Construct the filename based on the current month and year
  String fileName = "/" + currentMonthYear + ".json";

  // Load the JSON file for the current month
  if (!loadMonthlyJsonFile(fileName.c_str())) {
    delay(60000); // Wait a minute before trying again
    return;
  }

  // Check if the JSON contains today's date
  if (jsonDoc.containsKey(currentDate)) {
    JsonObject todayTimes = jsonDoc[currentDate];

    // Check each Adhan time and play corresponding MP3 file with relay
    if (isTimeForAdhan(todayTimes["fajr"])) {
      playAdhan("/fajr.mp3", RELAY_FAJR);
    } else if (isTimeForAdhan(todayTimes["dhuhr"])) {
      playAdhan("/dhuhr.mp3", RELAY_DHUHR);
    } else if (isTimeForAdhan(todayTimes["asr"])) {
      playAdhan("/asr.mp3", RELAY_ASR);
    } else if (isTimeForAdhan(todayTimes["maghrib"])) {
      playAdhan("/maghrib.mp3", RELAY_MAGHRIB);
    } else if (isTimeForAdhan(todayTimes["isha"])) {
      playAdhan("/isha.mp3", RELAY_ISHA);
    }
  } else {
    Serial.println("No Adhan times for today.");
  }

  delay(60000); // Check every minute
}
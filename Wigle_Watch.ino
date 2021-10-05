/*
 * GPSDisplay.ino: Example of displaying  GPS information on the screen
 * Copyright 2020 Lewis he
 */
// Only supports 2020 V2 version, other versions do not support
// Only supports 2020 V2 version, other versions do not support
// Only supports 2020 V2 version, other versions do not support

#include "config.h"

TTGOClass *ttgo = nullptr;
TFT_eSPI *tft = nullptr;
TinyGPSPlus *gps = nullptr;

TFT_eSprite *eSpLoaction = nullptr;
TFT_eSprite *eSpDate = nullptr;
TFT_eSprite *eSpTime = nullptr;
TFT_eSprite *eSpSpeed = nullptr;
TFT_eSprite *eSpSatellites = nullptr;

uint32_t last = 0;
uint32_t updateTimeout = 0;

#include <SPI.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include "WiFi.h"
#include <SD.h>
#define ARDUINO_USD_CS 4
#define LOG_FILE_PREFIX "/gpslog"
#define MAX_LOG_FILES 100
#define LOG_FILE_SUFFIX "csv"
#define LOG_COLUMN_COUNT 11
#define LOG_RATE 500
#define NOTE_DH2 661

char logFileName[13];
int totalNetworks = 0;
unsigned long lastLog = 0;

const String wigleHeaderFileFormat = "WigleWifi-1.4,appRelease=2.26,model=Feather,release=0.0.0,device=arduinoWardriving,display=3fea5e7,board=esp8266,brand=Adafruit";
char * log_col_names[LOG_COLUMN_COUNT] = {
  "MAC", "SSID", "AuthMode", "FirstSeen", "Channel", "RSSI", "Latitude", "Longitude", "AltitudeMeters", "AccuracyMeters", "Type"
};

HardwareSerial ss(2);
File root;

void setup(void)
{
    Serial.begin(115200);

    ttgo = TTGOClass::getWatch();

    ttgo->begin();

    ttgo->openBL();
    //Create a new pointer to save the display object
    tft = ttgo->tft;

    tft->fillScreen(TFT_BLACK);
    tft->setTextFont(2);
    tft->println("Begin GPS Module...");

    //Open gps power
    ttgo->trunOnGPS();

    ttgo->gps_begin();

    gps = ttgo->gps;

    // Display on the screen, latitude and longitude, number of satellites, and date and time

    eSpLoaction   = new TFT_eSprite(tft); // Sprite object for eSpLoaction
    eSpDate   = new TFT_eSprite(tft); // Sprite object for eSpDate
    eSpTime   = new TFT_eSprite(tft); // Sprite object for eSpTime
    eSpSatellites   = new TFT_eSprite(tft); // Sprite object for eSpSatellites

    eSpLoaction->createSprite(240, 48);
    eSpLoaction->setTextFont(2);

    eSpDate->createSprite(240, 48);
    eSpDate->setTextFont(2);

    eSpTime->createSprite(240, 48);
    eSpTime->setTextFont(2);

    eSpSatellites->createSprite(240, 48);
    eSpSatellites->setTextFont(2);

    last = millis();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (!SD.begin(ARDUINO_USD_CS)) {
      Serial.println("No SD card attached");
      while(true)
      delay(100);
      }
        delay(500);
        updateFileName();
        printHeader();
}


void loop(void)
{
    ttgo->gpsHandler();

    if (gps->location.isUpdated()) {
        updateTimeout = millis();
        eSpLoaction->fillSprite(TFT_BLACK);
        eSpLoaction->setTextColor(TFT_GREEN, TFT_BLACK);
        eSpLoaction->setCursor(0, 0);
        eSpLoaction->print("LOCATION ");
        eSpLoaction->print("Fix Age=");
        eSpLoaction->println(gps->location.age());
        eSpLoaction->print("Lat= ");
        eSpLoaction->print(gps->location.lat(), 6);
        eSpLoaction->print(" Long= ");
        eSpLoaction->print(gps->location.lng(), 6);
        eSpLoaction->pushSprite(0, 0);
        void lookForNetworks();

       
        
    } else {
        if (millis() - updateTimeout > 3000) {
            updateTimeout = millis();

            eSpLoaction->fillSprite(TFT_BLACK);
            eSpLoaction->setTextColor(TFT_GREEN);
            eSpLoaction->setCursor(0, 0);
            eSpLoaction->print("LOCATION ");
            eSpLoaction->setTextColor(TFT_RED);
            eSpLoaction->print("INVAILD");
            eSpLoaction->pushSprite(0, 0);
        }
    }



    if (gps->date.isUpdated()) {
        eSpDate->setTextColor(TFT_GREEN, TFT_BLACK);
        eSpDate->fillSprite(TFT_BLACK);
        eSpDate->setCursor(0, 0);
        eSpDate->print("DATE Fix Age= ");
        eSpDate->println(gps->date.age());
        eSpDate->print("Year=");
        eSpDate->print(gps->date.year());
        eSpDate->print(" Month=");
        eSpDate->print(gps->date.month());
        eSpDate->print(" Day=");
        eSpDate->println(gps->date.day());
        eSpDate->pushSprite(0, 49);

        Serial.print(F("DATE Fix Age="));
        Serial.print(gps->date.age());
        Serial.print(F("ms Raw="));
        Serial.print(gps->date.value());
        Serial.print(F(" Year="));
        Serial.print(gps->date.year());
        Serial.print(F(" Month="));
        Serial.print(gps->date.month());
        Serial.print(F(" Day="));
        Serial.println(gps->date.day());
    }


    if (gps->time.isUpdated()) {
        eSpTime->setTextColor(TFT_GREEN, TFT_BLACK);
        eSpTime->fillSprite(TFT_BLACK);
        eSpTime->setCursor(0, 0);
        eSpTime->print("TIME Fix Age= ");
        eSpTime->println(gps->time.age());
        eSpTime->print("Hour=");
        eSpTime->print(gps->time.hour());
        eSpTime->print(" Minute=");
        eSpTime->print(gps->time.minute());
        eSpTime->print(" Second=");
        eSpTime->println(gps->time.second());
        eSpTime->pushSprite(0, 49 * 2);

        Serial.print(F("TIME Fix Age="));
        Serial.print(gps->time.age());
        Serial.print(F("ms Raw="));
        Serial.print(gps->time.value());
        Serial.print(F(" Hour="));
        Serial.print(gps->time.hour());
        Serial.print(F(" Minute="));
        Serial.print(gps->time.minute());
        Serial.print(F(" Second="));
        Serial.print(gps->time.second());
        Serial.print(F(" Hundredths="));
        Serial.println(gps->time.centisecond());
    }

    if (gps->speed.isUpdated()) {
        Serial.print(F("SPEED      Fix Age="));
        Serial.print(gps->speed.age());
        Serial.print(F("ms Raw="));
        Serial.print(gps->speed.value());
        Serial.print(F(" Knots="));
        Serial.print(gps->speed.knots());
        Serial.print(F(" MPH="));
        Serial.print(gps->speed.mph());
        Serial.print(F(" m/s="));
        Serial.print(gps->speed.mps());
        Serial.print(F(" km/h="));
        Serial.println(gps->speed.kmph());
    }

    if (gps->altitude.isUpdated()) {
        Serial.print(F("ALTITUDE   Fix Age="));
        Serial.print(gps->altitude.age());
        Serial.print(F("ms Raw="));
        Serial.print(gps->altitude.value());
        Serial.print(F(" Meters="));
        Serial.print(gps->altitude.meters());
        Serial.print(F(" Miles="));
        Serial.print(gps->altitude.miles());
        Serial.print(F(" KM="));
        Serial.print(gps->altitude.kilometers());
        Serial.print(F(" Feet="));
        Serial.println(gps->altitude.feet());
    }


    if (gps->satellites.isUpdated()) {
        eSpSatellites->setTextColor(TFT_GREEN, TFT_BLACK);
        eSpSatellites->fillSprite(TFT_BLACK);
        eSpSatellites->setCursor(0, 0);
        eSpSatellites->print("SATELLITES Fix Age= ");
        eSpSatellites->print(gps->satellites.age());
        eSpSatellites->print("ms Value=");
        eSpSatellites->println(gps->satellites.value());

        Serial.print(F("SATELLITES Fix Age="));
        Serial.print(gps->satellites.age());
        Serial.print(F("ms Value="));
        Serial.println(gps->satellites.value());

        eSpSatellites->pushSprite(0, 49 * 3);
    }

    if (gps->hdop.isUpdated()) {
        Serial.print(F("HDOP       Fix Age="));
        Serial.print(gps->hdop.age());
        Serial.print(F("ms Value="));
        Serial.println(gps->hdop.value());
    }

    if (millis() - last > 5000) {
        Serial.println();
        if (gps->location.isValid()) {
            static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
            double distanceToLondon =
                TinyGPSPlus::distanceBetween(
                    gps->location.lat(),
                    gps->location.lng(),
                    LONDON_LAT,
                    LONDON_LON);
            double courseToLondon =
                TinyGPSPlus::courseTo(
                    gps->location.lat(),
                    gps->location.lng(),
                    LONDON_LAT,
                    LONDON_LON);

            Serial.print(F("LONDON     Distance="));
            Serial.print(distanceToLondon / 1000, 6);
            Serial.print(F(" km Course-to="));
            Serial.print(courseToLondon, 6);
            Serial.print(F(" degrees ["));
            Serial.print(TinyGPSPlus::cardinal(courseToLondon));
            Serial.println(F("]"));
        }

        Serial.print(F("DIAGS      Chars="));
        Serial.print(gps->charsProcessed());
        Serial.print(F(" Sentences-with-Fix="));
        Serial.print(gps->sentencesWithFix());
        Serial.print(F(" Failed-checksum="));
        Serial.print(gps->failedChecksum());
        Serial.print(F(" Passed-checksum="));
        Serial.println(gps->passedChecksum());

        if (gps->charsProcessed() < 10) {
            Serial.println(F("WARNING: No GPS data.  Check wiring."));
            tft->fillScreen(TFT_BLACK);
            tft->setTextColor(TFT_RED, TFT_BLACK);
            tft->println("ERROR: No GPS data.  Check wiring.");
            while (1);
        }

        last = millis();
        Serial.println();
    }
}




void lookForNetworks() {
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("No networks found");
    Serial.println("No networks found");
    //M5.Lcd.fillScreen(RED);
  } else {
    for (uint8_t i = 0; i <= n; ++i) {
      Serial.print("Network name: ");
      Serial.println(WiFi.SSID(i));
      if ((isOnFile(WiFi.BSSIDstr(i)) == -1) && (WiFi.channel(i) > 0) && (WiFi.channel(i) < 15)) { //Avoid erroneous channels
        totalNetworks++;
        File logFile = SD.open(logFileName, FILE_APPEND);
        Serial.print("New network found ");
        Serial.println(WiFi.BSSIDstr(i));
        String SSID = WiFi.SSID(i);
        logFile.print(WiFi.BSSIDstr(i));
        logFile.print(',');
        SSID = WiFi.SSID(i);
        // Commas in SSID brokes the csv file padding
        SSID.replace(",", ".");
        if(logFile.print(SSID)){
          Serial.println("************************************************************************Message appended");
    } else {
        Serial.println("******************************************************************************Append failed");
    
        }
        logFile.print(SSID);
        logFile.print(',');
        logFile.print(getEncryption(i));
        logFile.print(',');
        logFile.print(gps->date.year());
        logFile.print('-');
        logFile.print(gps->date.month());
        logFile.print('-');
        logFile.print(gps->date.day());
        logFile.print(' ');
        logFile.print(gps->time.hour());
        logFile.print(':');
        logFile.print(gps->time.minute());
        logFile.print(':');
        logFile.print(gps->time.minute());
        logFile.print(',');
        logFile.print(WiFi.channel(i));
        logFile.print(',');
        logFile.print(WiFi.RSSI(i));
        logFile.print(',');
        logFile.print(gps->location.lat()), 6);
        logFile.print(',');
        logFile.print(gps->location.lng(), 6);
        logFile.print(',');
        logFile.print(gps->altitude.meters(), 1);
        logFile.print(',');
        logFile.print((tinyGPS.hdop.value(), 1));
        logFile.print(',');
        logFile.print("WIFI");
        logFile.println();
        logFile.close();
      }
    }
  }
  
}



String getEncryption(uint8_t network) {
  byte encryption = WiFi.encryptionType(network);
  switch (encryption) {
    case 2:
      return "[WPA-PSK-CCMP+TKIP][ESS]";
    case 5:
      return "[WEP][ESS]";
    case 4:
      return "[WPA2-PSK-CCMP+TKIP][ESS]";
    case 7:
      return "[ESS]";
    case 8:
      return "[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][ESS]";
  }
}




int isOnFile(String mac) {
  File netFile = SD.open(logFileName);
  String currentNetwork;
  if (netFile) {
    while (netFile.available()) {
      currentNetwork = netFile.readStringUntil('\n');
      if (currentNetwork.indexOf(mac) != -1) {
        //Serial.println("The network was already found");
        //Serial.println("The network was already found");
        //Serial.println("The network was already found");
        //Serial.println("******************************************************************");
        netFile.close();
        //M5.Lcd.print("We think we already found this network debug????");
        //M5.Lcd.print("The index of the network is: ");
        //M5.Lcd.println(currentNetwork.indexOf(mac));
        return currentNetwork.indexOf(mac);
      }
    }
    netFile.close();
    Serial.println("We dont think we've seen this network before");
    //M5.Lcd.println("We dont think we've seen this network before");
    //M5.Lcd.print("The index of the network is: ");
    //M5.Lcd.println(currentNetwork.indexOf(mac));
    return currentNetwork.indexOf(mac);
  }
  Serial.println("netFile was not true");
  //M5.Lcd.print("netFile was not true");
}

void printHeader() {
  File logFile = SD.open(logFileName, FILE_WRITE);
  if (logFile) {
    int i = 0;
    logFile.println(wigleHeaderFileFormat); // comment out to disable Wigle header
    for (; i < LOG_COLUMN_COUNT; i++) {
      logFile.print(log_col_names[i]);
      if (i < LOG_COLUMN_COUNT - 1)
        logFile.print(',');
      else
        logFile.println();
    }
    logFile.close();
  }
}

void updateFileName() {
  int i = 0;
  for (; i < MAX_LOG_FILES; i++) {
    memset(logFileName, 0, strlen(logFileName));
    sprintf(logFileName, "%s%d.%s", LOG_FILE_PREFIX, i, LOG_FILE_SUFFIX);
    if (!SD.exists(logFileName)) {
      Serial.println("we picked a new file name");
      Serial.println(logFileName);
      break;
    } else {
      Serial.print(logFileName);
      Serial.println(" exists");
    }
  }
  
}

void screenWipe() {
//  M5.Lcd.setTextSize(3);
//  M5.Lcd.fillScreen(BLACK);
 // M5.Lcd.setCursor(0, 0);
}

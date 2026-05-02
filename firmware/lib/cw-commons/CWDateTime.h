#pragma once
#include <Arduino.h>

#include <ezTime.h>
#include <WiFi.h>

class CWDateTime
{
private:
  Timezone myTZ;
  bool use24hFormat = true;
  bool useSpanish = true;

  String translateToSpanish(String text);

public:
  void begin(const char *timeZone, bool use24format, const char *ntpServer, const char *posixTZ);
  void setSpanish(bool enabled);
  String getFormattedTime();
  String getFormattedTime(const char* format);

  char *getHour(const char *format);
  char *getMinute(const char *format);
  int getHour();
  int getHour24();
  int getMinute();
  int getSecond();
  long getMilliseconds();

  int getDay();
  int getMonth();
  int getWeekday();

  String getDayNameSpanish(bool abbreviated = true);
  String getMonthNameSpanish(bool abbreviated = true);

  bool isAM();
  bool is24hFormat();
};

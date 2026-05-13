#pragma once

#include <Arduino.h>
#include "IDateI18n.h"

class DateI18nES: public IDateI18n
{
  private:
    char buffer[5];
    char weekDayTemp[4]= "\0";
    const char* _weekDayWords = "DOM\0LUN\0MAR\0MIE\0JUE\0VIE\0SAB\0";

  public:
    virtual const char* formatDate(int day, int month) {
      sprintf(buffer, "%d/%d", day, month);
      return buffer;
    }

    virtual const char* weekDayName(int weekday) {
      strncpy(weekDayTemp, _weekDayWords + (weekday*4), 4);
      return weekDayTemp;
    }

    virtual void timeInWords(int h, int m, char* hWords, char* mWords) {
      int hour12 = (h % 12);

      if (h == 12) {
        hour12 = h;
      }

      char hours[][13] = {
        "cero", "una", "dos", "tres", "cuatro",
        "cinco", "seis", "siete", "ocho", "nueve",
        "diez", "once", "doce"
      };

      char min_dec[][10] = {
        "cero", "diez", "veinte", "treinta", "cuarenta", "cincuenta"
      };

      char min_uni[][19] = {
        "cero", "uno", "dos", "tres", "cuatro", "cinco",
        "seis", "siete", "ocho", "nueve", "diez",
        "once", "doce", "trece", "catorce", "quince",
        "dieciseis", "diecisiete", "dieciocho", "diecinueve"
      };

      char min_veinti[][12] = {
        "veinte", "veintiuno", "veintidos", "veintitres", "veinticuatro",
        "veinticinco", "veintiseis", "veintisiete", "veintiocho", "veintinueve"
      };

      if (h == 0 && m == 0) {
        strcpy(hWords, "media\nnoche");
        strcpy(mWords, "");
      } else if (h == 12 && m == 0) {
        strcpy(hWords, "medio\ndia");
        strcpy(mWords, "");
      } else if (m == 0) {
        strcpy(hWords, hours[hour12]);
        strcpy(mWords, "en punto");
      } else if (m == 30) {
        strcpy(hWords, hours[hour12]);
        strcpy(mWords, "y media");
      } else if (m == 15) {
        strcpy(hWords, hours[hour12]);
        strcpy(mWords, "y cuarto");
      } else if (m == 45) {
        strcpy(hWords, hours[hour12]);
        strcpy(mWords, "cuarenta\ny cinco");
      } else {
        strcpy(hWords, hours[hour12]);

        if (m < 10) {
          strcpy(mWords, "con\n");
          strcat(mWords, min_uni[m]);
        } else if (m < 20) {
          strcpy(mWords, "y\n");
          strcat(mWords, min_uni[m]);
        } else if (m < 30) {
          strcpy(mWords, "y\n");
          strcat(mWords, min_veinti[m - 20]);
        } else {
          strcpy(mWords, min_dec[(m - (m%10))/10]);

          if (m%10 > 0) {
            strcat(mWords, "\ny ");
            strcat(mWords, min_uni[m%10]);
          }
        }
      }
    }
};

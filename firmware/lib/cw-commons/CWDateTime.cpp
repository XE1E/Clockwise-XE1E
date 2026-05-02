#include "CWDateTime.h"

// Spanish day names
const char* DAYS_ES_ABBR[] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"};
const char* DAYS_ES_FULL[] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};

// Spanish month names
const char* MONTHS_ES_ABBR[] = {"Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};
const char* MONTHS_ES_FULL[] = {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};

void CWDateTime::begin(const char *timeZone, bool use24format, const char *ntpServer = NTP_SERVER, const char *posixTZ = "")
{
  Serial.printf("[Time] NTP Server: %s, Timezone: %s\n", ntpServer, timeZone);
  ezt::setServer(String(ntpServer));

  if (strlen(posixTZ) > 1) {
    // An empty value still contains a null character so not empty is a value greater than 1.
    // Set to defined Posix TZ
    myTZ.setPosix(posixTZ);
  } else {
    // Use automatic eztime remote lookup
    myTZ.setLocation(timeZone);
  }

  this->use24hFormat = use24format;
  this->useSpanish = true;
  ezt::updateNTP();
  waitForSync(10);
}

void CWDateTime::setSpanish(bool enabled)
{
  this->useSpanish = enabled;
}

String CWDateTime::getDayNameSpanish(bool abbreviated)
{
  int dayOfWeek = myTZ.dateTime("w").toInt(); // 0=Sunday, 6=Saturday
  if (abbreviated) {
    return String(DAYS_ES_ABBR[dayOfWeek]);
  }
  return String(DAYS_ES_FULL[dayOfWeek]);
}

String CWDateTime::getMonthNameSpanish(bool abbreviated)
{
  int month = myTZ.dateTime("n").toInt(); // 1-12
  if (abbreviated) {
    return String(MONTHS_ES_ABBR[month - 1]);
  }
  return String(MONTHS_ES_FULL[month - 1]);
}

String CWDateTime::translateToSpanish(String text)
{
  if (!useSpanish) return text;

  // Translate abbreviated day names
  text.replace("Sun", "Dom"); text.replace("Mon", "Lun"); text.replace("Tue", "Mar");
  text.replace("Wed", "Mie"); text.replace("Thu", "Jue"); text.replace("Fri", "Vie");
  text.replace("Sat", "Sab");

  // Translate full day names
  text.replace("Sunday", "Domingo"); text.replace("Monday", "Lunes");
  text.replace("Tuesday", "Martes"); text.replace("Wednesday", "Miercoles");
  text.replace("Thursday", "Jueves"); text.replace("Friday", "Viernes");
  text.replace("Saturday", "Sabado");

  // Translate abbreviated month names
  text.replace("Jan", "Ene"); text.replace("Feb", "Feb"); text.replace("Mar", "Mar");
  text.replace("Apr", "Abr"); text.replace("May", "May"); text.replace("Jun", "Jun");
  text.replace("Jul", "Jul"); text.replace("Aug", "Ago"); text.replace("Sep", "Sep");
  text.replace("Oct", "Oct"); text.replace("Nov", "Nov"); text.replace("Dec", "Dic");

  // Translate full month names
  text.replace("January", "Enero"); text.replace("February", "Febrero");
  text.replace("March", "Marzo"); text.replace("April", "Abril");
  text.replace("June", "Junio"); text.replace("July", "Julio");
  text.replace("August", "Agosto"); text.replace("September", "Septiembre");
  text.replace("October", "Octubre"); text.replace("November", "Noviembre");
  text.replace("December", "Diciembre");

  return text;
}

String CWDateTime::getFormattedTime()
{
  return translateToSpanish(myTZ.dateTime());
}

String CWDateTime::getFormattedTime(const char *format)
{
  return translateToSpanish(myTZ.dateTime(format));
}

char *CWDateTime::getHour(const char *format)
{
  static char buffer[3] = {'\0'};
  strncpy(buffer, myTZ.dateTime((use24hFormat ? "H" : "h")).c_str(), sizeof(buffer));
  return buffer;
}

char *CWDateTime::getMinute(const char *format)
{
  static char buffer[3] = {'\0'};
  strncpy(buffer, myTZ.dateTime("i").c_str(), sizeof(buffer));
  return buffer;
}

int CWDateTime::getHour()
{
  return myTZ.dateTime((use24hFormat ? "H" : "h")).toInt();
}

int CWDateTime::getHour24()
{
  return myTZ.dateTime("H").toInt();
}

int CWDateTime::getMinute()
{
  return myTZ.dateTime("i").toInt();
}

int CWDateTime::getSecond()
{
  return myTZ.dateTime("s").toInt();
}

int CWDateTime::getDay() 
{
  return myTZ.dateTime("d").toInt();
}
int CWDateTime::getMonth()
{
  return myTZ.dateTime("m").toInt();
}
int CWDateTime::getWeekday() 
{
  return myTZ.dateTime("w").toInt()-1;
}

long CWDateTime::getMilliseconds() 
{
  return myTZ.ms(TIME_NOW);
}

bool CWDateTime::isAM() 
{
  return myTZ.isAM();
}

bool CWDateTime::is24hFormat() 
{
  return this->use24hFormat;
}
/*
 * clocktime.h — Hora atual em horário local (NTP quando há rede, manual caso contrário).
 */
#ifndef GIZMO_CLOCKTIME_H
#define GIZMO_CLOCKTIME_H

#include "state.h"

inline int currentHour() {
  if (!useManualTime && timeClient.isTimeSet()) {
    return (timeClient.getHours() + settings.tzOffset + 24) % 24;
  }
  unsigned long elapsedMin = (millis() - manualBaseMs) / 60000UL;
  int totalMin = manualHour * 60 + manualMinute + (int)elapsedMin;
  return (totalMin / 60) % 24;
}

inline int currentMinute() {
  if (!useManualTime && timeClient.isTimeSet()) {
    return timeClient.getMinutes();   // offset inteiro de horas não muda os minutos
  }
  unsigned long elapsedMin = (millis() - manualBaseMs) / 60000UL;
  int totalMin = manualHour * 60 + manualMinute + (int)elapsedMin;
  return totalMin % 60;
}

inline String currentTimeStr() {
  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", currentHour(), currentMinute());
  return String(buf);
}

#endif // GIZMO_CLOCKTIME_H

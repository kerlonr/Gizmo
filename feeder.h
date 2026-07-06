/*
 * feeder.h — Controle do relé/motor, histórico, horários e snapshot de hora.
 * Opera sempre sobre o PERFIL ATIVO (activePet()).
 */
#ifndef GIZMO_FEEDER_H
#define GIZMO_FEEDER_H

#include "state.h"
#include "storage.h"
#include "clocktime.h"

inline const char* feedSourceLabel(FeedSource s) {
  switch (s) {
    case FEED_MANUAL:   return "manual";
    case FEED_SCHEDULE: return "agendado";
    default:            return "teste";
  }
}

inline void logFeed(FeedSource source) {
  for (int i = FEED_HISTORY - 1; i > 0; i--) feedHistory[i] = feedHistory[i - 1];
  String t = currentTimeStr();
  strncpy(feedHistory[0].when, t.c_str(), sizeof(feedHistory[0].when));
  feedHistory[0].when[sizeof(feedHistory[0].when) - 1] = '\0';
  feedHistory[0].source = source;
  if (feedHistoryCount < FEED_HISTORY) feedHistoryCount++;
}

inline void startRelay(unsigned long durationMs, FeedSource source, bool record) {
  if (motorRunning) return;
  if (record) logFeed(source);
  digitalWrite(RELAY_PIN, RELAY_ON);
  motorRunning = true;
  motorStartMs = millis();
  motorRunMs   = durationMs;
  Serial.printf("[GIZMO] Rele ON (%lums, %s)\n", durationMs, feedSourceLabel(source));
}

inline void triggerFeed(FeedSource source) {
  startRelay(activePet().portionMs, source, true);
}

inline void handleMotorTimer() {
  if (motorRunning && (millis() - motorStartMs >= motorRunMs)) {
    digitalWrite(RELAY_PIN, RELAY_OFF);
    motorRunning = false;
    Serial.println("[GIZMO] Rele OFF");
  }
}

inline void checkSchedules() {
  int h = currentHour();
  int m = currentMinute();
  int cur = h * 60 + m;
  if (cur == lastCheckedMinute) return;
  lastCheckedMinute = cur;
  PetProfile& p = activePet();
  for (int i = 0; i < p.numSchedules; i++) {
    if (!p.schedules[i].enabled) continue;
    if (p.schedules[i].hour == h && p.schedules[i].minute == m) {
      triggerFeed(FEED_SCHEDULE);
      break;
    }
  }
}

inline String nextFeedTime() {
  PetProfile& p = activePet();
  if (p.numSchedules == 0) return "--:--";
  int cur = currentHour() * 60 + currentMinute();
  int minDiff = 99999, nH = -1, nM = -1;
  for (int i = 0; i < p.numSchedules; i++) {
    if (!p.schedules[i].enabled) continue;
    int d = (p.schedules[i].hour * 60 + p.schedules[i].minute) - cur;
    if (d <= 0) d += 1440;
    if (d < minDiff) { minDiff = d; nH = p.schedules[i].hour; nM = p.schedules[i].minute; }
  }
  if (nH == -1) return "--:--";
  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", nH, nM);
  return String(buf);
}

inline String nextFeedCountdown() {
  PetProfile& p = activePet();
  if (p.numSchedules == 0) return "Nenhum horário";
  int cur = currentHour() * 60 + currentMinute();
  int minDiff = 99999;
  for (int i = 0; i < p.numSchedules; i++) {
    if (!p.schedules[i].enabled) continue;
    int d = (p.schedules[i].hour * 60 + p.schedules[i].minute) - cur;
    if (d <= 0) d += 1440;
    if (d < minDiff) minDiff = d;
  }
  if (minDiff == 99999) return "Nenhum ativo";
  char buf[16];
  int h = minDiff / 60, m = minDiff % 60;
  if (h > 0) snprintf(buf, sizeof(buf), "%dh %02dmin", h, m);
  else       snprintf(buf, sizeof(buf), "%dmin", m);
  return String(buf);
}

// Guarda a hora atual na EEPROM para religar perto do certo após queda de energia.
inline void saveTimeSnapshot() {
  settings.snapHour   = currentHour();
  settings.snapMinute = currentMinute();
  settings.snapValid  = 1;
  settingsSave();
  Serial.printf("[GIZMO] Snapshot de hora salvo: %02d:%02d\n", settings.snapHour, settings.snapMinute);
}

// Restaura o instantâneo no boot como base do relógio manual (NTP corrige depois).
inline void restoreTimeSnapshot() {
  if (!settings.snapValid) return;
  manualHour    = settings.snapHour;
  manualMinute  = settings.snapMinute;
  manualBaseMs  = millis();
  useManualTime = true;
}

#endif // GIZMO_FEEDER_H

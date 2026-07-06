/*
 * webui.h — Rotas HTTP e API JSON consumida pela página (index_html.h).
 */
#ifndef GIZMO_WEBUI_H
#define GIZMO_WEBUI_H

#include "state.h"
#include "storage.h"
#include "clocktime.h"
#include "feeder.h"
#include "network.h"
#include "index_html.h"

// ---------- Helpers de resposta ----------
inline void sendJson(int code, const String& body) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(code, "application/json", body);
}
inline void sendOk() { sendJson(200, "{\"ok\":true}"); }
inline void sendErr(int code, const char* msg) {
  sendJson(code, String("{\"error\":\"") + msg + "\"}");
}
inline String jsonEscape(const char* s) {
  String out;
  for (const char* p = s; *p; p++) {
    if (*p == '"' || *p == '\\') out += '\\';
    out += *p;
  }
  return out;
}

// ---------- Rotas ----------
inline void handleRoot() { server.send_P(200, "text/html", PAGE_HTML); }

inline void handleStatus() {
  PetProfile& p = activePet();
  String lastFeed = feedHistoryCount > 0 ? String(feedHistory[0].when) : "--:--";

  String json = "{";
  json += "\"time\":\""      + currentTimeStr() + "\",";
  json += "\"device\":\""    + jsonEscape(settings.deviceName) + "\",";
  json += "\"pet\":\""       + jsonEscape(p.name) + "\",";
  json += "\"motor\":"       + String(motorRunning ? "true" : "false") + ",";
  json += "\"lastFeed\":\""  + lastFeed + "\",";
  json += "\"nextTime\":\""  + nextFeedTime() + "\",";
  json += "\"countdown\":\"" + nextFeedCountdown() + "\",";
  json += "\"ntp\":"         + String(timeClient.isTimeSet() ? "true" : "false") + ",";
  json += "\"tz\":"          + String(settings.tzOffset) + ",";
  json += "\"portion\":"     + String(p.portionMs / 1000.0, 1) + ",";
  json += "\"activeProfile\":" + String(settings.activeProfile) + ",";

  // Estado de rede
  json += "\"wifi\":{";
  json += "\"ssid\":\""  + jsonEscape(settings.wifiSsid) + "\",";
  json += "\"sta\":"     + String(staConnected() ? "true" : "false") + ",";
  json += "\"staIp\":\"" + (staConnected() ? staIp() : String("")) + "\",";
  json += "\"apIp\":\""  + apIp() + "\"},";

  // Perfis de pets
  json += "\"profiles\":[";
  for (int i = 0; i < settings.numProfiles; i++) {
    if (i > 0) json += ",";
    json += "\"" + jsonEscape(settings.profiles[i].name) + "\"";
  }
  json += "],";

  // Agenda do perfil ativo
  json += "\"schedules\":[";
  for (int i = 0; i < p.numSchedules; i++) {
    if (i > 0) json += ",";
    char s[40];
    snprintf(s, sizeof(s), "{\"h\":%d,\"m\":%d,\"on\":%s}",
             p.schedules[i].hour, p.schedules[i].minute,
             p.schedules[i].enabled ? "true" : "false");
    json += s;
  }
  json += "],";

  // Histórico
  json += "\"history\":[";
  for (int i = 0; i < feedHistoryCount; i++) {
    if (i > 0) json += ",";
    json += "{\"t\":\"" + String(feedHistory[i].when) + "\",\"src\":\"" +
            feedSourceLabel(feedHistory[i].source) + "\"}";
  }
  json += "]}";

  sendJson(200, json);
}

inline void handleFeedNow()   { triggerFeed(FEED_MANUAL); sendOk(); }
inline void handleTestRelay() { startRelay(TEST_DURATION_MS, FEED_TEST, false); sendOk(); }

// ----- Horários (do perfil ativo) -----
inline void handleAddSchedule() {
  if (!server.hasArg("h") || !server.hasArg("m")) return sendErr(400, "h e m obrigatorios");
  int h = server.arg("h").toInt(), m = server.arg("m").toInt();
  if (h < 0 || h > 23 || m < 0 || m > 59) return sendErr(400, "Horario invalido");
  PetProfile& p = activePet();
  if (p.numSchedules >= MAX_SCHEDULES) return sendErr(400, "Maximo atingido");
  for (int i = 0; i < p.numSchedules; i++)
    if (p.schedules[i].hour == h && p.schedules[i].minute == m) return sendErr(400, "Ja cadastrado");
  p.schedules[p.numSchedules++] = { (uint8_t)h, (uint8_t)m, true };
  settingsSave();
  lastCheckedMinute = -1;
  sendOk();
}

inline void handleRemoveSchedule() {
  if (!server.hasArg("i")) return sendErr(400, "i obrigatorio");
  int idx = server.arg("i").toInt();
  PetProfile& p = activePet();
  if (idx < 0 || idx >= p.numSchedules) return sendErr(400, "Indice invalido");
  for (int i = idx; i < p.numSchedules - 1; i++) p.schedules[i] = p.schedules[i + 1];
  p.numSchedules--;
  settingsSave();
  sendOk();
}

inline void handleToggleSchedule() {
  if (!server.hasArg("i")) return sendErr(400, "i obrigatorio");
  int idx = server.arg("i").toInt();
  PetProfile& p = activePet();
  if (idx < 0 || idx >= p.numSchedules) return sendErr(400, "Indice invalido");
  p.schedules[idx].enabled = !p.schedules[idx].enabled;
  settingsSave();
  sendOk();
}

// ----- Perfis de pets -----
inline void handleProfileAdd() {
  if (settings.numProfiles >= MAX_PROFILES) return sendErr(400, "Maximo de pets atingido");
  String name = server.hasArg("name") ? server.arg("name") : "";
  name.trim();
  if (name.length() == 0) name = "Pet";
  PetProfile& np = settings.profiles[settings.numProfiles];
  memset(&np, 0, sizeof(np));
  strncpy(np.name, name.c_str(), sizeof(np.name) - 1);
  np.portionMs    = PORTION_MS_DEFAULT;
  np.numSchedules = 0;
  settings.activeProfile = settings.numProfiles;   // já entra no novo pet
  settings.numProfiles++;
  settingsSave();
  lastCheckedMinute = -1;
  sendOk();
}

inline void handleProfileSelect() {
  if (!server.hasArg("i")) return sendErr(400, "i obrigatorio");
  int idx = server.arg("i").toInt();
  if (idx < 0 || idx >= settings.numProfiles) return sendErr(400, "Indice invalido");
  settings.activeProfile = idx;
  settingsSave();
  lastCheckedMinute = -1;
  sendOk();
}

inline void handleProfileDelete() {
  if (!server.hasArg("i")) return sendErr(400, "i obrigatorio");
  int idx = server.arg("i").toInt();
  if (idx < 0 || idx >= settings.numProfiles) return sendErr(400, "Indice invalido");
  if (settings.numProfiles <= 1) return sendErr(400, "Mantenha ao menos 1 pet");
  for (int i = idx; i < settings.numProfiles - 1; i++)
    settings.profiles[i] = settings.profiles[i + 1];
  settings.numProfiles--;
  if (settings.activeProfile >= settings.numProfiles) settings.activeProfile = settings.numProfiles - 1;
  else if (settings.activeProfile > idx)              settings.activeProfile--;
  settingsSave();
  lastCheckedMinute = -1;
  sendOk();
}

// ----- Configurações -----
// device + tz = "infra" (globais); pet (nome) + portion = do perfil ativo.
inline void handleSaveConfig() {
  if (server.hasArg("device")) {
    String d = server.arg("device"); d.trim();
    if (d.length() > 0 && d.length() <= 32) {
      strncpy(settings.deviceName, d.c_str(), 32);
      settings.deviceName[32] = '\0';
    }
  }
  if (server.hasArg("tz")) {
    int tz = server.arg("tz").toInt();
    if (tz >= -12 && tz <= 14) settings.tzOffset = tz;
  }
  if (server.hasArg("pet")) {
    String pn = server.arg("pet"); pn.trim();
    if (pn.length() > 0) {
      strncpy(activePet().name, pn.c_str(), sizeof(activePet().name) - 1);
      activePet().name[sizeof(activePet().name) - 1] = '\0';
    }
  }
  if (server.hasArg("portion")) {
    long ms = (long)(server.arg("portion").toFloat() * 1000);
    activePet().portionMs = (uint16_t)constrain(ms, (long)PORTION_MS_MIN, (long)PORTION_MS_MAX);
  }
  settingsSave();
  lastCheckedMinute = -1;
  sendOk();
}

inline void handleSetWifi() {
  if (!server.hasArg("ssid")) return sendErr(400, "ssid obrigatorio");
  networkSetWifi(server.arg("ssid"), server.hasArg("pass") ? server.arg("pass") : "");
  settingsSave();
  sendJson(200, "{\"ok\":true,\"msg\":\"Conectando a rede...\"}");
}

inline void handleForgetWifi() {
  networkForget();
  settingsSave();
  sendOk();
}

inline void handleSetTime() {
  if (!server.hasArg("h") || !server.hasArg("m")) return sendErr(400, "h e m obrigatorios");
  manualHour    = constrain(server.arg("h").toInt(), 0, 23);
  manualMinute  = constrain(server.arg("m").toInt(), 0, 59);
  manualBaseMs  = millis();
  useManualTime = true;
  lastCheckedMinute = -1;
  saveTimeSnapshot();                 // já persiste para sobreviver a queda de energia
  sendOk();
}

inline void registerRoutes() {
  server.on("/",                handleRoot);
  server.on("/status",          handleStatus);
  server.on("/feed",            handleFeedNow);
  server.on("/test-relay",      handleTestRelay);
  server.on("/add-schedule",    handleAddSchedule);
  server.on("/remove-schedule", handleRemoveSchedule);
  server.on("/toggle-schedule", handleToggleSchedule);
  server.on("/profile-add",     handleProfileAdd);
  server.on("/profile-select",  handleProfileSelect);
  server.on("/profile-delete",  handleProfileDelete);
  server.on("/save-config",     handleSaveConfig);
  server.on("/wifi",            handleSetWifi);
  server.on("/wifi-forget",     handleForgetWifi);
  server.on("/set-time",        handleSetTime);
  server.onNotFound([]() { server.send(404, "text/plain", "404"); });
}

#endif // GIZMO_WEBUI_H

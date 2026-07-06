/*
 * network.h — Wi-Fi (AP + STA simultâneos), mDNS e sincronização NTP.
 *
 * O AP fica SEMPRE ligado (acesso garantido pelo celular em 192.168.4.1).
 * Em paralelo, se houver uma rede salva, o GIZMO entra como cliente (STA)
 * na sua rede de casa — aí dá para acessar pelo PC e o NTP funciona sozinho.
 */
#ifndef GIZMO_NETWORK_H
#define GIZMO_NETWORK_H

#include "state.h"
#include "clocktime.h"

inline bool staConnected() { return WiFi.status() == WL_CONNECTED; }
inline String apIp()  { return WiFi.softAPIP().toString(); }
inline String staIp() { return WiFi.localIP().toString(); }

inline void staTryConnect() {
  if (strlen(settings.wifiSsid) == 0) return;
  Serial.printf("[GIZMO] Conectando na rede \"%s\"...\n", settings.wifiSsid);
  WiFi.begin(settings.wifiSsid, settings.wifiPass);
  lastWifiTry = millis();
}

inline void networkBegin() {
  WiFi.persistent(false);            // só nós controlamos as credenciais (na EEPROM)
  WiFi.setAutoReconnect(true);       // tenta voltar sozinho se a rede cair
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AP_SSID_DEFAULT, AP_PASS_DEFAULT);
  Serial.printf("[GIZMO] AP: %s | IP: %s\n", AP_SSID_DEFAULT, apIp().c_str());

  staTryConnect();

  timeClient.begin();
  MDNS.begin(MDNS_HOST);
  MDNS.addService("http", "tcp", 80);
}

// Aplica novas credenciais de Wi-Fi vindas da interface.
inline void networkSetWifi(const String& ssid, const String& pass) {
  strncpy(settings.wifiSsid, ssid.c_str(), sizeof(settings.wifiSsid));
  settings.wifiSsid[sizeof(settings.wifiSsid) - 1] = '\0';
  strncpy(settings.wifiPass, pass.c_str(), sizeof(settings.wifiPass));
  settings.wifiPass[sizeof(settings.wifiPass) - 1] = '\0';
  WiFi.disconnect();
  staTryConnect();
}

inline void networkForget() {
  settings.wifiSsid[0] = '\0';
  settings.wifiPass[0] = '\0';
  WiFi.disconnect();
}

inline void networkLoop() {
  MDNS.update();

  // Reconexão não-bloqueante.
  if (strlen(settings.wifiSsid) > 0 && !staConnected() &&
      millis() - lastWifiTry > WIFI_RETRY_MS) {
    staTryConnect();
  }

  if (staConnected()) {
    timeClient.update();
    if (timeClient.isTimeSet()) useManualTime = false;
  }
}

#endif // GIZMO_NETWORK_H
